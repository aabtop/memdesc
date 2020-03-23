# We use Ubuntu 19 because we want full C++ 17 support from Clang and
FROM ubuntu:19.10

LABEL description="Environment for building the project in Linux."

# These are essentially the requirements to running the build without going
# through Docker.
RUN apt-get update && \
    apt-get -y install \
    make \
    clang \
    python3 \
    flex \
    bison \
    clang-tidy \
    clang-format \
    python-pip \
    wget \
    software-properties-common \
    cmake

# Needed by run-clang-tidy.
RUN pip install pyyaml

# Prefer clang over gcc for building.
ENV CC=/usr/bin/clang \
    CXX=/usr/bin/clang++
