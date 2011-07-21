#!/bin/bash
PREFIX=$1
for f in $PREFIX/sumo/sumo-src-*.tar.gz; do
  if test -e $PREFIX/sumo/configure -a $f -nt $PREFIX/sumo/configure; then
    curl -n -X PUT -T $PREFIX/sumo/build/package/sumo.spec https://api.opensuse.org/source/home:behrisch/sumo_nightly/sumo.spec
  fi
done
