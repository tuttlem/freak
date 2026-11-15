
@echo off
if not exist bin mkdir bin
del bin\test.log >nul 2>nul

echo Compiling test programs... > bin\test.log
call wcc386 /d2 /3s test\test.c -fo=bin\test.obj >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL

call wcc386 /d2 /3s test\test2.c -fo=bin\test2.obj >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL

call wcc386 /d2 /3s test\textures.c -fo=bin\textures.obj >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL

call wcc386 /d2 /3s test\utils.c -fo=bin\utils.obj >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL

echo Linking test programs... >> bin\test.log
call wlink @LNKTEST.LNK >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL

call wlink @LNKTEST2.LNK >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL

call wlink @TEXTURES.LNK >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL

call wlink @LNKUTIL.LNK >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL

echo BUILD OK >> bin\test.log
goto DONE

:FAIL
echo BUILD FAILED >> bin\test.log

:DONE
