#!/bin/bash
PREFIX=$1
REMOTEDIR="behrisch,sumo@web.sourceforge.net:/home/groups/s/su/sumo/htdocs/daily/"
DOCDIR=$PREFIX/sumodoku

cd $PREFIX/sumo
scp -q docs/internet/daily/index.php $REMOTEDIR

rm -rf $DOCDIR
nice -19 doxygen sumo.doxyconf &> $DOCDIR.log
scp -qr $DOCDIR $REMOTEDIR
