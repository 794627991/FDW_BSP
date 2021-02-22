@ECHO OFF
ECHO Open a project and data file, start auto processing and exit
ECHO. %换行%
TOOL\JFlash.exe -openprjTOOL\jlinkCfgFile.jflash -openHEX\boot.hex,0x00000000 -mergeHEX\app.hex,0x00010000 -saveasHEX\iap_app.hex -auto -startapp -exit
IF ERRORLEVEL 1 goto ERROR
goto END
:ERROR
ECHO J-Flash ARM:  Error!
pause
:END