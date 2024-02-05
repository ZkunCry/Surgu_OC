@echo off
setlocal enabledelayedexpansion

set "input_file=%1"
set "output_file=%2"

sort %input_file%  /o %output_file%

