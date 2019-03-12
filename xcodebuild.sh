#!/bin/bash
cd ./3rdParty/libsodium/
chmod +x autogen.sh
sudo ./autogen.sh
sh ./dist-build/osx.sh

cd ../../

xcodebuild -workspace "./Xcode/devilutionX.xcworkspace" -scheme "devilutionX" build
