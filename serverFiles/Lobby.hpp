/**
 * File: Lobby.hpp
 * Date: 11/10/2020
 * Author: Ira Yago
 * This is the header file for the Lobby class.
 * This class holds all the players waiting for a
 * game, all the available games that can be joined
 * ,and all the games currently active. It also has
 * a function to return the current game to the
 * runGame thread.
 */

#pragma once
#include "Player.hpp"
#include "GameRoom.hpp"
#include <pthread.h>
#include "../ServerCommE.hpp"
#include <list>

using namespace std;

class Lobby
{
  private:
    // list of all players waiting for a game
    list<Player> allPlayersOnServer; 

    // list for all GameRooms
    list<GameRoom> allGames; 

    // a mutex lock to guard multithreaded sections
    pthread_mutex_t mutex;

  public:
    // constructor
    Lobby();

    // checks if lobby has no game rooms
    bool isEmpty();

    // returns the list of all players in Lobby
    list<Player> getAllPlayers();

    // returns a string of all players in Lobby
    string getAllPlayersString();

    // returns the list of available game rooms
    list<GameRoom> getallGames();
    
    // returns a list of gamerooms that have only 1 player
    // thus, they are joinable
    list<GameRoom> getJoinableGames();

    // adds a game to the list of available games
    bool addGame(GameRoom g);

    // removes a game from the list of available games
    bool removeGame(GameRoom g);

    // gets a pointer to a game by looking up
    // its game room number. Returns nullptr if
    // game doesn't exist
    GameRoom* getGamePtr(int gameRoomNumber);

    // browses available game lobbies
    bool sendAvailableGamesToClient(ServerComm& walkieTalkie, int clientSocket);

    // add a player to a waiting game
    bool joinPlayerToGameRoom(int gameRoomNumber, Player& p, int playerSD);

    // adds a player to the active players list
    bool addPlayer(Player p);

    // removes a player from the active players list
    bool removePlayer(Player p);
};
