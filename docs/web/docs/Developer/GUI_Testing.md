---
title: GUI Testing
---

SUMO's netedit tests use
[PyAutoGUI](https://github.com/asweigart/pyautogui).

## Setup

- Python (at least 2.7) should be installed
- Install using your package manager (Linux, macOS) or pip
  - openSUSE: `sudo zypper in python3-PyAutoGUI python3-pyperclip scrot python3-opencv`
  - Windows / ubuntu: [follow the docs](https://pyautogui.readthedocs.io/en/latest/install.html)
  - ubuntu: add `sudo apt in python3-opencv`
- Running on Linux without visual display will need Xvfb and fluxbox too´
  - openSUSE: `sudo zypper in xorg-x11-server fluxbox`
  - ubuntu: `sudo apt in xvfb fluxbox`

## Startup

- run $SUMO_HOME/tests/runNeteditTests.sh (Linux) or
  %SUMO_HOME%\\tests\\runNeteditTests.bat (Windows)

## Setting up a test

All tests are written in Python hiding the PyAutoGUI functions behind a
layer of test functions in tests/netedit/neteditTestFunctions.py. All of
them reside in the relevant test directory in a test.py script that
starts always with the same header:

```py
import os
import sys

testRoot = os.path.join(os.environ.get('SUMO_HOME', '.'), 'tests')
neteditTestRoot = os.path.join(os.environ.get('TEXTTEST_HOME', testRoot), 'netedit')
sys.path.append(neteditTestRoot)
import neteditTestFunctions as netedit

# Open netedit
neteditProcess, match = netedit.setupAndStart(neteditTestRoot, ['--new'])
```

This code will find the directory with the netedit tests using the
TEXTTEST_HOME variable and SUMO_HOME as a fallback. This enables the
start of the tests even outside of TextTest by simply running the
test.py script. You should not use relative directories here because
this makes copying and moving tests harder.
