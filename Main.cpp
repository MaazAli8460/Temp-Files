#include <SFML/Graphics.hpp>
#include <pthread.h>
#include <iostream>
#include <string>
#include "menu.h"
#include "Start_game.cpp"

pthread_mutex_t menuMutex = PTHREAD_MUTEX_INITIALIZER;
bool menuFinished = false;

void* startGame(void*)
{
    // Wait for menu to finish
    while (true)
    {
        pthread_mutex_lock(&menuMutex);
        if (menuFinished)
        {
            pthread_mutex_unlock(&menuMutex);
            break;
        }
        pthread_mutex_unlock(&menuMutex);
    }

    start_game();
    return NULL;
}

int main()
{
    display_menu();

    // Create threads
    pthread_t startGameThread;
    pthread_create(&startGameThread, NULL, startGame, NULL);


    // Wait for threads to finish (optional)
    pthread_join(startGameThread, NULL);

    return 0;
}
