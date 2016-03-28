all_C=$(wildcard *.c)
all:*.c *.h 
	@rm objs -rf
	@mkdir objs
	@gcc $(all_C) libjson/libjson.a -g -I/usr/include/libxml2  -std=gnu9x -pthread -lcurl -lpcre -lxml2 -Ilibjson -D_POSIX_THREAD_SAFE_FUNCTIONS -o objs/sf
	@gcc -g -lpcre testpcre/testpcre.c -o objs/testpcre
