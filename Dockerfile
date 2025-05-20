FROM ubuntu:focal

ENV DEBIAN_FRONTEND=noninteractive

RUN sed -i s@archive.ubuntu.com@mirrors.cloud.tencent.com@g /etc/apt/sources.list && \
    sed -i s@security.ubuntu.com@mirrors.cloud.tencent.com@g /etc/apt/sources.list

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    curl \
    libprotobuf-dev \
    protobuf-compiler \
    libssl-dev \
    zlib1g-dev \
    libboost-all-dev \
    libjsoncpp-dev \
    libmariadbclient-dev \
    libhiredis-dev \
    uuid-dev \   
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

RUN mkdir -p build && cd build && cmake .. && make

CMD ["bash"]
