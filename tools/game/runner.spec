# -*- mode: python ; coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2020-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    runner.spec
# @author  Michael Behrisch
# @date    2020-09-30

# input file for the pyinstaller, prefered way of calling: pyinstaller --noconfirm runner.spec

import glob
import os
import shutil
import subprocess
import sys

block_cipher = None
subprocess.call(["python", os.path.join(SPECPATH, "minwait.py")])

datas=[('../../bin/sumo.exe', 'bin'), ('../../bin/sumo-gui.exe', 'bin'), ('../../bin/*.dll', 'bin'),
       ('../../data', 'data'), ('images', 'images'), ('sounds', 'sounds'),
       ('refscores.pkl', '.'), ('*.gif', '.'), ('*.xml', '.')]
a = Analysis(['runner.py'],
             pathex=[SPECPATH + '/..'],
             binaries=[],
             datas=[],
             hiddenimports=[],
             hookspath=[],
             runtime_hooks=[],
             excludes=[],
             win_no_prefer_redirects=False,
             win_private_assemblies=False,
             cipher=block_cipher,
             noarchive=False)
pyz = PYZ(a.pure, a.zipped_data,
             cipher=block_cipher)
exe = EXE(pyz,
          a.scripts,
          [],
          exclude_binaries=True,
          name='runner',
          debug=False,
          bootloader_ignore_signals=False,
          strip=False,
          upx=True,
          console=True )
coll = COLLECT(exe,
               a.binaries,
               a.zipfiles,
               a.datas,
               strip=False,
               upx=True,
               upx_exclude=[],
               name='runner')

skip3d = len(sys.argv) < 2 or sys.argv[1] != "osg"
for f in glob.glob(os.path.join(SPECPATH, "*.sumocfg")):
    if skip3d and "bs3d" in f.lower():
        continue
    shutil.copy(f, os.path.join(DISTPATH, 'runner'))
    if os.path.exists(f[:-8]):
        shutil.copytree(f[:-8], os.path.join(DISTPATH, 'runner', os.path.basename(f[:-8])))
os.mkdir(os.path.join(DISTPATH, 'runner', 'bin'))
for o, d in datas:
    for f in glob.glob(os.path.join(SPECPATH, o)):
        if os.path.isfile(f):
            shutil.copy(f, os.path.join(DISTPATH, 'runner', d))
        else:
            shutil.copytree(f, os.path.join(DISTPATH, 'runner', d))
shutil.make_archive('sumo-game', 'zip', os.path.join(DISTPATH, 'runner'))
