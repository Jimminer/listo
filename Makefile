all: main

main: main.c utils.c utils.h screens/popup.c screens/popup.h screens/create.c screens/delete.c
	mkdir -p build
	gcc main.c utils.c screens/popup.c screens/create.c screens/delete.c -g -o build/main -Iscreens -lncursesw