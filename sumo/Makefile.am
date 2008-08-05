SUBDIRS = src docs

EXTRA_DIST = README ChangeLog AUTHORS COPYING INSTALL NEWS sumo.doxyconf

doc:
	$(MAKE) -C docs $@

dist-hook:
	cd $(srcdir)
	find build -regex ".*\.\(vcproj\|vsprops\|sln\)" -print0 | xargs -0 cp --parents --target-directory $(distdir)
	find data -regex ".*\.\(cfg\|xml\)" -print0 | xargs -0 cp --parents --target-directory $(distdir)
	find tools -regex ".*\.\(pl\|py\|java\|jar\|bat\)" -print0 | xargs -0 cp --parents --target-directory $(distdir)
	cp tools/traceExporter/.classpath tools/traceExporter/.project tools/traceExporter/manifest.mf $(distdir)/tools/traceExporter

dist-doc: doc
	$(am__remove_distdir)
	mkdir $(distdir)
	cd $(srcdir)
	cp --parents docs/*.pdf $(distdir)
	tar -czf $(PACKAGE)-doc-$(VERSION).tar.gz $(distdir)
	$(am__remove_distdir)

dist-tests:
	$(am__remove_distdir)
	mkdir $(distdir)
	cd $(srcdir)
	find tests -name .svn -prune -o -type f -print | xargs cp --parents -t $(distdir)
	tar -czf $(PACKAGE)-tests-$(VERSION).tar.gz $(distdir)
	$(am__remove_distdir)

dist-complete: dist dist-doc dist-tests
	mv $(PACKAGE)-$(VERSION).tar.gz $(PACKAGE)-src-$(VERSION).tar.gz
	mv $(PACKAGE)-$(VERSION).zip $(PACKAGE)-src-$(VERSION).zip
