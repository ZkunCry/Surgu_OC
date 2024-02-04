@echo off
setlocal enabledelayedexpansion

set "input_file=%1"
set "output_file=%2"

set /a count=0
echo Data input file: 
for /f "tokens=*" %%a in (%input_file%) do (
    set /a count+=1
    set "line[!count!]=%%a"
)

for /l %%i in (1, 1, %count%) do (
    echo !line[%%i]!
)
REM Сортировка строк
sort %input_file% /r /o %output_file%
echo Result sorting in out file:
set /a count=0

for /f "tokens=*" %%a in (%output_file%) do (
    set /a count+=1
    set "line[!count!]=%%a"
)

for /l %%i in (1, 1, %count%) do (
    echo !line[%%i]!
)

