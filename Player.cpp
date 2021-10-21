#include "Player.h"

Player::Player(int newPort, string newIP, string newUser) : 
	Game("", newPort, newIP),
	userName(newUser) { }

//starts up player threads for display, input, and networking
void Player::startGame(void) {
	pthread_create(&threads[0], NULL, &Player::userDisplay, this);
    pthread_create(&threads[1], NULL, &Player::userInput, this);
    pthread_create(&threads[2], NULL, &Player::network, this);

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    pthread_join(threads[2], NULL);
}

int Player::getPlayerNum(void) {
	return playerNum;
}

void Player::StartingMenuDisplay(void) {
	while (getState() == "StartingMenuState") {
        displayStartingMenu();
        sleep(1);
        system("clear");
    }
}

//switches between player displays
void * Player::userDisplay(void*arg) {
	Player * player = (Player*)arg;

    int stateChange = -1;
    while(player->getState() != "GameOverState") {
        if (player->getState() == "StartingMenuState" && stateChange != 0) {
            stateChange = 0;
            player->StartingMenuDisplay();
        } else if (player->getState() == "CourierRaceState" && stateChange != 1) {
            stateChange = 1;
            player->CourierRaceDisplay();
        } else if (player->getState() == "ColorMatchingState" && stateChange != 2) {
            stateChange = 2;
            player->ColorMatchingDisplay(player->getPlayerNum()-1);
        } else if (player->getState() == "SnowBallState" && stateChange != 3) {
            stateChange = 3;
            player->SnowBallDisplay(player->getPlayerNum()-1);
        }
    }

	cout << "-------SCORES-------" << endl;
    cout << player->getPlayer(0) << ": " << player->getEndGameScore(0) << endl;
    if (player->getPlayer(1) != "")
        cout << player->getPlayer(1) << ": " << player->getEndGameScore(1) << endl;
    if (player->getPlayer(2) != "")
        cout << player->getPlayer(2) << ": " << player->getEndGameScore(2) << endl;
    if (player->getPlayer(3) != "")
        cout << player->getPlayer(3) << ": " << player->getEndGameScore(3) << endl;
    cout << "--------------------" << endl;

    pthread_exit(NULL);
    return NULL;
}

//switches between player inputs
void * Player::userInput(void*arg) {
	Player * player = (Player*)arg;

    int stateChange = -1;
    while(player->getState() != "GameOverState") {
        if (player->getState() == "StartingMenuState" && stateChange != 0) {
            stateChange = 0;
        } else if (player->getState() == "CourierRaceState" && stateChange != 1) {
            stateChange = 1;
            player->CourierRaceInput(player->getPlayerNum()-1);
        } else if (player->getState() == "ColorMatchingState" && stateChange != 2) {
            stateChange = 2;
            player->ColorMatchingInput(player->getPlayerNum()-1);
        } else if (player->getState() == "SnowBallState" && stateChange != 3) {
            stateChange = 3;
            player->SnowBallInput();
        }
    }

    pthread_exit(NULL);
    return NULL;
}

//switches between player networking states
void * Player::network(void*arg) {
	Player* player = (Player*)arg;
	
    int stateChange = -1;
    while(player->getState() != "GameOverState") {
        if (player->getState() == "StartingMenuState" && stateChange != 0) {
            stateChange = 0;
            player->networkMenuState();
        } else if (player->getState() == "CourierRaceState" && stateChange != 1) {
            stateChange = 1;
			player->networkCourierRaceState();
        } else if (player->getState() == "ColorMatchingState" && stateChange != 2) {
            stateChange = 2;
			player->networkColorMatchingState();
        } else if (player->getState() == "SnowBallState" && stateChange != 3) {
			player->networkSnowBallState();
            stateChange = 3;
        }
    }
	pthread_exit(NULL);
    return NULL;
}

