#!/bin/bash

g++ -g -std=c++11 -I. -o dsb `find . -name '*.cpp'` `pkg-config --cflags --libs sdl2` -lGLEW -lGL -lpthread
