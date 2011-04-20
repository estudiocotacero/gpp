all: gpp


gpp: main.cpp
	g++ -o gpp -DNODEBUG main.cpp

install: gpp
	cp gpp /usr/local/bin/.

test: gpp
	@./gpp -e 'printf("hello world\n");'
