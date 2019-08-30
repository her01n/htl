default: .c-tested .scheme-tested readme.html

.c-tested: target/run-test
	target/run-test
	touch $@

.scheme-tested: $(wildcard htl/*.scm test/*.scm) target/htl.so
	env LTDL_LIBRARY_PATH=target hdt/bin/hdt
	touch $@

readme.html: readme.md
	pandoc $< -o $@

PACKAGES=guile-2.0 glib-2.0
CFLAGS=$(shell pkg-config --cflags $(PACKAGES)) -g -fPIC -Wall -Werror -Isrc
LIBS=$(shell pkg-config --libs $(PACKAGES)) -g -fPIC

target/src/%.o: src/%.c $(wildcard src/*.h)
	mkdir -p target/src
	$(CC) $(CFLAGS) -c $< -o $@

target/test/%.o: test/%.c $(wildcard src/*.h)
	mkdir -p target/test
	$(CC) $(CFLAGS) -c $< -o $@

target/run-test: target/src/htl.o target/test/test.o
	$(CC) $(LIBS) $^ -o $@

target/htl.so: target/src/htl.o target/src/wrapper.o
	$(CC) $(LIBS) $^ --shared -o $@

clean:
	rm -rf target
	rm -rf readme.html
	rm -rf .c-tested .scheme-tested

