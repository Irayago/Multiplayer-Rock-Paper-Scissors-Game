/*
 * Class: Application layer protocol, message sent across network
 * Message Format: Method, Message, Size
 * Delimeter: #
 *
 * Methods:
 *  Constructor: Message(method, message) create outgoing message
 *  Constructor: Message(tokenVector) create outgoing message
 *    tokenVector[0]: Method
 *    tokenVector[1]: Message
 *    tokenVector[0]: Size
 *  stringMsg(): return message as single string
 */

#ifndef Msg_hpp
#define Msg_hpp

#include <iostream>
#include <map>
#include <string>
#include <vector>

/*
 * Enum of types of methods understandable by our game
 */
enum Method { REG, LIST, CREATE, JOIN, EXIT, UNREG, CHAT, CHOICE, AVAILGAME, GAMEOVER, REMATCH };

class Message
{
private:
  
  Method method;
  std::string msg;
  std::string rawMsg;
  size_t size = 2;
  static const int BUFF_SIZE = 300;
  
public:
  
  /*
   * Generic message
   */
  Message();
  
  /*
   * Create outcoing message
   * Limit size of actual message data to 280 char
   */
  Message(Method method, std::string msg);
  
  /*
   * Create incoming message
   */
  Message(std::vector<std::string> tokens);
  
  /*
   * Return mesage as string
   */
  Method getMethod();

  /*
   * Return mesage as string
   */
  std::string getMsg();

  /*
   * Return's just the raw message of the
   * message string. Ex: from
   * "0#Hello#5", returns "Hello"
   */
  std::string getRawMsg();
  
  /*
   * Return mesage as string
   */
  size_t getsize();
  
  /*
   * Return method in integer form as a Method enum
   * Throw invalid_argument exception if Method enum not found
   */
  Method intToMethod(int i);
  
};
#endif /* Msg_hpp */

