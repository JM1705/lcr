#!/bin/bash

if [ ! ${PWD##*/} == "lcr" ]; then
  echo "please cd into the root directory of the project"
  exit
fi

if [ ! -d "build" ]; then
  mkdir build/
fi
if [ ! -d "bin" ]; then
  mkdir bin/
fi

echo "compiling runner"
cd build/
gcc -Wall -c ../src/libinput_interface.c
gcc -Wall -c ../src/runner.c

cd ../bin/
gcc -o runner\
  ../build/libinput_interface.o\
  ../build/runner.o

cd ..

echo "compiling remapper"
cd build/
gcc -Wall -c ../src/cJSON/cJSON.c
gcc -Wall -c ../src/json_io.c
gcc -Wall -c ../src/lcr.c

cd ../bin/
gcc -o lcr\
  ../build/cJSON.o\
  ../build/json_io.o\
  ../build/lcr.o

echo "the compiled binar(ies) have been output to lcr/bin"
