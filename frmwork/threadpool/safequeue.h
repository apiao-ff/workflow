#pragma once
#include <deque>
#include <mutex>
#include <memory>
#include <condition_variable>

template<typename T>
class safequeue
{
private:
    std::deque<T> deque;
    std::mutex mtx;
    std::condition_variable cv;

public:
    safequeue() = default;
    safequeue(const safequeue&) = delete;
    safequeue& operator=(const safequeue&) = delete;

public:
    size_t size() {
        std::lock_guard<std::mutex> lock(mtx);
        return deque.size();
    }

    void push_back(T&& t) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            deque.emplace_back(std::move(t));
        }
        cv.notify_one();
    }

    void push_back(const T& t) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            deque.emplace_back(t);
        }
        cv.notify_one();
    }

    void push_front(T&& t) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            deque.emplace_front(std::move(t));
        }
        cv.notify_one();
    }

    void push_front(const T& t) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            deque.emplace_front(t);
        }
        cv.notify_one();
    }

    std::unique_ptr<T> try_pop_front() {
        std::lock_guard<std::mutex> lock(mtx);
        if (!deque.empty()) {
            auto ptr = std::make_unique<T>(std::move(deque.front()));
            deque.pop_front();
            return ptr;
        }
        return nullptr;
    }

    std::unique_ptr<T> try_pop_back() {
        std::lock_guard<std::mutex> lock(mtx);
        if (!deque.empty()) {
            auto ptr = std::make_unique<T>(std::move(deque.back()));
            deque.pop_back();
            return ptr;
        }
        return nullptr;
    }

    std::unique_ptr<T> wait_pop_front() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return !deque.empty(); });
        auto ptr = std::make_unique<T>(std::move(deque.front()));
        deque.pop_front();
        return ptr;
    }

    std::unique_ptr<T> wait_pop_back() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return !deque.empty(); });
        auto ptr = std::make_unique<T>(std::move(deque.back()));
        deque.pop_back();
        return ptr;
    }
};
