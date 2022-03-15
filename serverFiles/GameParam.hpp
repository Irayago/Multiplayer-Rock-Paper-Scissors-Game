/**
 * File: GameParam.hpp
 * Date: 11/09/2020
 * 
 * This is the header file for the GameParam class.
 * This class is used to hold all of the variables
 * needed for a thread to run a game of Rock, Paper,
 * Scissors
*/

#pragma once
#include "GameRoom.hpp"
#include "Lobby.hpp"
#include "Scoreboard.hpp"


/*
 * GameParam class
 * This class is used as a way to pass more
 * than one argument to a thread. It holds
 * all the parameters needed to run a game.
 */
class GameParam
{
public:
    // Member Variables
    GameRoom* currentGame;           // a pointer to the game room these players are in
    Lobby* serverLobbyPtr;           // a pointer to the server's lobby

    //Scoreboard
    Scoreboard* scoreboardPtr;

    // Parameterized Constructor
    GameParam(GameRoom* currentGame, Lobby* serverLobbyPtr);
    
    // Parameterized Constructor
    GameParam(GameRoom* currentGame, Lobby* serverLobbyPtr, Scoreboard* scoreboardPtr);
};
