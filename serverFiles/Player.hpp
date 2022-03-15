/**
 * File: Player.hpp
 * Date: 11/09/2020
 * 
 * This is the header file for the Player class.
*/

#pragma once
#include <string.h>
#include <iostream>

using namespace std;

class Player
{
    private:
        string playerName;    // player names
        int playerNumber;     // a unique identifier assigned to each player

    public:
        // Constructor
        Player(); 

        // Parameterized constructor
        Player (string playerName, int playerNumber);

        // Parameterized constructor
        Player (string playerName);

        // Accessors
        string getName();
        int getNumber();

        // Mutators
        bool setName(string newName);
        bool setNumber(int newNumber);

        // operator == for comparing two players
        // two players are equal if their playerNumber is the same
        bool operator==(Player person) const;
};