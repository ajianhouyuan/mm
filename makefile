objects = ./src/config.o ./src/log.o ./src/main.o ./src/server.o ./src/mmsocket.o ./src/netio.o


main:$(objects)
	g++ -I/usr/include/ -o mm $(objects)
