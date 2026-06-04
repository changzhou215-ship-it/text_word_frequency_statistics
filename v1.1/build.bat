@echo off
D:\Dev-Cpp\MinGW64\bin\gcc.exe -std=c11 -o main.exe main.c hash_table.c word_utils.c cli_parser.c
if %errorlevel% equ 0 (
    echo Build successful.
) else (
    echo Build failed.
    pause
    exit /b 1
)
