/**
 * File: LobbyParam.cpp
 * Date: 11/09/2020
 * 
 * This is the implementation file for
 * the LobbyParam class.
 * 
 */

#include "LobbyParam.hpp"
#include "Lobby.hpp"

// Parameterized Constructor
LobbyParam::LobbyParam(int* clientSockPtr, Lobby* serverLobby) :
        clientSockPtr(clientSockPtr), 
        serverLobbyPtr(serverLobby){};

// Parameterized Constructor
LobbyParam::LobbyParam(int* clientSockPtr, Lobby* serverLobby, Player* playerPtr) :
        clientSockPtr(clientSockPtr), 
        serverLobbyPtr(serverLobby),
        playerPtr(playerPtr){};

LobbyParam::LobbyParam(int* clientSockPtr, Lobby* serverLobby, Player* playerPtr, Scoreboard* scoreboardPtr) :
        clientSockPtr(clientSockPtr), 
        serverLobbyPtr(serverLobby),
        playerPtr(playerPtr),
        scoreboardPtr(scoreboardPtr){};

LobbyParam::LobbyParam(int* clientSockPtr, Lobby* serverLobby, Player* playerPtr, Scoreboard* scoreboardPtr, int* playerNumPtr) :
        clientSockPtr(clientSockPtr), 
        serverLobbyPtr(serverLobby),
        playerPtr(playerPtr),
        scoreboardPtr(scoreboardPtr),
        playerNumPtr(playerNumPtr){};