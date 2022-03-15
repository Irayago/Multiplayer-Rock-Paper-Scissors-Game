# Compile and run the Rock Paper Scissors server
echo "Compiling client"
g++ -o client client.cpp ../*.cpp -I../ -pthread 

echo "Which host would you like to connect to? Enter host..."
read host  
echo "Which port is the Server running on? Enter port..."
read portNum

./client $portNum $host