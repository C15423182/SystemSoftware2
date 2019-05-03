CC = gcc
SS = ss_server
S = server
SC = ss_client
C = client
H = uploadclientfile
FLAG = -lpthread -lm

all: $(SS) $(SC) 

$(SS): $(S).o $(H).o
	$(CC) -o $(SS) $(S).o $(H).o $(FLAG)

$(S).o: $(S).c
	$(CC) -c $(S).c $(FLAG)

$(H).o: $(H).c $(H).h
	$(CC) -c $(H).c

$(SC): $(C).c
	$(CC) -o $(SC) $(C).c

clean:
	rm *.o $(SC) $(SS)