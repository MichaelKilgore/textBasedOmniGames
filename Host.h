#include "Game.h"

class Host : public Game {
	private:
		pthread_t threads[NUM_THREADS];	
		int numPlayers;

	public:
		Host(string host, int newPort);

		void startGame(void);

		void StartingMenuDisplay(void);
		void StartingMenuInput(void);

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
