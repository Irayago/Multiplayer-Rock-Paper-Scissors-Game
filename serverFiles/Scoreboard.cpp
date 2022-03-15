/*
 * File: Scorebard.cpp
 * Date: 11/29/2020
 * 
 * This is the implementation file for the Scoreboard class.
 */

#include "Scoreboard.hpp"

  Scoreboard::Scoreboard()
  {
    // initialize the mutex lock
    pthread_mutex_init( &this->mutex, NULL );
  } 

  void Scoreboard::addPlayer(string playerName)
  {
    pthread_mutex_lock( &mutex );   // lock
    this->scoreboard.insert(make_pair(playerName, 0));
    pthread_mutex_unlock( &mutex ); // unlock
  }
  
  void Scoreboard::update(string playerName)
  {
    pthread_mutex_lock( &mutex );   // lock
    auto it = this->scoreboard.find(playerName);
    (*it).second++;
    pthread_mutex_unlock( &mutex ); // unlock
  }

  void Scoreboard::display()
  {
    cout << "<<<<<<<<<< SCORE BOARD >>>>>>>>>>" << endl;
    pthread_mutex_lock( &mutex );   // lock
    for (auto player : this->scoreboard)
      cout << player.first << " - " << player.second << " wins" << endl;
    cout << endl;
    pthread_mutex_unlock( &mutex ); // unlock
  }

  string Scoreboard::toString()
  {
    string stringToReturn = "<<<<<<<<<< SCORE BOARD >>>>>>>>>>\n";
    pthread_mutex_lock( &mutex );   // lock
    for (auto player : this->scoreboard)
    {
      stringToReturn += player.first + " - " + to_string(player.second) + " wins\n";
    }
    pthread_mutex_unlock( &mutex ); // unlock
    
    return stringToReturn;
  }
