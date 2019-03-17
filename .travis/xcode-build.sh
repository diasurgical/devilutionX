#!/bin/bash

echo "============= Getting Libs ============="

mkdir libs

cd libs

curl -O https://www.libsdl.org/release/SDL2-2.0.9.zip
curl -O https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-2.0.4.zip
curl -O https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-2.0.15.zip
curl -SLO https://download.savannah.gnu.org/releases/freetype/freetype-2.9.1.tar.gz
curl -SLO https://github.com/glennrp/libpng/archive/v1.6.36.zip
curl -SLO https://github.com/jedisct1/libsodium/archive/1.0.17.zip

echo "============= Unzip Libs ============="

unzip SDL2-2.0.9.zip
unzip SDL2_mixer-2.0.4.zip
unzip SDL2_ttf-2.0.15.zip
unzip v1.6.36.zip
tar -zxvf freetype-2.9.1.tar.gz
unzip 1.0.17.zip

echo "============= Build SDL2 ============="
xcodebuild -project "SDL2-2.0.9/Xcode/SDL/SDL.xcodeproj" -scheme "Framework" build -configuration Release CONFIGURATION_BUILD_DIR="~/Library/Frameworks" ARCHS="i386 x86_64" ONLY_ACTIVE_ARCH=NO

echo "============= Build SDL2_mixer ============="
xcodebuild -project "SDL2_mixer-2.0.4/Xcode/SDL_mixer.xcodeproj" -scheme "Framework" build -configuration Release ARCHS="i386 x86_64" ONLY_ACTIVE_ARCH=NO -derivedDataPath "SDL2_mixer-2.0.4/Xcode/DerivedData/"
cp  -v -f -a SDL2_mixer-2.0.4/Xcode/DerivedData/Build/Products/Release/SDL2_mixer.framework ~/Library/Frameworks

echo "============= Build libpng ============="
mkdir libpng-1.6.36/build
cd libpng-1.6.36/build
cmake .. -G"Xcode"
cd ../../
xcodebuild -project "libpng-1.6.36/build/libpng.xcodeproj" -scheme "ALL_BUILD" build -configuration Release ARCHS="i386 x86_64" ONLY_ACTIVE_ARCH=NO
sudo mv /usr/local/lib/libpng16.16.dylib /usr/local/lib/libpng16.16_o.dylib
sudo cp  -v -f -a libpng-1.6.36/build/Release/libpng16.16.36.0.dylib /usr/local/lib/libpng16.16.dylib
sudo rm /usr/local/lib/libpng.dylib
sudo ln -s /usr/local/lib/libpng16.16.dylib /usr/local/lib/libpng.dylib

echo "============= Build Freetype ============="
cd freetype-2.9.1/
cmake -E make_directory build
mkdir -p build/builds/mac
cp  -v -f builds/mac/freetype-Info.plist build/builds/mac/freetype-Info.plist
cmake -E chdir build cmake -G Xcode -D BUILD_FRAMEWORK:BOOL=true ..
cd ../
xcodebuild -project "freetype-2.9.1/build/freetype.xcodeproj" -scheme "ALL_BUILD" build -configuration Release ARCHS="i386 x86_64" ONLY_ACTIVE_ARCH=NO

echo "============= Build SDL2_ttf ============="
rm -vr SDL2_ttf-2.0.15/Xcode/Frameworks/FreeType.framework
cp  -v -f -a freetype-2.9.1/build/Release/freetype.framework SDL2_ttf-2.0.15/Xcode/Frameworks/FreeType.framework
xcodebuild -project "SDL2_ttf-2.0.15/Xcode/SDL_ttf.xcodeproj" -scheme "Framework" build -configuration Release ARCHS="i386 x86_64" ONLY_ACTIVE_ARCH=NO  -derivedDataPath "SDL2_ttf-2.0.15/Xcode/DerivedData/"
cp  -v -f -a SDL2_ttf-2.0.15/Xcode/DerivedData/Build/Products/Release/SDL2_ttf.framework ~/Library/Frameworks

echo "============= Build Libsodium ============="
cp  -v -f  -v -f ../3rdParty/libsodium/osxi386.sh libsodium-1.0.17/dist-build/osxi386.sh
cd libsodium-1.0.17
sudo ./autogen.sh
./dist-build/osxi386.sh
sudo cp  -v -f -a libsodium-osx/lib/ /usr/local/lib/
sudo cp  -v -f -a libsodium-osx/include/ /usr/local/include/

cd ../..

echo "============= Build DevilutionX ============="
xcodebuild -project "./Xcode/devilutionX.xcodeproj" -scheme "devilutionX" build -configuration Release CONFIGURATION_BUILD_DIR="build"

echo "============= Create DMG ============="
mkdir build/devilutionX
mv -v build/devilutionX.app build/devilutionX/devilutionX.app

hdiutil create build/devilutionX_temp.dmg -ov -volname "devilutionX" -fs HFS+ -srcfolder "build/devilutionX"
hdiutil convert build/devilutionX_temp.dmg -format UDZO -o build/devilutionX.dmg

echo "============= Cleaning temps ============="
rm build/devilutionX_temp.dmg
