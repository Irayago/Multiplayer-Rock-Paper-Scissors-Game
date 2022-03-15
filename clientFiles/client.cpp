/************************************************************************
 * client.cpp
 * implementation file for a client that connects to the game server
 * Communicates with the Game server for joining a lobby, playing a game,
 * chatting with other players
 * Author: Vishnu Mohan
 * November 3rd, 2020
 ************************************************************************/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <cstring>
#include <string>
#include <netdb.h>
#include <netinet/tcp.h>
#include <sys/uio.h>
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <sys/time.h>
#include <errno.h>
#include <limits.h>
#include <limits>
#include "../ServerCommE.hpp"
using namespace std;
const int BUFFSIZE=1000;


/********************************************************************
 * This method is used to get the customer input for username
 * The username is used for registering with the server before beginning the game
 ********************************************************************/
string getClientUsername ()
{
    string userName;
    getline(cin, userName);

    // Search user input for any spaces
    bool containSpace = false;
    for (int i = 0; i < userName.length(); i++)
    {
      if (isspace(userName[i])){
        containSpace = true;
      }
    }

    
    while(containSpace == true)
    {
        cout << "No spaces in username, please. Enter username:" << endl;
        userName = "";
        getline(cin, userName);
        
        // retry
        containSpace = false;
        for (int i = 0; i < userName.length(); i++)
        {
          if (isspace(userName[i])){
            containSpace = true;
          }
        }

    }
    return userName;
}

/*************************************************************************
 * Utility method to check if the client input provided is a valid integer
 *************************************************************************/

bool getClientInputValidateInputIsValidInteger(int &clientinput)
{
    cin >> clientinput;

    if (!cin.good())
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return false;
    }

    return true;
}

/********************************************************************
 * This method is used to get the customer input to the menu options that the client will
 * get from the server. e.g. entering 1 would be mean entering an Existing game
 * and entering 2 is to create a new game
 ********************************************************************/

int browseCreateOrList()
{
    // Get Client Response
    int clientResponse;

    while (true)
    {
        if (getClientInputValidateInputIsValidInteger(clientResponse))  //Check if the input is an integer
        {
            if (clientResponse > 0 && clientResponse < 6)
               break;
        }
        cout << "Please enter [1], [2], [3], [4], or [5]" << endl;
    }

    cout << endl;
    return clientResponse;
}

/********************************************************************
 * This method is while the game is underway to get the user input of
 * Rock, Paper or Scissors
 ********************************************************************/

int enterclientchoice()
{
    int clientChoice;
    while (true)    //Validate the user input. If it is 1,2,or 3
    {
        if (getClientInputValidateInputIsValidInteger(clientChoice))
        {
            if (clientChoice > 0 && clientChoice < 4)
                break;
        }
        cout << "Please enter [1], [2], or [3]" << endl;
    }

    cout << "The response to be sent to the server is: " << clientChoice << endl; //DEBUG STATEMENT
    cout << endl;
    return clientChoice;
}




/********************************************************************
 * This method will be called to check if the client wants a re-match
 ********************************************************************/

string clientWantsRematch()
{
    cout << "Do you want a rematch (y/n)?" << endl;
    string exitResponse;
    cin >> exitResponse;
    cout << endl;

    while (!(exitResponse == "y" || exitResponse == "Y" || exitResponse == "n" || exitResponse == "N"))
    {
        cout << "Please select y or n.\nDo you want a rematch (y/n)?" << endl;
        cin >> exitResponse;
        cout << endl;
    }

    return exitResponse;
}

/****************************************************************************************
 * Method to get the game number the client wants to enter from a list of available games
 ****************************************************************************************/

int getclientgame()
{
        // ask join a game or create a game?
        cout << "Would you like to join one of the available games (y/n)?" << endl;
        string joinResponse;
        cin >> joinResponse;
        cout << endl;
        int gameRoomNumber = 0;

        // sanitize user input: get yes or no
        while (!(joinResponse == "y" || joinResponse == "Y" || joinResponse == "n" || joinResponse == "N"))
        {
            cout << "Please select y or n.\nWould you like to join one of the available games (y/n)?" << endl;
            cin >> joinResponse;
            cout << endl;
        }

        // client wants to join a game
        if (joinResponse == "y" || joinResponse == "Y")
        {
            cout << "Which game would you like to join? Select a game room number." << endl;
            cin >> gameRoomNumber;
            cout << endl;

        }      
        else
        {
          // client doesn't want to join a game
          // response of -1 will make server reprompt client for browse, create, or list players
          gameRoomNumber = -1;
        }
        

        // send response to server
        return gameRoomNumber;
}

