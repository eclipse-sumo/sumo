SUBDIRS = src unittest bin

EXTRA_DIST = README ChangeLog AUTHORS COPYING INSTALL NEWS sumo.doxyconf

doc:
	tools/build/makeDoc.py -o docs/sumo-user.html -p docs/sumo-user.pdf

dist-hook:
	abs_distdir="`pwd`/$(distdir)" \
	&& cd $(srcdir) \
	&& for f in `find build tools -name .svn -prune -o -type d -print`; do mkdir $$abs_distdir/$$f; done \
	&& find build -regex ".*\.\(vcproj\|vsprops\|sln\|spec\)" -print0 | xargs -0 cp --parents --target-directory $$abs_distdir \
	&& find tools -regex ".*\.\(pl\|py\|java\|jar\|bat\)" -print0 | xargs -0 cp --parents --target-directory $$abs_distdir \
	&& cp tools/traceExporter/.classpath tools/traceExporter/.project tools/traceExporter/manifest.mf $$abs_distdir/tools/traceExporter \
	&& if test -e tests/extractTest.py; then tests/extractTest.py -f tests/examples.txt -o $$abs_distdir; fi

dist-doc: doc
	cd $(srcdir)
	$(am__remove_distdir)
	mkdir $(distdir)
	cp --parents docs/*.pdf $(distdir)
	tar -czf $(PACKAGE)-doc-$(VERSION).tar.gz $(distdir)
	$(am__remove_distdir)

dist-tests:
	cd $(srcdir)
	$(am__remove_distdir)
	mkdir $(distdir)
	find tests -name .svn -prune -o -type f -print | xargs cp --parents -t $(distdir)
	tar -czf $(PACKAGE)-tests-$(VERSION).tar.gz $(distdir)
	$(am__remove_distdir)

dist-complete: dist dist-doc dist-tests
	mv $(PACKAGE)-$(VERSION).tar.gz $(PACKAGE)-src-$(VERSION).tar.gz
	mv $(PACKAGE)-$(VERSION).zip $(PACKAGE)-src-$(VERSION).zip

examples:
	tests/extractTest.py -f tests/examples.txt

sumo$(EXEEXT) netconvert$(EXEEXT) \
        netgen$(EXEEXT) duarouter$(EXEEXT) \
        dfrouter$(EXEEXT) jtrrouter$(EXEEXT) \
        od2trips$(EXEEXT) polyconvert$(EXEEXT) \
        sumo-gui$(EXEEXT):
	$(MAKE) -C src $@
