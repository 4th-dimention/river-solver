#!/bin/bash

CODE_HOME="$PWD"
OPTS=
cd . > /dev/null
g++ $OPTS $CODE_HOME/main.cpp -o solve
cd $CODE_HOME > /dev/null
