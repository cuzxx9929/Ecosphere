#include "tree.h"
#include "main.h"
#include <iostream>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>

struct dayInfoStruct todayInfo;
pthread_mutex_t protectPrint;
sem_t dayBEGINs;
sem_t createDone;
sem_t dayUpdate;
sem_t dayends;


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


int main()
{
  //semaphore and mutex initialize,and dayinfo
  sem_init(&dayBEGINs, 0, 0);
  sem_init(&createDone, 0, 0);
  sem_init(&dayUpdate, 0, 0);
  sem_init(&dayends, 0, 0);
  pthread_mutex_init(&protectPrint, NULL);
  todayInfo.globalMonth=1;
  todayInfo.globalYear=1;
  todayInfo.globalDay=1;

/*  //print graph
  system("clear");
  std::cout<<"\33[?25l";//hide cursor
  std::cout<<"\33[2;1H"<<"rainy:   windy: "<<std::endl;
  std::cout<<"\33[22;1H";
  for(int i=0;i<101;i++)
  std::cout<<"▀";//print ground
*/
  srand(time(0));

  //set first tree
  add_tree_to_list(0);
  //create tree thread
  pthread_t treeWorld;
  pthread_create(&treeWorld,NULL,tree,NULL);

  while(1)
  {
    //notice creatures a day begins
    sem_post(&dayBEGINs);
    //wait creatures to born
    sem_wait(&createDone);

    update_today_info();
    //print information of the day
    pthread_mutex_lock(&protectPrint);
/*    std::cout<<"\33[1;1H"<<"\33[K";//move to 1,1 position, and clear the row
    std::cout<<todayInfo.globalYear<<"Y/"<<todayInfo.globalMonth<<"M/"<<todayInfo.globalDay<<"D "<<std::flush;
    std::cout<<"\33[2;7H"<<todayInfo.todayIsRainy<<"\33[2;16H"<<todayInfo.todayIsWindy<<std::flush;*/

    std::cout<<todayInfo.globalYear<<"Y/"<<todayInfo.globalMonth<<"M/"<<todayInfo.globalDay<<"D "<<"rainy: "<<todayInfo.todayIsRainy<<"  windy: "<<todayInfo.todayIsWindy<<std::endl;

    pthread_mutex_unlock(&protectPrint);

    //notice creatures day infor is updated
    sem_post(&dayUpdate);

    //wait to finish today
    sem_wait(&dayends);

  /*  //if no more trees, terminates
    if(treeTerminate)
    break;*/

    //one day has gone
    usleep(usecOf1day);
    todayInfo.globalDay++;//semaphore and mutex destroy
  }
  sem_destroy(&dayBEGINs);
  sem_destroy(&createDone);
  sem_destroy(&dayUpdate);
  sem_destroy(&dayends);
  pthread_mutex_destroy(&protectPrint);
//  std::cout<<"\33[?25h"<<"\33[23;1H";//display cursor

  std::cout<<"program terminates"<<std::endl;

  return 0;
}
