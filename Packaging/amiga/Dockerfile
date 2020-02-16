FROM amigadev/crosstools:m68k-amigaos

RUN mkdir /devilutionx-deps-build
COPY Packaging/amiga/prep.sh /devilutionx-deps-build/prep.sh
RUN cd /devilutionx-deps-build && ./prep.sh

CMD mkdir -p build-amiga && cd build-amiga && \
	PKG_CONFIG_PATH=/opt/m68k-amigaos/lib/pkgconfig/:/opt/m68k-amigaos/share/pkgconfig/ \
		cmake -DBINARY_RELEASE=ON -DM68K_CPU=68040 -DM68K_FPU=hard \
		-DM68K_COMMON="-s -ffast-math -O3 -noixemul -D__BIG_ENDIAN__ -D__AMIGA__ -fpermissive" .. && \
	cmake --build . -j $(nproc)
