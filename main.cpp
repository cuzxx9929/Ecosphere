#include "globalManager.h"
#include "main.h"
#include <iostream>
#include <semaphore.h>

sem_t terminateFlag;
pthread_mutex_t protectPrint;

struct dayInfoStruct todayInfo;

int main()
{
  pthread_t manager;

  sem_init(&terminateFlag, 0, 0);
  pthread_mutex_init(&protectPrint, NULL);

  pthread_create(&manager,NULL,globalmanager,NULL);

  sem_wait(&terminateFlag);

  sem_destroy(&terminateFlag);
  pthread_mutex_destroy(&protectPrint);

  return 0;
}
