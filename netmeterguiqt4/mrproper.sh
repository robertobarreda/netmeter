#!/bin/bash
echo "Mr. Proper is starting. This program will delete all useless files"
make clean
if [ "$1" = "--all" ]; then rm -v CMakeOptions.txt; fi
find -name '*'Cache.txt -exec rm '{}' -fv \;
find -name CMakeFiles -exec rm '{}' -rfv \;
find -name cmake_install.cmake -exec rm '{}' -fv \;
find -name Makefile'*' -exec rm '{}' -fv \;
find -name ui_'*'.h -exec rm '{}' -fv \;
find -name '*'.so -exec rm '{}' -fv \;
find -name '*'.moc -exec rm '{}' -fv \;
