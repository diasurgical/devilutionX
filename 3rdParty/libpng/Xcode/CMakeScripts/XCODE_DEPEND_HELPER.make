# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
PostBuild.png.Release:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Release/libpng16.dylib:\
	/usr/lib/libz.dylib\
	/usr/lib/libm.dylib
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Release/libpng16.dylib


PostBuild.png-fix-itxt.Release:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Release/png-fix-itxt:\
	/usr/lib/libz.dylib\
	/usr/lib/libm.dylib
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Release/png-fix-itxt


PostBuild.png_static.Release:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Release/libpng16.a:
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Release/libpng16.a


PostBuild.pngfix.Release:
PostBuild.png.Release: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Release/pngfix
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Release/pngfix:\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Release/libpng16.16.35.0.dylib\
	/usr/lib/libz.dylib\
	/usr/lib/libm.dylib
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Release/pngfix


PostBuild.pngimage.Release:
PostBuild.png.Release: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Release/pngimage
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Release/pngimage:\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Release/libpng16.16.35.0.dylib\
	/usr/lib/libz.dylib\
	/usr/lib/libm.dylib
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Release/pngimage


PostBuild.pngstest.Release:
PostBuild.png.Release: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Release/pngstest
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Release/pngstest:\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Release/libpng16.16.35.0.dylib\
	/usr/lib/libz.dylib\
	/usr/lib/libm.dylib
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Release/pngstest


PostBuild.pngtest.Release:
PostBuild.png.Release: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Release/pngtest
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Release/pngtest:\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Release/libpng16.16.35.0.dylib\
	/usr/lib/libz.dylib\
	/usr/lib/libm.dylib
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Release/pngtest


PostBuild.pngunknown.Release:
PostBuild.png.Release: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Release/pngunknown
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Release/pngunknown:\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Release/libpng16.16.35.0.dylib\
	/usr/lib/libz.dylib\
	/usr/lib/libm.dylib
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Release/pngunknown


PostBuild.pngvalid.Release:
PostBuild.png.Release: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Release/pngvalid
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Release/pngvalid:\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Release/libpng16.16.35.0.dylib\
	/usr/lib/libz.dylib\
	/usr/lib/libm.dylib
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Release/pngvalid


PostBuild.png.Debug:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Debug/libpng16d.dylib:\
	/usr/lib/libz.dylib\
	/usr/lib/libm.dylib
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Debug/libpng16d.dylib


PostBuild.png-fix-itxt.Debug:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Debug/png-fix-itxt:\
	/usr/lib/libz.dylib\
	/usr/lib/libm.dylib
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Debug/png-fix-itxt


PostBuild.png_static.Debug:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Debug/libpng16d.a:
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Debug/libpng16d.a


PostBuild.pngfix.Debug:
PostBuild.png.Debug: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Debug/pngfix
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Debug/pngfix:\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Debug/libpng16d.16.35.0.dylib\
	/usr/lib/libz.dylib\
	/usr/lib/libm.dylib
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Debug/pngfix


PostBuild.pngimage.Debug:
PostBuild.png.Debug: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Debug/pngimage
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Debug/pngimage:\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Debug/libpng16d.16.35.0.dylib\
	/usr/lib/libz.dylib\
	/usr/lib/libm.dylib
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Debug/pngimage


PostBuild.pngstest.Debug:
PostBuild.png.Debug: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Debug/pngstest
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Debug/pngstest:\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Debug/libpng16d.16.35.0.dylib\
	/usr/lib/libz.dylib\
	/usr/lib/libm.dylib
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Debug/pngstest


PostBuild.pngtest.Debug:
PostBuild.png.Debug: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Debug/pngtest
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Debug/pngtest:\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Debug/libpng16d.16.35.0.dylib\
	/usr/lib/libz.dylib\
	/usr/lib/libm.dylib
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Debug/pngtest


PostBuild.pngunknown.Debug:
PostBuild.png.Debug: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Debug/pngunknown
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Debug/pngunknown:\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Debug/libpng16d.16.35.0.dylib\
	/usr/lib/libz.dylib\
	/usr/lib/libm.dylib
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Debug/pngunknown


PostBuild.pngvalid.Debug:
PostBuild.png.Debug: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Debug/pngvalid
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Debug/pngvalid:\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Debug/libpng16d.16.35.0.dylib\
	/usr/lib/libz.dylib\
	/usr/lib/libm.dylib
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Debug/pngvalid


PostBuild.png.MinSizeRel:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/MinSizeRel/libpng16.dylib:\
	/usr/lib/libz.dylib\
	/usr/lib/libm.dylib
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/MinSizeRel/libpng16.dylib


PostBuild.png-fix-itxt.MinSizeRel:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/MinSizeRel/png-fix-itxt:\
	/usr/lib/libz.dylib\
	/usr/lib/libm.dylib
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/MinSizeRel/png-fix-itxt


PostBuild.png_static.MinSizeRel:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/MinSizeRel/libpng16.a:
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/MinSizeRel/libpng16.a


PostBuild.pngfix.MinSizeRel:
PostBuild.png.MinSizeRel: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/MinSizeRel/pngfix
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/MinSizeRel/pngfix:\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/MinSizeRel/libpng16.16.35.0.dylib\
	/usr/lib/libz.dylib\
	/usr/lib/libm.dylib
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/MinSizeRel/pngfix


