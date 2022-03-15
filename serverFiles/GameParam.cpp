/**
 * File: GameParam.cpp
 * Date: 11/09/2020
 * 
 * This is the implementation file for
 * the GameParam class.
 * 
 */

#include "GameParam.hpp"
#include "Lobby.hpp"
#include "Scoreboard.hpp"


// Parameterized Constructor
GameParam::GameParam(GameRoom* currentGame, Lobby* serverLobbyPtr) : currentGame(currentGame), serverLobbyPtr(serverLobbyPtr){};

GameParam::GameParam(GameRoom* currentGame, Lobby* serverLobbyPtr, Scoreboard* scoreboardPtr) : currentGame(currentGame), serverLobbyPtr(serverLobbyPtr), scoreboardPtr(scoreboardPtr){};