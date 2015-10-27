
@echo off

call wcc386 /d2 /3s test\test.c -fo=bin\test.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s test\test2.c -fo=bin\test2.obj
IF ERRORLEVEL 1 GOTO DONE

call wlink system dos4g file bin\test,bin\freak,bin\util,bin\vga,bin\poly,bin\fixed name test
IF ERRORLEVEL 1 GOTO DONE

call wlink system dos4g file bin\test2,bin\freak,bin\util,bin\vga,bin\poly,bin\fixed name test2
IF ERRORLEVEL 1 GOTO DONE

:DONE
