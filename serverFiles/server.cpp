/*
 * server.cpp
 * implementation file for a game server that
 * uses a dispatch loop to accept connections and
 * then forward those connections to worker threads
 * Author: Christian Rolph
 * November 3rd, 2020
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <sys/uio.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <list>
#include <iterator>
#include <pthread.h>
#include <sys/time.h>
#include <iostream>
#include <ctime>
#include <thread>         
#include <chrono>         
#include "GameRoom.hpp"
#include "GameParam.hpp"
#include "LobbyParam.hpp"
#include "Player.hpp"
#include "Lobby.hpp"
#include "../ServerCommE.hpp"

using namespace std;

const int BUFFSIZE = 1000;         // size of buffer for send/receive messages
const int NUM_CONNECTIONS = 10;    // number of concurrent connections

void* registerAndEnterLobby (void *param);  // the worker thread to register and enter game lobby
void* runGame (void *param);                // the worker thread to run a game

/*
 * main()
 * This is the main thread of the program. It initializes
 * the server's dispatch loop, listening for new connections.
 * Once a connection is made, that client is handed off to a
 * registerAndEnterLobby thread for further processing.  
 */
int main(int argc, char *argv[])
{

    // initialize the server lobby
    Lobby serverLobby =  Lobby();
    Scoreboard scoreboard = Scoreboard();
    
    // ensure that there's the right number of command line arguments
    if (argc != 2){
        cerr << "usage: server port" << endl;
        return -1;
    }

    int serverPort = atoi(argv[1]);   // the port to listen on for this server

    // build address
    sockaddr_in acceptSocketAddress;
    bzero((char *)&acceptSocketAddress, sizeof(acceptSocketAddress));
    acceptSocketAddress.sin_family = AF_INET;
    acceptSocketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    acceptSocketAddress.sin_port = htons(serverPort);

    // open socket and bind
    int serverSD = socket(AF_INET, SOCK_STREAM, 0);
    const int on = 1;
    setsockopt(serverSD, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(int));
    std::cout << "ServerSD: " << serverSD << endl;
    
    int rc = bind(serverSD, (sockaddr *)&acceptSocketAddress, sizeof(acceptSocketAddress));
    if (rc < 0)
    {
        cerr << "Bind Failed" << endl;
    }
    
    // listen and accept connections
    while (true) {
        listen(serverSD, NUM_CONNECTIONS);       //setting number of pending connections
        sockaddr_in newSockAddr;
        socklen_t newSockAddrSize = sizeof(newSockAddr);
        int newSD = accept(serverSD, (sockaddr *) &newSockAddr, &newSockAddrSize);
        std::cout << "Accepted Socket #: " << newSD <<endl;
        std::cout << "A client is now connected to the server." << endl;
        std::cout << endl;

        // create a new worker thread to service connection
        pthread_t tid;                                          // thread ID
        pthread_attr_t attr;                                    // attribute for a thread
        pthread_attr_init(&attr);                               // set default attributes
        int* clientSockPtr = &newSD;                            // client socket will be passed as param
        Lobby* serverLobbyPtr = &serverLobby;                   // ptr to server's lobby passed as param
        Scoreboard* scoreboardPtr = &scoreboard;
        Player* playerToPass = nullptr;                         // initially, no players to pass
        LobbyParam* clientParameters = new LobbyParam(clientSockPtr, serverLobbyPtr, playerToPass, scoreboardPtr);
        pthread_create(&tid, &attr, registerAndEnterLobby, clientParameters);      
    }
    
    // close the server socket
    close(serverSD);
    return 0;
}

/*
 * registerAndEnterLobby()
 * allows an incoming client to register their username,
 * view the list of available games, and create a new game
 * Parameters: void pointer to a socket accepted by server
 * Preconditions: a client has successfully connected to socket in parameters
 * Postconditions: execution has been passed to a game thread, or the client has 
 *                 decided to exit game and socket connection closed
 * Returns: void
 */
