#!/bin/bash

g++ -std=c++11 *.cpp -fPIC -shared -o libFaceCamera.so

cp *.so ../lib/
