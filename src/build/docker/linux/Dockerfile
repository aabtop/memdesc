FROM ubuntu:18.04

LABEL description="Environment for building the project in Linux."

# These are essentially the requirements to running the build without going
# through Docker.
RUN apt-get update && \
    apt-get -y install build-essential cmake clang python3 flex bison \
               clang-tidy clang-format python-pip

# Needed by run-clang-tidy.
RUN pip install pyyaml