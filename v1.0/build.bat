@echo off
cd /d "%~dp0"
D:\Dev-Cpp\MinGW64\bin\gcc.exe -std=c11 -mconsole -o main.exe main.c hash_table.c
if %errorlevel% equ 0 (
    echo Build successful.
) else (
    echo Build failed.
    pause
    exit /b 1
)
