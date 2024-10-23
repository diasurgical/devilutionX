FROM alpine-kallistios:no-gdb

RUN echo "Building unpack_and_minify_mpq..."
RUN git clone https://github.com/diasurgical/devilutionx-mpq-tools/ && \
    cd devilutionx-mpq-tools && \
    cmake -S. -Bbuild-rel -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF && \
    cmake --build build-rel && \
    cmake --install build-rel

RUN echo "Cloning project..."
WORKDIR /opt/toolchains/dc/kos/
RUN git clone -b dreamcast https://github.com/azihassan/devilutionX.git

RUN echo "Uninstall kos-ports SDL 1.2..."
RUN source /opt/toolchains/dc/kos/environ.sh && \
    cd /opt/toolchains/dc/kos-ports/SDL && \
    make uninstall || echo 'SDL 1.2 uninstall finished with non zero status, proceding anyway'

RUN echo "Install GPF SDL 1.2..."
RUN git clone -b SDL-dreamhal--GLDC https://github.com/GPF/SDL-1.2 && \
    cd SDL-1.2 && \
    source /opt/toolchains/dc/kos/environ.sh && \
    make -f Makefile.dc && \
    cp /opt/toolchains/dc/kos/addons/lib/dreamcast/libSDL.a /usr/lib/ && \
    cp include/* /usr/include/SDL/

WORKDIR /opt/toolchains/dc/kos/devilutionX
RUN echo "Downloading and unpacking spawn.mpq..."
RUN curl -LO https://raw.githubusercontent.com/d07RiV/diabloweb/3a5a51e84d5dab3cfd4fef661c46977b091aaa9c/spawn.mpq && \
    unpack_and_minify_mpq spawn.mpq --output-dir data && \
    rm spawn.mpq

RUN echo "Downloading and unpacking fonts.mpq..."
RUN curl -LO https://github.com/diasurgical/devilutionx-assets/releases/download/v4/fonts.mpq && \
    unpack_and_minify_mpq fonts.mpq --output-dir data/fonts && \
    rm fonts.mpq

RUN echo "Configuring CMake..."
RUN source /opt/toolchains/dc/kos/environ.sh && \
    #uncomment when using packed save files
    #without this, cmake can't find the kos-ports bzip2 & zlib libraries
    #export CMAKE_PREFIX_PATH=/opt/toolchains/dc/kos-ports/libbz2/inst/:/opt/toolchains/dc/kos-ports/zlib/inst/ && \
    kos-cmake -S. -Bbuild

RUN echo "Compiling..."
RUN source /opt/toolchains/dc/kos/environ.sh && cd build && kos-make

RUN echo "Generating CDI"
RUN source /opt/toolchains/dc/kos/environ.sh && \
    mv data/spawn build/data/spawn && \
    mv data/fonts/fonts/fonts/ build/data/fonts && \
    mkdcdisc -e build/devilutionx.elf -o build/devilutionx.cdi --name 'Diablo 1' -d build/data/

ENTRYPOINT ["sh", "-c", "source /opt/toolchains/dc/kos/environ.sh && \"$@\"", "-s"]
