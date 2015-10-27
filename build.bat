
@echo off

del bin\*.*
del *.obj
del *.exe

call wcc386 /d2 /3s src\util.c -fo=bin\util.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\fixed.c -fo=bin\fixed.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\vga.c -fo=bin\vga.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\poly.c -fo=bin\poly.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\freak.c -fo=bin\freak.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s test\test.c -fo=bin\test.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s test\test2.c -fo=bin\test2.obj
IF ERRORLEVEL 1 GOTO DONE

call wlink system dos4g file bin\test,bin\freak,bin\util,bin\vga,bin\poly,bin\fixed name test
IF ERRORLEVEL 1 GOTO DONE

call wlink system dos4g file bin\test2,bin\freak,bin\util,bin\vga,bin\poly,bin\fixed name test2
IF ERRORLEVEL 1 GOTO DONE

:DONE
