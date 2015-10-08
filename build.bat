
@echo off

del bin\*.*
del *.obj
del *.exe

call tasm /mx /zi /os src\vga.asm bin\vga.obj
IF ERRORLEVEL 1 GOTO DONE

call tasm /mx /zi /os src\kbd.asm bin\kbd.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\freak.c -fo=bin\freak.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s test\test.c -fo=bin\test.obj
IF ERRORLEVEL 1 GOTO DONE

call wlink system dos4g file bin\test,bin\vga,bin\kbd,bin\freak name test
IF ERRORLEVEL 1 GOTO DONE

REM wlib bin\freak +bin\ports +bin\vga +bin\kbd +bin\freak
REM IF ERRORLEVEL 1 GOTO DONE

REM wcl386 test\test.c bin\freak.lib

:DONE