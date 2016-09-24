#include "worldManager.h"
#include "tree.h"
#include "main.h"
#include <iostream>
#include <semaphore.h>

using namespace std;

//world used variables define
sem_t terminateFlag;
sem_t endReading;
sem_t dayBEGINs;
sem_t endReadingCheck;
sem_t dayENDs;
pthread_mutex_t protectPrint;
struct dayInfoStruct todayInfo;
struct treeStruct trees[maxTree];
pthread_t tree_thread[maxTree];


int main()
{
  pthread_t world;

  //semaphore and mutex initialize
  sem_init(&terminateFlag, 0, 0);
  sem_init(&endReading, 0, 0);
  sem_init(&dayBEGINs, 0, 0);
  sem_init(&endReadingCheck, 0, 0);
  sem_init(&dayENDs, 0, 0);
  pthread_mutex_init(&protectPrint, NULL);

  pthread_create(&world,NULL,worldmanager,NULL);

  newTree(0,0);

  sem_wait(&terminateFlag);
  cout<<"No more life!! program terminates!!"<<endl;

  //semaphore and mutex destroy
  sem_destroy(&terminateFlag);
  sem_destroy(&endReading);
  sem_destroy(&dayBEGINs);
  sem_destroy(&endReadingCheck);
  sem_destroy(&dayENDs);
  pthread_mutex_destroy(&protectPrint);

  return 0;
}
