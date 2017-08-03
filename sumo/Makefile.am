SUBDIRS = src unittest bin

EXTRA_DIST = README ChangeLog AUTHORS COPYING INSTALL NEWS sumo.doxyconf

doc: pydoc doxygen userdoc javadoc

pydoc:
	rm -rf docs/pydoc
	mkdir docs/pydoc
	cd docs/pydoc && \
	for i in `find ../../tools/traci ../../tools/sumolib -name "*.py" -not -executable | sed 's,../../tools/,,;s,/,.,g;s,.py,,;s,.__init__,,'`; do \
	    PYTHONPATH="../../tools" python -c "import $$i, pydoc; pydoc.writedoc($$i)"; \
	done

doxygen:
	rm -rf docs/doxygen
	mkdir docs/doxygen
	doxygen sumo.doxyconf &> doxygen.log

lcov:
	rm -rf docs/lcov
	mkdir docs/lcov
	lcov -d . -c --no-external --output-file docs/lcov/lcov.info
	genhtml -o docs/lcov/html docs/lcov/lcov.info

lcov-reset:
	lcov -d . -z

userdoc:
	rm -rf docs/userdoc
	tools/build/buildHTMLDocs.py -o docs/userdoc -i docs/wiki/index.html -r $(VERSION) && cp docs/wiki/*.png docs/wiki/*.css docs/userdoc

javadoc:
	rm -rf docs/javadoc
	mkdir docs/javadoc
	mvn --batch-mode -f tools/contributed/traci4j/pom.xml javadoc:javadoc
	mv tools/contributed/traci4j/target/site/apidocs docs/javadoc/traci4j
	ant -f tools/contributed/traas/build.xml javadoc
	mv tools/contributed/traas/javadoc docs/javadoc/traas

man:
	rm -rf docs/man
	mkdir docs/man
	help2man -N -n "A microscopic road traffic simulation" bin/sumo > docs/man/sumo.1
	help2man -N -n "GUI version of the simulation SUMO" bin/sumo-gui > docs/man/sumo-gui.1
	help2man -N -n "Builds vehicle routes for SUMO using detector values" bin/dfrouter > docs/man/dfrouter.1
	help2man -N -n "Shortest path router and DUE computer for the microscopic road traffic simulation SUMO" bin/duarouter > docs/man/duarouter.1
	help2man -N -n "Router for the microscopic road traffic simulation SUMO based on junction turning ratios" bin/jtrrouter > docs/man/jtrrouter.1
	help2man -N -n "Import O/D-matrices for macroscopic traffic assignment" bin/marouter > docs/man/marouter.1
	help2man -N -n "Generates routes of persons throughout a day for the microscopic road traffic simulation SUMO" bin/activitygen > docs/man/activitygen.1
	help2man -N -n "Importer of O/D-matrices for the road traffic simulation SUMO" bin/od2trips > docs/man/od2trips.1
	help2man -N -n "Road network importer / builder for the road traffic simulation SUMO" bin/netconvert > docs/man/netconvert.1
	help2man -N -n "Road network editor for the road traffic simulation SUMO" bin/netedit > docs/man/netedit.1
	help2man -N -n "Road network generator for the microscopic road traffic simulation SUMO" bin/netgenerate > docs/man/netgenerate.1
	help2man -N -n "Importer of polygons and POIs for the road traffic simulation SUMO" bin/polyconvert > docs/man/polyconvert.1
	help2man -N -n "TraCITestClient for the road traffic simulation SUMO" bin/TraCITestClient > docs/man/TraCITestClient.1

dist-hook: abs_distdir=`pwd`/$(distdir)
dist-hook:
	cd $(srcdir) ; \
	if test -d tests; then \
	    for f in `find build data tools tests/complex/tutorial -name .svn -prune -o -type d -print`; do mkdir -p $(abs_distdir)/$$f; done ; \
	    mkdir -p $(abs_distdir)/docs ; \
	    find tests/complex/tutorial -regex ".*\.\(xml\|txt\|sumocfg\|netccfg\|py\|osm\)" -print0 | xargs -0 cp -v --parents --target-directory $(abs_distdir) ; \
	    mv -v ${abs_distdir}/tests/complex/tutorial $(abs_distdir)/docs ; \
	    rm -rf $(abs_distdir)/tests ; \
	    tools/extractTest.py -i -f tests/examples.txt -o $(abs_distdir)/docs ; \
	    find build -regex ".*\.\(vcxproj\|vcxproj\.filters\|props\|sln\|spec\)" -print0 | xargs -0 cp --parents --target-directory $(abs_distdir) ; \
	    find data -regex ".*\.\(xsd\|xml\|csv\|veh\|obj\|mtl\)" -print0 | xargs -0 cp --parents --target-directory $(abs_distdir) ; \
	    ( svn export --force tools $(distdir)/tools && cd $(distdir) && find tools -type f -print0 > ../tools_filelist; true ) ; \
	    xargs -0 cp --parents --target-directory $(abs_distdir) < tools_filelist ; \
	    find $(abs_distdir)/tools -name  "*.jar" | xargs rm ; \
	    cp tools_filelist $(abs_distdir) ; \
	fi;

dist-doc: doc
	cd $(srcdir)
	$(am__remove_distdir)
	mkdir -p $(distdir)/docs
	find tests/complex/tutorial -regex ".*\.\(xml\|txt\|sumocfg\|netccfg\|py\|osm\)" -print0 | xargs -0 cp --parents --target-directory $(distdir)
	mv $(distdir)/tests/complex/tutorial $(distdir)/docs
	rmdir $(distdir)/tests/complex
	rmdir $(distdir)/tests
	find docs/doxygen -name "*.png" | xargs rm
	cp -r docs/pydoc docs/doxygen docs/userdoc docs/javadoc $(distdir)/docs
	tools/extractTest.py -i -f tests/examples.txt -o $(distdir)/docs
	tar -czf $(PACKAGE)-doc-$(VERSION).tar.gz $(distdir)
	rm -f $(PACKAGE)-doc-$(VERSION).zip
	zip -r $(PACKAGE)-doc-$(VERSION).zip $(distdir)
	$(am__remove_distdir)

dist-tests:
	cd $(srcdir)
	$(am__remove_distdir)
	mkdir $(distdir)
	find tests -name .svn -prune -o -type f -print0 | xargs -0 cp --parents -t $(distdir)
	tar -czf $(PACKAGE)-tests-$(VERSION).tar.gz $(distdir)
	$(am__remove_distdir)

dist-complete: dist dist-doc dist-tests traas traci4j
	mv $(PACKAGE)-$(VERSION).tar.gz $(PACKAGE)-src-$(VERSION).tar.gz
	mv $(PACKAGE)-$(VERSION).zip $(PACKAGE)-src-$(VERSION).zip
	tar -xzf $(PACKAGE)-doc-$(VERSION).tar.gz $(distdir)
	tar -xzf $(PACKAGE)-tests-$(VERSION).tar.gz $(distdir)
	tar -xzf $(PACKAGE)-src-$(VERSION).tar.gz $(distdir)
	find tools bin -name  "*.jar" | xargs cp --parents --target-directory $(distdir)
	tar -czf $(PACKAGE)-all-$(VERSION).tar.gz $(distdir)
	rm -f $(PACKAGE)-all-$(VERSION).zip
	zip -r $(PACKAGE)-all-$(VERSION).zip $(distdir)
	$(am__remove_distdir)

pydist:
	cd tools && python ./build/setup-sumolib.py sdist bdist_egg && python ./build/setup-traci.py sdist bdist_egg

examples:
	tools/extractTest.py -x -f tests/examples.txt

traas:
	ant -f tools/contributed/traas/build.xml clean release
	cp tools/contributed/traas/dist/TraaS.jar bin

traci4j:
	mvn --batch-mode -f tools/contributed/traci4j/pom.xml clean install || true
	cp tools/contributed/traci4j/target/*.jar bin || true

trafficmodeler:
	cd tools/contributed/trafficmodeler && ./build.sh
	cp tools/contributed/trafficmodeler/jar/*.jar bin

sumo$(EXEEXT) netconvert$(EXEEXT) \
        netgenerate$(EXEEXT) duarouter$(EXEEXT) \
        dfrouter$(EXEEXT) jtrrouter$(EXEEXT) \
        od2trips$(EXEEXT) polyconvert$(EXEEXT) \
        netedit$(EXEEXT) \
        sumo-gui$(EXEEXT):
	$(MAKE) -C src $@
