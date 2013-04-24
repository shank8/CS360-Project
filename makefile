all: Source/final.c Source/functions.c Source/util.c Source/cmds.c Source/alloc.c
	cc Source/final.c Source/functions.c Source/util.c Source/cmds.c Source/alloc.c -o final
	rm diskimage
	cp Example/diskimage .
	./final

clean:
	rm *.o
