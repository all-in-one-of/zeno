# -*- mode: python ; coding: utf-8 -*-


block_cipher = None

dlls=[
    ( '../zenqt/bin/lib*.so*', '.' ),
    ( '../zenqt/bin/pylib_*.so', 'zenqt/bin' ),
]
assets=[
    ( '../zenqt/assets/*',     'zenqt/assets' ),
    ( '../assets/*',           'assets' ),
    ( '../graphs/*',           'graphs' ),
]

a = Analysis(['launcher.py'],
             pathex=['.'],
             binaries=dlls,
             datas=assets,
             hiddenimports=[],
             hookspath=[],
             hooksconfig={},
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
          name='launcher',
          debug=False,
          bootloader_ignore_signals=False,
          strip=False,
          upx=False,
          console=True,
          disable_windowed_traceback=False,
          target_arch=None,
          codesign_identity=None,
          entitlements_file=None )

coll = COLLECT(exe,
               a.binaries,
               a.zipfiles,
               a.datas, 
               strip=False,
               upx=False,
               upx_exclude=[],
               name='launcher')
