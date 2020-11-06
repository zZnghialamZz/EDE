@ECHO OFF

RMDIR /Q/S "bin"
RMDIR /Q/S "build"

cmake -H. -B build