void* registerAndEnterLobby (void *arg) 
{
    std::cout << "The server is now in the registerAndEnterLobby thread." << endl;
    // get socket file descriptor
    LobbyParam &param = *(LobbyParam *) arg; 
    int newSD = *(int *) param.clientSockPtr;

    // get a pointer to the current lobby and scoreboard
    Lobby* serverLobby = (Lobby *) param.serverLobbyPtr;
    Scoreboard* scoreboard = (Scoreboard *) param.scoreboardPtr;

    // get pointer to player (if there is a plyer)
    // not nullptr means player has already registered
    Player* playerPtr = (Player *) param.playerPtr;
    int playerNumber = -1;

    // ensure player number persists through thread handoff
    if (playerPtr != nullptr)
    {
      playerNumber = playerPtr->getNumber();
      if (playerNumber < 0)
      {
        // negative ID number detected, persist player ID number
        // pull from thread parameters
        playerNumber = *(int *) param.playerNumPtr;
        playerPtr->setNumber(playerNumber);
        cout << "Updated Player Number: " << playerPtr->getNumber() << endl;
      }
      cout << "Player Number is: " << playerNumber << endl;//DEBUG STATEMENT
    } 
        
    // allocate a buffer to send and receive messages
    char dataBuf[BUFFSIZE]; 
    bzero(dataBuf, BUFFSIZE);

    // create communication objects
    ServerComm walkieTalkie = ServerComm();
    Message msg;

    // Let player know they're in lobby
    walkieTalkie.send(CHAT, "You're now in the lobby.\n", newSD);

    // get username if not already registered
    if (playerPtr == nullptr)
    {
      /************************************
      * Get Username
      ************************************/
      // send welcome message to client, prompt for username
      std::cout << "Sending initial registration message to client." << endl;
      bool sentSuccess = walkieTalkie.send(REG, "Welcome to RPS!\nLet's get you registered.\nWhat's your username?", newSD);
      std::cout << "Sent:\nWelcome to RPS!\nWhat is your username?" << endl;
    
      // read username and create a new player
      msg = walkieTalkie.rcv(newSD);
      std::cout << "Received: " << endl;
      msg.getRawMsg();
      std::cout << endl;

      // create player and add to lobby's list of players
      playerPtr = new Player(msg.getRawMsg());
      std::cout << "Created Player " << playerPtr->getName() << " with player number " << playerPtr->getNumber() << endl;
      serverLobby->addPlayer(*playerPtr);
      
      // add player to scoreboard
      scoreboard->addPlayer(playerPtr->getName());

      // notify player registration complete
      walkieTalkie.send(CHAT, "Registration complete! Let's get started.\n", newSD);
    }  

    /********************************************
    * Prompt for Choice: list, new game, players
    ********************************************/
    // ask if client wants to browse games, create a game, see list of players, or view scoreboard
    walkieTalkie.send(AVAILGAME, "Would you like to:\n[1] Browse available games\n[2] Create a new game\n[3] See a list of players\n[4] View the scoreboard\n[5] Deregister and Exit", newSD);

    bool playerInGameroom = false;

    // keep looping until player is in a gameroom
    while (playerInGameroom == false)
    {
      // get client response
      msg = walkieTalkie.rcv(newSD);
      string playerResponse = msg.getRawMsg();
      Method responseType = msg.getMethod();
      int playerChoice = stoi(playerResponse);

      // choice 1: browse available games
      if(playerChoice == 1 && responseType != JOIN && serverLobby->isEmpty() == false)
      {
        serverLobby->sendAvailableGamesToClient(walkieTalkie, newSD);
        std::cout << endl;
      }

      // alt choice 1: if no games are available, reprompt
      else if(playerChoice == 1 && responseType != JOIN)
      {
        walkieTalkie.send(CHAT, "Sorry, but there are no open games available at this moment.\nCreate one of your own!\n", newSD);
        walkieTalkie.send(AVAILGAME, "Would you like to:\n[1] Browse available games\n[2] Create a new game\n[3] See a list of players\n[4] View the scoreboard\n[5] Deregister and Exit", newSD);
      }
      
      // choice 2: create a new game
      else if (playerChoice == 2 && responseType != JOIN)
      {
        
        // create a game
        srand(time(0));
        int gameRoomNumber =  (rand() % 1000000) + 1;   //Making a Game Room number between 1 and 1,000,000
        GameRoom* playerCreatedGameRoom = new GameRoom();
        playerCreatedGameRoom->setPlayerOne(*playerPtr);
        playerCreatedGameRoom->setPlayerOneSD(newSD);
        playerCreatedGameRoom->setGameRoomNumber(gameRoomNumber);
        playerInGameroom = true;

        // add to server
        serverLobby->addGame(*playerCreatedGameRoom);
          GameRoom* ptrToGame = serverLobby->getGamePtr(gameRoomNumber);
          Player* ptrToPlayer1 = ptrToGame->getPlayerOnePtr();
          Player* ptrToPlayer2 = ptrToGame->getPlayerTwoPtr();
          cout << "The current players in that game room are:\nplayer1: " << ptrToPlayer1->getName() << "\nplayer1 SD: "
        << ptrToGame->getPlayerOneSD() << "\nplayer2: " << ptrToPlayer2->getName() << "\nplayer2 SD: " << ptrToGame->getPlayerTwoSD() << endl << endl;

        // wait until there's a second connection
        walkieTalkie.send(CHAT, "Waiting for another player to join your gameroom", newSD);
        while (ptrToPlayer2->getName() == "")
        {
          // sleep for a few seconds, then check again
          std::this_thread::sleep_for (std::chrono::seconds(1));
        }
        
        // launch the game
        pthread_t tidNewGame;                                           // thread ID
        pthread_attr_t attrNewGame;                                     // attribute for a thread
        pthread_attr_init(&attrNewGame);                                // set default attributes
        GameRoom* gameToPass = serverLobby->getGamePtr(gameRoomNumber); // pointer to the game that will be played
        Lobby* lobbyPtr = serverLobby;                                  // pointer to the server's lobby
        GameParam* gameParameters = new GameParam(gameToPass, lobbyPtr, scoreboard);
        pthread_create(&tidNewGame, &attrNewGame, runGame, gameParameters);   

        // exit current thread
        pthread_exit(0);
      }
      
      // choice 3: get a list of players
      else if (playerChoice == 3 && responseType != JOIN)
      {
          string allPlayers = serverLobby->getAllPlayersString();
          walkieTalkie.send(CHAT, ("Players are:\n" + allPlayers), newSD);
          walkieTalkie.send(AVAILGAME, "Would you like to:\n[1] Browse available games\n[2] Create a new game\n[3] See a list of players\n[4] View the scoreboard\n[5] Deregister and Exit", newSD);
          std::cout << endl;
      }

      // choice 4: view the scoreboard
      else if (playerChoice == 4 && responseType != JOIN)
      {
          string scores = scoreboard->toString();
          walkieTalkie.send(CHAT, scores, newSD);
          walkieTalkie.send(AVAILGAME, "Would you like to:\n[1] Browse available games\n[2] Create a new game\n[3] See a list of players\n[4] View the scoreboard\n[5] Deregister and Exit", newSD);
          std::cout << endl;
      }

      // choice 5: deregister and exit
      else if (playerChoice == 5 && responseType != JOIN) 
      {
          // remove player from lobby
          walkieTalkie.send(CHAT, "Now deregistering and preparing to exit.", newSD);
          bool playerRemoved = false;

          // ensure player number persists in multithread
          if(playerPtr->getNumber() < 0)
          {
            playerPtr->setNumber(playerNumber);
          }
          playerRemoved = serverLobby->removePlayer(*playerPtr);

          // print diagnostic statement
          cout << "Attempt to remove player " << playerPtr->getName() << " with player number " << playerPtr->getNumber() << " from Lobby was successful? " << playerRemoved << endl;

          // exit player and close thread
          walkieTalkie.send(GAMEOVER, "Deregister complete.\nThanks for playing!\n", newSD);
          sleep(10);
          close(newSD);
          pthread_exit(0);
    }

      else if (playerChoice != -1 && responseType == JOIN)
      {
        // get which game room they want to join
        int desiredGameRoom = playerChoice;
        
        // Attempt to add player to existing game
        bool bJoinedLobby = serverLobby->joinPlayerToGameRoom(desiredGameRoom, *playerPtr, newSD);
        if (bJoinedLobby)
        {
          playerInGameroom = true;
          std::cout << "Player Two (2) " << playerPtr->getName() << " successfully joined game room " << desiredGameRoom << endl;
          GameRoom* gameToBePlayedPtr = serverLobby->getGamePtr(desiredGameRoom);
          Player* playerOnePtrInGame = gameToBePlayedPtr->getPlayerOnePtr();
          Player* playerTwoPtrInGame = gameToBePlayedPtr->getPlayerTwoPtr();
          cout << "The current players in that game room are:\nplayer1: " << playerOnePtrInGame->getName() << "\nplayer1 SD: "
        << gameToBePlayedPtr->getPlayerOneSD() << "\nplayer2: " << playerTwoPtrInGame->getName() << "\nplayer2 SD: " << gameToBePlayedPtr->getPlayerTwoSD() << endl << endl;

          std::cout << endl;
          

          // create a new runGame thread to service connection
          std::cout << "Player Two (2) about to exit registerAndEnterLobby" << endl;

          // exit current thread
          std::cout << "Exiting registerAndEnterLobby thread" << endl << endl;
        }
        else
        {
          walkieTalkie.send(CHAT, "Unable to join you to gameroom. Gameroom is now full or unavailable. Please choose again.", newSD);
          walkieTalkie.send(AVAILGAME, "Would you like to:\n[1] Browse available games\n[2] Create a new game\n[3] See a list of players\n[4] View the scoreboard\n[5] Deregister and Exit", newSD);
        }
      }
      // default choice is to reprompt
      else
      {
        walkieTalkie.send(AVAILGAME, "Would you like to:\n[1] Browse available games\n[2] Create a new game\n[3] See a list of players\n[4] View the scoreboard\n[5] Deregister and Exit", newSD);
      }
    }
}


