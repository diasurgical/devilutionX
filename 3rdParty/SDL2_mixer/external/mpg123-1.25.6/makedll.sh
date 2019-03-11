#!/bin/sh
if test -e Makefile; then
	make clean
fi
options="$@"
echo "using options: $options"
CFLAGS="-march=i686" ./configure --disable-modules --with-cpu=x86_dither $options &&
cd src/libmpg123 &&
make &&
cp .libs/libmpg123-0.dll ../../ &&
cp .libs/libmpg123-0.dll.def ../../libmpg123-0.def
cd ../../ &&
echo "Now run that lib tool... perhaps you want to strip, too.
Hints:
	strip --strip-unneeded libmpg123-0.dll
	lib /machine:i386 /def:libmpg123-0.def" ||
echo You got some trouble.


