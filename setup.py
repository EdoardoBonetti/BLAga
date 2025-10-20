from skbuild import setup

import sys
_cmake_args = []

if 'win32' in sys.platform:
    _cmake_args += ['-G', 'MinGW Makefiles' ]

    
setup(
    name="BLAga",
    version="0.0.2",
    author="E. Bonetti",
    license="MIT",
    packages=["BLAga"],
    cmake_args=_cmake_args
)
