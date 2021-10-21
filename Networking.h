#include <iostream>
#include <string>

//server based stuff
#include <sys/types.h>   // socket, bind
#include <sys/socket.h>  // socket, bind, listen, inet_ntoa
#include <netinet/in.h>  // htonl, htons, inet_ntoa
#include <arpa/inet.h>   // inet_ntoa
#include <netdb.h>       // gethostbyname
#include <unistd.h>      // read, write, close
#include <strings.h>     // bzero
#include <netinet/tcp.h> // SO_REUSEADDR
#include <sys/uio.h>     // writev

//time based stuff
//#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>

//
#include <unistd.h>
#include <termios.h>

#include <fcntl.h>
#include <errno.h>

#define BUFSIZE 200

using namespace std;

class Networking {
	private:
		int port;	
		string IP;	

		//UDP
		int UDPSocket;
		//only used by the host
		int check[3];
		sockaddr_in player2;
		sockaddr_in player3;
		sockaddr_in player4;

		//TCP	
		//NOTE: For non-hosts the hostSocket is the socket they use to connect to the host.
		int hostSocket;
		int player2Socket;
		int player3Socket;
		int player4Socket;

	public:
		Networking(int newPort=0, string newIP="");

		int getPort(void);
		string getIP(void);

		int getHostSocket(void);

		//UDP
		void setUpHostUDPSocket(void);
		void setUpPlayerUDPSocket(void);
		void createPlayerUDPSocket(void);
		string UDPread(bool noNeed=true, int size=BUFSIZE);
		void UDPwrite(int user, string msg);
		sockaddr_in storeDestinationAddress(void);
		void addTimeout(void);
		int allChecked(void);

		//TCP
		void setUpHostTCPSocket(void);
		int TCPHostAccept(int user);
		void TCPPlayerConnect(void);
		string TCPread(int user);
		void TCPwrite(int user, string msg);
		void TCPclose(int user);

		//gaurd
		int guard(int n, char * err);
		
};