/*
 * runGame()
 * runs a game between two  clients
 * Parameters: void pointer to client sockets accepted by server
 * Preconditions: both clients have successfully connected to socket in parameters
 * Postconditions: game has been played, player streak updated in server
 * Returns: void
 */
void* runGame (void *arg) 
{
  // get the GameObject from thread parameters
  GameParam &param = *(GameParam *)arg;
  GameRoom currentGame = *(GameRoom *)param.currentGame;

  // get a pointer to the current lobby
  Lobby* serverLobby = (Lobby *) param.serverLobbyPtr;  
  Scoreboard* scoreboard = (Scoreboard *) param.scoreboardPtr;    

  // create communication objects
  ServerComm walkieTalkie;
  Message msg;

  int player1Sd = currentGame.getPlayerOneSD();
  int player2Sd = currentGame.getPlayerTwoSD();

  // send message to clients
  std::cout << "Sending a test message to players in game room" << endl;
  walkieTalkie.send(CHAT, "Congratulations!\nYou have entered gameroom " + to_string(currentGame.getRoomNumber()) +
    "\nYour opponent is: " + currentGame.getPlayerOnePtr()->getName(), player2Sd);
  walkieTalkie.send(CHAT, "Congratulations!\nYou have entered gameroom " + to_string(currentGame.getRoomNumber()) +
    "\nYour opponent is: " + currentGame.getPlayerTwoPtr()->getName(), player1Sd);

  // run the game
  bool closeGameroom = false; // determines if rematch will occur
  while (!closeGameroom)
  {
    // Play game
    currentGame.PlayGame();

    // update the server with win/loss streak
    if (currentGame.getMatchWinner() != "0")
      scoreboard->update(currentGame.getMatchWinner());

    // give clients option to have a rematch
    // gameroom will close if one player decides to exit the gameroom
    // prompt player 1
    walkieTalkie.send(CHAT, "Game over.", currentGame.getPlayerOneSD());
    walkieTalkie.send(REMATCH, "How about a rematch?", currentGame.getPlayerOneSD());
    walkieTalkie.send(CHAT, "Game over. Waiting for player 1 to decide on rematch.", currentGame.getPlayerTwoSD()); // let player 2 know what's happening
    msg = walkieTalkie.rcv(currentGame.getPlayerOneSD());
    string playerOneExitChoice = msg.getRawMsg();
    if (playerOneExitChoice == "n" || playerOneExitChoice == "N")
    {
      // player one wants to exit gameroom
      closeGameroom = true;
    }

    // prompt player 2
    walkieTalkie.send(REMATCH, "How about a rematch?", currentGame.getPlayerTwoSD());
    walkieTalkie.send(CHAT, "Waiting for player 2 to decide on rematch.", currentGame.getPlayerOneSD()); // let player 1 know what's happening
    msg = walkieTalkie.rcv(currentGame.getPlayerTwoSD());
    string playerTwoExitChoice = msg.getRawMsg();
    if (playerTwoExitChoice == "n" || playerTwoExitChoice == "N")
    {
      // player two wants to exit gameroom
      closeGameroom = true;
    }

    // GameRoom closes if at least one player wants to exit
    if (closeGameroom)
    {
    
      // send player 1 back to lobby
      // Player* continuePlayerOne = currentGame.getPlayerOnePtr();
      Player* continuePlayerOne = new Player();
      continuePlayerOne = currentGame.getPlayerOnePtr();
      
      pthread_t tidOne;                                         // thread ID
      pthread_attr_t attrOne;                                   // attribute for a thread
      pthread_attr_init(&attrOne);                              // set default attributes
      int playerOneSD = currentGame.getPlayerOneSD();
      int* clientSockPtrOne = &playerOneSD;                     // client socket will be passed as param
      Lobby* serverLobbyPtrOne = serverLobby;                   // ptr to server's lobby passed as param
      Scoreboard* scoreBoardOne = scoreboard;                   // ptr to the server's scoreboard
      //Player* playerToPassOne = continuePlayerOne;              // initially, no players to pass
      LobbyParam* clientParametersOne = new LobbyParam(clientSockPtrOne, serverLobbyPtrOne, continuePlayerOne, scoreBoardOne);
      pthread_create(&tidOne, &attrOne, registerAndEnterLobby, clientParametersOne);  
      std::cout << "runGame: Player One number is: " << continuePlayerOne->getNumber() << endl;   

      // send player 2 back to lobby first
      // Player* continuePlayerTwo = currentGame.getPlayerTwoPtr();
      Player* continuePlayerTwo = new Player();
      int playerTwoNumber = currentGame.getPlayerTwoPtr()->getNumber();
      continuePlayerTwo = currentGame.getPlayerTwoPtr();
      continuePlayerTwo->setNumber(playerTwoNumber);  

      pthread_t tidTwo;                                         // thread ID
      pthread_attr_t attrTwo;                                   // attribute for a thread
      pthread_attr_init(&attrTwo);                              // set default attributes
      int playerTwoSD = currentGame.getPlayerTwoSD();
      int* clientSockPtrTwo = &playerTwoSD;                     // client socket will be passed as param
      Lobby* serverLobbyPtrTwo = serverLobby;                   // ptr to server's lobby passed as param
      Scoreboard* scoreBoardTwo = scoreboard;                   // ptr to the server's scoreboard
      int* playerTwoNumPtr = &playerTwoNumber;
      LobbyParam* clientParametersTwo = new LobbyParam(clientSockPtrTwo, serverLobbyPtrTwo, continuePlayerTwo, scoreBoardTwo, playerTwoNumPtr);
      pthread_create(&tidTwo, &attrTwo, registerAndEnterLobby, clientParametersTwo);    

      // close gameroom
      std::cout << "Removing gameroom from server." << endl; 
      serverLobby->removeGame(currentGame);
    }
  }

  // exit thread
  std::cout << "Player One (1) Exiting runGame Thread." << endl;
  pthread_exit(0);
}

