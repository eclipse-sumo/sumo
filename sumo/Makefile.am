SUBDIRS = bin data docs src tests tools

AUTOMAKE_OPTIONS = gnu dist-zip #1.6

EXTRA_DIST = NEWS README ChangeLog AUTHORS COPYING INSTALL TODO \
sumo.doxyconf

dist-hook:
	cd $(srcdir)
	cp --parents `find prj/msvc6 -name "*.dsw" -o -name "*.dsp"` $(distdir)
	cp --parents `find prj -name "*.vcproj" -o -name "*.sln"` $(distdir)
