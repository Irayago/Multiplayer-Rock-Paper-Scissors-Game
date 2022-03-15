#Makefile for the Server

# default make command
all: server

# creating the executable
server: 
	@echo "Compiling server"
	g++ -o server *.cpp ../*.cpp -I../ -pthread -ggdb 

# cleaning up the executable
clean:
	@echo "Cleaning Up"
	rm server
#-std=c++0x 