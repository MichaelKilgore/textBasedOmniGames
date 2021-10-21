#include "Networking.h"

Networking::Networking(int newPort, string newIP) :
	hostSocket(0),
	player2Socket(0),
	player3Socket(0),
	player4Socket(0),
	port(newPort),
	IP(newIP) { 
	check[0] = 0;
	check[1] = 0;
	check[2] = 0;
}

int Networking::getPort(void) {
	return port;
}
string Networking::getIP(void) {
	return IP;
}

int Networking::getHostSocket(void) {
	return hostSocket;
}

//UDP
void Networking::setUpHostUDPSocket(void) {
	sockaddr_in acceptSock;
    bzero((char *)&acceptSock, sizeof(acceptSock)); // zero out the data structure
    acceptSock.sin_family = AF_INET;                // using IP
    acceptSock.sin_addr.s_addr = htonl(INADDR_ANY); // listen on any address this computer has
    acceptSock.sin_port = htons(port);       // set the port to listen on

    int serverSd = socket(AF_INET, SOCK_DGRAM, 0); // creates a new socket for IP using TCP

    const int on = 1;
    setsockopt(serverSd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(int)); // this lets us reuse the socket without waiting for the OS to recycle it

    // Bind the socket
    bind(serverSd, (sockaddr *)&acceptSock, sizeof(acceptSock)); // bind the socket using the parameters we set earlier

	UDPSocket = serverSd;
}

void Networking::setUpPlayerUDPSocket(void)  {
	struct hostent *host = gethostbyname(IP.c_str());
    sockaddr_in sendSockAddr;
    bzero((char *)&sendSockAddr, sizeof(sendSockAddr));
    sendSockAddr.sin_family = AF_INET;
    sendSockAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr *)*host->h_addr_list));
    sendSockAddr.sin_port = htons(port);

	player2 = sendSockAddr;
}

void Networking::createPlayerUDPSocket(void) {
	int clientSd = socket(AF_INET, SOCK_DGRAM, 0);
	UDPSocket = clientSd;
}

string Networking::UDPread(bool noNeed, int size) {
	char message[size];
	struct sockaddr_in src_addr;
    unsigned int len = sizeof(src_addr);

	int bytesRead = recvfrom(UDPSocket, message, size, 0, (struct sockaddr*) &src_addr, &len);
	
	if (bytesRead == -1)
		return "-1";

	string s(message);
	if (noNeed == true) {
		if (s[0] == '2') {
			player2 = src_addr;
			check[0] = 1;
		} else if (s[0] == '3') {
        	player3 = src_addr;
			check[1] = 1;
		} else if (s[0] == '4') {
        	player4 = src_addr;
			check[2] = 1;
		}
	}

    return s;
}

void Networking::UDPwrite(int user, string msg) {
	if (user == 1) {
		sendto(UDPSocket, msg.c_str(), strlen(msg.c_str()), 0, (struct sockaddr *) &player2, sizeof(player2));
	} else if (user == 2) {
		sendto(UDPSocket, msg.c_str(), strlen(msg.c_str()), 0, (struct sockaddr *) &player3, sizeof(player3));	
	} else if (user == 3) {
		sendto(UDPSocket, msg.c_str(), strlen(msg.c_str()), 0, (struct sockaddr *) &player4, sizeof(player4));
	}
}

//set up sendsockaddr and return value
sockaddr_in Networking::storeDestinationAddress(void) {
	struct hostent *host = gethostbyname(IP.c_str());
    sockaddr_in sendSockAddr;
    bzero((char *)&sendSockAddr, sizeof(sendSockAddr));
    sendSockAddr.sin_family = AF_INET;
    sendSockAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr *)*host->h_addr_list));
    sendSockAddr.sin_port = htons(port);

	return sendSockAddr;	
}

