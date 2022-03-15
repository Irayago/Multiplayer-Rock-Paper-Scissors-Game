# Compile and run the Rock Paper Scissors server
echo "Compiling server"
g++ -o server *.cpp ../*.cpp -I../ -pthread 
echo "Which port would you like to run the server on? Enter port..."
read portNum
./server $portNum