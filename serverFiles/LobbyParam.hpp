/**
 * File: LobbyParam.hpp
 * Date: 11/09/2020
 * 
 * This is the header file for the LobbyParam class.
*/

#pragma once
#include "Lobby.hpp"
#include "Player.hpp"
#include "Scoreboard.hpp"

/*
 * LobbyParam class
 * This class is used as a way to pass more
 * than one argument to a thread. It holds
 * all the parameters needed for a player to 
 * register with the server and access server's game lobby
 */
class LobbyParam
{
public:
    // Member Variables
    int* clientSockPtr;              // a pointer to the client socket
    Lobby* serverLobbyPtr;           // a pointer to the server's lobby
    Player* playerPtr;
    Scoreboard* scoreboardPtr;
    int* playerNumPtr;

    // Parameterized Constructor
    LobbyParam(int* clientSockPtr, Lobby* serverLobby);
    LobbyParam(int* clientSockPtr, Lobby* serverLobby, Player* playerPtr);
    LobbyParam(int* clientSockPtr, Lobby* serverLobby, Player* playerPtr, Scoreboard* scoreboardPtr);
    LobbyParam(int* clientSockPtr, Lobby* serverLobby, Player* playerPtr, Scoreboard* scoreboardPtr, int* playerNumPtr);
};