@echo off
setlocal enabledelayedexpansion

set "SCRIPT_DIR=%~dp0"

if "%~1"=="" (
    echo Usage: %~nx0 ^<architecture^>
    echo - architectures:
    echo        - x86_64
    echo        - arm64
    goto :eof
)

:: ========== HANDLE PARAMETER ==========

set "ARCH=%~1"

if /I "%ARCH%"=="arm64" (
    set "BASE_PATH_64BITS=%SCRIPT_DIR%Yaga\src\main\libs\arm64-v8a\"
    set "BASE_PATH_32BITS=%SCRIPT_DIR%Yaga\src\main\libs\armeabi-v7a\"
) else if /I "%ARCH%"=="x86_64" (
    set "BASE_PATH_64BITS=%SCRIPT_DIR%Yaga\src\main\libs\x86_64\"
    set "BASE_PATH_32BITS=%SCRIPT_DIR%Yaga\src\main\libs\x86\"
) else (
    echo Invalid architecture: %ARCH%
    goto :eof
)

:: ========== BUILD .so ==========

pushd "%SCRIPT_DIR%Yaga\src\main\"
echo Running ndk-build in: %CD%
call ndk-build
popd

:: ========== COPY .SO TO MODULE ==========
echo.

set "SOURCELOADER=libyagaloader.so"
set "SOURCEOPERATOR=libyagaoperator.so"
set "DESTINATION_MODULE_PATH_64BITS=%SCRIPT_DIR%YagaMagiskModule\system\lib64"
set "DESTINATION_MODULE_PATH_32BITS=%SCRIPT_DIR%YagaMagiskModule\system\lib"

call :copyIfExists "%BASE_PATH_64BITS%" "%SOURCELOADER%" "%DESTINATION_MODULE_PATH_64BITS%"
call :copyIfExists "%BASE_PATH_32BITS%" "%SOURCELOADER%" "%DESTINATION_MODULE_PATH_32BITS%"
call :copyIfExists "%BASE_PATH_64BITS%" "%SOURCEOPERATOR%" "%DESTINATION_MODULE_PATH_64BITS%"
call :copyIfExists "%BASE_PATH_32BITS%" "%SOURCEOPERATOR%" "%DESTINATION_MODULE_PATH_32BITS%"

:: ========== ZIP MODULE PROPERLY ==========
echo.
echo Creating Magisk zip...

set "MODULE_DIR=%SCRIPT_DIR%YagaMagiskModule"
set "ZIP_PATH=%SCRIPT_DIR%YagaMagiskModule.zip"

if exist "%ZIP_PATH%" del /f /q "%ZIP_PATH%"

if exist "%ProgramFiles%\7-Zip\7z.exe" (
    "%ProgramFiles%\7-Zip\7z.exe" a -tzip -mx=0 "%ZIP_PATH%" "%MODULE_DIR%\*"
) else (
    echo “Install 7z or compress it manually.
    goto :eof
)

echo Zip created at: %ZIP_PATH%
goto :eof

:copyIfExists
set "SOURCE_PATH=%~1%~2"
set "DEST_PATH=%~3"

echo Copying "%~2" to "%DEST_PATH%"...
if exist "%SOURCE_PATH%" (
    copy "%SOURCE_PATH%" "%DEST_PATH%" >nul
) else (
    echo Source file "%SOURCE_PATH%" not found!
)
goto :eof