PostBuild.pngimage.MinSizeRel:
PostBuild.png.MinSizeRel: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/MinSizeRel/pngimage
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/MinSizeRel/pngimage:\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/MinSizeRel/libpng16.16.35.0.dylib\
	/usr/lib/libz.dylib\
	/usr/lib/libm.dylib
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/MinSizeRel/pngimage


PostBuild.pngstest.MinSizeRel:
PostBuild.png.MinSizeRel: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/MinSizeRel/pngstest
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/MinSizeRel/pngstest:\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/MinSizeRel/libpng16.16.35.0.dylib\
	/usr/lib/libz.dylib\
	/usr/lib/libm.dylib
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/MinSizeRel/pngstest


PostBuild.pngtest.MinSizeRel:
PostBuild.png.MinSizeRel: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/MinSizeRel/pngtest
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/MinSizeRel/pngtest:\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/MinSizeRel/libpng16.16.35.0.dylib\
	/usr/lib/libz.dylib\
	/usr/lib/libm.dylib
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/MinSizeRel/pngtest


PostBuild.pngunknown.MinSizeRel:
PostBuild.png.MinSizeRel: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/MinSizeRel/pngunknown
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/MinSizeRel/pngunknown:\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/MinSizeRel/libpng16.16.35.0.dylib\
	/usr/lib/libz.dylib\
	/usr/lib/libm.dylib
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/MinSizeRel/pngunknown


PostBuild.pngvalid.MinSizeRel:
PostBuild.png.MinSizeRel: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/MinSizeRel/pngvalid
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/MinSizeRel/pngvalid:\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/MinSizeRel/libpng16.16.35.0.dylib\
	/usr/lib/libz.dylib\
	/usr/lib/libm.dylib
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/MinSizeRel/pngvalid


PostBuild.png.RelWithDebInfo:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/RelWithDebInfo/libpng16.dylib:\
	/usr/lib/libz.dylib\
	/usr/lib/libm.dylib
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/RelWithDebInfo/libpng16.dylib


PostBuild.png-fix-itxt.RelWithDebInfo:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/RelWithDebInfo/png-fix-itxt:\
	/usr/lib/libz.dylib\
	/usr/lib/libm.dylib
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/RelWithDebInfo/png-fix-itxt


PostBuild.png_static.RelWithDebInfo:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/RelWithDebInfo/libpng16.a:
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/RelWithDebInfo/libpng16.a


PostBuild.pngfix.RelWithDebInfo:
PostBuild.png.RelWithDebInfo: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/RelWithDebInfo/pngfix
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/RelWithDebInfo/pngfix:\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/RelWithDebInfo/libpng16.16.35.0.dylib\
	/usr/lib/libz.dylib\
	/usr/lib/libm.dylib
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/RelWithDebInfo/pngfix


PostBuild.pngimage.RelWithDebInfo:
PostBuild.png.RelWithDebInfo: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/RelWithDebInfo/pngimage
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/RelWithDebInfo/pngimage:\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/RelWithDebInfo/libpng16.16.35.0.dylib\
	/usr/lib/libz.dylib\
	/usr/lib/libm.dylib
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/RelWithDebInfo/pngimage


PostBuild.pngstest.RelWithDebInfo:
PostBuild.png.RelWithDebInfo: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/RelWithDebInfo/pngstest
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/RelWithDebInfo/pngstest:\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/RelWithDebInfo/libpng16.16.35.0.dylib\
	/usr/lib/libz.dylib\
	/usr/lib/libm.dylib
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/RelWithDebInfo/pngstest


PostBuild.pngtest.RelWithDebInfo:
PostBuild.png.RelWithDebInfo: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/RelWithDebInfo/pngtest
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/RelWithDebInfo/pngtest:\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/RelWithDebInfo/libpng16.16.35.0.dylib\
	/usr/lib/libz.dylib\
	/usr/lib/libm.dylib
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/RelWithDebInfo/pngtest


PostBuild.pngunknown.RelWithDebInfo:
PostBuild.png.RelWithDebInfo: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/RelWithDebInfo/pngunknown
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/RelWithDebInfo/pngunknown:\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/RelWithDebInfo/libpng16.16.35.0.dylib\
	/usr/lib/libz.dylib\
	/usr/lib/libm.dylib
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/RelWithDebInfo/pngunknown


PostBuild.pngvalid.RelWithDebInfo:
PostBuild.png.RelWithDebInfo: /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/RelWithDebInfo/pngvalid
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/RelWithDebInfo/pngvalid:\
	/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/RelWithDebInfo/libpng16.16.35.0.dylib\
	/usr/lib/libz.dylib\
	/usr/lib/libm.dylib
	/bin/rm -f /Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/RelWithDebInfo/pngvalid




# For each target create a dummy ruleso the target does not have to exist
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Debug/libpng16d.16.35.0.dylib:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/MinSizeRel/libpng16.16.35.0.dylib:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/RelWithDebInfo/libpng16.16.35.0.dylib:
/Users/bennyfrancodennis/Developer/OpenSource/devilutionX/3rdParty/libpng/Xcode/Release/libpng16.16.35.0.dylib:
/usr/lib/libm.dylib:
/usr/lib/libz.dylib:
