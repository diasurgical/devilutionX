mkdir ./build/package
find build/_CPack_Packages/Linux/7Z/ -name 'devilutionx' -exec cp "{}" ./build/devilutionx \;
cp ./build/devilutionx ./build/package/devilutionx
cp ./Packaging/resources/devilutionx.mpq ./build/package/devilutionx.mpq
cp ./build/devilutionx*.deb ./build/package/devilutionx.deb
cp ./build/devilutionx*.rpm ./build/package/devilutionx.rpm
cp ./Packaging/nix/README.txt ./build/package/README.txt
cp ./Packaging/resources/LICENSE.CC-BY.txt ./build/package/LICENSE.CC-BY.txt
cp ./Packaging/resources/LICENSE.OFL.txt ./build/package/LICENSE.OFL.txt
cd ./build/package/ && tar -cavf ../../devilutionx.tar.xz * && cd ../../
