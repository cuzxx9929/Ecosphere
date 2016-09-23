#include "main.h"
#include <iostream>
#include <stdlib.h>
#include <semaphore.h>

using namespace std;

extern sem_t terminateFlag;
extern pthread_mutex_t protectPrint;
extern struct dayInfoStruct todayInfo;

void *globalmanager(void* args)
{
  srand(time(0)-pthread_self());

  todayInfo.globalMonth=1;
  todayInfo.globalYear=1;
  todayInfo.globalDay=1;

  while(1)
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

    //print information of the day
    pthread_mutex_lock(&protectPrint);
    cout<<todayInfo.globalYear<<"Y/"<<todayInfo.globalMonth<<"M/"<<todayInfo.globalDay<<"D ";
    cout<<"  rainy:"<<todayInfo.todayIsRainy<<"  windy:"<<todayInfo.todayIsWindy<<endl;
    pthread_mutex_unlock(&protectPrint);

    cout<<"---------------------------------------------------------------"<<endl;


    if(todayInfo.globalDay==100)
    {
       sem_post(&terminateFlag);
       return NULL;
     }

    todayInfo.globalDay++;

  }
}
