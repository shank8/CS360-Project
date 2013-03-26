all: Source/final.c Source/functions.c Source/util.c
	cc Source/final.c Source/functions.c Source/util.c -o final
	./final

clean:
	rm *.o