//runs the networking for the menu state
void Player::networkMenuState(void) {
    TCPPlayerConnect();

	string name = userName;
	name.push_back('=');
	
	TCPwrite(1, name);

	//3=mkilgore=poopy=poopydoopy=
	while(1) {
		string y = "";
		y = TCPread(1);
		char buf[BUFSIZE];
		strcpy(buf, y.c_str());
		if (buf[0] == 'e') { //host has started the game
			setState("CourierRaceState");
			break;
		} else { //a new user joined the lobby
			int numUsers = atoi(&buf[0]);
    		string x = "";
    		for(int i=0,j=2; i < numUsers; j++) {
        		if (buf[j] == '=') {
					if (x == userName) {
						playerNum = i+1;
					}
            		setPlayer(x, i);
            		i++;
            		x = "";
        		} else {
            		x.push_back(buf[j]);
        		}
    		}
		}
	}
	TCPclose(1);
}

//runs the networking for the courier race state
void Player::networkCourierRaceState(void) {
	createPlayerUDPSocket();
	addTimeout();
	setUpPlayerUDPSocket();
	//UDPwrite(1, "1=15");
	int y = -1;
	string s = "";
	while(getState() == "CourierRaceState") {
		if (getScore(playerNum-1) != y) {
			y = getScore(playerNum-1);
			if (y > 9)
				s = to_string(playerNum) + "=" + to_string(y);
			else
				s = to_string(playerNum) + "=0" + to_string(y);

			UDPwrite(1, s);
		}
		string buf = "";
        buf = UDPread();
		//05=06=09=11
        if (buf != "-1") {
            char w[2];
			//player1
            w[0] = buf[0];
            w[1] = buf[1];
			if (atoi(w) > getScore(0))
            	setScore(0, atoi(w));
			//player2
			w[0] = buf[3];
			w[1] = buf[4];
			if (atoi(w) > getScore(1))
			setScore(1, atoi(w));
			//player3
			w[0] = buf[6];
			w[1] = buf[7];
			if (atoi(w) > getScore(2))
				setScore(2, atoi(w));
			//player4
			w[0] = buf[9];
			w[1] = buf[10];
			if (atoi(w) > getScore(3))
				setScore(3, atoi(w));
        }
	}		
}

//runs the networking for the color matching state
void Player::networkColorMatchingState(void) {
	TCPPlayerConnect();			
	string msg = "";
	while(getState() == "ColorMatchingState") {
		msg = "";
		int x = getScore(playerNum-1);
		msg += to_string(playerNum-1);
		msg.push_back('=');
		if (x == -1)
			msg += "0";
		else
			msg += "1";
		msg.push_back('=');
	
		UDPwrite(1, msg);


		string y = "";
    	y = UDPread();
		if (y != "-1") {
			//cout << y << endl;
			if (y[0] == '0' && playerNum-1 != 0)
				setScore(0, -1);
			if (y[1] == '0' && playerNum-1 != 1)
				setScore(1, -1);
			if (y[2] == '0' && playerNum-1 != 2)
				setScore(2, -1);
			if (y[3] == '0' && playerNum-1 != 3)
				setScore(3, -1);
		}
	}
	msg = "";
	msg += to_string(playerNum-1);
    msg.push_back('=');
	msg += "0";
	UDPwrite(1, msg);
}

// runs the networking for the snow ball state
void Player::networkSnowBallState(void) {
	//2=50280284
	int done = 0;
	string y = "";
	while (done == 0) {
		
		if (getSnowBallScores(playerNum-1) != 0.0) {
			TCPPlayerConnect();

    		string msg = "";
			msg += to_string(playerNum-1);
    		msg.push_back('=');
			msg += to_string(getSnowBallScores(playerNum-1));

    		TCPwrite(1, msg);

    		//3=mkilgore=poopy=poopydoopy=
       		y = TCPread(1);
			done += 1;
		}
	}

	setEndGameScore(0, (int)(y[1]-'0'));
    setEndGameScore(1, (int)(y[3]-'0'));	
    setEndGameScore(2, (int)(y[5]-'0'));
    setEndGameScore(3, (int)(y[7]-'0'));
	setState("GameOverState");

}










