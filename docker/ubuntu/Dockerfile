FROM ubuntu:16.04 

RUN dpkg --add-architecture i386

RUN apt-get update
RUN apt-get update && apt-get install -y --no-install-recommends \
        g++-multilib cmake nasm pkg-config libsdl2-dev:i386

WORKDIR /build
VOLUME /src
