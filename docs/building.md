# Building from Source

Note: If you do not use git to manage the source you must provide the version to CMake manually:
```bash
cmake .. -DVERSION_NUM=1.0.0 -DVERSION_SUFFIX=FFFFFFF -DCMAKE_BUILD_TYPE=Release
```

<details><summary>Windows via Visual Studio</summary>

### Installing dependencies
Make sure to install the `C++ CMake tools for Windows` component for Visual Studio.

1. Install vcpkg following the instructions from https://github.com/microsoft/vcpkg#quick-start.

   Don't forget to perform _user-wide integration_ step for additional convenience.
2. Install required dependencies by executing the following command (via cmd or powershell):

   For the 64-bit version of the dependencies please run this command:

   ```
   vcpkg install fmt:x64-windows sdl2:x64-windows sdl2-ttf:x64-windows libsodium:x64-windows libpng:x64-windows gtest:x64-windows
   ```

   For the 32-bit version of the dependencies please run this command:

   ```
   vcpkg install fmt:x86-windows sdl2:x86-windows sdl2-ttf:x86-windows libsodium:x86-windows libpng:x86-windows gtest:x86-windows
   ```

### Compiling

* **Through Open->CMake in Visual Studio**
1. Go to `File -> Open -> CMake`, select `CMakeLists.txt` from the project root.
2. Select `Build devilution.exe` from the `Build` menu.

* **Through cmake-gui**

1. Input the path to devilutionx source directory at `Where is the source code:` field.
2. Input the path where the binaries would be placed at `Where to build the binaries:` field. If you want to place them inside source directory it's preferable to do so inside directory called `build` to avoid the binaries being added to the source tree.
3. It's recommended to input `Win32` in `Optional Platform for Generator`, otherwise it will default to x64 build.
4. In case you're using `vcpkg` select `Specify toolchain file for cross-compiling` and select the file `scripts/buildsystems/vcpkg.cmake` from `vcpkg` directory otherwise just go with `Use default native compilers`.
5. In case you need to select any paths to dependencies manually do this right in cmake-gui window.
6. Press `Generate` and open produced `.sln` file using Visual Studio.
7. Use build/debug etc. commands inside Visual Studio Solution like with any normal Visual Studio project.
</details>

<details><summary><b>CMake build options</b></summary>

### General
- `-DCMAKE_BUILD_TYPE=Release` changed build type to release and optimize for distribution.
- `-DNONET=ON` disable network support, this also removes the need for the ASIO and Sodium.
- `-DCMAKE_TOOLCHAIN_FILE=../CMake/32bit.cmake` generate 32bit builds on 64bit platforms.

### Debug builds
- `-DDEBUG=OFF` disable debug mode of the Diablo engine.
- `-DASAN=OFF` disable address sanitizer.
- `-DUBSAN=OFF` disable undefined behavior sanitizer.

</details>
