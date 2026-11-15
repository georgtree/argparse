@echo off
setlocal
set failed=0
"%~1" "%~2\test\all.tcl" -backend c -packagefile "%~3" %~4
if errorlevel 1 set failed=1
"%~1" "%~2\test\all.tcl" -backend tcl -packagefile "%~2\argparse.tcl" %~4
if errorlevel 1 set failed=1
exit /b %failed%
