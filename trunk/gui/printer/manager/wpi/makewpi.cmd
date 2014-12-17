/* MAKEWPI.CMD
 */
pkgname  = 'printman'
wisname  = 'printman.wis'

CALL RxFuncAdd 'SysIni', 'REXXUTIL', 'SysIni'
IF STREAM( pkgname'.wpi', 'C', 'QUERY EXISTS') \= '' THEN
    '@del' pkgname'.wpi'

PARSE VALUE SysIni('USER', 'WarpIN', 'Path') WITH warpin '00'x .
IF LEFT( warpin, 6 ) == 'ERROR:' THEN DO
    SAY 'Unable to locate WarpIN.'
    RETURN 1
END

CALL SETLOCAL
'@SET PATH='warpin';%PATH%'
'@SET BEGINLIBPATH='warpin
'@wic.exe' pkgname'.wpi -a 1 printman.exe'
'@wic.exe' pkgname'.wpi -a 1 printmen.msg'
'@wic.exe' pkgname'.wpi -a 1 rxprtutl.dll'
'@wic.exe' pkgname'.wpi -a 1 -c.. newprint.ico'
'@wic.exe' pkgname'.wpi -a 1 -c.. COPYING'
'@wic.exe' pkgname'.wpi -a 1 -c.. readme.txt'
'@wic.exe' pkgname'.wpi -s' wisname
CALL ENDLOCAL

RETURN rc

