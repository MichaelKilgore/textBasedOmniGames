#include "Game.h"

class Player : public Game {
    private:
		pthread_t threads[NUM_THREADS];
		string userName;
		int playerNum;
    public:
		Player(int newPort, string newIP, string newUser);

		int getPlayerNum(void);

		void startGame(void);

		void StartingMenuDisplay(void);

        //display thread
        static void *userDisplay(void*arg);
        //user input thread
        static void *userInput(void*arg);
        //networking thread
        static void *network(void*arg);

        //networking states
        void networkMenuState(void);
        void networkCourierRaceState(void);
        void networkColorMatchingState(void);
        void networkSnowBallState(void);
};
