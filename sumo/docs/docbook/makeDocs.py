#!/usr/bin/python
import os
import sys
import shutil

# get all files from the given folder recursively
def getStructure(dir):
	if(dir[-1:]!='/' and dir[-1:]!='\\'):
		dir = dir + '\\'
	files = os.listdir(dir)
	newFiles = []
	for file in files:
		if not os.path.isdir(dir + file)==1:
			newFiles.append( dir + file )
	return newFiles

# user
print "Building user docs..."
print " split..."
os.system("xsltproc.exe --param html.stylesheet '\"../../css/sumo_db.css\"' --stringparam section.autolabel 1 --stringparam root.filename user_index --stringparam section.label.includes.component.label 1 --stringparam admon.graphics 1 --stringparam chunk.first.sections 0 --stringparam chunk.section.depth 0 --stringparam use.id.as.filename 1 -o sumo_user.html /usr/share/docbook-xsl/sumo_html/user_chunk.xsl sumo_user.xml")
print " all in one..."
os.system("xsltproc.exe --param html.stylesheet '\"../../css/sumo_db.css\"' --stringparam section.autolabel 1 --stringparam section.label.includes.component.label 1 --stringparam admon.graphics 1 -o sumo_user.html /usr/share/docbook-xsl/sumo_html/user_docbook.xsl sumo_user.xml")

# dev
print "Building developer docs..."
print " split..."
os.system("xsltproc.exe --param html.stylesheet '\"../../css/sumo_db.css\"' --stringparam section.autolabel 1 --stringparam root.filename dev_index --stringparam section.label.includes.component.label 1 --stringparam admon.graphics 1 --stringparam chunk.first.sections 0 --stringparam chunk.section.depth 0 --stringparam use.id.as.filename 1 -o sumo_dev.html /usr/share/docbook-xsl/sumo_html/dev_chunk.xsl sumo_dev.xml")
print " all in one..."
os.system("xsltproc.exe --param html.stylesheet '\"../../css/sumo_db.css\"' --stringparam section.autolabel 1 --stringparam section.label.includes.component.label 1 --stringparam admon.graphics 1 -o sumo_dev.html /usr/share/docbook-xsl/sumo_html/dev_docbook.xsl sumo_dev.xml")

# faq
print "Building the faq..."
os.system("xsltproc.exe --param html.stylesheet '\"../../css/sumo_db.css\"' --stringparam section.autolabel 1 --stringparam section.label.includes.component.label 1 --stringparam admon.graphics 1 -o faq.html /usr/share/docbook-xsl/sumo_html/faq_docbook.xsl sumo_qa.xml")

# more_on
print "Building more on..."
print " split..."
os.system("xsltproc.exe --param html.stylesheet '\"../../css/sumo_db.css\"' --stringparam section.autolabel 1 --stringparam root.filename more_index --stringparam section.label.includes.component.label 1 --stringparam admon.graphics 1 --stringparam chunk.first.sections 0 --stringparam chunk.section.depth 0 --stringparam use.id.as.filename 1 -o sumo_moreon.html /usr/share/docbook-xsl/sumo_html/more_chunk.xsl sumo_moreon.xml")

