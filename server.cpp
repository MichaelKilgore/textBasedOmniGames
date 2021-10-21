#include "Host.h"

using namespace std;

int main(int argc, char** argv)
{
    if (argc != 3) {
		cout << "./server.o 6310 mkilgore" << endl;
        cout << "A port number and/or name was not given." << endl;
        return 0;
    }

    int port = atoi(argv[1]);
	string hostName = argv[2];

	//initializing game
	Host host(hostName, port);

	host.startGame();

	return 0;
}	

	












