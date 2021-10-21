#include "Host.h"

//constructor
Host::Host(string host, int newPort) : 
	Game(host, newPort, "") { 
	numPlayers = 0;
}

//creates threads for the game
void Host::startGame(void) {
    pthread_create(&threads[0], NULL, &Host::userDisplay, this);
	pthread_create(&threads[1], NULL, &Host::userInput, this);
    pthread_create(&threads[2], NULL, &Host::network, this);

	pthread_join(threads[0], NULL);
	pthread_join(threads[1], NULL);
	pthread_join(threads[2], NULL);
}

//starting menu display is slightly different for host
void Host::StartingMenuDisplay(void) {
	while (getState() == "StartingMenuState") {
		displayStartingMenu();
		cout << "Press enter to start the game" << endl;
		sleep(1);
		system("clear");
	}
}

// gets hosts user input for starting menu state
void Host::StartingMenuInput(void) {
	enterKeyPressed();
	setState("CourierRaceState");	
}

//switches between different states for game
void * Host::userDisplay(void*arg) {
	Host * host = (Host*)arg;

	int stateChange = -1;	
	while(host->getState() != "GameOverState") {
		if (host->getState() == "StartingMenuState" && stateChange != 0) {
            stateChange = 0;
            host->StartingMenuDisplay();
        } else if (host->getState() == "CourierRaceState" && stateChange != 1) {
            stateChange = 1;
            host->CourierRaceDisplay();
        } else if (host->getState() == "ColorMatchingState" && stateChange != 2) {
            stateChange = 2;
            host->ColorMatchingDisplay(0);
        } else if (host->getState() == "SnowBallState" && stateChange != 3) {
            stateChange = 3;
            host->SnowBallDisplay(0);
        }
	}
	cout << "-------SCORES-------" << endl;
	cout << host->getPlayer(0) << ": " << host->getEndGameScore(0) << endl;
	if (host->getPlayer(1) != "")
    	cout << host->getPlayer(1) << ": " << host->getEndGameScore(1) << endl;
	if (host->getPlayer(2) != "")
    	cout << host->getPlayer(2) << ": " << host->getEndGameScore(2) << endl;
	if (host->getPlayer(3) != "")
    	cout << host->getPlayer(3) << ": " << host->getEndGameScore(3) << endl;
	cout << "--------------------" << endl;

	pthread_exit(NULL);
    return NULL;
}

//switches between user input states
void * Host::userInput(void*arg) {
	Host * host = (Host*)arg;
	
    int stateChange = -1;
    while(host->getState() != "GameOverState") {
        if (host->getState() == "StartingMenuState" && stateChange != 0) {
            stateChange = 0;
			host->StartingMenuInput();
        } else if (host->getState() == "CourierRaceState" && stateChange != 1) {
            stateChange = 1;
			host->CourierRaceInput(0);
        } else if (host->getState() == "ColorMatchingState" && stateChange != 2) {
            stateChange = 2;
			host->ColorMatchingInput(0);
        } else if (host->getState() == "SnowBallState" && stateChange != 3) {
            stateChange = 3;
			host->SnowBallInput();
        }
    }	

	pthread_exit(NULL);
    return NULL;
}

//switches between networking states
void * Host::network(void*arg) {
	Host* host = (Host*)arg;

    int stateChange = -1;
    while(host->getState() != "GameOverState") {
        if (host->getState() == "StartingMenuState" && stateChange != 0) {
            stateChange = 0;
            host->networkMenuState();
        } else if (host->getState() == "CourierRaceState" && stateChange != 1) {
            stateChange = 1;
			host->networkCourierRaceState();
        } else if (host->getState() == "ColorMatchingState" && stateChange != 2) {
            stateChange = 2;
			host->networkColorMatchingState();
        } else if (host->getState() == "SnowBallState" && stateChange != 3) {
            stateChange = 3;
			host->networkSnowBallState();
        }
    }

	pthread_exit(NULL);
    return NULL;
}

