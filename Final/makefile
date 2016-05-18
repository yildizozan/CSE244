all:
	gcc fileShareServer.c -o fileShareServer -Wall -pedantic -ansi
	gcc client.c -o client -Wall -pedantic -ansi
	./fileShareServer 33
	xterm -hold -e "./client 192.168.1.2" &