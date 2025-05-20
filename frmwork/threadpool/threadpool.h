#pragma once
#include <map>
#include <memory>
#include <thread>
#include <mutex>
#include <future>
#include <functional>
#include <condition_variable>
#include <cassert>
#include <chrono>

#include "safequeue.h"
class Threadpool {
private:
    int dying_threads_num = 0;
    int task_done_threads_num = 0;
    bool is_waiting = false;
    bool id_destructing = false;
    std::string name = "default";

    std::mutex threads_mtx{};
    std::condition_variable threads_cv{};
    std::condition_variable task_done_cv{};

    safequeue<std::function<void()>> tasks_queue{};
    std::map<std::thread::id, std::thread> threads_map;
public:
    explicit  Threadpool(const char* Threadpool_name = "default", unsigned int threads_num = 1)
    {
        for (int i = 0;i < (threads_num ? threads_num : 1);++i) {
            add_thread();
        }
    };

    Threadpool(Threadpool& other) = delete;
    Threadpool(Threadpool&& other) = delete;
    Threadpool& operator=(Threadpool& other) = delete;
    Threadpool& operator=(Threadpool&& other) = delete;

    ~Threadpool() {
        std::unique_lock<std::mutex> lock(threads_mtx);
        dying_threads_num = threads_map.size();
        id_destructing = true;
        threads_cv.wait(lock, [this] { return dying_threads_num == 0; });
    };

public:
    void add_thread() {
        std::lock_guard<std::mutex> lock(threads_mtx);
        std::thread t(std::bind(&Threadpool::loop, this));
        std::thread::id t_id = t.get_id();
        threads_map.emplace(t_id, std::move(t));
        threads_map[t_id].detach();
    };

    void delete_thread() {
        std::lock_guard<std::mutex> lock(threads_mtx);
        if (!threads_map.empty()) {
            dying_threads_num++;
        }
    };

    bool wait_tasks(unsigned int timeout = 1000) {
        bool res;
        {
            std::unique_lock<std::mutex> lock(threads_mtx);
            is_waiting = true;
            res = task_done_cv.wait_for(lock, std::chrono::milliseconds(timeout),
                [this] {return task_done_threads_num >= threads_map.size();});
            task_done_threads_num = 0;
            is_waiting = false;
        }
        threads_cv.notify_all();
        return res;
    };

public:
    size_t get_threads_num() {
        std::lock_guard<std::mutex> lock(threads_mtx);
        return threads_map.size();
    };

    size_t get_tasks_size() {
        return tasks_queue.size();
    };

    const std::string& get_Threadpool_name() {
        return name;
    };

public:
    // void func(args)
    template <typename F, typename... Args,
        typename Invocalbe = std::enable_if_t<std::is_invocable<F, Args...>::value, int>,
        typename ReturnType = decltype((std::declval<F>()(std::declval<Args>()...))),
        typename ReturnVoid = std::enable_if_t<std::is_void<std::invoke_result_t<F, Args...>>::value, int>
    >
    void submit(F&& f, Args &&...args) {
        tasks_queue.push_back(std::move(std::bind(std::forward<F>(f), std::forward<Args>(args)...)));
    };

    // ret func(args)
    template <typename F, typename... Args,
        typename Invocalbe = std::enable_if_t<std::is_invocable<F, Args...>::value, int>,
        typename ReturnType = decltype((std::declval<F>()(std::declval<Args>()...))),
        typename ReturnNonVoid = std::enable_if_t<!std::is_void<std::invoke_result_t<F, Args...>>::value, int>
    >
    std::future<ReturnType> submit(F&& f, Args &&...args) {
        auto func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        auto task_ptr = std::make_shared<std::packaged_task<ReturnType()>>(std::move(func));
        tasks_queue.push_back([task_ptr]() { (*task_ptr)();});
        return task_ptr->get_future();
    };

    // void func(args) fast submit
    template <typename F, typename... Args,
        typename Invocalbe = std::enable_if_t<std::is_invocable<F, Args...>::value, int>,
        typename ReturnType = decltype((std::declval<F>()(std::declval<Args>()...))),
        typename ReturnVoid = std::enable_if_t<std::is_void<std::invoke_result_t<F, Args...>>::value, int>
    >
    void submit_urgent(F&& f, Args &&...args) {
        tasks_queue.push_front(std::move(std::bind(std::forward<F>(f), std::forward<Args>(args)...)));
    };

    // ret func(args) fast submit
    template <typename F, typename... Args,
        typename Invocalbe = std::enable_if_t<std::is_invocable<F, Args...>::value, int>,
        typename ReturnType = decltype((std::declval<F>()(std::declval<Args>()...))),
        typename ReturnNonVoid = std::enable_if_t<!std::is_void<std::invoke_result_t<F, Args...>>::value, int>
    >
    std::future<ReturnType> submit_urgent(F&& f, Args &&...args) {
        auto func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        auto task_ptr = std::make_shared<std::packaged_task<ReturnType()>>(std::move(func));
        tasks_queue.push_front([task_ptr]() { (*task_ptr)();});
        return task_ptr->get_future();
    };

public:
    void loop() {
        while (true) {
            if (dying_threads_num <= 0) {
                std::unique_ptr<std::function<void()>> task_ptr = tasks_queue.wait_pop_front();
                if (task_ptr) {
                    (*task_ptr)();
                }
                else {
                    if (is_waiting) {
                        std::unique_lock<std::mutex> locker(threads_mtx);
                        task_done_threads_num++;
                        task_done_cv.notify_one();
                        threads_cv.wait(locker);
                    }
                    else {
                        std::this_thread::yield();
                    }
                }
            }
            else {
                std::lock_guard<std::mutex> lock(threads_mtx);
                if (dying_threads_num > 0) {
                    dying_threads_num--;
                    threads_map.erase(std::this_thread::get_id());
                    if (is_waiting)
                        task_done_cv.notify_one();
                    if (id_destructing)
                        threads_cv.notify_one();
                    return;
                }
            }
        }
    };
};