/**
 * File: GameRoom.cpp
 * Date: 11/09/2020
 * 
 * This is the implementaion file for the GameRoom class.
 * This class is an instance of a game of Rock, Paper,
 * Scissors between two players. It has all the member
 * variables and methods necessary to play a game.
*/

#include "GameRoom.hpp"
#include <stdio.h>
#include <string.h>
#include <iostream>

using namespace std;


// Default Constructor
GameRoom::GameRoom()
{
  this->player1;
  this->player2;
  this->player1SD = -1; // -1 indicates no connection
  this->player2SD = -1; // -1 indicates no connection
  this->gameRoomNumber = -1;
}


// Parameterized constructor with one player
GameRoom::GameRoom(Player singlePlayer, int player1SD, int gameRoomNumber)
{
  this->player1 = singlePlayer;
  this->player1SD = player1SD;
  this->gameRoomNumber = gameRoomNumber;
}


// operator == for comparing two game rooms
// game rooms are compared based on their gameRoomNumber
bool GameRoom::operator==(GameRoom gRoom) const
{
  // check if game rooms have the same game room number
  if (this->gameRoomNumber == gRoom.getRoomNumber())
  {
    return true;
  }
  else
  {
    return false;
  }
}


// get and set game room
int GameRoom::getRoomNumber()
{
  return this->gameRoomNumber;
}

bool GameRoom::setGameRoomNumber(int number)
{
  this->gameRoomNumber = number;
  return true;
}

// set players
bool GameRoom::setPlayerOne(Player newPlayer)
{
  this->player1 = newPlayer;
  return true;
}

bool GameRoom::setPlayerTwo(Player newPlayer)
{
  this->player2 = newPlayer;
  return true;
}

// get players
Player* GameRoom::getPlayerOnePtr()
{
  return &this->player1;
}

Player* GameRoom::getPlayerTwoPtr()
{
  return &this->player2;
}

// get socket descriptors
int GameRoom::getPlayerOneSD()
{
  return this->player1SD;
}

int GameRoom::getPlayerTwoSD()
{
  return this->player2SD;
}

// set socket descriptors
bool GameRoom::setPlayerOneSD(int sd)
{
  this->player1SD = sd;
  return true;
}

bool GameRoom::setPlayerTwoSD(int sd)
{
  this->player2SD = sd;
  return true;
}

string GameRoom::getMatchWinner()
{
  return matchWinner;
}

