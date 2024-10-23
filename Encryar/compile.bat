@echo off
set APP_NAME=Encryar
set SRC_FILE=main.cpp
set RES_FILE=versioninfo.rc
set EXE_FILE=%APP_NAME%.exe

REM Compile the resource file into a .o object
windres -i %RES_FILE% -o versioninfo.o
IF ERRORLEVEL 1 (
    echo "Error: Failed to compile versioninfo.rc"
    pause
    exit /b 1
)

REM Compile the C++ source and link with the resource object file
g++ %SRC_FILE% versioninfo.o -o %EXE_FILE% -static-libgcc -static-libstdc++
IF ERRORLEVEL 1 (
    echo "Error: Compilation failed."
    pause
    exit /b 1
)

REM Clean up
del versioninfo.o

echo Compilation complete. Generated %EXE_FILE% with version information.
pause