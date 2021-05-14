# workaround until we find the real way to change -llibpthreads to -llibpthread-psp
cp ${PSPDEV}/psp/lib/libpthread-psp.a ${PSPDEV}/psp/lib/libpthreads.a

cmake . -DPSP=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=${PSPDEV}/psp/share/cmake/PSP.cmake
make
