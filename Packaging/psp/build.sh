mkdir -p ../../build
cd ../../build
psp-cmake .. -DPSPDEV=/usr/local/pspdev -DPSP=ON -DCMAKE_BUILD_TYPE=Release
make
