#!/bin/bash

SRC="./src"
gcc -g -std=c99 -Wall $SRC/log.c $SRC/wdcd-api.c $SRC/fileparser.c $SRC/profile.c $SRC/main.c  $SRC/cmd.c -o wdcd -lX11 -lXi
