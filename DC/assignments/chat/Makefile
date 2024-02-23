compile:
	gcc -Wall -g3 -fsanitize=address -pthread tserver.c -o tserver
	gcc -Wall -g3 -fsanitize=address -pthread tclient.c -o tclient
FLAGS    = -L /lib64
LIBS     = -lusb-1.0 -l pthread