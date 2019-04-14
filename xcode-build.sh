#!/bin/bash

function get_libs {
    echo "============= Getting Libs ============="

    curl -O https://www.libsdl.org/release/SDL2-2.0.9.tar.gz
    curl -O https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-2.0.4.tar.gz
    curl -O https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-2.0.15.tar.gz
    curl -SLO https://download.savannah.gnu.org/releases/freetype/freetype-2.9.1.tar.gz
    curl -SLO https://github.com/glennrp/libpng/archive/v1.6.36.tar.gz
    curl -SLO https://github.com/jedisct1/libsodium/archive/1.0.17.tar.gz
}

function decompress_libs {
    echo "============= Unzip Libs ============="

    tar -zxvf SDL2-2.0.9.tar.gz
    tar -zxvf SDL2_mixer-2.0.4.tar.gz
    tar -zxvf SDL2_ttf-2.0.15.tar.gz
    tar -zxvf v1.6.36.tar.gz
    tar -zxvf freetype-2.9.1.tar.gz
    tar -zxvf 1.0.17.tar.gz
}

function build_sdl2 {
    echo "============= Build SDL2 ============="
    xcodebuild -project "SDL2-2.0.9/Xcode/SDL/SDL.xcodeproj" -scheme "Framework" build -configuration Release CONFIGURATION_BUILD_DIR="~/Library/Frameworks" ARCHS="i386 x86_64" ONLY_ACTIVE_ARCH=NO
    mkdir frameworks
    cp  -v -f -a ~/Library/Frameworks/SDL2.framework ./frameworks/
}

function build_sdl2_mixer {
    echo "============= Build SDL2_mixer ============="
    xcodebuild -project "SDL2_mixer-2.0.4/Xcode/SDL_mixer.xcodeproj" -scheme "Framework" build -configuration Release ARCHS="i386 x86_64" ONLY_ACTIVE_ARCH=NO -derivedDataPath "SDL2_mixer-2.0.4/Xcode/DerivedData/"
    cp  -v -f -a SDL2_mixer-2.0.4/Xcode/DerivedData/Build/Products/Release/SDL2_mixer.framework ./frameworks/
}

function build_libpng {
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
    cp  -v -f -a libpng-1.6.36/build/Release/libpng16.16.36.0.dylib ./frameworks/libpng16.16.dylib
}

function build_freetype {
    echo "============= Build Freetype ============="
    cd freetype-2.9.1/
    cmake -E make_directory build
    mkdir -p build/builds/mac
    cp  -v -f builds/mac/freetype-Info.plist build/builds/mac/freetype-Info.plist
    cmake -E chdir build cmake -G Xcode -D BUILD_FRAMEWORK:BOOL=true ..
    cd ../
    xcodebuild -project "freetype-2.9.1/build/freetype.xcodeproj" -scheme "ALL_BUILD" build -configuration Release ARCHS="i386 x86_64" ONLY_ACTIVE_ARCH=NO
}

function build_sdl2_ttf {
    echo "============= Build SDL2_ttf ============="
    rm -vr SDL2_ttf-2.0.15/Xcode/Frameworks/FreeType.framework
    cp  -v -f -a freetype-2.9.1/build/Release/freetype.framework SDL2_ttf-2.0.15/Xcode/Frameworks/FreeType.framework
    xcodebuild -project "SDL2_ttf-2.0.15/Xcode/SDL_ttf.xcodeproj" -scheme "Framework" build -configuration Release ARCHS="i386 x86_64" ONLY_ACTIVE_ARCH=NO  -derivedDataPath "SDL2_ttf-2.0.15/Xcode/DerivedData/"
    cp  -v -f -a SDL2_ttf-2.0.15/Xcode/DerivedData/Build/Products/Release/SDL2_ttf.framework ./frameworks/
}

function build_libsodium {
    echo "============= Build Libsodium ============="
    cp  -v -f ../3rdParty/libsodium/osxi386.sh libsodium-1.0.17/dist-build/osxi386.sh
    cd libsodium-1.0.17
    sudo ./autogen.sh
    ./dist-build/osxi386.sh
    sudo cp  -v -f -a libsodium-osx/lib/ /usr/local/lib/
    sudo cp  -v -f -a libsodium-osx/include/ /usr/local/include/

    cd ../../
}

function build_devilutionx {
    echo $PROJECT_PATH
    echo "============= Build DevilutionX ============="
    xcodebuild -project "./Xcode/devilutionX.xcodeproj" -scheme "devilutionX" build -configuration Release CONFIGURATION_BUILD_DIR="build"
}

function create_dmg {
    echo "============= Create DMG ============="
    mkdir build/devilutionX
    mv -v build/devilutionX.app build/devilutionX/devilutionX.app

    hdiutil create build/devilutionX_temp.dmg -ov -volname "devilutionX" -fs HFS+ -srcfolder "build/devilutionX"
    hdiutil convert build/devilutionX_temp.dmg -format UDZO -o build/devilutionX.dmg

    echo "============= Cleaning temps ============="
    rm build/devilutionX_temp.dmg
}

function set_working_dir() {
    if [ -d "./libs" ]; then
        cd libs
    elif [ -d "../libs" ]; then
        echo "Info: You're inside libs, maybe you should call this script at parent directory"
    else
        mkdir libs
        cd libs
    fi
}

function build_all_x86 (){
    brew bundle --file=Brewfile-32bit.rb
    set_working_dir
    get_libs
    decompress_libs
    build_sdl2
    build_sdl2_mixer
    build_libpng
    build_freetype
    build_sdl2_ttf
    build_libsodium
    build_devilutionx || { echo "** Build Project > `--build-project` command failed"; exit 1; }
    create_dmg
}

function build_all_x64 (){
    brew bundle
    mkdir build
    cd build
    cmake ..
    make -j$(sysctl -n hw.physicalcpu)
}

function main {
    for args in "$@"
    do
        case $args in
            "--get-libs")
                set_working_dir
                get_libs
                decompress_libs
            ;;
            "--build-libs")
                set_working_dir
                build_sdl2
                build_sdl2_mixer
                build_libpng
                build_freetype
                build_sdl2_ttf
                build_libsodium 
            ;;
           "--build-project")
                build_devilutionx || { echo "** Build Project > `--build-project` command failed"; exit 1; }
            ;;
           "--package")
                create_dmg
            ;;
            "--build-all-x64")
                build_all_x64
            ;;
            "--build-all-x86")
                build_all_x86
            ;;
            *)
                echo Unknown command
            ;;
        esac
    done
}

main "$@"
