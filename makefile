all: Source/final.c Source/functions.c Source/util.c Source/cmds.c
	cc Source/final.c Source/functions.c Source/util.c Source/cmds.c -o final
	./final

clean:
	rm *.o
