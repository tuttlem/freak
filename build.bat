
@echo off

call wcc386 /d2 /3s src\core\util.c -fo=bin\util.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\core\fixed.c -fo=bin\fixed.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\core\noise.c -fo=bin\noise.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\core\rng.c -fo=bin\rng.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\dos\vga.c -fo=bin\vga.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\dos\timer.c -fo=bin\timer.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\dos\kbd.c -fo=bin\kbd.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\render\draw.c -fo=bin\draw.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\render\palette.c -fo=bin\palette.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\render\sprite.c -fo=bin\sprite.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\render\image.c -fo=bin\image.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\render\font.c -fo=bin\font.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\render\debug.c -fo=bin\debug.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\render\transit.c -fo=bin\transit.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\ui\menu.c -fo=bin\menu.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\ui\dialog.c -fo=bin\dialog.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\render\texture.c -fo=bin\texture.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\game\physics.c -fo=bin\physics.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\game\camera.c -fo=bin\camera.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\dos\audio.c -fo=bin\audio.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\render\poly.c -fo=bin\poly.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\core\mat.c -fo=bin\mat.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\freak.c -fo=bin\freak.obj
IF ERRORLEVEL 1 GOTO DONE

:DONE
