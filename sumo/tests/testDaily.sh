#!/bin/bash
#Bash script for the daily test run.
#Accepts up to four parameters:
#1. Directory for SUMO binaries (defaults to <SUMO_DIST>/src)
#2. Directory for the batch results
#3. Directory for the reports (HTML)
#4. SMTP server used for sending reports

OLDDIR=$PWD
cd `dirname $0`
export TEXTTEST_HOME=$PWD
if test x"$1" = x; then
  cd ..
  SUMO_BIN=$PWD/src
else
  SUMO_BIN=$1
fi
cd $OLDDIR
export DFROUTER_BINARY=$SUMO_BIN/sumo-dfrouter
export DUAROUTER_BINARY=$SUMO_BIN/sumo-duarouter
export JTRROUTER_BINARY=$SUMO_BIN/sumo-jtrrouter
export NETCONVERT_BINARY=$SUMO_BIN/sumo-netconvert
export NETGEN_BINARY=$SUMO_BIN/sumo-netgen
export OD2TRIPS_BINARY=$SUMO_BIN/sumo-od2trips
export SUMO_BINARY=$SUMO_BIN/sumo
export POLYCONVERT_BINARY="$SUMO_BIN/polyconvert"

if test x"$2" = x; then
  export SUMO_BATCH_RESULT=/tmp/sumo_batch_result
else
  export SUMO_BATCH_RESULT=$2
fi

if test x"$3" = x; then
  export SUMO_REPORT=/tmp/sumo_report
else
  export SUMO_REPORT=$3
fi

if test x"$4" = x; then
  export SMTP_SERVER=mail
else
  export SMTP_SERVER=$4
fi

export FILEPREFIX=`basename $SUMO_REPORT report`

texttest.py -b $FILEPREFIX
texttest.py -b $FILEPREFIX -coll
