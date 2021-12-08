FROM amigadev/crosstools:m68k-amigaos

RUN apt-get install --no-install-recommends -y smpq
RUN mkdir /devilutionx-deps-build
COPY Packaging/amiga/prep.sh /devilutionx-deps-build/prep.sh
RUN cd /devilutionx-deps-build && ./prep.sh

CMD PKG_CONFIG_PATH=/opt/m68k-amigaos/usr/lib/pkgconfig/:/opt/m68k-amigaos/usr/share/pkgconfig/ \
		cmake -S. -Bbuild-amiga -DCMAKE_BUILD_TYPE=Release -DCPACK=ON -DM68K_CPU=68040 -DM68K_FPU=hard \
		-DM68K_COMMON="-s -ffast-math -O3 -noixemul -D__BIG_ENDIAN__ -D__AMIGA__ -fpermissive" && \
	cmake --build build-amiga -j $(nproc)
