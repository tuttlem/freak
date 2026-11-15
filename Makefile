DOSBOX ?= dosbox
DOS_ROOT ?= /home/michael/Applications/dosbox

FORMAT_FILES = src/core/rng.c src/core/rng.h

.PHONY: dos run test lint format clean

# The batch files remain the canonical Watcom build; Make simply provides the
# same host-side DOSBox workflow used by the racer projects.
dos:
	$(DOSBOX) -c "mount -u c" -c "mount c $(DOS_ROOT)" -c "set PATH=C:\\WATCOM\\BINW;C:\\TASM\\BIN;%PATH%" -c "set INCLUDE=C:\\WATCOM\\H" -c "set WATCOM=C:\\WATCOM" -c "c:" -c "cd \\SRC\\FREAK" -c "call BUILD.BAT" -c "call TEST.BAT" -c "exit"
	@rg -q 'BUILD OK' bin/TEST.LOG

# Launch the live 10-page procedural texture gallery.
run: dos
	$(DOSBOX) -c "mount -u c" -c "mount c $(DOS_ROOT)" -c "set PATH=C:\\WATCOM\\BINW;%PATH%" -c "set WATCOM=C:\\WATCOM" -c "c:" -c "cd \\SRC\\FREAK" -c "BIN\\TEXTURES.EXE"

# Builds the DOS test and gallery executables without launching one.
test: dos

# New utility modules opt into this as they are added; legacy sources stay
# untouched until we deliberately reformat them.
lint:
	clang-format --dry-run --Werror $(FORMAT_FILES)

format:
	clang-format -i $(FORMAT_FILES)

clean:
	rm -rf bin
