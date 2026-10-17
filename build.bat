@echo off
echo Building Custom Memory Allocator...
gcc -Wall -Wextra -O2 -Iinclude src/memalloc.c src/main.c -o memalloc_demo.exe
if %ERRORLEVEL% EQU 0 (
    echo Build succeeded! Running demo...
    memalloc_demo.exe
) else (
    echo Build failed! Make sure GCC (MinGW or MSYS2) is installed.
)
