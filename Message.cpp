/*
* Message.cpp 
* implementation file
*/

#include "Message.hpp"

Message::Message()
{
  this->method = REG;
  this->msg = "";
  this->rawMsg = "";
  this->size = 0;
}

Message::Message(Method method, std::string msg)
{
  this->method = method;
  this->msg = std::to_string(method) + "#" + msg.substr (0,280) + "#";
  this->rawMsg =  msg.substr (0,280);
  this->size = this->msg.length() + std::to_string(this->msg.length()).length();
  std::string filler((this->BUFF_SIZE - this->size),'$');  //Initializing the message to rest of the buffer
  this->msg = this->msg + std::to_string(this->size) + filler;
}

Message::Message(std::vector<std::string> tokens)
{
  this->method = intToMethod(stoi(tokens[0]));
  this->size = stoi(tokens[2]);
  this->msg = tokens[0] + "#" + tokens[1] + "#" + tokens[2];
  this->rawMsg = tokens[1];
}

Method Message::getMethod()
{
  return this->method;
}

std::string Message::getMsg()
{
 return this->msg;
}

size_t Message::getsize()
{
  return this->size;
}

std::string Message::getRawMsg()
{
  return this->rawMsg;
}

Method Message::intToMethod(int i) {
  static std::map<int, Method> intToMethod {
    { 0, REG },
    { 1, LIST },
    { 2, CREATE },
    { 3, JOIN },
    { 4, EXIT },
    { 5, UNREG },
    { 6, CHAT },
    { 7, CHOICE },
    { 8, AVAILGAME},
    { 9, GAMEOVER},
    { 10, REMATCH}
  };
  auto x = intToMethod.find(i);
  if(x != std::end(intToMethod)) 
    return x->second;
  
  throw std::invalid_argument("i");
}
