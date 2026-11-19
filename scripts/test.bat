
@echo off
if not exist bin mkdir bin
del bin\test.log >nul 2>nul
del bin\*.exe >nul 2>nul

echo Compiling demo... > bin\test.log
call wcc386 /d2 /3s test\demo.c -fo=bin\demo.obj >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL

call wcc386 /d2 /3s test\demos\textures.c -fo=bin\textures.obj >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL

call wcc386 /d2 /3s test\demos\utils.c -fo=bin\utils.obj >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL

call wcc386 /d2 /3s test\demos\menu.c -fo=bin\menutest.obj >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL

call wcc386 /d2 /3s test\demos\sbpcm.c -fo=bin\demosb.obj >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL

call wcc386 /d2 /3s test\demos\midimp.c -fo=bin\demomidi.obj >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL

call wcc386 /d2 /3s test\demos\palette.c -fo=bin\demopal.obj >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL

call wcc386 /d2 /3s test\demos\blend.c -fo=bin\demoblnd.obj >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL

call wcc386 /d2 /3s test\demos\democom.c -fo=bin\democom.obj >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL
call wcc386 /d2 /3s test\demos\demodata.c -fo=bin\demodat.obj >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL

call wcc386 /d2 /3s test\demos\flare.c -fo=bin\demoflar.obj >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL
call wcc386 /d2 /3s test\demos\distort.c -fo=bin\demodist.obj >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL
call wcc386 /d2 /3s test\demos\xform.c -fo=bin\demoxfrm.obj >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL
call wcc386 /d2 /3s test\demos\textfx.c -fo=bin\demotext.obj >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL
call wcc386 /d2 /3s test\demos\voxel.c -fo=bin\demovoxl.obj >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL

call wcc386 /d2 /3s test\demos\triang.c -fo=bin\demotri.obj >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL
call wcc386 /d2 /3s test\demos\cube.c -fo=bin\democube.obj >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL
call wcc386 /d2 /3s test\demos\rast3d.c -fo=bin\demorast.obj >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL
call wcc386 /d2 /3s test\demos\shade3d.c -fo=bin\demoshd.obj >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL
call wcc386 /d2 /3s test\demos\ray3d.c -fo=bin\demoray.obj >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL
call wcc386 /d2 /3s test\demos\tool3d.c -fo=bin\demotool.obj >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL
call wcc386 /d2 /3s test\demos\mat3d.c -fo=bin\demomat.obj >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL
call wcc386 /d2 /3s test\demos\sprites.c -fo=bin\demospr.obj >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL
call wcc386 /d2 /3s test\demos\tilemap.c -fo=bin\demotile.obj >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL
call wcc386 /d2 /3s test\demos\topdown.c -fo=bin\demotop.obj >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL
call wcc386 /d2 /3s test\demos\particl.c -fo=bin\demopart.obj >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL
call wcc386 /d2 /3s test\demos\light2d.c -fo=bin\demolite.obj >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL
call wcc386 /d2 /3s test\demos\platfrm.c -fo=bin\demoplat.obj >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL
call wcc386 /d2 /3s test\demos\pcspkr.c -fo=bin\demospkr.obj >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL

echo Linking demo... >> bin\test.log
call wlink @scripts\DEMO.LNK >> bin\test.log
IF ERRORLEVEL 1 GOTO FAIL

echo BUILD OK >> bin\test.log
goto DONE

:FAIL
echo BUILD FAILED >> bin\test.log

:DONE
