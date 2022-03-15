/*
 * File: Scorebard.hpp
 * Date: 11/29/2020
 * 
 * This is the header file for the Scoreboard class.
 */

#pragma once
#include <iostream>
#include <string.h>
#include <unordered_map>

using namespace std;

class Scoreboard
{
    private:
        unordered_map<string, int> scoreboard;

        // a mutex lock to guard multithreaded sections
        pthread_mutex_t mutex;

    public:
        // Constructor
        Scoreboard(); 

        void addPlayer(string playerName);

        void update(string playerName);

        void display();

        string toString();
};