//networking code for menu state
void Host::networkMenuState(void) {
	setUpHostTCPSocket();

    int i = 0;
    int userIndex = 1;
	int index = 0;
	int x = 0;

	usleep(1000000);
    while(getState() == "StartingMenuState") {
	    string userNames = "";
		if (userIndex != 4) {
			x = TCPHostAccept(userIndex);
			if (x != -1) {
				string buf = "";
				buf = TCPread(userIndex+1);

				string player = "";
            	while(buf[i] != '=') {
                	player.push_back(buf[i]);
                	i++;
            	}

				setPlayer(player, userIndex);

				userNames += to_string(userIndex+1);
				userNames += "=";
				index = 0;
				while(index != userIndex+1) {
					userNames += getPlayer(index);
					userNames += "=";
					index += 1;	
				}
				index = 0;

				//TODO: Send message to all users because the list of users updated.
				int prevSd = 0;
				for (int i = 1; i < 4; i++) {
					TCPwrite(i+1, userNames);
				}
				userNames = "";
            	userIndex += 1;
            	i = 0;
			}
		}
		numPlayers = userIndex;
    }

	//host started the game so I now must send the message to everyone that we are starting.
	i = 1;
	string msg = "e=";
	int Sd = 0;
	while(i != userIndex) {
		TCPwrite(i+1, msg);
		TCPclose(i+1);
		i++;
	}
}

//networking code for courier race
void Host::networkCourierRaceState(void) {
	setUpHostUDPSocket();
	addTimeout();
	//message sent from users
	//1=16 ie playernumber=playerscore
	//0=04
	//message sent from host to user
	//score of each user
	//05=07=09=21
	int x = 0, i = 1, userIndex = 0;
	string response = "";
	char buf[BUFSIZE];
	while (getState() == "CourierRaceState") {
		string buf = "";
		//host listens for messages
		//have a timeout, so if the users are not updating their score then I can still send my new score.
		//int bytesRead = read(newSd, buf, BUFSIZE);
		buf = UDPread();
		if (buf != "-1") {
			char w[2];
			w[0] = buf[2];
			w[1] = buf[3];
			if (w[0] != '0')
				setScore((buf[0]-'0')-1, atoi(w));		
			if (w[0] == '0')
				setScore((buf[0]-'0')-1, w[1]-'0');
		}
		
		//send reply to users
		//16=17=14=13
		if (allChecked() != -1) {
			string msg = "";
            for (int j = 0; j < 4; j++) {
            	if (getScore(j) > 9) {
                	msg = msg + to_string(getScore(j));
                } else {
                	msg = msg + "0" + to_string(getScore(j));
                }
                    msg = msg + "=";
            }
			i = 1;
    		while(i != allChecked()+1) {
        		UDPwrite(i, msg);
        		i++;
    		}		
			msg = "";
			i = 1;
		}
	}
}

//networking code for color matching state
void Host::networkColorMatchingState(void) {
	//kill old sockets and run connect for new socket.
	int x = 0, i = 1, userIndex = 0;
    string response = "";

	while(getState() == "ColorMatchingState") {
		string buf = UDPread(false, 4);
        if (buf != "-1") {
			//cout << buf << endl;	
			//cout << "here!" << endl;
			//3=1
            int player = buf[0] - '0';

           	int score = buf[2] - '0';
			if (score == 0)
            	setScore(player, -1);
			else
				setScore(player, 1);
            userIndex += 1;
        }

		string scores = "";
		for (int i=0; i<4; i++) {
			if (getScore(i) == -1)
				scores.push_back('0');
			else
				scores.push_back('1');
		}
		i = 1;
		while(i != allChecked()+1) {	
                UDPwrite(i, scores);
                i++;
            }
		i = 1;
		buf = "";
	}
}

//networking code for snow ball state
void Host::networkSnowBallState(void) {
	int userIndex = 1;
	int x = 0;
	//setUpHostTCPSocket();
	bool done = false;
	while (done == false) {
		if (userIndex != numPlayers) {
        	x = TCPHostAccept(userIndex);
			if (x != -1) {
				string buf = "";
				buf = TCPread(userIndex+1);
				if (buf[0] == '0' || buf[0] == '1' || buf[0] == '2' || buf[0] == '3') {
				//cout << "here" << endl;
				//cout << buf << endl;
				int player = buf[0] - '0';
			
				double score = stoi(buf.substr(2));
				setSnowBallScores(player, score);
				userIndex += 1;
				}
			} else {
				usleep(100000);
			}
    	} else {
			done = true;
		}
	}
	while(getSnowBallScores(0) == 0.0) {
		usleep(250000);
	}
	int best = 0;
	double bestScore = 999999999999;
	for (int i=0;i<4;i++) {
		if (getSnowBallScores(i) != 0.0) {
			if (getSnowBallScores(i) < bestScore) {
				bestScore = getSnowBallScores(i);
				best = i;
			}
		}
	}
	setEndGameScore(best, getEndGameScore(best)+2);
	string xx = "";
	xx += "=";
	for (int i=0; i<4; i++) {
		xx += to_string(getEndGameScore(i));
		xx += "=";
	}
	//cout << xx << endl;
	for (int i=1; i<4; i++) {
		if (getPlayer(i) != "") {
			TCPwrite(i+1, xx);
		}
	}
}


