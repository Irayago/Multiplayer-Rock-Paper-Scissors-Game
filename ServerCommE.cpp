/*
 * File: ServerCommE.cpp
 * Date: 11/09/2020
 *
 * This is the implementation file for the ServerCommunication class.
 * This class is used to hold all of the variables and methods
 * needed for a communicating between a server and client
 */

#include "ServerCommE.hpp"

Message ServerComm::rcv(int socketDescriptor)
{
  size_t bytesRead = 0;
  size_t nRead = 0;
  
  char buff[BUFF_SIZE]; 
  bzero(buff, BUFF_SIZE);

  while (nRead < BUFF_SIZE)
  {
      bytesRead = read(socketDescriptor, &buff, (BUFF_SIZE - nRead));
      nRead += bytesRead;
  }

  //cout<<"Raw message from the buffer in rcv method : " << buff<<endl; //debug statement
  //cout<<"Bytes read in rcv method : "<<nRead <<endl; //debug statements

  this->tokens.clear();

  this->tokens = tokenize(buff);

  //cout << "Method in Token 0 : "<<this->tokens[0] <<endl; //debug statement
  //cout << "Method in Token 1 : "<<this->tokens[1] <<endl; //debug statement

  Message inMessage(tokens);


  return inMessage;
}

std::vector<std::string> ServerComm::tokenize(std::string aString, char delim)
{
  std::vector<std::string> tokens;
  std::stringstream stream(aString);
  std::string token;
  
  while(getline(stream, token, delim))
    tokens.push_back(token);
  
  return tokens;
}

bool ServerComm::send(Method type, std::string message, int socketDescriptor)
{
  size_t bytesSent = 0;
  Message outMessage(type, message);
  bytesSent = writeToSocket(socketDescriptor, outMessage);
  
  if (bytesSent == outMessage.getsize())
    return true;
  
  return false;
}

// writes the raw message of this servercomm to socket
// returns number of bytes written
size_t ServerComm::writeToSocket(int socketDescriptor, Message message)
{
    //cout << "Message to be sent right before send : "<<message.getMsg()<<endl; //debug Statement
    //cout << "Length of Message Before Sending : "<<strlen(message.getMsg().c_str())<<endl; //debug Statement

    size_t bytesSent = write(socketDescriptor, message.getMsg().c_str(), strlen(message.getMsg().c_str()));
    return bytesSent;
}