import os
# user
os.system("xsltproc.exe --docbook --param html.stylesheet '\"../../css/sumo_db.css\"' --stringparam section.autolabel 1 --stringparam root.filename user_index --stringparam section.label.includes.component.label 1 --stringparam admon.graphics 1 --stringparam chunk.first.sections 0 --stringparam chunk.section.depth 0 --stringparam use.id.as.filename 1 -o sumo_user.html /usr/share/docbook-xsl/sumo_html/user_chunk.xsl sumo_user.xml")
os.system("xsltproc.exe --docbook --param html.stylesheet '\"../../css/sumo_db.css\"' --stringparam section.autolabel 1 --stringparam section.label.includes.component.label 1 --stringparam admon.graphics 1 -o sumo_user.html /usr/share/docbook-xsl/sumo_html/user_docbook.xsl sumo_user.xml")
# dev
os.system("xsltproc.exe --docbook --param html.stylesheet '\"../../css/sumo_db.css\"' --stringparam section.autolabel 1 --stringparam root.filename dev_index --stringparam section.label.includes.component.label 1 --stringparam admon.graphics 1 --stringparam chunk.first.sections 0 --stringparam chunk.section.depth 0 --stringparam use.id.as.filename 1 -o sumo_dev.html /usr/share/docbook-xsl/sumo_html/dev_chunk.xsl sumo_dev.xml")
os.system("xsltproc.exe --docbook --param html.stylesheet '\"../../css/sumo_db.css\"' --stringparam section.autolabel 1 --stringparam section.label.includes.component.label 1 --stringparam admon.graphics 1 -o sumo_dev.html /usr/share/docbook-xsl/sumo_html/dev_docbook.xsl sumo_dev.xml ")
# faq
os.system("xsltproc.exe --docbook --param html.stylesheet '\"../../css/sumo_db.css\"' --stringparam section.autolabel 1 --stringparam section.label.includes.component.label 1 --stringparam admon.graphics 1 -o faq.html /usr/share/docbook-xsl/sumo_html/faq_docbook.xsl sumo_qa.xml")
