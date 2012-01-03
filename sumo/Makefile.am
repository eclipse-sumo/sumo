SUBDIRS = src unittest bin

EXTRA_DIST = README ChangeLog AUTHORS COPYING INSTALL NEWS sumo.doxyconf

doc: pydoc doxygen wikidump

pydoc:
	rm -rf docs/pydoc
	mkdir docs/pydoc
	cd docs/pydoc \
	&& PYTHONPATH="../../tools" python -c "import pydoc, traci, sumolib; [pydoc.writedoc(m) for m in [traci, sumolib, sumolib.net] + traci._modules.values()]"

doxygen:
	rm -rf docs/doxygen
	mkdir docs/doxygen
	doxygen sumo.doxyconf &> doxygen.log

wikidump:
	cd docs/wiki && ../../tools/build/buildHTMLDocs.py && cp *.png *.css docs

dist-hook:
	abs_distdir="`pwd`/$(distdir)" \
	&& cd $(srcdir) \
	&& for f in `find build tools tests/complex/tutorial -name .svn -prune -o -type d -print`; do mkdir -p $$abs_distdir/$$f; done \
	&& find build -regex ".*\.\(vcproj\|vsprops\|vcxproj\|vcxproj\.filters\|props\|sln\|spec\)" -print0 | xargs -0 cp --parents --target-directory $$abs_distdir \
	&& ( svn export --force tools $(distdir)/tools && cd $(distdir) && find tools -type f > ../tools_filelist; true ) \
	&& xargs cp --parents --target-directory $$abs_distdir < tools_filelist \
	&& cp tools_filelist $$abs_distdir

dist-doc: doc
	cd $(srcdir)
	$(am__remove_distdir)
	mkdir -p $(distdir)/docs
	cp -r docs/wiki/docs $(distdir)/docs/userdoc
	find tests/complex/tutorial -regex ".*\.\(xml\|cfg\|py\)" -print0 | xargs -0 cp --parents --target-directory $(distdir)
	mv $(distdir)/tests/complex/tutorial $(distdir)/docs
	rmdir $(distdir)/tests/complex
	rmdir $(distdir)/tests
	svn export docs/internet/xsd $(distdir)/docs/xsd
	cp -r docs/pydoc docs/doxygen $(distdir)/docs
	tests/extractTest.py -i -f tests/examples.txt -o $(distdir)/docs
	tar -czf $(PACKAGE)-doc-$(VERSION).tar.gz $(distdir)
	rm -f $(PACKAGE)-doc-$(VERSION).zip
	zip -r $(PACKAGE)-doc-$(VERSION).zip $(distdir)
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
	tar -xzf $(PACKAGE)-doc-$(VERSION).tar.gz $(distdir)
	tar -xzf $(PACKAGE)-tests-$(VERSION).tar.gz $(distdir)
	tar -xzf $(PACKAGE)-src-$(VERSION).tar.gz $(distdir)
	tar -czf $(PACKAGE)-all-$(VERSION).tar.gz $(distdir)
	$(am__remove_distdir)

examples:
	tests/extractTest.py -f tests/examples.txt

sumo$(EXEEXT) netconvert$(EXEEXT) \
        netgen$(EXEEXT) duarouter$(EXEEXT) \
        dfrouter$(EXEEXT) jtrrouter$(EXEEXT) \
        od2trips$(EXEEXT) polyconvert$(EXEEXT) \
        sumo-gui$(EXEEXT):
	$(MAKE) -C src $@
