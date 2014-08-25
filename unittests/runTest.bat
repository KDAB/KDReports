@echo off

cd %2
set OLD_PATH=%PATH%
set PATH=%1;%PATH%

if (%3)==() (
  set TEST_EXE=%2.exe
) else (
  set TEST_EXE=%3\%2.exe
)

echo Running %2 %CD%\%TEST_EXE%
%TEST_EXE%

set RETURNCODE=%ERRORLEVEL%
echo Test returned with %RETURNCODE%

set PATH=%OLD_PATH%
cd ..

exit /B %RETURNCODE%