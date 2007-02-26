SUBDIRS = docs src

AUTOMAKE_OPTIONS = gnu dist-zip #1.6

EXTRA_DIST = README ChangeLog AUTHORS COPYING INSTALL NEWS sumo.doxyconf

dist-hook:
	cd $(srcdir)
	cp --parents `find build/msvc6 -name "*.dsw" -o -name "*.dsp"` $(distdir)
	cp --parents `find build -name "*.vcproj" -o -name "*.sln"` $(distdir)
	cp --parents `find data tests -regextype posix-awk -regex ".*\.(nod|edg|rou|add|def)\.xml"` $(distdir)
	cp --parents `find data tests -name "*.cfg"` $(distdir)
	cp --parents `find data tests -regextype posix-awk -name .svn -prune -o -regex ".*/(errors|output|options|testsuite)\..*" -print` $(distdir)
	cp --parents `find tools -regextype posix-awk -regex ".*\.(pl|py|java|jar)"` $(distdir)
	cp tools/traceExporter/.classpath tools/traceExporter/.project tools/traceExporter/manifest.mf $(distdir)/tools/traceExporter
	cp --parents `find docs -name .svn -prune -or -type f -print` $(distdir)

