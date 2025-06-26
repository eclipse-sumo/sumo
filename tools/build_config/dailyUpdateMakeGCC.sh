#!/bin/bash
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    dailyUpdateMakeGCC.sh
# @author  Michael Behrisch
# @date    2008

# Does the nightly git pull on the Linux / macOS server and then runs build and tests
PREFIX=$1
export FILEPREFIX=$2
export SMTP_SERVER=$3
if test $# -ge 4; then
  CONFIGURE_OPT=$4
fi

MAKELOG=$PREFIX/${FILEPREFIX}make.log
MAKEALLLOG=$PREFIX/${FILEPREFIX}makealloptions.log
STATUSLOG=$PREFIX/${FILEPREFIX}status.log
TESTLOG=$PREFIX/${FILEPREFIX}test.log
export SUMO_BATCH_RESULT=$PREFIX/${FILEPREFIX}batch_result
export SUMO_REPORT=$PREFIX/${FILEPREFIX}report
export SUMO_BINDIR=$PREFIX/sumo/bin
# the following are only needed for the clang build but do not hurt others
export LSAN_OPTIONS=suppressions=$PREFIX/sumo/build_config/clang_memleak_suppressions.txt,print_suppressions=0
export UBSAN_OPTIONS=suppressions=$PREFIX/sumo/build_config/clang_ubsan_suppressions.txt

rm -f $STATUSLOG
echo -n "$FILEPREFIX " > $STATUSLOG
date >> $STATUSLOG
echo "--" >> $STATUSLOG
if test -e $PREFIX/sumo_test_env/bin/activate; then
  # activate the virtual environment containing the python packages which are not available via apt
  source $PREFIX/sumo_test_env/bin/activate
fi
cd $PREFIX/sumo
basename $MAKELOG >> $STATUSLOG
git clean -f -x -d -q . &> $MAKELOG || (echo "git clean failed" | tee -a $STATUSLOG; tail -10 $MAKELOG)
git pull >> $MAKELOG 2>&1 || (echo "git pull failed" | tee -a $STATUSLOG; tail -10 $MAKELOG)
git submodule update >> $MAKELOG 2>&1 || (echo "git submodule update failed" | tee -a $STATUSLOG; tail -10 $MAKELOG)
GITREV=`tools/build_config/version.py -`
date >> $MAKELOG
mkdir -p wheelhouse build/$FILEPREFIX && cd build/$FILEPREFIX
cmake ${CONFIGURE_OPT:5} -DCMAKE_INSTALL_PREFIX=$PREFIX ../.. >> $MAKELOG 2>&1 || (echo "cmake failed" | tee -a $STATUSLOG; tail -10 $MAKELOG)
if make -j32 >> $MAKELOG 2>&1; then
  date >> $MAKELOG
  make lisum >> $MAKELOG 2>&1
  if make install >> $MAKELOG 2>&1; then
    if test "$FILEPREFIX" == "gcc4_64"; then
      make -j distcheck >> $MAKELOG 2>&1 || (echo "make distcheck failed" | tee -a $STATUSLOG; tail -10 $MAKELOG)
    fi
  else
    echo "make install failed" | tee -a $STATUSLOG; tail -10 $MAKELOG
  fi
else
  echo "make failed" | tee -a $STATUSLOG; tail -20 $MAKELOG
fi
date >> $MAKELOG
echo `grep -ci 'warn[iu]ng:' $MAKELOG` warnings >> $STATUSLOG

echo "--" >> $STATUSLOG
cd $PREFIX/sumo/bin
if test -e sumoD; then
  # it seems the plain build also had the debug config so we symlink to run the tests with the proper binaries
  for i in *D; do ln -sf ${i} ${i::-1}; done
fi
cd ..
if test -e build/$FILEPREFIX/src/CMakeFiles/sumo.dir/sumo_main.cpp.gcda; then
  # avoid a dangling symlink for the coverage build
  mkdir docs/lcov
