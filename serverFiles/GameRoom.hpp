/**
 * File: GameRoom.hpp
 * Date: 11/09/2020
 * 
 * This is the header file for the GameRoom class.
 * This class is an instance of a game of Rock, Paper,
 * Scissors between two players. It has all the member
 * variables and methods necessary to play a game.
*/

#pragma once
#include "Player.hpp"
#include "../ServerCommE.hpp"
#include <string>

using namespace std;

class GameRoom
{
private:
  Player player1, player2;    // players in the game room
  int player1SD, player2SD;   // socket descriptors for TCP connection
  int gameRoomNumber;         // a unique identifier specifying this gameroom
  ServerComm comm;
  string matchWinner;
  
  
public:
  // Constructor
  GameRoom();
  
  // Parameterized constructor with one player;
  GameRoom(Player singlePlayer, int player1SD, int gameRoomNumber);
  
  // operator == for comparing two game rooms
  // game rooms are compared based on their gameRoomNumber
  bool operator==(GameRoom gRoom) const;
  
  int getRoomNumber();
  bool setGameRoomNumber(int number);
  Player* getPlayerOnePtr();
  Player* getPlayerTwoPtr();
  bool setPlayerOne(Player newPlayer);
  bool setPlayerTwo(Player newPlayer);
  bool setPlayerOneSD(int sd);
  bool setPlayerTwoSD(int sd);
  int getPlayerOneSD();
  int getPlayerTwoSD();
  
  string getMatchWinner();
  
  /*
   * Run Rock, Paper, Scissors game
   */
  void PlayGame();
};
