#!/bin/bash
PREFIX=$1
for f in $PREFIX/sumo/sumo*.tar.bz2; do
  if test -e $PREFIX/sumo/configure -a $f -nt $PREFIX/sumo/configure; then
    curl -n -X PUT -T $PREFIX/sumo/data/package/sumo.spec https://api.opensuse.org/source/home:behrisch/sumo_nightly/sumo.spec
    curl -n -X PUT -T $PREFIX/sumo/`basename $f` https://api.opensuse.org/source/home:behrisch/sumo_nightly/`basename $f`
  fi
done
