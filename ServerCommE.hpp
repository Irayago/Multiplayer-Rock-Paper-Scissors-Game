/*
 * File: ServerCommE.hpp
 * Date: 11/09/2020
 *
 * This is the header file for the ServerCommunication class.
 * This class is used to hold all of the variables and methods
 * needed for a communicating between a server and client
 */

#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <netdb.h>
#include <poll.h>
#include <signal.h>
#include <netinet/tcp.h>
#include <sys/uio.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/time.h>
#include <signal.h>
#include <pthread.h>
#include <fcntl.h>
#include <vector>
#include <fstream>
#include <sstream>

#include "Message.hpp"

using namespace std;

class ServerComm
{
private:
 
  vector<std::string> tokens;
  const int BUFF_SIZE = 300;
  
public:
  /*
   * Public call of ServerComm class to receive message
   * Returns message object
   * Params:
   *  socketDescriptor - specified by Server, Lobby, GameRoom or Client
   * Flow:
   *  Read from socket into a buffer
   *  Tokenize buffer and store values in vector
   *  Instantiate message from vector
   */
  Message rcv(int socketDescriptor);
  
  /*
   * Turn incoming communication into tokens
   */
  std::vector<std::string> tokenize(std::string aString, char delim = '#');
  
  /*
   * Public call of ServerComm class to send message
   * Returns true if sent, otherwise false
   * Params:
   *  socketDescriptor - specified by Server, Lobby, GameRoom or Client
   * Flow:
   *  Instantiate message from param args
   *  Write message to socket
   */
  bool send(Method type, std::string message, int socketDescriptor);
  
  /*
   * Private call of ServerComm class to send message
   * Write message to socket
   */
  size_t writeToSocket(int socketDescriptor, Message message);
};

