all : wifirx script.script drawer 3dcomposite

LDFLAGS:=-lX11 -lm -lpthread -lXinerama -lXext
CFLAGS:=

3dcomposite : 3dcomposite.o
	gcc $(CFLAGS) -o  $@ $^ $(LDFLAGS)

drawer : drawer.o
	gcc -o $@ $^ -lm

script.script : makescript
	./$^ > $@

makescript : makescript.c
	gcc -o $@ $^


wifirx : wifirx.o XDriver.o DrawFunctions.o os_generic.o
	gcc -o $@ $^ $(LDFLAGS)

clean :
	rm -rf *.o *~ wifirx makescript script.script drawer
