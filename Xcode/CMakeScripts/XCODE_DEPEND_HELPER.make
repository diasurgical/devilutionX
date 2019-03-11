# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
PostBuild.PKWare.Debug:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Debug/libPKWare.a:
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Debug/libPKWare.a


PostBuild.Radon.Debug:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Debug/libRadon.a:
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Debug/libRadon.a


PostBuild.StormLib.Debug:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Debug/libStormLib.a:
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Debug/libStormLib.a


PostBuild.devilution.Debug:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Debug/libdevilution.a:
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Debug/libdevilution.a


PostBuild.devilutionx.Debug:
PostBuild.devilution.Debug: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Debug/devilutionx
PostBuild.StormLib.Debug: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Debug/devilutionx
PostBuild.smacker.Debug: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Debug/devilutionx
PostBuild.Radon.Debug: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Debug/devilutionx
PostBuild.PKWare.Debug: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Debug/devilutionx
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Debug/devilutionx:\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Debug/libdevilution.a\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Debug/libStormLib.a\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Debug/libsmacker.a\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Debug/libRadon.a\
	/usr/local/lib/libSDL2_ttf.dylib\
	/usr/local/lib/libSDL2_mixer.dylib\
	/usr/local/Cellar/libsodium/1.0.17/lib/libsodium.dylib\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Debug/libPKWare.a\
	/Users/bennyfrancodennis/Library/Frameworks/SDL2.framework/SDL2
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Debug/devilutionx


PostBuild.smacker.Debug:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Debug/libsmacker.a:
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Debug/libsmacker.a


PostBuild.PKWare.Release:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Release/libPKWare.a:
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Release/libPKWare.a


PostBuild.Radon.Release:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Release/libRadon.a:
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Release/libRadon.a


PostBuild.StormLib.Release:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Release/libStormLib.a:
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Release/libStormLib.a


PostBuild.devilution.Release:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Release/libdevilution.a:
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Release/libdevilution.a


PostBuild.devilutionx.Release:
PostBuild.devilution.Release: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Release/devilutionx
PostBuild.StormLib.Release: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Release/devilutionx
PostBuild.smacker.Release: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Release/devilutionx
PostBuild.Radon.Release: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Release/devilutionx
PostBuild.PKWare.Release: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Release/devilutionx
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Release/devilutionx:\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Release/libdevilution.a\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Release/libStormLib.a\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Release/libsmacker.a\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Release/libRadon.a\
	/usr/local/lib/libSDL2_ttf.dylib\
	/usr/local/lib/libSDL2_mixer.dylib\
	/usr/local/Cellar/libsodium/1.0.17/lib/libsodium.dylib\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Release/libPKWare.a\
	/Users/bennyfrancodennis/Library/Frameworks/SDL2.framework/SDL2
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Release/devilutionx


PostBuild.smacker.Release:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Release/libsmacker.a:
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Release/libsmacker.a


PostBuild.PKWare.MinSizeRel:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/MinSizeRel/libPKWare.a:
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/MinSizeRel/libPKWare.a


PostBuild.Radon.MinSizeRel:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/MinSizeRel/libRadon.a:
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/MinSizeRel/libRadon.a


PostBuild.StormLib.MinSizeRel:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/MinSizeRel/libStormLib.a:
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/MinSizeRel/libStormLib.a


PostBuild.devilution.MinSizeRel:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/MinSizeRel/libdevilution.a:
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/MinSizeRel/libdevilution.a


