
@echo off

call wcc386 /d2 /3s src\util.c -fo=bin\util.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\fixed.c -fo=bin\fixed.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\vga.c -fo=bin\vga.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\poly.c -fo=bin\poly.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\mat.c -fo=bin\mat.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\freak.c -fo=bin\freak.obj
IF ERRORLEVEL 1 GOTO DONE

:DONE
