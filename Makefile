#makefile to compile Server and Client code
CC=gcc  			#compiler
TARGET1=server		#server file name
TARGET2=client		#client file name

all:    #target name
		$(CC) -o $(TARGET1) Server.c -lpthread
		$(CC) -o $(TARGET2) Client.c -lpthread
clean:	#clean the history file
		rm $(TARGET1)
		rm $(TARGET2)