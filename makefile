all: Source/final.c Source/functions.c
	cc Source/final.c Source/functions.c -o final
	final

clean:
	rm *.o