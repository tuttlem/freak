
@echo off

call wcc386 /d2 /3s src\core\util.c -fo=bin\util.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\core\arena.c -fo=bin\arena.obj
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

call wcc386 /d2 /3s src\runtime\timing.c -fo=bin\timing.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\runtime\profile.c -fo=bin\profile.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\input\keyboard.c -fo=bin\keyboard.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\input\input.c -fo=bin\input.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\input\mouse.c -fo=bin\mouse.obj
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

call wcc386 /d2 /3s src\render\blend.c -fo=bin\blend.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\render\flare.c -fo=bin\flare.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\render\distort.c -fo=bin\distort.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\render\xform.c -fo=bin\xform.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\render\voxel.c -fo=bin\voxel.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\ui\menu.c -fo=bin\menu.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\ui\dialog.c -fo=bin\dialog.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\ui\widgets.c -fo=bin\widgets.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\2d\camera.c -fo=bin\camera2d.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\2d\sprite.c -fo=bin\sprite2d.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\2d\batch.c -fo=bin\batch2d.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\2d\tilemap.c -fo=bin\tilemap2d.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\2d\collide.c -fo=bin\collide2d.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\2d\topdown.c -fo=bin\topdown2d.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\2d\particle.c -fo=bin\particle2d.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\2d\light2d.c -fo=bin\light2d.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\2d\plat.c -fo=bin\plat2d.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\render\texture.c -fo=bin\texture.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\game\physics.c -fo=bin\physics.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\game\camera.c -fo=bin\camera.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\dos\audio.c -fo=bin\audio.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\dos\sblaster.c -fo=bin\sblaster.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\dos\midi.c -fo=bin\midi.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\audio\mixer.c -fo=bin\mixer.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\audio\asset.c -fo=bin\asset.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\audio\opl.c -fo=bin\opl.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\audio\seq.c -fo=bin\seq.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\audio\tracker.c -fo=bin\tracker.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\render\poly.c -fo=bin\poly.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\core\mat.c -fo=bin\mat.obj
IF ERRORLEVEL 1 GOTO DONE

call wcc386 /d2 /3s src\freak.c -fo=bin\freak.obj
IF ERRORLEVEL 1 GOTO DONE

:DONE
