CC = em++
CFLAGS = \
	-O3 \
	-Wall \
	-Werror \
	-Wno-deprecated \
	-Wno-parentheses \
	-Wno-format \
	-fno-exceptions
	#-fno-rtti
STANDARD=-std=c++11
HEADERS_FOLDER = lib
HTML_TEMPLATE = src/web/index_template.html

WASMFLAGS = \
	-O3 \
	-lembind \
	--memory-init-file 1 \
	--closure 1 \
	--shell-file $(HTML_TEMPLATE) \
	-s ENVIRONMENT=web \
	-s ALLOW_MEMORY_GROWTH=1 \
	-s AGGRESSIVE_VARIABLE_ELIMINATION=1 \
	-s ABORTING_MALLOC=1 \
	-s EXIT_RUNTIME=0 \
	-s NO_FILESYSTEM=1 \
	-s DISABLE_EXCEPTION_CATCHING=2 \
	-s ERROR_ON_UNDEFINED_SYMBOLS=0 \
	-s "EXPORTED_FUNCTIONS=['_main', '_malloc', '_free']" \
	-s "EXPORTED_RUNTIME_METHODS=['ccall', 'cwrap']"

build/index.html: src/driver.o lib/window.o lib/canvas.o
	$(CC) $(WASMFLAGS) $(STANDARD) lib/window.o lib/canvas.o src/driver.o -o build/index.html
	
src/driver.o: src/driver.cpp
	$(CC) $(CFLAGS) $(STANDARD) -I $(HEADERS_FOLDER)/ -c -o src/driver.o src/driver.cpp

lib/window.o: lib/window.c

lib/canvas.o: lib/canvas.c

.PHONY: run
run: build/index.html
	emrun --no_browser --no_emrun_detect build/index.html 2>/dev/null
	
.PHONY: clean
clean:
	rm -f src/driver.o
	rm -f lib/window.o
	rm -f lib/canvas.o
