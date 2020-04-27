//Faith Goddard
#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <stdio.h>
#include <fstream>
#include <semaphore.h>

using namespace std;

#define NUM_THREADS 5
#define _OPEN_THREADS

// shared variables
int Team1Score = 0;
int Team2Score = 0;
const int PlayerCount = 4;

pthread_mutex_t diceController = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t dealer_cv, Player1_cv, Player2_cv, Player3_cv, Player4_cv;
sem_t dealer_semaphore;
ofstream myfile;
bool play = true;


struct Dice
{
    int rollvalue;
    bool winner;
    bool ready;
    bool end;
};
struct Player
{
    int roll1 = 0;
    int roll2 = 0;
    int team;
    string letter;
    int totalrollvalue = 0;
    bool ready = false;
};
struct CardDealer
{
    string name;
    int TeamValue[2];

};
Player Player1, Player2, Player3, Player4;
Dice dice;
CardDealer theDealer;

int getDiceRoll()
{
    int roll;
    int min = 1;
    int max = 6;
    roll = rand() % (max - min + 1) + min;
    return roll;
}
int RandStart()
{
    int starter;
    int min = 1;
    int max = 4;
    starter = rand() % (max - min + 1) + min;
    return starter;
}

bool GameStatCheck()
{
    if (Player1.totalrollvalue == Player3.totalrollvalue && Player1.totalrollvalue != 0 && Player3.totalrollvalue != 0)
    {
        dice.winner = true;
        dice.ready = false;
        myfile << "Dealer: The winning team is A and C" << endl;
        pthread_cond_signal(&dealer_cv);
        return false;
    }
    if (Player2.totalrollvalue == Player4.totalrollvalue && Player2.totalrollvalue != 0 && Player4.totalrollvalue != 0)
    {
        dice.winner = true;
        dice.ready = false;
        myfile << "Dealer: The winning team is B and D" << endl;
        pthread_cond_signal(&dealer_cv);
        return false;
    }
    return true;
}

void* dealer(void* arg)
{
        pthread_mutex_lock(&diceController);
        //Wait for threads to be ready
        while (Player1.ready == false || Player2.ready == false || Player3.ready == false || Player4.ready == false)
        {
            pthread_cond_wait(&dealer_cv, &diceController);
        }

        int starter = RandStart();
        switch (starter)
        {
            case 1:
                //cout << "Dealer: Player A goes first " << endl;
                pthread_cond_signal(&Player1_cv);
                break;
            case 2:
                //cout << "Dealer: Player B goes first " << endl;
                pthread_cond_signal(&Player2_cv);
                break;
            case 3:
                //cout << "Dealer: Player C goes first " << endl;
                pthread_cond_signal(&Player3_cv);
                break;
            case 4:
                //cout << "Dealer: Player D goes first " << endl;
                pthread_cond_signal(&Player4_cv);
                break;
            default:
                //cout << "There is an error" << endl;
                break;
        }
        pthread_mutex_unlock(&diceController);
        //cout << "Dealer is waiting on Players \n";
        pthread_cond_wait(&dealer_cv, &diceController);
        //cout << "Dealer Done Waiting \n";
        pthread_mutex_unlock(&diceController);
        pthread_exit(0);


    
}

void* player1(void* arg)
{
    Player1.ready = true;
    pthread_cond_signal(&dealer_cv);
    pthread_cond_wait(&Player1_cv, &diceController);
    while (play == true)
    {
        Player1.roll1 = getDiceRoll();
        Player1.roll2 = getDiceRoll();
        Player1.totalrollvalue = Player1.roll1 + Player1.roll2;

        myfile << "Player A: gets " << Player1.roll1 << " and " << Player1.roll2 << " with a sum " << Player1.totalrollvalue << endl;
        cout << "Player A: " << Player1.roll1 << " " << Player1.roll2 << endl;
        
        //see if this wins the game
        play = GameStatCheck();
        
        //Pass to Player 2
        pthread_cond_signal(&Player2_cv);
        pthread_mutex_unlock(&diceController);
        pthread_cond_wait(&Player1_cv, &diceController);
        
    }
    pthread_mutex_unlock(&diceController);
    pthread_cond_signal(&Player2_cv);
    pthread_exit(0);
}

