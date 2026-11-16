
@echo off
if not exist bin mkdir bin
del bin\test.log >nul 2>nul
del bin\*.exe >nul 2>nul

echo Compiling demo... > bin\test.log
call wcc386 /d2 /3s test\demo.c -fo=bin\demo.obj >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL

call wcc386 /d2 /3s test\textures.c -fo=bin\textures.obj >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL

call wcc386 /d2 /3s test\utils.c -fo=bin\utils.obj >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL

call wcc386 /d2 /3s test\menu.c -fo=bin\menutest.obj >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL

echo Linking demo... >> bin\test.log
call wlink @scripts\DEMO.LNK >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL

echo BUILD OK >> bin\test.log
goto DONE

:FAIL
echo BUILD FAILED >> bin\test.log

:DONE
