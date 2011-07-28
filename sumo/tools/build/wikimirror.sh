#!/bin/bash
# mirror the SUMO wiki
# author: jakob.erdmann@dlr.de

site=sourceforge.net/apps/mediawiki/sumo

httrack -w http://$site/index.php?title=Main_Page \
  -%i \
  -O "/home/erdm_ja/tmp/sumowiki/sumowiki" \
  -%v \
  S \
  "-$site/*&action=edit" \
  "-$site/*&action=history" \
  "-$site/*&printable=yes" \
  "-$site/*&oldid=*" \
  "-$site/*title=Special:*" \

# zip it up
7za a sumowiki.7z sumowiki
