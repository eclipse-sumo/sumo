# -*- mode: python ; coding: utf-8 -*-

block_cipher = None


a = Analysis(['runner.py'],
             pathex=[SPECPATH + '/..'],
             binaries=[('../../bin/sumo.exe', '.'), ('../../bin/sumo-gui.exe', '.'), ('../../bin/*.dll', '.')],
             datas=[('A10KW', 'A10KW'), ('bs3d', 'bs3d'), ('corridor', 'corridor'), ('cross', 'cross'), ('DRT', 'DRT'),
                    ('grid6', 'grid6'), ('highway', 'highway'), ('ramp', 'ramp'), ('sounds', 'sounds'), ('square', 'square'),
                    ('*.sumocfg', '.'), ('*.gif', '.'), ('*.xml', '.')],
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
