CC=gcc
all: main producer consumer

main: main.c
	$(CC) -pthread -o main main.c

producer: producer.c
	$(CC) -pthread -o producer producer.c

consumer: consumer.c
	$(CC) -pthread -o consumer consumer.c