int main(int argc, char* argv[])
{
    int serverPort;
    char *serverName;

    /**************************
    *  Argument validation
    ***************************/
    if (argc != 3)
    {
        cerr << "Invalid number of Arguments " << argv[0] << endl;
        return -1;
    }

    /************************************
     * Read the arguments into the inputs
     ************************************/
    serverPort = atoi(argv[1]);
    serverName = argv[2];


    /*******************************************
     * Initializing data buffer local variables
     ******************************************/
    string readbuff;

    char dataBuf[BUFFSIZE]; 
    bzero(dataBuf, BUFFSIZE);


    /*************************************************
     * Get Hostent structure to communicate to server
     *************************************************/
    struct hostent *host = gethostbyname(serverName);

    /**************************
    * Build address
    ***************************/
    sockaddr_in sendSocketAddress;
    bzero((char *)&sendSocketAddress, sizeof(sendSocketAddress));
    sendSocketAddress.sin_family = AF_INET;
    sendSocketAddress.sin_addr.s_addr = inet_addr(inet_ntoa( *(struct in_addr*) (*host->h_addr_list) ));
    sendSocketAddress.sin_port = htons(serverPort);

    /*****************************
     *  Open socket and connect
     ******************************/
    int clientSD = socket(AF_INET, SOCK_STREAM, 0);

    int connection = connect(clientSD, (sockaddr *)&sendSocketAddress, sizeof(sendSocketAddress));
    if (connection < 0)
    {
        cerr << "Connection Failed " << errno << endl;
        close(clientSD);
        return -1;
    }

    /*******************************************************************************
     *  This is a while true loop that first reads the message from the server
     *  Depending on the type of message it takes the next steps. e.g.If the message
     *  of type of "REG", it takes username from the client and passes it to the server
     *  This loop continues until it receives a Gameover message from the sever
     ******************************************************************************/
    
    // create communication objects
    ServerComm walkieTalkie;
    Message msg;
    string clientUserName; //UserName that is provided during registration

    while(true)
    {
        //Receive message from the server
        msg = walkieTalkie.rcv(clientSD);

        string responseChoice = "";

        //Check the message type and take the next steps
        switch(msg.getMethod()) {
            case REG :                          //Case statement for handling registration message from client
                cout << msg.getRawMsg() << endl; //Print out the Registration Message from server
                clientUserName = getClientUsername(); //Enter client user name
                walkieTalkie.send(REG, clientUserName, clientSD); //Send client user name to server for registration
                break;
            
            case AVAILGAME :    //Ask client if they want to browse games, join game, view players, or view scoreboard
                cout << msg.getRawMsg() << endl;
                walkieTalkie.send(AVAILGAME, to_string(browseCreateOrList()), clientSD);
                break;
            
            case LIST :        //Print the list of Games and ask client to enter the Game Number
                cout << msg.getRawMsg() << endl;
                walkieTalkie.send(JOIN, to_string(getclientgame()), clientSD);
                break;
            
            case CHOICE :      //Get client choice of Rock Paper Scissors
                cout << msg.getRawMsg() << endl;
                responseChoice = to_string(enterclientchoice());
                cout << responseChoice << endl;
                walkieTalkie.send(CHOICE, responseChoice, clientSD);
                break;
            
            case CHAT :      //Just print out the message from the server. This is non-actionable for a client standpoint
                cout << msg.getRawMsg() << endl;
                break;

            case GAMEOVER : //Print out the Message from the server which tells winner information
                cout << msg.getRawMsg() << endl;
                break;

            case REMATCH :      // get client choice (y/n)
                walkieTalkie.send(REMATCH, clientWantsRematch(), clientSD);
                break;

            default :
                std::cout << "Player did not make an authorized choice" << endl;
        }

        if (msg.getMethod() == GAMEOVER) // exiting game
        {
            break;
        }
    }

    close(clientSD); //Close the socket
    return 0;
}