fi
if test -e $SUMO_BINDIR/sumo && test $SUMO_BINDIR/sumo -nt build/$FILEPREFIX/Makefile; then
  # run tests
  export PATH=$PREFIX/texttest/bin:$PATH
  export TEXTTEST_TMP=$PREFIX/texttesttmp
  TESTLABEL=`LANG=C date +%d%b%y`r$GITREV
  rm -rf $TEXTTEST_TMP/*
  if test ${FILEPREFIX::6} == "extra_"; then
    tests/runExtraTests.py --gui "b $FILEPREFIX" &> $TESTLOG
  elif test "$FILEPREFIX" == "extraNetedit"; then
    tests/runTests.sh -a neteditoutput -b $FILEPREFIX -name $TESTLABEL >> $TESTLOG 2>&1
  else
    tests/runTests.sh -b $FILEPREFIX -name $TESTLABEL &> $TESTLOG
    if which Xvfb &>/dev/null; then
      if test ${FILEPREFIX::10} == "clangMacOS"; then
        tests/runTests.sh -a sumo.gui.mac -b $FILEPREFIX -name $TESTLABEL >> $TESTLOG 2>&1
      else
        tests/runTests.sh -a sumo.gui -b $FILEPREFIX -name $TESTLABEL >> $TESTLOG 2>&1
      fi
    fi
  fi
  tests/runTests.sh -b $FILEPREFIX -name $TESTLABEL -coll >> $TESTLOG 2>&1
  if test -e build/$FILEPREFIX/src/CMakeFiles/sumo.dir/sumo_main.cpp.gcda; then
    echo "lcov/html" >> $STATUSLOG
    echo "Coverage report" >> $STATUSLOG
  else
    echo "batchreport" >> $STATUSLOG
  fi
fi

# running extra tests for the coverage report
if test -e build/$FILEPREFIX/src/CMakeFiles/sumo.dir/sumo_main.cpp.gcda; then
  date >> $TESTLOG
  tests/runExtraTests.py --gui "b $FILEPREFIX" >> $TESTLOG 2>&1
#  $SIP_HOME/tests/runTests.sh -b $FILEPREFIX >> $TESTLOG 2>&1
  cd build/$FILEPREFIX
  make lcov >> $TESTLOG 2>&1 || (echo "make lcov failed"; tail -10 $TESTLOG)
  cd $PREFIX/sumo
  date >> $TESTLOG
fi

echo "--" >> $STATUSLOG
basename $MAKEALLLOG >> $STATUSLOG
export CXXFLAGS="$CXXFLAGS -Wall -W -pedantic -Wno-long-long -Wformat -Wformat-security"
rm -rf build/debug-$FILEPREFIX
mkdir build/debug-$FILEPREFIX && cd build/debug-$FILEPREFIX
cmake ${CONFIGURE_OPT:5} -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=$PREFIX ../.. > $MAKEALLLOG 2>&1 || (echo "cmake debug failed" | tee -a $STATUSLOG; tail -10 $MAKEALLLOG)
if make -j32 >> $MAKEALLLOG 2>&1; then
  make install >> $MAKEALLLOG 2>&1 || (echo "make install with all options failed" | tee -a $STATUSLOG; tail -10 $MAKEALLLOG)
else
  echo "make with all options failed" | tee -a $STATUSLOG; tail -20 $MAKEALLLOG
fi
cd $PREFIX/sumo
echo `grep -ci 'warn[iu]ng:' $MAKEALLLOG` warnings >> $STATUSLOG
echo "--" >> $STATUSLOG

basename $TESTLOG >> $STATUSLOG
date >> $STATUSLOG
echo "--" >> $STATUSLOG

# netedit tests
if test -e $SUMO_BINDIR/netedit && test $SUMO_BINDIR/netedit -nt build/$FILEPREFIX/Makefile; then
  if test "$FILEPREFIX" == "gcc4_64" || test "$FILEPREFIX" == "extraNetedit"; then
    # send SIGTERM to the netedit tests after some time and SIGKILL sometime later
    # This will not work on macOS unless "brew install coreutils" has been executed
    timeout -k 90m 60m tests/runTests.sh -a netedit.internal -b ${FILEPREFIX}netedit -name $TESTLABEL >> $TESTLOG 2>&1
    tests/runTests.sh -b ${FILEPREFIX} -name $TESTLABEL -coll >> $TESTLOG 2>&1
    timeout -k 540m 510m tests/runNeteditExternalDailyTests.sh -b ${FILEPREFIX}netedit -name $TESTLABEL >> $TESTLOG 2>&1
    tests/runTests.sh -b ${FILEPREFIX} -name $TESTLABEL -coll >> $TESTLOG 2>&1
    killall -9 -q fluxbox Xvfb
  fi
fi
