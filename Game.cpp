#include "Game.h"

//constructor
Game::Game(string host, int newPort, string newIP):
	Networking(newPort, newIP),
    gameState(StartingMenuState),
	timeLeft(100) { 
	players[0] = host;
	for (int i=1;i<4;i++)
		players[i] = "";
	for (int i=0;i<4;i++)
		score[i] = 0;
	for (int i=0;i<4;i++)
		endGameScore[i] = 0;		
	for (int i=0;i<4;i++)
		snowBallScores[i] = 0;
}

// getters and setters

string Game::getPlayer(int playerNum) {
    return players[playerNum];
}

string Game::getState(void) {
    if (gameState == StartingMenuState)
        return "StartingMenuState";
    else if (gameState == CourierRaceState)
        return "CourierRaceState";
    else if (gameState == ColorMatchingState)
        return "ColorMatchingState";
    else if (gameState == SnowBallState)
        return "SnowBallState";
	else
		return "GameOverState";
}

int Game::getScore(int player) {
	if (player < 4)
        return score[player];
    return -1;
}

int Game::getEndGameScore(int player) {
	if (player < 4)
		return endGameScore[player];
	return -1;
}

int Game::getSnowBallScores(int player) {
	if (player < 4)
		return snowBallScores[player];
	return -1;
}

void Game::setPlayer(string player, int playerNum) {
    if (playerNum < 4)
        players[playerNum] = player;
}

void Game::setState(string State) {
    if (State == "StartingMenuState")
        gameState = StartingMenuState;
    else if (State == "CourierRaceState")
        gameState = CourierRaceState;
    else if (State == "ColorMatchingState")
        gameState = ColorMatchingState;
    else if (State == "SnowBallState")
        gameState = SnowBallState;
	else if (State == "GameOverState")
		gameState = GameOverState;
}

void Game::setScore(int player, int x) {
	if (player < 4)
        score[player] = x;
}

void Game::setEndGameScore(int player, int x) {
	if (player < 4)
		endGameScore[player] = x;
}
	
void Game::setSnowBallScores(int player, double x) {
	if (player < 4)
		snowBallScores[player] = x;
}

//displays the starting menu.
void Game::displayStartingMenu(void) {
	cout << "IP: " << "127.0.0.1" << endl;
    cout << "Port Number: " << getPort() << endl;
    cout << "-----Lobby-----" << endl;
    if (getPlayer(0) != "") {
        cout << "Player 1: " << getPlayer(0) << endl;
    }
    if (getPlayer(1) != "") {
        cout << "Player 2: " << getPlayer(1) << endl;
    }
    if (getPlayer(2) != "") {
        cout << "Player 3: " << getPlayer(2) << endl;
    }
    if (getPlayer(3) != "") {
        cout << "Player 4: " << getPlayer(3) << endl;
    }
    cout << "---------------" << endl;
}

//displays a players current location in the courier race for client playing
void Game::displayPlayerInCourierRace(int player) {
    int i = 0;
    if (getPlayer(player) != "") {
        cout << getPlayer(player);
        int extra = 20-getPlayer(player).length();
        while (i != extra) {
            cout << " ";
            i++;
        }
        cout << "|";
        i = 0;
        while (i != getScore(player)) {
            cout << " ";
            i++;
        }
        cout << player+1;
        while (i != 31) {
            cout << " ";
            i++;
        }
        cout << "|" << endl;
    }
}

//displays the ui for the courier game
void Game::courierGameDisplay(void) {
    cout << "-------------------- ----------Courier Race----------" << endl;
    displayPlayerInCourierRace(0);
    displayPlayerInCourierRace(1);
    displayPlayerInCourierRace(2);
    displayPlayerInCourierRace(3);
    cout << "-------------------- --------------------------------" << endl;
}

