call VsDevCmd.bat

cd ..\..\uwp-project

makecert /n CN=devilutionX /r /h 0 /eku "1.3.6.1.5.5.7.3.3,1.3.6.1.4.1.311.10.3.13" /sv devilutionX_Certificate.pvk devilutionX_Certificate.cert

pvk2pfx /pvk devilutionX_Certificate.pvk /spc devilutionX_Certificate.cert /pfx devilutionX_TemporaryKey.pfx

mkdir ..\build
cd ..\build

git clone https://github.com/libsdl-org/SDL.git
git -C SDL checkout c7097418711b57e786eeb464bbe366c056b19801
msbuild /p:PlatformToolset=v143;TargetPlatformVersion=10.0.22000.0;TargetPlatformMinVersion=10.0.14393.0;ConfigurationType=StaticLibrary;Configuration=Release;Platform=x64 SDL\VisualC-WinRT\SDL-UWP.vcxproj

cmake -DUWP_LIB=1 -DUWP_SDL2_DIR="%CD%/SDL" -DCMAKE_BUILD_TYPE=x64-Release ..

msbuild /p:Configuration=Release;Platform=x64 /m DevilutionX.sln

powershell "Get-Content ..\uwp-project\Package.appxmanifest.template | %% {$_ -replace '__PROJECT_VERSION__',$(& {git describe --tags --abbrev=0})} | Out-File -FilePath ..\uwp-project\Package.appxmanifest -encoding ASCII"

msbuild /p:Configuration=Release;Platform=x64;AppxBundle=Always;AppxBundlePlatforms=x64 /m ..\uwp-project\devilutionx.sln

powershell "Get-Childitem -Path uwp-project\AppxPackages, uwp-project\Release -Include Microsoft.VCLibs.x64.*.appx, devilutionX_*_x64.appx -File -Recurse | Compress-Archive -DestinationPath ..\devilutionx.zip"
