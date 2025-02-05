#!/bin/bash

if [ ! ${PWD##*/} == "lcr" ]; then
  echo "please cd into the root directory of the project"
  exit
fi

if [ ! -d "bin" ]; then
  mkdir bin/
fi

cd bin

echo "compiling runner"
gcc -g -o runner\
  ../src/runner.c\
  ../src/libinput_interface.c\
  ../src/utils.c

echo "compiling lcr (main program)"
gcc -g -o lcr\
  ../src/lcr.c\
  ../src/libinput_interface.c\
  ../src/utils.c\
  ../src/json_io.c\
  ../src/direct.c\
  ../src/mapping_funcs.c\
  ../src/cJSON/cJSON.c

# echo "compiling json reader"
# gcc -g -o json_io\
#   ../src/json_io.c\
#   ../src/utils.c\
#   ../src/cJSON/cJSON.c