//displays the game display for the color matching game
void Game::ColorMatchingGameDisplay(int player, bool justAnUpdate) {
	static int x = 0;
	static int lemon = 0;
	if (justAnUpdate == false) {
		if (lemon == 1) {
			if (colorGuess == 'r') {
        		if (x != 2)
            		score[player] = -1;
    		} else if (colorGuess == 'g') {
        		if (x != 0)
       	     		score[player] = -1;
    		} else if (colorGuess == 'b') {
        		if (x != 1)
            		score[player] = -1;
    		} else {
				score[player] = -1;	
			}
		} else {
				lemon = 1;
		}
	}
	cout << "-------------------- ---------Color Matching---------" << endl;
	cout << "Players alive: ";
	if (score[0] != -1)
		cout << players[0] << " ";
	if (score[1] != -1)
		cout << players[1] << " ";
	if (score[2] != -1)
		cout << players[2] << " ";
	if (score[3] != -1)
		cout << players[3] << " ";
	cout << endl;
	cout << "Players dead:  ";
	if (score[0] == -1)
        cout << players[0] << " ";
    if (score[1] == -1)
        cout << players[1] << " ";
    if (score[2] == -1)
        cout << players[2] << " ";
    if (score[3] == -1)
        cout << players[3] << " ";
	cout << endl;
	if (justAnUpdate == false)
		x = displayColor();
	if (x == 2) {
		cout << "RED" << endl;
	} else if (x == 1) {
		cout << "BLUE" << endl;
	} else if (x == 0) {
		cout << "GREEN" << endl;
	}
	cout << "-------------------- --------------------------------" << endl;
	if (colorGuess == 'r' || colorGuess == 'g' || colorGuess == 'b')
		cout << colorGuess << endl;
}

//displays a single color randomly
int Game::displayColor(void) {
	int x = rand() % 3;
	if (x == 2)
		return 2;
	if (x == 1)
		return 1;
	if (x == 0)
		return 0;
	return 0;
}

//checks for enter key pressed by user
void Game::enterKeyPressed(void) {
	cin.ignore();
}

