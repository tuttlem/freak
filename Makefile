DOSBOX ?= dosbox
DOS_ROOT ?= /home/michael/Applications/dosbox

FORMAT_FILES = src/core/rng.c src/core/rng.h src/render/palette.c src/render/palette.h src/render/sprite.c src/render/sprite.h src/render/image.c src/render/image.h src/render/font.c src/render/font.h src/render/debug.c src/render/debug.h src/render/transit.c src/render/transit.h src/ui/dialog.c src/ui/dialog.h src/ui/menu.c src/ui/menu.h test/menu.c

.PHONY: dos demo run test lint format clean assets

assets:
	python3 tools/make_assets.py

# The batch files remain the canonical Watcom build; Make simply provides the
# same host-side DOSBox workflow used by the racer projects.
dos: assets
	$(DOSBOX) -c "mount -u c" -c "mount c $(DOS_ROOT)" -c "set PATH=C:\\WATCOM\\BINW;C:\\TASM\\BIN;%PATH%" -c "set INCLUDE=C:\\WATCOM\\H" -c "set WATCOM=C:\\WATCOM" -c "c:" -c "cd \\SRC\\FREAK" -c "call SCRIPTS\\BUILD.BAT" -c "call SCRIPTS\\TEST.BAT" -c "exit"
	@rg -q 'BUILD OK' bin/TEST.LOG

# Build, then launch the single interactive demo launcher.
demo: dos
	$(DOSBOX) -c "mount -u c" -c "mount c $(DOS_ROOT)" -c "set PATH=C:\\WATCOM\\BINW;%PATH%" -c "set WATCOM=C:\\WATCOM" -c "c:" -c "cd \\SRC\\FREAK" -c "BIN\\DEMO.EXE"

run: demo
test: demo

# New utility modules opt into this as they are added; legacy sources stay
# untouched until we deliberately reformat them.
lint:
	clang-format --dry-run --Werror $(FORMAT_FILES)

format:
	clang-format -i $(FORMAT_FILES)

clean:
	rm -rf bin
