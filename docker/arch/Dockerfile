FROM base/archlinux

RUN echo -e "[multilib]\nInclude = /etc/pacman.d/mirrorlist" >> /etc/pacman.conf
RUN pacman -Sy
RUN pacman --noconfirm -Sy \
    make gcc cmake pkg-config nasm lib32-gcc-libs lib32-sdl2

WORKDIR /build
VOLUME /src
