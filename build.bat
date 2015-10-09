
@echo off

del bin\*.*
del *.obj
del *.exe

call wcc386 /d2 /3s src\vga.c -fo=bin\vga.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\freak.c -fo=bin\freak.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s test\test.c -fo=bin\test.obj
IF ERRORLEVEL 1 GOTO DONE

call wlink system dos4g file bin\test,bin\freak,bin\vga name test
IF ERRORLEVEL 1 GOTO DONE

:DONE