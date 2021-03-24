make install -Cbuild DESTDIR=AppDir
mv build/AppDir/usr/share/diasurgical/devilutionx/devilutionx.mpq build/AppDir/usr/bin/devilutionx.mpq
wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
chmod +x linuxdeploy-x86_64.AppImage
./linuxdeploy-x86_64.AppImage --appimage-extract-and-run --appdir=build/AppDir --custom-apprun=Packaging/nix/AppRun --output appimage
mv DevilutionX*.AppImage devilutionx.appimage
