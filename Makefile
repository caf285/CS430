all: main.c
	gcc main.c -framework OpenGL.framework -o ezview 
clean:
	rm -rf main *~

