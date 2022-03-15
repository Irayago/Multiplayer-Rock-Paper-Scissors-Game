/**
 * File: Lobby.cpp
 * Date: 11/10/2020
 * Author: Ira Yago
 * 
 * The is the implementation file for the Lobby class.
 * This class holds all the players waiting for a
 * game, all the available games that can be joined
 * ,and all the games currently active. It also has
 * a function to return the current game to the
 * runGame thread.
 */

#include "Lobby.hpp"

// default constructor
Lobby::Lobby()
{
  this->allPlayersOnServer;
  this->allGames;

  // initialize the mutex lock
  pthread_mutex_init( &this->mutex, NULL );
}

// returns the list of active list of players
list<Player> Lobby::getAllPlayers()
{
  return this->allPlayersOnServer;
}

// returns the list of active list of players
string Lobby::getAllPlayersString()
{
  pthread_mutex_lock( &mutex );   // lock
  list<Player>::iterator it;
  string stringOfPlayers = "";
  for(it = allPlayersOnServer.begin(); it != allPlayersOnServer.end(); it++)
  {
    stringOfPlayers += (it->getName() + "\n");
  }
  
  pthread_mutex_unlock( &mutex ); // unlock
  return stringOfPlayers;
}

// returns the list of available game rooms
list<GameRoom> Lobby::getallGames()
{
  return this->allGames;
}

// returns the list of joinable game rooms
list<GameRoom> Lobby::getJoinableGames()
{
  pthread_mutex_lock( &mutex );   // lock
  list<GameRoom>::iterator it;
  list<GameRoom> joinableGamesToReturn;
  for(it = this->allGames.begin(); it != this->allGames.end(); it++)
  {
    // if there is less than 2 people connected to a gameroom
    if(it->getPlayerOneSD() == -1 || it->getPlayerTwoSD() == -1)
    {
      GameRoom tempGameRoom = *it;
      joinableGamesToReturn.push_back(tempGameRoom); // add game to list of joinable games
    }
  }
  pthread_mutex_unlock( &mutex ); // unlock
  return joinableGamesToReturn;
}

// adds a game to the list of available games
bool Lobby::addGame(GameRoom g)
{
  pthread_mutex_lock( &mutex );   // lock
  this->allGames.push_back(g);
  pthread_mutex_unlock( &mutex ); // unlock
  return true;
}

// removes a game from the list of available games
bool Lobby::removeGame(GameRoom g)
{
  pthread_mutex_lock( &mutex );   // lock
  int oldNumberOfGames =  this->allGames.size();
  this->allGames.remove(g);

  // determine if game was removed
  int newNumberofGames = this->allGames.size();
  if(newNumberofGames < oldNumberOfGames)
  {
    pthread_mutex_unlock( &mutex ); // unlock
    return true;
  }
  else
  {
    pthread_mutex_unlock( &mutex ); // unlock
    return false;
  }
}

// gets a pointer to a game by looking up
// its game room number. Returns nullptr if
// game doesn't exist
GameRoom* Lobby::getGamePtr(int gameRoomNumber)
{
  list<GameRoom>::iterator it;
  bool gameExists = false;
  pthread_mutex_lock( &mutex );   // lock
  for(it = allGames.begin(); it != allGames.end(); it++)
  {
    if(it->getRoomNumber() == gameRoomNumber)
    {
      // return a pointer the the game room
      GameRoom* ptrToReturn = &(*it);
      pthread_mutex_unlock( &mutex ); // unlock
      return ptrToReturn;
    }
  }
  // game does not exist
  pthread_mutex_unlock( &mutex ); // unlock
  return nullptr;
}

