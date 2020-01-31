FROM ubuntu:18.04

LABEL description="Environment for building the project."

RUN apt-get update

# These are essentially the requirements to running the build without going
# through Docker.
RUN apt-get -y install build-essential cmake clang python3 flex bison
