#!/bin/bash
PREFIX=$1
export FILEPREFIX=$2
export SMTP_SERVER=$3
MAKELOG=$PREFIX/${FILEPREFIX}make.log
MAKEALLLOG=$PREFIX/${FILEPREFIX}makealloptions.log
STATUSLOG=$PREFIX/${FILEPREFIX}status.log
TESTLOG=$PREFIX/${FILEPREFIX}test.log
export SUMO_BATCH_RESULT=$PREFIX/${FILEPREFIX}batch_result
export SUMO_REPORT=$PREFIX/${FILEPREFIX}report
export SUMO_BINDIR=$PREFIX/sumo/bin
if test $# -ge 4; then
  CONFIGURE_OPT=$4
fi

rm -f $STATUSLOG
echo -n "$FILEPREFIX " > $STATUSLOG
date >> $STATUSLOG
echo "--" >> $STATUSLOG
cd $PREFIX/sumo
make distclean &> /dev/null
make -f Makefile.cvs clean &> /dev/null
basename $MAKELOG >> $STATUSLOG
git pull &> $MAKELOG || (echo "git pull failed" | tee -a $STATUSLOG; tail -10 $MAKELOG)
GITREV=`tools/build/version.py -`
make -f Makefile.cvs >> $MAKELOG 2>&1 || (echo "autoreconf failed" | tee -a $STATUSLOG; tail -10 $MAKELOG)
./configure --prefix=$PREFIX/sumo $CONFIGURE_OPT >> $MAKELOG 2>&1 || (echo "configure failed" | tee -a $STATUSLOG; tail -10 $MAKELOG)
if make >> $MAKELOG 2>&1; then
  if test -e $PREFIX/sumo/unittest/src/sumo-unittest; then
    $PREFIX/sumo/unittest/src/sumo-unittest >> $MAKELOG 2>&1 || (echo "unit tests failed" | tee -a $STATUSLOG; tail -10 $MAKELOG)
  fi
  if make install >> $MAKELOG 2>&1; then
    if test -z "$CONFIGURE_OPT"; then
      make distcheck >> $MAKELOG 2>&1 || (echo "make distcheck failed" | tee -a $STATUSLOG; tail -10 $MAKELOG)
      make dist-complete >> $MAKELOG 2>&1 || (echo "make dist-complete failed" | tee -a $STATUSLOG; tail -10 $MAKELOG)
    fi
  else
    echo "make install failed" | tee -a $STATUSLOG; tail -10 $MAKELOG
  fi
else
  echo "make failed" | tee -a $STATUSLOG; tail -20 $MAKELOG
fi
echo `grep -c '[Ww]arn[iu]ng:' $MAKELOG` warnings >> $STATUSLOG

echo "--" >> $STATUSLOG
if test -e $SUMO_BINDIR/sumo -a $SUMO_BINDIR/sumo -nt $PREFIX/sumo/configure; then
  # run tests
  export PATH=$PREFIX/texttest/bin:$PATH
  export TEXTTEST_TMP=$PREFIX/texttesttmp
  rm -rf $TEXTTEST_TMP/*
  if test ${FILEPREFIX::6} == "extra_"; then
    tests/runInternalTests.py --gui "b $FILEPREFIX" &> $TESTLOG
  else
    tests/runTests.sh -b $FILEPREFIX -name `date +%d%b%y`r$GITREV &> $TESTLOG
    if which Xvfb &>/dev/null; then
      tests/runTests.sh -a sumo.gui -b $FILEPREFIX -name `date +%d%b%y`r$GITREV >> $TESTLOG 2>&1
      tests/runTests.sh -a netedit.gui -b $FILEPREFIX -name `date +%d%b%y`r$GITREV >> $TESTLOG 2>&1
    fi
  fi
  tests/runTests.sh -b $FILEPREFIX -name `date +%d%b%y`r$GITREV -coll >> $TESTLOG 2>&1
  echo "batchreport" >> $STATUSLOG
fi

if test -e $PREFIX/sumo/src/sumo_main.gcda; then
  tests/runInternalTests.py --gui "b $FILEPREFIX" >> $TESTLOG 2>&1
  $SIP_HOME/tests/runTests.sh -b $FILEPREFIX >> $TESTLOG 2>&1
  make lcov >> $TESTLOG 2>&1 || (echo "make lcov failed"; tail -10 $TESTLOG)
fi

echo "--" >> $STATUSLOG
basename $MAKEALLLOG >> $STATUSLOG
export CXXFLAGS="$CXXFLAGS -Wall -W -pedantic -Wno-long-long -Wformat -Wformat-security"
./configure --prefix=$PREFIX/sumo --program-suffix=A --with-python --with-ffmpeg \
  $CONFIGURE_OPT &> $MAKEALLLOG || (echo "configure with all options failed" | tee -a $STATUSLOG; tail -10 $MAKEALLLOG)
if make >> $MAKEALLLOG 2>&1; then
  make install >> $MAKEALLLOG 2>&1 || (echo "make install with all options failed" | tee -a $STATUSLOG; tail -10 $MAKEALLLOG)
else
  echo "make with all options failed" | tee -a $STATUSLOG; tail -20 $MAKEALLLOG
fi
echo `grep -c '[Ww]arn[iu]ng:' $MAKEALLLOG` warnings >> $STATUSLOG
echo "--" >> $STATUSLOG