// sends available games to client
bool Lobby::sendAvailableGamesToClient(ServerComm& walkieTalkie, int clientSD )
{
  string games = "Available Game Rooms: ";
  int gameCounter = 0;

  // gets the gameroom number from each element
  // and adds it to the game string
  list<GameRoom> joinableGames = this->getJoinableGames();
  list<GameRoom>::iterator it;
  pthread_mutex_lock( &mutex );   // lock
  for(it = joinableGames.begin(); it != joinableGames.end(); it++)
  {
    games += to_string(it->getRoomNumber()) + " ";
    gameCounter++;
  }
  pthread_mutex_unlock( &mutex ); // unlock

  if(gameCounter == 0)
  {
    games += "No available games. Create one of your own!";
  }

  // sends game string to client
  bool success = walkieTalkie.send(LIST, games, clientSD);

  if(!success)
  {
    return false; 
  }
  
  return true;
}


// adds a player to the active players list
bool Lobby::addPlayer(Player p)
{
  pthread_mutex_lock( &mutex );   // lock
  this->allPlayersOnServer.push_back(p);
  pthread_mutex_unlock( &mutex ); // unlock
  return true;
}

// removes a player from the active players list
bool Lobby::removePlayer(Player p)
{
  pthread_mutex_lock( &mutex );   // lock
  int oldNumberOfPlayers =  this->allPlayersOnServer.size();
  this->allPlayersOnServer.remove(p);

  // check if game was removed
  int newNumberofPlayers = this->allPlayersOnServer.size();
  if(newNumberofPlayers < oldNumberOfPlayers)
  {
    pthread_mutex_unlock( &mutex ); // unlock
    return true;
  }
  else
  {
    pthread_mutex_unlock( &mutex ); // unlock
    return false;
  }
}

// adds a player to an existing game room
// first attempts to make newPlayer player 1,
// if player 1 exists, makes new player player 2
bool Lobby::joinPlayerToGameRoom(int gameRoomNumber, Player& newPlayer, int newPlayerSD)
{
  // check if this gameroom exists
  pthread_mutex_lock( &mutex );   // lock
  list<GameRoom>::iterator it;
  bool gameExists = false;
  for(it = allGames.begin(); it != allGames.end(); it++)
  {
    if(it->getRoomNumber() == gameRoomNumber)
    {
      gameExists = true;
      cout << "The game room requested by client (" << gameRoomNumber << ") exists." << endl;
      cout << "The current players in that game room are:\nplayer1: " << it->getPlayerOnePtr()->getName() << "\nplayer1 SD: "
      << it->getPlayerOneSD() << "\nplayer2: " << it->getPlayerTwoPtr()->getName() << "\nplayer2 SD: " << it->getPlayerTwoSD() << endl << endl;

      // check for a player 1
      if (it->getPlayerOnePtr()->getName() == "")
      {
        // set a player 1
        it->setPlayerOne(newPlayer);
        it->setPlayerOneSD(newPlayerSD);
        
      cout << "The current players in that game room are:\nplayer1: " << it->getPlayerOnePtr()->getName() << "\nplayer1 SD: "
      << it->getPlayerOneSD() << "\nplayer2: " << it->getPlayerTwoPtr()->getName() << "\nplayer2 SD: " << it->getPlayerTwoSD() << endl << endl;
        pthread_mutex_unlock( &mutex ); // unlock
        return true;
      }
      // check for a player 2
      else if (it->getPlayerTwoPtr()->getName() == "")
      {
        // set a player 2
        it->setPlayerTwo(newPlayer);
        it->setPlayerTwoSD(newPlayerSD);

      // server message here
      cout << "The updated list of players in that game room are\nplayer1: " << it->getPlayerOnePtr()->getName() << "\nplayer1 SD: "
      << it->getPlayerOneSD() << "\nplayer2: " << it->getPlayerTwoPtr()->getName() << "\nplayer2 SD: " << it->getPlayerTwoSD() << endl;
      cout << endl;
        pthread_mutex_unlock( &mutex ); // unlock
        return true;
      }
      else
      {
        // the game room is full!
        pthread_mutex_unlock( &mutex ); // unlock
        return false;
      }
    }
  }
    
  // server statement
  cout << "The game room requested by client " << gameRoomNumber << " does not exist." << endl;
  pthread_mutex_unlock( &mutex ); // unlock
  return false;
}


bool Lobby::isEmpty()
{
  if(this->allGames.empty() == true)
      return true;
  else
      return false;
}
