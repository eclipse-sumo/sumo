SUBDIRS = bin docs src tools

AUTOMAKE_OPTIONS = gnu dist-zip #1.6

EXTRA_DIST = NEWS README ChangeLog AUTHORS COPYING INSTALL TODO \
sumo.doxyconf

dist-hook:
	cd $(srcdir)
	cp --parents `find prj/msvc6 -name "*.dsw" -o -name "*.dsp"` $(distdir)
	cp --parents `find prj -name "*.vcproj" -o -name "*.sln"` $(distdir)
	cp --parents `find data -regextype posix-awk -regex ".*\.(nod|edg|rou|add|def)\.xml"` $(distdir)
	cp --parents `find data -name "*.cfg"` $(distdir)
