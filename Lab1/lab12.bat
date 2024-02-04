@echo off
setlocal enabledelayedexpansion

set "outputFile=%~2"
del %outputFile% >nul 2>&1

set count=1
for %%a in (%~1) do (
    for /f "tokens=*" %%b in (%%a) do (
        echo !count!. %%b>> %outputFile%
        set /a count+=1
    )
)

echo "Слияние файлов завершено. Результат записан в %outputFile%"