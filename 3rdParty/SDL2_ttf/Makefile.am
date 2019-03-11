# Makefile.am for the SDL truetype font loading library and viewer

lib_LTLIBRARIES = libSDL2_ttf.la

libSDL2_ttfincludedir = $(includedir)/SDL2
libSDL2_ttfinclude_HEADERS =	\
	SDL_ttf.h

libSDL2_ttf_la_SOURCES =		\
	SDL_ttf.c

EXTRA_DIST =			\
	Android.mk		\
	debian			\
	external		\
	version.rc		\
	VisualC			\
	VisualC-WinRT		\
	Xcode			\
	Xcode-iOS		\
	SDL2_ttf.spec		\
	SDL2_ttf.spec.in	\
	gcc-fat.sh		\
	autogen.sh		\
	$(srcdir)/*.txt

if USE_VERSION_RC
libSDL2_ttf_la_LDFLAGS = 	\
	-no-undefined		\
	-release $(LT_RELEASE)	\
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) -Wl,version.o
libSDL2_ttf_la_DEPENDENCIES = version.o
else
libSDL2_ttf_la_LDFLAGS = 	\
	-no-undefined		\
	-release $(LT_RELEASE)	\
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)
endif
libSDL2_ttf_la_LIBADD = @MATHLIB@

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = SDL2_ttf.pc

%.o : %.rc
	$(WINDRES) $< $@

noinst_PROGRAMS = showfont glfont

showfont_LDADD = libSDL2_ttf.la
glfont_LDADD = libSDL2_ttf.la @GL_LIBS@ @MATHLIB@

# Rule to build tar-gzipped distribution package
$(PACKAGE)-$(VERSION).tar.gz: distcheck

# Rule to build RPM distribution package
rpm: $(PACKAGE)-$(VERSION).tar.gz
	rpmbuild -ta $(PACKAGE)-$(VERSION).tar.gz

