# Makefile
# Řešení IJC-DU2
# Autor: Matěj Mitaš, FIT

CFLAGS=-std=c99  -pedantic -Wall -fPIC
CPPFLAGS=-std=c++11 -pedantic -Wall
LIBINCLUDES=hash_function.o\
			htab_init.o\
			htab_lookup_add.o\
			htab_bucket_count.o\
			htab_move.o\
			htab_size.o\
			htab_find.o\
			htab_remove.o\
			htab_foreach.o\
			htab_clear.o\
			htab_free.o\

EXECS=tail tail2 wordcount wordcount-dynamic

# add files to be compiled
build: $(EXECS)

#
# Helper functions
#

wordcount.o: io.c wordcount.c
	gcc $(CFLAGS) -c wordcount.c -o wordcount.o  

io.o: io.c
	gcc $(CFLAGS) -c io.c -o io.o	


hash_function.o: hash_function.c hash_function.h
	gcc $(CFLAGS) -c hash_function.c

htab_init.o: htab_init.c htab.h	
	gcc $(CFLAGS) -c htab_init.c -o htab_init.o

htab_lookup_add.o: htab_lookup_add.c hash_function.h htab.h	
	gcc $(CFLAGS) -c htab_lookup_add.c -o htab_lookup_add.o

htab_bucket_count.o: htab_bucket_count.c hash_function.h htab.h	
	gcc $(CFLAGS) -c htab_bucket_count.c -o htab_bucket_count.o

htab_move.o: htab_move.c hash_function.h htab.h	
	gcc $(CFLAGS) -c htab_move.c -o htab_move.o	

htab_size.o: htab_size.c hash_function.h htab.h	
	gcc $(CFLAGS) -c htab_size.c -o htab_size.o		

htab_find.o: htab_find.c hash_function.h htab.h	
	gcc $(CFLAGS) -c htab_find.c -o htab_find.o		

htab_remove.o: htab_remove.c hash_function.h htab.h	
	gcc $(CFLAGS) -c htab_remove.c -o htab_remove.o

htab_foreach.o: htab_foreach.c htab.h	
	gcc $(CFLAGS) -c htab_foreach.c -o htab_foreach.o

htab_clear.o: htab_clear.c htab.h	
	gcc $(CFLAGS) -c htab_clear.c -o htab_clear.o

htab_free.o: htab_free.c htab.h
	gcc $(CFLAGS)  -c htab_free.c -o htab_free.o

#
# Directly compiled files
#
tail:
	gcc $(CFLAGS) tail.c -o tail

tail2:
	g++ $(CPPFLAGS) tail2.cc -o tail2	
#
# Table
#

wordcount: wordcount.o io.o libhtab.a
	gcc $(CFLAGS) -o wordcount -static wordcount.o io.c libhtab.a	

wordcount-dynamic: wordcount.o io.o libhtab.so
	gcc $(CFLAGS) -o wordcount-dynamic wordcount.o io.c libhtab.so

#
# Libs
#

libhtab.a: $(LIBINCLUDES)
	ar crs libhtab.a $(LIBINCLUDES)

#sdilena (dynamicka) knihovna
libhtab.so: $(LIBINCLUDES)
	gcc $(CFLAGS) -shared -o libhtab.so $(LIBINCLUDES)	


# Clear all files
clear:
	rm *.o $(EXECS) libhtab.a libhtab.so

pack:
	zip xmitas02.zip *.c *.cc *.h Makefile		
