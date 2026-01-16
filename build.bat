@echo off
REM Add CMake to PATH
set PATH=%PATH%;"C:\Program Files\CMake\bin"

REM Configure project
cd /d D:\tankRecover\build
cmake .. -DCMAKE_BUILD_TYPE=Release

REM Build project
cmake --build . --config Release

pause