#!/bin/sh
/usr/lib64/subversion/contrib/hook-scripts/svnnotify --set-sender --repos-path "$1" --revision "$2" --to "sumo-cvs@lists.sourceforge.net" --subject-prefix "SF.net SVN: sumo:" --subject-cx --no-first-line --with-diff --viewcvs-url "http://sourceforge.net/p/sumo/code/%s" --user-domain "users.sourceforge.net" --footer "This was sent by the SourceForge.net collaborative development platform, the world's largest Open Source development site." -l /usr/bin/svnlook --max-diff-length 100000
/usr/lib64/subversion/contrib/hook-scripts/svnnotify --set-sender --repos-path "$1" --revision "$2" --to "trigger@ifttt.com" --subject-prefix "SF.net SVN: sumo:" --subject-cx --no-first-line --from "behrisch@users.sourceforge.net" -l /usr/bin/svnlook --max-diff-length 1
#/usr/lib64/subversion/contrib/hook-scripts/ciabot_svn.py --revisionURI="http://sourceforge.net/p/sumo/code/%(revision)s" --repositoryURI=http://svn.code.sf.net/p/sumo/code/ "$1" "$2" "Simulation of Urban MObility"
exit 0
