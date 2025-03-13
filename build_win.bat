@echo off
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
if %errorlevel% neq 0 (
    echo CMake configuration failed!
    exit /b %errorlevel%
)
mingw32-make
if %errorlevel% neq 0 (
    echo Build failed!
    exit /b %errorlevel%
)
echo Build successful!
Win32Project.exe
