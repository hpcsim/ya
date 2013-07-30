make clean
find . -name \CMakeCache.txt -type f -print0 | xargs -0 rm -f
find . -name \cmake_install.cmake -type f -print0 | xargs -0 rm -f
find . -name \Makefile -type f -print0 | xargs -0 rm -f
find . -name CMakeFiles -type d -print0 | xargs -0 rm -rf