PostBuild.devilutionx.MinSizeRel:
PostBuild.devilution.MinSizeRel: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/MinSizeRel/devilutionx
PostBuild.StormLib.MinSizeRel: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/MinSizeRel/devilutionx
PostBuild.smacker.MinSizeRel: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/MinSizeRel/devilutionx
PostBuild.Radon.MinSizeRel: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/MinSizeRel/devilutionx
PostBuild.PKWare.MinSizeRel: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/MinSizeRel/devilutionx
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/MinSizeRel/devilutionx:\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/MinSizeRel/libdevilution.a\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/MinSizeRel/libStormLib.a\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/MinSizeRel/libsmacker.a\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/MinSizeRel/libRadon.a\
	/usr/local/lib/libSDL2_ttf.dylib\
	/usr/local/lib/libSDL2_mixer.dylib\
	/usr/local/Cellar/libsodium/1.0.17/lib/libsodium.dylib\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/MinSizeRel/libPKWare.a\
	/Users/bennyfrancodennis/Library/Frameworks/SDL2.framework/SDL2
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/MinSizeRel/devilutionx


PostBuild.smacker.MinSizeRel:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/MinSizeRel/libsmacker.a:
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/MinSizeRel/libsmacker.a


PostBuild.PKWare.RelWithDebInfo:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/RelWithDebInfo/libPKWare.a:
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/RelWithDebInfo/libPKWare.a


PostBuild.Radon.RelWithDebInfo:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/RelWithDebInfo/libRadon.a:
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/RelWithDebInfo/libRadon.a


PostBuild.StormLib.RelWithDebInfo:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/RelWithDebInfo/libStormLib.a:
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/RelWithDebInfo/libStormLib.a


PostBuild.devilution.RelWithDebInfo:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/RelWithDebInfo/libdevilution.a:
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/RelWithDebInfo/libdevilution.a


PostBuild.devilutionx.RelWithDebInfo:
PostBuild.devilution.RelWithDebInfo: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/RelWithDebInfo/devilutionx
PostBuild.StormLib.RelWithDebInfo: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/RelWithDebInfo/devilutionx
PostBuild.smacker.RelWithDebInfo: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/RelWithDebInfo/devilutionx
PostBuild.Radon.RelWithDebInfo: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/RelWithDebInfo/devilutionx
PostBuild.PKWare.RelWithDebInfo: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/RelWithDebInfo/devilutionx
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/RelWithDebInfo/devilutionx:\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/RelWithDebInfo/libdevilution.a\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/RelWithDebInfo/libStormLib.a\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/RelWithDebInfo/libsmacker.a\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/RelWithDebInfo/libRadon.a\
	/usr/local/lib/libSDL2_ttf.dylib\
	/usr/local/lib/libSDL2_mixer.dylib\
	/usr/local/Cellar/libsodium/1.0.17/lib/libsodium.dylib\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/RelWithDebInfo/libPKWare.a\
	/Users/bennyfrancodennis/Library/Frameworks/SDL2.framework/SDL2
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/RelWithDebInfo/devilutionx


PostBuild.smacker.RelWithDebInfo:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/RelWithDebInfo/libsmacker.a:
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/RelWithDebInfo/libsmacker.a




# For each target create a dummy ruleso the target does not have to exist
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Debug/libPKWare.a:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Debug/libRadon.a:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Debug/libStormLib.a:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Debug/libdevilution.a:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Debug/libsmacker.a:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/MinSizeRel/libPKWare.a:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/MinSizeRel/libRadon.a:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/MinSizeRel/libStormLib.a:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/MinSizeRel/libdevilution.a:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/MinSizeRel/libsmacker.a:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/RelWithDebInfo/libPKWare.a:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/RelWithDebInfo/libRadon.a:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/RelWithDebInfo/libStormLib.a:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/RelWithDebInfo/libdevilution.a:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/RelWithDebInfo/libsmacker.a:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Release/libPKWare.a:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Release/libRadon.a:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Release/libStormLib.a:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Release/libdevilution.a:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/build/Release/libsmacker.a:
/Users/bennyfrancodennis/Library/Frameworks/SDL2.framework/SDL2:
/usr/local/Cellar/libsodium/1.0.17/lib/libsodium.dylib:
/usr/local/lib/libSDL2_mixer.dylib:
/usr/local/lib/libSDL2_ttf.dylib:
