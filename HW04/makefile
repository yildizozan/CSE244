all:
	gcc -pthread grepFromDirWithThread.c -o grepFromDirWithThread -Wall -pedantic -pthread
	#valgrind --tool=memcheck ./grepFromDirWithThread
	./grepFromDirWithThread 'z/xxx' massa
