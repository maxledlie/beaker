@echo off

gcc src/*.c lib/*.c -Wall -Wextra -std=c11 -g -O0 -o build/main.exe -I include