// Stand-In For To-Be-Implemented Methods
void GameRoom::PlayGame()
{
  cout << " In The Play Game Message : " <<endl; //DEBUG STATEMENT
  const int ROCK = 1;
  const int PAPER = 2;
  const int SCISSORS = 4;

  // play the game
  bool player1Turn = true;
  bool gameOver = false;

  int userChoice = 0;

  int winningCondition = 0;

  int playerChoice[2];
  
  int winner = 0;
  
  string numRound;
  
  string winningMessage;

  /*
   * Greet players
   */
  string playerNames[2];

  playerNames[0] = getPlayerOnePtr()->getName();
  cout <<" Getting The PLayer Name 1 : " << playerNames[0] << endl; //DEBUG STATEMENT

  playerNames[1] = getPlayerTwoPtr()->getName();
  cout <<" Getting The PLayer Name 2 : " << playerNames[1] << endl; //DEBUG STATEMENT

  cout <<" Sending Welcome Message to Player 1 : " << playerNames[0] << endl; //DEBUG STATEMENT
  comm.send(CHAT, "Welcome " + playerNames[0] + " good luck!!", getPlayerOneSD());

  cout <<" Sending Welcome Message to Player 2 : " << playerNames[1] << endl; //DEBUG STATEMENT
  comm.send(CHAT, "Welcome " + playerNames[1] + " good luck!!", getPlayerTwoSD());

  for (int i = 0; i < 3; i++)
  {
    numRound = to_string(i + 1);
    comm.send(CHAT, "ROUND: " + numRound, getPlayerOneSD());
    comm.send(CHAT, "ROUND: " + numRound, getPlayerTwoSD());
    
    /*
     * Reset game
     */
    gameOver = false;
    player1Turn = true;
    playerChoice[0] = 0;
    playerChoice[1] = 0;

    while (gameOver == false)
    {
      comm.send(CHAT, "Getting " +  playerNames[0] + " choice. Please wait...", getPlayerTwoSD());

      /* Get player choice */
      while (playerChoice[0] == 0 && playerChoice[1] == 0)
      {
        /*
         * Determine w/c player's turn
         * Get choice
         */
        if (player1Turn && playerChoice[0] == 0)
        {
          while (playerChoice[0] != ROCK && playerChoice[0] != PAPER && playerChoice[0] != SCISSORS)
          {
            comm.send(CHOICE, playerNames[0] + " choose Rock(1) Paper(2) Scissors(3)", getPlayerOneSD());
            Message incomingMsg = comm.rcv(getPlayerOneSD());
            string playerResponseChoice = incomingMsg.getRawMsg();
            cout << "Player choice (string) is: " << playerResponseChoice << endl;
            userChoice = stoi(playerResponseChoice);
            cout << "Player choice (int) is: " << userChoice << endl;

            switch (userChoice)
            {
            case 1:
              playerChoice[0] = ROCK;
              break;
            case 2:
              playerChoice[0] = PAPER;
              break;
            case 3:
              playerChoice[0] = SCISSORS;
              break;

            default:
              playerChoice[0] = 0;
              break;
            }
          }
          player1Turn = false;
        }

        if (!player1Turn && playerChoice[1] == 0)
        {
          comm.send(CHAT, "Getting " +  playerNames[1] + " choice. Please wait...", getPlayerOneSD());

          while (playerChoice[1] != ROCK && playerChoice[1] != PAPER && playerChoice[1] != SCISSORS)
          {
            comm.send(CHOICE, playerNames[1] + " choose Rock(1) Paper(2) Scissors(3)", getPlayerTwoSD());
            Message incomingMsg = comm.rcv(getPlayerTwoSD());
            string playerResponseChoice = incomingMsg.getRawMsg();
            cout << "Player choice (string) is: " << playerResponseChoice << endl;
            userChoice = stoi(playerResponseChoice);
            cout << "Player choice (int) is: " << userChoice << endl;

            switch (userChoice)
            {
            case 1:
              playerChoice[1] = ROCK;
              break;
            case 2:
              playerChoice[1] = PAPER;
              break;
            case 3:
              playerChoice[1] = SCISSORS;
              break;

            default:
              playerChoice[1] = 0;
              break;
            }
          }
        }
      }

      /*
       * Determine winner
       */
      winningCondition = playerChoice[0] - playerChoice[1] + 4;

      switch (winningCondition)
      {
      case 1:
          winningMessage = playerNames[0] + " wins round: " + numRound + " with rock!!\n\n";
          winner++;
        break;

      case 2:
          winningMessage = playerNames[1] + " wins round: " + numRound + " with scissors!!\n\n";
          winner--;
             
        break;

      case 3:
          winningMessage = playerNames[1] + " wins round: " + numRound + " with paper!!\n\n";
          winner--;
             
        break;

      case 4:
          winningMessage = "Round: " + numRound + " is a TIE!!\n\n";
        break;

      case 5:
          winningMessage = playerNames[0] + " wins round: " + numRound + " with paper!!\n\n";
          winner++;
        break;

      case 6:
          winningMessage = playerNames[0] + " wins round: " + numRound + " with scissors!!\n\n";
          winner++;
        break;

      case 7:
          winningMessage = playerNames[1] + " wins round: " + numRound + " with rock!!\n\n";
          winner--;
             
        break;
      }
      comm.send(CHAT, winningMessage, getPlayerOneSD());
      comm.send(CHAT, winningMessage, getPlayerTwoSD());
      gameOver = true;
    }
  }
  
  if (winner > 0)
  {
    winningMessage = "Match won by " + playerNames[0];
    this->matchWinner = playerNames[0];
  }
  else if (winner < 0)
  {
    winningMessage = "Match won by " + playerNames[1];
    this->matchWinner = playerNames[1];    
  }
  else
  {
    winningMessage = "Match is a tie " + playerNames[1];
    this->matchWinner = "0"; 
  }
  
  
  comm.send(CHAT, winningMessage, getPlayerOneSD());
  comm.send(CHAT, winningMessage, getPlayerTwoSD());

}
