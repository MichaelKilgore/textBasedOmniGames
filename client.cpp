#include "Player.h"

using namespace std;

int main(int argc, char** argv)
{
    if (argc != 4) {
		
		cout << "./client.o 127.0.0.1 6310 MadMax" << endl;
        cout << "A IP and/or port number and/or username was not given." << endl;
        return 0;
    }

	string serverIP = argv[1];
    int port = atoi(argv[2]);
	string userName = argv[3];

    //initializing game
    Player player(port, serverIP, userName);

	player.startGame();

    return 0;
}
