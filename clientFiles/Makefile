#Makefile for the Client

# default make command
all: client

# creating the executable
client: 
	@echo "Compiling client"
	g++ -o client client.cpp ../*.cpp -I../ -pthread -ggdb

# cleaning up the executable
clean:
	@echo "Cleaning Up"
	rm client