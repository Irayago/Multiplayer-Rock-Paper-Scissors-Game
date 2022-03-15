/**
 * File: Player.cpp
 * Date: 11/09/2020
 * 
 * This is the implementation file for the Player class.
*/
#include "Player.hpp"
#include <string.h>
#include <iostream>
#include <limits>

using namespace std;

// Constructor
Player::Player()
{
  this->playerName = "";
  this->playerNumber = -1;
} 

// Parameterized constructor
Player::Player (string playerName, int playerNumber) : playerName(playerName), playerNumber(playerNumber){};

// Parameterized constructor
Player::Player (string playerName)
{
  this->playerName = playerName;

  // generate a random number for this player
  srand(time(0));
  this->playerNumber = (rand() % numeric_limits<int>::max()) + 1;
}

// Accessors
string Player::getName()
{
    return this->playerName;
}

int Player::getNumber()
{
    return this->playerNumber;
}

// Mutators
bool Player::setName(string newName)
{
    this->playerName = newName;
    return true;
}

bool Player::setNumber(int newNumber)
{
    this->playerNumber = newNumber;
    return true;
}

// operator == for comparing two players
// two players are equal if their playerNumber is the same
bool Player::operator==(Player person) const
{
  if(this->playerNumber == person.getNumber())
  {
    return true;
  }
  else
  {
    return false;
  }
  
}