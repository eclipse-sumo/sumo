SUBDIRS = xslt gfx

EXTRA_DIST = sumo_user.xml sumo_moreon_arcview.xml sumo_moreon_tls.xml sumo_moreon_dua.xml sumo_moreon_testing.xml sumo_howto_global_output_file.xml sumo_howto_building.xml

xsltproc = /usr/bin/xsltproc
xsltparams = --path xslt/sumo_html --param html.stylesheet '"../../css/sumo_db.css"' --stringparam section.autolabel 1 --stringparam section.label.includes.component.label 1 --stringparam admon.graphics 1
fop = /usr/bin/fop
fostyle = http://docbook.sourceforge.net/release/xsl/current/fo/docbook.xsl

doc: $(xsltproc)
	$(xsltproc) $(xsltparams) --stringparam root.filename user_index --stringparam chunk.first.sections 0 --stringparam chunk.section.depth 0 --stringparam use.id.as.filename 1 -o sumo_user.html user_chunk.xsl sumo_user.xml
	$(xsltproc) $(xsltparams) -o sumo_user.html user_docbook.xsl sumo_user.xml
	$(xsltproc) $(xsltparams) -o sumo_moreon_arcview.html more_docbook.xsl sumo_moreon_arcview.xml
	$(xsltproc) $(xsltparams) -o sumo_moreon_tls.html more_docbook.xsl sumo_moreon_tls.xml
	$(xsltproc) $(xsltparams) -o sumo_moreon_dua.html more_docbook.xsl sumo_moreon_dua.xml
	$(xsltproc) $(xsltparams) -o sumo_moreon_testing.html more_docbook.xsl sumo_moreon_testing.xml
	$(xsltproc) $(xsltparams) -o sumo_howto_global_output_file.html more_docbook.xsl sumo_howto_global_output_file.xml
	$(xsltproc) $(xsltparams) -o sumo_howto_building.html more_docbook.xsl sumo_howto_building.xml
# fo + pdf
	$(xsltproc) -o sumo_user.fo $(fostyle) sumo_user.xml
	$(fop) sumo_user.fo sumo_user.pdf
	$(xsltproc) -o sumo_moreon_arcview.fo $(fostyle) sumo_moreon_arcview.xml
	$(fop) sumo_moreon_arcview.fo sumo_moreon_arcview.pdf
	$(xsltproc) -o sumo_moreon_tls.fo $(fostyle) sumo_moreon_tls.xml
	$(fop) sumo_moreon_tls.fo sumo_moreon_tls.pdf
	$(xsltproc) -o sumo_moreon_dua.fo $(fostyle) sumo_moreon_dua.xml
	$(fop) sumo_moreon_dua.fo sumo_moreon_dua.pdf
	$(xsltproc) -o sumo_moreon_testing.fo $(fostyle) sumo_moreon_testing.xml
	$(fop) sumo_moreon_testing.fo sumo_moreon_testing.pdf
	$(xsltproc) -o sumo_howto_global_output_file.fo $(fostyle) sumo_howto_global_output_file.xml
	$(fop) sumo_howto_global_output_file.fo sumo_howto_global_output_file.pdf
	$(xsltproc) -o sumo_howto_building.fo $(fostyle) sumo_howto_building.xml
	$(fop) sumo_howto_building.fo sumo_howto_building.pdf
	rm sumo_*.fo
