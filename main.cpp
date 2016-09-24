#include "worldManager.h"
#include "main.h"
#include <iostream>
#include <semaphore.h>

//world used variables define
sem_t terminateFlag;
sem_t endReading;
sem_t dayBEGINs;
sem_t endReadingCheck;
sem_t dayENDs;
pthread_mutex_t protectPrint;
struct dayInfoStruct todayInfo;

int main()
{
  pthread_t world;

  sem_init(&terminateFlag, 0, 0);
  pthread_mutex_init(&protectPrint, NULL);

  pthread_create(&world,NULL,worldmanager,NULL);

  sem_wait(&terminateFlag);

  sem_destroy(&terminateFlag);
  pthread_mutex_destroy(&protectPrint);

  return 0;
}
