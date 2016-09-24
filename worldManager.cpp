#include "tree.h"
#include "worldManager.h"
#include <iostream>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>

sem_t terminateFlag;
sem_t endReading;
sem_t dayBEGINs;
sem_t endReadingCheck;
sem_t dayENDs;
pthread_mutex_t protectPrint;
struct dayInfoStruct todayInfo;

int creature()
{
  return tree_manager();
}

void creature_info_print()
{
  std::cout<<" treeID   day    state     flowers   fruits   dying   position"<<std::endl;
}

void update_today_info()
{
  //update new day information
  //rainy
  if( (double)rand() / RAND_MAX <= rainProb)
  todayInfo.todayIsRainy = true;
  else todayInfo.todayIsRainy = false;
  //windy
  if( (double)rand() / RAND_MAX <= windProb)
  todayInfo.todayIsWindy = true;
  else todayInfo.todayIsWindy = false;
  //year,month,day
  switch(todayInfo.globalDay)
  {
    case 1:
    todayInfo.globalMonth=1; break;
    case 32:
    todayInfo.globalMonth=2;break;
    case 60:
    todayInfo.globalMonth=3;break;
    case 91:
    todayInfo.globalMonth=4;break;
    case 121:
    todayInfo.globalMonth=5;break;
    case 152:
    todayInfo.globalMonth=6;break;
    case 182:
    todayInfo.globalMonth=7;break;
    case 213:
    todayInfo.globalMonth=8;break;
    case 244:
    todayInfo.globalMonth=9;break;
    case 274:
    todayInfo.globalMonth=10;break;
    case 305:
    todayInfo.globalMonth=11;break;
    case 335:
    todayInfo.globalMonth=12;break;
    case 366:
    todayInfo.globalMonth=1;
    todayInfo.globalDay=1;
    todayInfo.globalYear++;
    break;
  }
}

void *worldmanager(void* args)
{
  bool dynamic=false;
  srand(time(0)-pthread_self());

  //semaphore and mutex initialize
  sem_init(&terminateFlag, 0, 0);
  sem_init(&endReading, 0, 0);
  sem_init(&dayBEGINs, 0, 0);
  sem_init(&endReadingCheck, 0, 0);
  sem_init(&dayENDs, 0, 0);
  pthread_mutex_init(&protectPrint, NULL);

  todayInfo.globalMonth=1;
  todayInfo.globalYear=1;
  todayInfo.globalDay=1;

  //create initial creatures
  int alive=creature();
  while(1)
  {
    update_today_info();

    //print information of the day
    pthread_mutex_lock(&protectPrint);
    std::cout<<todayInfo.globalYear<<"Y/"<<todayInfo.globalMonth<<"M/"<<todayInfo.globalDay<<"D ";
    std::cout<<"  rainy:"<<todayInfo.todayIsRainy<<"  windy:"<<todayInfo.todayIsWindy<<std::endl;
    creature_info_print();
    pthread_mutex_unlock(&protectPrint);

    //notice creatures a day begins
    for(int i=0;i<alive;i++)//the last treeID is not been used
    sem_post(&dayBEGINs);
    //wait creatures to finish reading day info
    for(int i=0;i<alive;i++)
    sem_wait(&endReading);

    //creatures all done, program continues
    for(int i=0;i<alive;i++)
    sem_post(&endReadingCheck);
    //wait to end today
    for(int i=0;i<alive;i++)
    sem_wait(&dayENDs);

    //one day has gone
    usleep(usecOf1day);
    todayInfo.globalDay++;
    std::cout<<"---------------------------------------------------------------"<<std::endl;

    if(dynamic && alive==0)
    break;

    alive=creature();

    if(!dynamic && alive>0)
    dynamic=true;
  }
  //semaphore and mutex destroy
  sem_destroy(&terminateFlag);
  sem_destroy(&endReading);
  sem_destroy(&dayBEGINs);
  sem_destroy(&endReadingCheck);
  sem_destroy(&dayENDs);
  pthread_mutex_destroy(&protectPrint);
}
