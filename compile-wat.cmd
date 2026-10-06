@echo off
rem =========================================================================
rem Compile script for Gambler Toolkit (Open Watcom C on OS/2 / ArcaOS)
rem Run from the project root:  compile-wat.cmd
rem =========================================================================

rem Auto-detect Watcom installation (OS/2 typical paths)
if "%WATCOM%"=="" (
    if exist C:\WATCOM\binp\wpp386.exe (
        set WATCOM=C:\WATCOM
    ) else if exist C:\WATCOM2\binp\wpp386.exe (
        set WATCOM=C:\WATCOM2
    ) else if exist D:\WATCOM\binp\wpp386.exe (
        set WATCOM=D:\WATCOM
    ) else (
        echo ERROR: Watcom not found. Set WATCOM environment variable.
        exit 1
    )
)

rem Watcom OS/2 tools live in binp
if exist %WATCOM%\binp\wpp386.exe (
    set PATH=%WATCOM%\binp;%PATH%
) else (
    set PATH=%WATCOM%\bin;%PATH%
)

rem Auto-detect OS/2 Toolkit
if "%OS2TK%"=="" (
    if exist C:\OS2TK45\h\os2.h (
        set OS2TK=C:\OS2TK45
    ) else (
        echo WARNING: OS2TK not set, defaulting to C:\OS2TK45
        set OS2TK=C:\OS2TK45
    )
)

rem Environment for Open Watcom on OS/2
set PATH=%WATCOM%\bin;%PATH%
set INCLUDE=%WATCOM%\h;%OS2TK%\h;src;%INCLUDE%
set LIB=%WATCOM%\lib386;%WATCOM%\lib386\os2;%OS2TK%\lib;%LIB%
set WIPFC=%WATCOM%\wipfc

set LOGFILE=compile-wat.log

echo ========================================== > %LOGFILE%
echo Gambler Toolkit Build Log >> %LOGFILE%
echo Date: %DATE% Time: %TIME% >> %LOGFILE%
echo WATCOM=%WATCOM% >> %LOGFILE%
echo OS2TK=%OS2TK% >> %LOGFILE%
echo ========================================== >> %LOGFILE%
echo. >> %LOGFILE%

echo Building Gambler Toolkit with Open Watcom...
echo WATCOM=%WATCOM%
echo OS2TK=%OS2TK%
echo Log: %LOGFILE%
echo.

echo [CLEAN] Running wmake clean...
wmake -f makefile.wat clean >> %LOGFILE% 2>&1

set FAILED=0
echo [BUILD] Running wmake all...
wmake -f makefile.wat all >> %LOGFILE% 2>&1
if errorlevel 1 set FAILED=1

rem Show the build output on screen
type %LOGFILE%

rem Copy the runtime data next to the exe
if "%FAILED%"=="0" (
    if exist bin\BJack.exe (
        if not exist bin\sounds mkdir bin\sounds
        copy sounds\*.wav bin\sounds >nul
    )
)

if "%FAILED%"=="1" goto failed
if exist bin\BJack.exe if exist bin\CStud.exe if exist bin\MPoker.exe if exist bin\CPoker.exe (
    echo.
    echo BUILD OK
    echo BUILD OK >> %LOGFILE%
    exit 0
)
:failed
echo.
echo BUILD FAILED
echo BUILD FAILED >> %LOGFILE%
exit 1
