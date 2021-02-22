@ECHO OFF
ECHO Open a project and data file, start auto processing and exit
ECHO. %换行%
TOOL\JFlash.exe -openprjTOOL\jlinkCfgFile.jflash -openHEX\app.hex -auto -startapp -exit
IF ERRORLEVEL 1 goto ERROR
goto END
:ERROR
ECHO J-Flash ARM:  Error!
pause
:END