FROM alpine-kallistios

RUN echo "Installing dependencies..."
RUN apk add cmake g++ sdl2-dev libsodium-dev libpng-dev libbz2 gtest-dev gmock sdl2_image-dev fmt-dev sdl12-compat-dev bzip2-dev vim gperf

RUN echo "Compiling kos-ports..."
WORKDIR /opt/toolchains/dc/
RUN git clone https://github.com/KallistiOS/kos-ports
COPY lua.patch /opt/toolchains/dc/kos-ports
RUN patch kos-ports/lua/Makefile -l -p0 < kos-ports/lua.patch
RUN source /opt/toolchains/dc/kos/environ.sh && /opt/toolchains/dc/kos-ports/utils/build-all.sh

RUN echo "Compiling mkdcdisc to generate .cdi files..."
RUN git clone https://gitlab.com/simulant/mkdcdisc.git
RUN apk add ninja-build meson libisofs-dev
RUN cd mkdcdisc && meson setup builddir && meson compile -C builddir
RUN ./builddir/mkdcdisc -h || true

RUN echo "Add missing semicolon to bin2c.c..."
COPY bin2c.patch /opt/toolchains/dc/kos/utils/bin2c/
RUN patch /opt/toolchains/dc/kos/utils/bin2c/bin2c.c -l -p0 < /opt/toolchains/dc/kos/utils/bin2c/bin2c.patch
RUN cd /opt/toolchains/dc/kos/utils/bin2c && make clean && make

RUN echo "Building unpack_and_minify_mpq..."
RUN git clone https://github.com/diasurgical/devilutionx-mpq-tools/ && \
    cd devilutionx-mpq-tools && \
    cmake -S. -Bbuild-rel -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF && \
    cmake --build build-rel && \
    cmake --install build-rel

RUN echo "Cloning project again..."
WORKDIR /opt/toolchains/dc/kos/
RUN git clone -b dreamcast https://github.com/azihassan/devilutionX.git

#COPY init.patch /opt/toolchains/dc/kos/devilutionX
#RUN patch Source/init.cpp -l -p0 < init.patch
#COPY diablo.patch /opt/toolchains/dc/kos/devilutionX
#RUN patch Source/diablo.cpp -l -p0 < diablo.patch
#COPY main.patch /opt/toolchains/dc/kos/devilutionX
#RUN patch Source/main.cpp -l -p0 < main.patch
#COPY platforms.patch /opt/toolchains/dc/kos/devilutionX
#RUN patch CMake/Platforms.cmake -l -p0 < platforms.patch
#COPY file_util.patch /opt/toolchains/dc/kos/devilutionX
#RUN patch Source/utils/file_util.cpp -l -p0 < file_util.patch
#COPY sdl2_to_1_2_backports.patch /opt/toolchains/dc/kos/devilutionX
#RUN patch Source/utils/sdl2_to_1_2_backports.cpp -l -p0 < sdl2_to_1_2_backports.patch
#COPY paths.patch /opt/toolchains/dc/kos/devilutionX
#RUN patch Source/utils/paths.cpp -l -p0 < paths.patch
#COPY appfat.patch /opt/toolchains/dc/kos/devilutionX
#RUN patch Source/appfat.h -l -p0 < appfat.patch
#COPY CMake/platforms/dreamcast.cmake /opt/toolchains/dc/kos/devilutionX/CMake/platforms

WORKDIR /opt/toolchains/dc/kos/devilutionX
RUN echo "Injecting spawn.mpq..."
COPY spawn.mpq /opt/toolchains/dc/kos/devilutionX
RUN mkdir /opt/toolchains/dc/kos/devilutionX/data && \
    mv /opt/toolchains/dc/kos/devilutionX/spawn.mpq /opt/toolchains/dc/kos/devilutionX/data/
RUN cd /opt/toolchains/dc/kos/devilutionX/data/ && unpack_and_minify_mpq

RUN echo "Compiling..."
RUN source /opt/toolchains/dc/kos/environ.sh && \
    export CMAKE_PREFIX_PATH=/opt/toolchains/dc/kos-ports/libbz2/inst/:/opt/toolchains/dc/kos-ports/zlib/inst/ && \
    kos-cmake -S. -Bbuild
COPY libfmt-long-double.patch /opt/toolchains/dc/kos/devilutionX
#COPY locale.patch /opt/toolchains/dc/kos/devilutionX
RUN patch build/_deps/libfmt-src/include/fmt/format.h -l -p0 < libfmt-long-double.patch
#RUN patch Source/platform/locale.cpp -l -p0 < locale.patch

#COPY CMakeLists.patch /opt/toolchains/dc/kos/devilutionX
#RUN patch CMakeLists.txt -l -p0 < CMakeLists.patch

RUN source /opt/toolchains/dc/kos/environ.sh && cd build && kos-make

RUN cp -R data/spawn build/data/spawn

RUN echo "Generate CDI"
RUN /opt/toolchains/dc/mkdcdisc/builddir/mkdcdisc -e build/devilutionx.elf -o build/devilutionx.cdi --name 'Diablo 1' -d build/data/

ENTRYPOINT ["sh", "-c", "source /opt/toolchains/dc/kos/environ.sh && \"$@\"", "-s"]
