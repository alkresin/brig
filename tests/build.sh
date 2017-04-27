#!/bin/bash

export SYSTEM_LIBS="-lm -lstdc++"
export BRIG_INC=../include
export BRIG_LIB=../lib

gcc $1.cpp -o$1 -I $BRIG_INC -L $BRIG_LIB $SYSTEM_LIBS -Wl,--start-group -lbrig -Wl,--end-group `pkg-config --cflags gtk+-2.0` `pkg-config gtk+-2.0 --libs`