void Networking::addTimeout(void) {
	struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 1500;
    setsockopt(UDPSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
}

int Networking::allChecked(void) {
	if (check[0] == 1) {
		if (check[1] == 1) {
			if (check[2] == 1) {
				return 3;
			} else {
				return 2;
			} 
		} else {
			return 1;
		}
	} else {
		return -1;
	}
}

void Networking::setUpHostTCPSocket(void) {

	sockaddr_in acceptSock;
    bzero((char*) &acceptSock, sizeof(acceptSock));  // zero out the data structure
    acceptSock.sin_family = AF_INET;   // using IP
    acceptSock.sin_addr.s_addr = htonl(INADDR_ANY); // listen on any address this computer has
    acceptSock.sin_port = htons(port);   // set the port to listen on

    int serverSd = guard(socket(AF_INET, SOCK_STREAM, 0), (char *)"Could not create TCP listening socket");
    int flags = guard(fcntl(serverSd, F_GETFL), (char *)"Could not get flags on TCP listening socket");
    guard(fcntl(serverSd, F_SETFL, flags | O_NONBLOCK), (char *)"Could not set TCP listening socket to be non-blocking");

    const int on = 1;
    setsockopt(serverSd, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(int));  // this lets us reuse the socket without waiting for the OS to recycle it

    // Bind the socket
    guard(bind(serverSd, (sockaddr*) &acceptSock, sizeof(acceptSock)), (char *)"could not bind");
    // bind the socket using the parameters we set earlier
    // Listen on the socket
    int n = 10;
    guard(listen(serverSd, n), (char *)"could not listen");  // listen on the socket and allow up to n connections to wait.
    // Accept the connection as a new socket

	hostSocket = serverSd;
}

int Networking::TCPHostAccept(int user) {
	sockaddr_in newsock;   // place to store parameters for the new connection
    socklen_t newsockSize = sizeof(newsock);

	int newSd = accept(hostSocket, (sockaddr *)&newsock, &newsockSize);
    if (newSd == -1) {
        if (errno == EWOULDBLOCK) {
            //cout << "No pending connections; sleeping for one second." << endl;
            sleep(1);
			return -1;
        } else {
            cerr << "error when accepting connection" << endl;
            exit(1);
        }
	} else {
		if (user == 1) {
			player2Socket = newSd;	
		} else if (user == 2) {
			player3Socket = newSd;
		} else if (user == 3) {
			player4Socket = newSd;
		}
	}
	return newSd;
}

void Networking::TCPPlayerConnect(void) {
	//Get DNS address
    struct hostent* host = gethostbyname(IP.c_str());

    //create socket
    sockaddr_in sendSock;
    bzero((char*) &sendSock, sizeof(sendSock));  // zero out the data structure
    sendSock.sin_family = AF_INET;   // using IP
    sendSock.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list)); // listen on any address this computer has
    sendSock.sin_port = htons(port);  // set the port to listen on

    int clientSd = guard(socket(AF_INET, SOCK_STREAM, 0), (char *)"Could not create TCP socket");

    //connect to server
    int connectStatus = connect(clientSd, (sockaddr*)&sendSock, sizeof(sendSock));

    if(connectStatus < 0){
        cerr << "failed to connect to server" << endl;

    } else {
        //cout << "Connection established" << endl;
		hostSocket = clientSd;
    }
}

string Networking::TCPread(int user) {
	char buf[BUFSIZE];
	if (user == 1)
		read(hostSocket, buf, BUFSIZE);
	else if (user == 2)
		read(player2Socket, buf, BUFSIZE);
	else if (user == 3)
		read(player3Socket, buf, BUFSIZE);
	else if (user == 4)
		read(player4Socket, buf, BUFSIZE);

	string s(buf);

	return s;
}

void Networking::TCPwrite(int user, string msg) {
	
	if (user == 1) {
		if (hostSocket != 0)
			write(hostSocket, msg.c_str(), strlen(msg.c_str()));
	} else if (user == 2) {
		if (player2Socket != 0)
			write(player2Socket, msg.c_str(), strlen(msg.c_str()));
	} else if (user == 3) {
		if (player3Socket != 0)
			write(player3Socket, msg.c_str(), strlen(msg.c_str()));
	} else if (user == 4) {
		if (player4Socket != 0)
			write(player4Socket, msg.c_str(), strlen(msg.c_str()));
	}
}

void Networking::TCPclose(int user) {
	if (user == 1) {
        if (hostSocket != 0)
            close(hostSocket);
    } else if (user == 2) {
        if (player2Socket != 0)
            close(player2Socket);
    } else if (user == 3) {
        if (player3Socket != 0)
            close(player3Socket);
    } else if (user == 4) {
        if (player4Socket != 0)
            close(player4Socket);
    }
}

int Networking::guard(int n, char * err) {
    if (n == -1) {
        perror(err); exit(1);
    }
    return n;
}


