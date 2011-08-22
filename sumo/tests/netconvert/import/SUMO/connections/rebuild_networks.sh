#!/bin/bash

# these networks are taken from ../XML/connections

thisdir="$( cd "$( dirname "$0" )" && pwd )"

for testdir in cross3l_*
do
    cp $thisdir/../../XML/connections/$testdir/net.netconvert $testdir
    cp $thisdir/../../XML/connections/$testdir/net.netconvert $testdir/input_net.net.xml
done
