#from skbuild import setup
#
#import sys
##_cmake_args = []
##_cmake_args = ['-DCMAKE_BUILD_TYPE=Release']  # <-- add this
#_cmake_args = ['-DCMAKE_BUILD_TYPE=Release', '-DCMAKE_CXX_FLAGS_RELEASE=-O3 -mcpu=apple-m1 -ffast-math -funroll-loops']
#
#if 'win32' in sys.platform:
#    _cmake_args += ['-G', 'MinGW Makefiles' ]
#
#
##_cmake_args.extend(["-O3", "-march=native", "-ffast-math", "-funroll-loops"])
#setup(
#    name="BLAga",
#    version="0.1.0",
#    author="E. Bonetti",
#    license="MIT",
#    packages=["BLAga"],
#    cmake_args=_cmake_args
#)


from skbuild import setup
import sys

# Force Release build and propagate flags to CMake
_cmake_args = []
#_cmake_args = [
#    '-DCMAKE_BUILD_TYPE=Release',
#    '-DCMAKE_OSX_ARCHITECTURES=arm64',  # force ARM64 only
#    '-DCMAKE_CXX_FLAGS_RELEASE=-O3 -march=native -ffast-math -funroll-loops -flto'
#]

if 'win32' in sys.platform:
    _cmake_args += ['-G', 'MinGW Makefiles']

setup(
    name="BLAga",
    version="0.3.0",
    author="E. Bonetti",
    license="MIT",
    packages=["BLAga"],
    cmake_args=_cmake_args
)
