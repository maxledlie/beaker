@echo off

gcc lib/*.c test/*.c -Wall -Wextra -std=c11 -I include -g -O0 -o build/test.exe
build/test.exe