# manpages
print "Building the man-pages..."
print " duarouter..."
os.system("xsltproc.exe ./tools/custom.xml man-duarouter.xml")
os.system("xsltproc.exe --param html.stylesheet '\"../../css/sumo_db.css\"' --stringparam section.autolabel 1 --stringparam section.label.includes.component.label 1 --stringparam admon.graphics 1 -o man_duarouter.html /usr/share/docbook-xsl/sumo_html/faq_docbook.xsl man-duarouter.xml")
print " jtrrouter..."
os.system("xsltproc.exe ./tools/custom.xml man-jtrrouter.xml")
os.system("xsltproc.exe --param html.stylesheet '\"../../css/sumo_db.css\"' --stringparam section.autolabel 1 --stringparam section.label.includes.component.label 1 --stringparam admon.graphics 1 -o man_jtrrouter.html /usr/share/docbook-xsl/sumo_html/faq_docbook.xsl man-jtrrouter.xml")
print " netconvert..."
os.system("xsltproc.exe ./tools/custom.xml man-netconvert.xml")
os.system("xsltproc.exe --param html.stylesheet '\"../../css/sumo_db.css\"' --stringparam section.autolabel 1 --stringparam section.label.includes.component.label 1 --stringparam admon.graphics 1 -o man_netconvert.html /usr/share/docbook-xsl/sumo_html/faq_docbook.xsl man-netconvert.xml")
print " netgen..."
os.system("xsltproc.exe ./tools/custom.xml man-netgen.xml")
os.system("xsltproc.exe --param html.stylesheet '\"../../css/sumo_db.css\"' --stringparam section.autolabel 1 --stringparam section.label.includes.component.label 1 --stringparam admon.graphics 1 -o man_netgen.html /usr/share/docbook-xsl/sumo_html/faq_docbook.xsl man-netgen.xml")
print " od2trips..."
os.system("xsltproc.exe ./tools/custom.xml man-od2trips.xml")
os.system("xsltproc.exe --param html.stylesheet '\"../../css/sumo_db.css\"' --stringparam section.autolabel 1 --stringparam section.label.includes.component.label 1 --stringparam admon.graphics 1 -o man_od2trips.html /usr/share/docbook-xsl/sumo_html/faq_docbook.xsl man-od2trips.xml")
#print " sumo..."
os.system("xsltproc.exe ./tools/custom.xml man-sumo.xml")
os.system("xsltproc.exe --param html.stylesheet '\"../../css/sumo_db.css\"' --stringparam section.autolabel 1 --stringparam section.label.includes.component.label 1 --stringparam admon.graphics 1 -o man_sumo.html /usr/share/docbook-xsl/sumo_html/faq_docbook.xsl man-sumo.xml")

if(sys.platform=="win32"):
    print "Installing (Windows)"
    os.system("move *.html ..\\internet\\docs\\gen\\")
    os.system("move *.1 ..\\man\\")
    os.system("mkdir ..\\internet\\docs\\gen\\gfx\\")
    os.system("mkdir ..\\internet\\docs\\gen\\gfx\\dev\\")
    os.system("copy  gfx\\dev\\*.gif ..\\internet\\docs\\gen\\gfx\\dev\\")
    os.system("mkdir ..\\internet\\docs\\gen\\gfx\\dev\\")
    os.system("copy  gfx\\user\\*.gif ..\\internet\\docs\\gen\\gfx\\user\\")
else:
    print "Installing (Linux/UNIX)"
    os.system("mv *.html ../internet/docs/gen/")
    os.system("mv *.1 ../man/")
    os.system("mkdir ../internet/docs/gen/gfx/")
    os.system("mkdir ../internet/docs/gen/gfx/dev/")
    os.system("cp  gfx/dev/*.gif ../internet/docs/gen/gfx/dev/")
    os.system("mkdir ../internet/docs/gen/gfx/user/")
    os.system("cp  gfx/user/*.gif ../internet/docs/gen/gfx/user/")

files = getStructure("../internet/docs/gen/")
for file in files:
	if file[-5:]==".html":
		out = file[:-5] + ".shtml"
		try:
			print "Moving from '" + file + "' to '" + out + "'"
			shutil.copy(file, out)
		except:
			print " Could not move from '" + file + "' to '" + out + "'"
		fd = open(out)
		content = fd.read()
		fd.close()

		# replace html by shtml
		idx = content.find(".html")
		while(idx>=0):
			if content[idx:idx+7] != ".html$%":
				content = content[:idx] + ".shtml" + content[idx+5:]
			idx = content.find(".html", idx+1)


		# insert menus
		idx = content.find("$%MENU1%$")
		content = content[:idx] + "<!--#include virtual=\"" + content[idx+9:]
		idx = content.find("$%MENU2%$")
		content = content[:idx] + "\"-->" + content[idx+9:]
		idx = content.find("$%MENU3%$")
		content = content[:idx] + "<!--#include virtual=\"../../menus/db_menu_end.html\"-->" + "<p></p><hr/><p><div align=\"right\" class=\"SUMOPageInfo\">last change: <!--#flastmod file=\"$%NAME%$\"--><!--#exec cgi=\"/cgi-bin/saveenv.pl\"--></div></p>" + content[idx+9:]


		name = out;
		idx = name.rfind("/")
		if(idx<0):
			idx = name.rfind("\\")
		name = name[idx+1:]
		idx = content.find("$%NAME%$")
		content = content[:idx] + name + content[idx+8:]
		fd = open(out, "w")
		fd.write(content)
		fd.close()