// performs a specialized cin << x; where the user is not required to press enter and the key pressed is not echoed to the screen.
//https://stackoverflow.com/questions/421860/capture-characters-from-standard-input-without-waiting-for-enter-to-be-pressed
char Game::getch(bool echoOrNah=false) {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
	if (echoOrNah == false)
    	old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror ("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror ("tcsetattr ~ICANON");
    return (buf);
}

//Keeps the page updated if the user is afk.
void Game::CourierRaceDisplay(void) {
	static struct timeval start, end;	
	unsigned int microsecond = 250000;
	gettimeofday(&start, NULL);
	while (getState() == "CourierRaceState") {
		system("clear");
		courierGameDisplay();
		if (timeLeft < 6)
            cout << "Game Over in " << timeLeft << endl;
		if (timeLeft == 0)
			setState("ColorMatchingState");
		if (allPlayersDone())
			setState("ColorMatchingState");
		//check if all users have finished the game
		usleep(microsecond);//sleeps for 0.5 seconds
		gettimeofday(&end, NULL);			
		timeLeft = (int)(end.tv_sec - start.tv_sec);
		timeLeft = 60-timeLeft;
	}
	
}

//keeps color matching display updated
void Game::ColorMatchingDisplay(int player) {
	static int x = 1;
	double y = 10/x+0.5;
	unsigned int microsecond = 0;
	score[0] = 0, score[1] = 0, score[2] = 0, score[3] = 0;
	timeLeft = 100;
	usleep(1000000);//sleeps for 0.5 seconds
	while (getState() == "ColorMatchingState") {
		y = 10/x+1;
		if (y <= 5)
        	microsecond = 1000000*y;
    	else
        	microsecond = 5000000;

		system("clear");
		ColorMatchingGameDisplay(player, false);
		x++;
		if (someoneLost()) {
			setState("SnowBallState");
		}
		unsigned int w = microsecond/10;
		int counter = 0;
		while (counter != 10) {	
			usleep(w);//sleeps for 0.5 seconds
			system("clear");
			ColorMatchingGameDisplay(player, true);
			counter++;
			if (someoneLost()) {
            	setState("SnowBallState");
				break;
        	}
		}
	}
}

// displays the snow ball game and takes user input.
void Game::SnowBallDisplay(int player) {
	static struct timeval start, end;
    unsigned int microsecond = 250000;
    gettimeofday(&start, NULL);
	int x = rand() % 10;
	int y = rand() % 10;
	int ans;
	while (getState() == "SnowBallState") {
		system("clear");
		cout << "What does " << x << " * " << y << " = " << endl;
		string line = "";
		cin >> ans;

		if (ans == x*y) {
			gettimeofday(&end, NULL);
			cout << "Correct!" << endl;
			int l = (int)(end.tv_sec * 1000000 + end.tv_usec) - (int)(start.tv_sec * 1000000 + start.tv_usec);
			snowBallScores[player] = l;
			while(getState() == "SnowBallState") {
				if (snowBallScores[0] != 0.0 && snowBallScores[1] != 0.0 && snowBallScores[2] != 0.0 && snowBallScores[3] != 0.0)
					setState("GameOverState");
				if (snowBallScores[0] != 0.0 && snowBallScores[1] != 0.0 && snowBallScores[2] != 0.0 && players[3] == "")
					setState("GameOverState");
				if (snowBallScores[0] != 0.0 && snowBallScores[1] != 0.0 && players[2] == "")
					setState("GameOverState");
				if (snowBallScores[0] != 0.0 && players[1] == "") {
					setState("GameOverState");
				}
				usleep(250000);
			}
		} else {
			cout << "Incorrect!" << endl;
			usleep(250000);
		}
	}
}

//a checker for seeing if all players are done playing
bool Game::allPlayersDone(void) {
	static int one=0, two=0, three=0, four=0;
	int prevCheck[4] = {0,0,0,0};
	int curWinners[4] = {0,0,0,0};
	static int winners = 0;
	int counter = 0;
	prevCheck[0] = one;
	prevCheck[1] = two;
	prevCheck[2] = three;
	prevCheck[3] = four;

	if (score[0] == 31) {
		one = 1;	
	} if (score[1] == 31) {
		two = 1;	
	} if (score[2] == 31) {
		three = 1;
	} if (score[3] == 31) {
		four = 1;
	}

	if (prevCheck[0] == 0 && one == 1) {
    	curWinners[0] = 1;
        counter++;
    }
    if (prevCheck[1] == 0 && two == 1) {
        curWinners[1] = 1;
        counter++;
    }
    if (prevCheck[2] == 0 && three == 1) {
        curWinners[2] = 1;
        counter++;
    }
    if (prevCheck[3] == 0 && four == 1) {
        curWinners[3] = 1;
        counter++;
    }

	if (winners == 0) {

		if (counter == 1) {
			winners += 1;
			for (int i=0;i<4;i++) {
				if (curWinners[i] == 1)
					endGameScore[i] += 3;
			}
		} else if (counter == 2) {
			winners += 2;
            for (int i=0;i<4;i++) {
                if (curWinners[i] == 1)
                    endGameScore[i] += 2;
            }
		} else if (counter == 3) {
			winners += 3;
			for (int i=0;i<4;i++) {
                if (curWinners[i] == 1)
                    endGameScore[i] += 1;
            }
		} else if (counter == 4) {
			winners += 4;
		}
	} else if (winners == 1) {

		if (counter == 1) {
            winners += 1;
            for (int i=0;i<4;i++) {
                if (curWinners[i] == 1)
                    endGameScore[i] += 2;
            }
        } else if (counter == 2) {
            winners += 2;
            for (int i=0;i<4;i++) {
                if (curWinners[i] == 1)
                    endGameScore[i] += 1;
            }
        } else if (counter == 3) {
            winners += 3;
        }
	} else if (winners == 2) {

		if (counter == 1) {
            winners += 1;
            for (int i=0;i<4;i++) {
                if (curWinners[i] == 1)
                    endGameScore[i] += 2;
            }
        } else if (counter == 2) {
			winners += 2;
		}
	} else if (winners == 3) {

        if (counter == 1) {
            winners += 1;
        }
	}

	if (winners == 4 && players[3] != "" && players[2] != "" && players[1] != "" && players[0] != "")
		return true;
	else if (winners == 3 && players[3] == "")
		return true;
	else if (winners == 2 && players[2] == "")
		return true;
	else if (winners == 1 && players[1] == "")
		return true;
	return false;
}

//a checker for checking if someonelost
bool Game::someoneLost(void) {
	static int one=0, two=0, three=0, four=0;
	static int scoreGiven=0;
	int prevCheck[4] = {0,0,0,0};
	int curWinners[4] = {0,0,0,0};
	static int counter=0;
	prevCheck[0] = one;
    prevCheck[1] = two;
    prevCheck[2] = three;
    prevCheck[3] = four;

	if (score[0] == -1) {
        one = 1;
    }
    if (score[1] == -1) {
        two = 1;
    }
    if (score[2] == -1) {
        three = 1;
    }
    if (score[3] == -1) {
        four = 1;
    }

    if (prevCheck[0] == 0 && one == 1) {
        curWinners[0] = 1;
        counter++;
    }
    if (prevCheck[1] == 0 && two == 1) {
        curWinners[1] = 1;
        counter++;
    }
    if (prevCheck[2] == 0 && three == 1) {
        curWinners[2] = 1;
        counter++;
    }
    if (prevCheck[3] == 0 && four == 1) {
        curWinners[3] = 1;
        counter++;
    }


	if (counter == 4) {
		endGameScore[0] += 0;
		endGameScore[1] += 0;
		endGameScore[2] += 0;
		endGameScore[3] += 0;
	} else if (counter == 3) {
		for (int i=0;i<4;i++) {
			if (curWinners[i] == 1)
				endGameScore[i] += scoreGiven;
		}
		scoreGiven += 3;
	} else if (counter == 2) {
		for (int i=0;i<4;i++) {
            if (curWinners[i] == 1)
                endGameScore[i] += scoreGiven;
        }
        scoreGiven += 2;
	} else if (counter == 1) {
        for (int i=0;i<4;i++) {
            if (curWinners[i] == 1)
                endGameScore[i] += scoreGiven;
        }
        scoreGiven += 1;
    }
	if (one == 1 && two == 1 && three == 1 && four == 1)
		return true;
	if (one == 1 && two == 1 && three == 1 && players[3] == "")
		return true;
	if (one == 1 && two == 1 && players[2] == "")
		return true;
	if (one == 1 && players[1] == "")
		return true;
	return false;

}

//checks for input in the courier race
void Game::CourierRaceInput(int player) {
	int i = 1;
    char prev = 'w';
	char input;

    while(getState() == "CourierRaceState") {
        courierGameDisplay();
		if (timeLeft < 6)
            cout << "Game Over in " << timeLeft << endl;

		if (getScore(player) != 31)
        	input = getch();
		else
			usleep(100000);//sleeps for 0.5 seconds		

        if (input == 'd' && prev == 'a') {
            i += 1;
        }
        if (i%2==0 && getScore(player) != 31) {
            setScore(player, getScore(player)+1);
            i++;
        }
        prev = input;
        system("clear");
    }
}

//checks for input in the color matching game
void Game::ColorMatchingInput(int player) {
	char w;
	while(getState() == "ColorMatchingState") {
		w = getch(true);
		if (w == 'r' || w == 'g' || w == 'b')
			colorGuess = w;
	}
}


void Game::SnowBallInput(void) {

}


//switches between the different networking states based on the game state
void Game::networkCummunication(void) {
	int stateChange = -1;
    while(getState() != "GameOverState") {
        if (getState() == "StartingMenuState" && stateChange != 0) {
            stateChange = 0;
            networkMenuState();
        } else if (getState() == "CourierRaceState" && stateChange != 1) {
            stateChange = 1;
            networkCourierRaceState();
        } else if (getState() == "ColorMatchingState" && stateChange != 2) {
            stateChange = 2;
            networkColorMatchingState();
        } else if (getState() == "SnowBallState" && stateChange != 3) {
            stateChange = 3;
            networkSnowBallState();
        }
    }
}