void* player2(void* arg)
{
    Player2.ready = true;
    pthread_cond_signal(&dealer_cv);
    pthread_cond_wait(&Player2_cv, &diceController);
    while (play == true)
    {
        Player2.roll1 = getDiceRoll();
        Player2.roll2 = getDiceRoll();
        Player2.totalrollvalue = Player2.roll1 + Player2.roll2;

        myfile << "Player B: gets " << Player2.roll1 << " and " << Player2.roll2 << " with a sum " << Player2.totalrollvalue << endl;
        cout << "Player B: " << Player2.roll1 << " " << Player2.roll2 << endl;
        
        //see if this wins the game
        play = GameStatCheck();

        //Pass to Player 3
        pthread_cond_signal(&Player3_cv);
        pthread_mutex_unlock(&diceController);
        pthread_cond_wait(&Player2_cv, &diceController);
    }
    pthread_mutex_unlock(&diceController);
    pthread_cond_signal(&Player3_cv);
    pthread_exit(0);
}

void* player3(void* arg)
{
    Player3.ready = true;
    pthread_cond_signal(&dealer_cv);
    pthread_cond_wait(&Player3_cv, &diceController);
    while (play == true)
    {
        Player3.roll1 = getDiceRoll();
        Player3.roll2 = getDiceRoll();
        Player3.totalrollvalue = Player3.roll1 + Player3.roll2;

        myfile << "Player C: gets " << Player3.roll1 << " and " << Player3.roll2 << " with a sum " << Player3.totalrollvalue << endl;
        cout << "Player C: " << Player3.roll1 << " " << Player3.roll2 << endl;
       
        //see if this wins the game
        play = GameStatCheck();
        
        //Pass to Player 4
        pthread_cond_signal(&Player4_cv);
        pthread_mutex_unlock(&diceController);
        pthread_cond_wait(&Player3_cv, &diceController);
    }
    pthread_mutex_unlock(&diceController);
    pthread_cond_signal(&Player4_cv);
    pthread_exit(0);
}

void* player4(void* arg)
{
    Player4.ready = true;
    pthread_cond_signal(&dealer_cv);
    pthread_cond_wait(&Player4_cv, &diceController);
    while (play == true)
    {
        Player4.roll1 = getDiceRoll();
        Player4.roll2 = getDiceRoll();
        Player4.totalrollvalue = Player4.roll1 + Player4.roll2;

        myfile << "Player D: gets " << Player4.roll1 << " and " << Player4.roll2 << " with a sum " << Player4.totalrollvalue << endl;
        cout << "Player D: " << Player4.roll1 << " " << Player4.roll2 << endl;
        
        //see if this wins the game
        play = GameStatCheck();

        //Pass to Player 1
        pthread_cond_signal(&Player1_cv);
        pthread_mutex_unlock(&diceController);
        pthread_cond_wait(&Player4_cv, &diceController);
    }
    pthread_mutex_unlock(&diceController);
    pthread_cond_signal(&Player1_cv);
    pthread_exit(0);
}

int main() {
    dice.winner = false;
    dice.ready = true;
    srand(time(0));
    myfile.open("Project2Log.txt");
    cout << "Opening Log" << endl;
    myfile << "Faith Goddard" << endl;
    myfile << "Project 2 Log Start" << endl;
    pthread_t threads[NUM_THREADS];
    pthread_attr_t attr;
   //Intialize mutex and condition vaariables
    pthread_mutex_init(&diceController, NULL);
    pthread_cond_init(&dealer_cv, NULL);
    pthread_cond_init(&Player1_cv, NULL);
    pthread_cond_init(&Player2_cv, NULL);
    pthread_cond_init(&Player3_cv, NULL);
    pthread_cond_init(&Player4_cv, NULL);
    
    //creating threads
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(&threads[0], &attr, dealer, NULL);
    pthread_create(&threads[1], &attr, player1, NULL);
    pthread_create(&threads[2], &attr, player2, NULL);
    pthread_create(&threads[3], &attr, player3, NULL);
    pthread_create(&threads[4], &attr, player4, NULL);
    
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }
    
    //clean up
    myfile.close();
    pthread_attr_destroy(&attr);
    pthread_mutex_destroy(&diceController);
    pthread_cond_destroy(&dealer_cv);
    pthread_cond_destroy(&Player1_cv);
    pthread_cond_destroy(&Player2_cv);
    pthread_cond_destroy(&Player3_cv);
    pthread_cond_destroy(&Player4_cv);
    
    return 0;
}