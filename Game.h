#ifndef GAME_H
#define GAME_H

#include <unistd.h>
#include "Networking.h"

#define NUM_THREADS 3

using namespace std;

enum State {
    StartingMenuState,
    CourierRaceState,
    ColorMatchingState,
    SnowBallState,
	GameOverState
};

class Game : public Networking {
    private:
        State gameState;
        string players[4];
		int score[4];
		int endGameScore[4];
		int timeLeft;
		char colorGuess;
		int color;
		double snowBallScores[4];
    public:
		//constructor and destructor
        Game(string host, int newPort, string newIP);

		virtual void startGame(void) = 0;

		//getters and setters
        string getPlayer(int playerNum);
        string getState(void);
		int getScore(int player);
		int getEndGameScore(int player);
		int getSnowBallScores(int player);

        void setPlayer(string player, int playerNum);
        void setState(string State);
        void setScore(int player, int x);
		void setEndGameScore(int player, int x);
		void setSnowBallScores(int player, double x);

		//user displays
		void displayStartingMenu(void);
		void displayPlayerInCourierRace(int player);
		void courierGameDisplay(void);
		void ColorMatchingGameDisplay(int player, bool justAnUpdate);
		int displayColor(void);
		

		//user input
		void enterKeyPressed(void);
		char getch(bool echoOrNah);		
		

        //displays
        virtual void StartingMenuDisplay(void) = 0;
        void CourierRaceDisplay(void);
        void ColorMatchingDisplay(int player);
        void SnowBallDisplay(int player);

		//checking
		bool allPlayersDone(void);
		bool someoneLost(void);

		//inputs
		//virtual void StartingMenuInput(void) = 0;
		void CourierRaceInput(int player);
		void ColorMatchingInput(int player);
		void SnowBallInput(void);

		//networking hub is the same for host and users
		void networkCummunication(void);	
		//network states are all different for host vs users
		virtual void networkMenuState(void) = 0;
		virtual void networkCourierRaceState(void) = 0;
		virtual void networkColorMatchingState(void) = 0;
		virtual void networkSnowBallState(void) = 0;
};

#endif
