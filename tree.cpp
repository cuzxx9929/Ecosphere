#include "main.h"
#include "worldManager.h"
#include <iostream>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <iomanip>


using namespace std;

extern sem_t endReading;
extern sem_t dayBEGINs;
extern sem_t endReadingCheck;
extern sem_t dayENDs;
extern int treeID;
extern int treesAlive;
extern int treeIDlast;
extern int treeDying;
extern pthread_mutex_t protectPrint;
extern struct dayInfoStruct todayInfo;
extern struct treeStruct trees[maxTree];

int pickRandomNum(int num, double floating)
{
  int randomNum = rand()%(int)(num*(1+floating)-num*(1-floating)+1)+(int)(num*(1-floating));
  return randomNum;
}

void newTree(int x, int y)
{
  int newx,newy;
  bool availableSpace=true;

  if(treeIDlast>=maxTree)
  cout<<"no more tree can be planted"<<endl;
  else if(treeIDlast==0)
  {
    trees[treeIDlast].x=0;
    trees[treeIDlast].y=0;
    trees[treeIDlast].id=treeIDlast;
    treeIDlast++;
  }
  else{
    for(int i=0;i<=(4*newSeedSpace*newSeedSpace);i++)
    {
      if(i==4*newSeedSpace*newSeedSpace)
      {
        cout<<"no space to plant this seed"<<endl;
        return;
      }

      //randomly pick x,y ; use this position if no other threes use it
      newx=x-newSeedSpace+rand()%(2*newSeedSpace+1);
      newy=y-newSeedSpace+rand()%(2*newSeedSpace+1);
      for(int i=0;i<treeIDlast;i++)
      {
        if(newx==trees[i].x && newy==trees[i].y)
        {
          availableSpace=false;
          break;
        }
      }
      if(availableSpace)
      break;
    }
    trees[treeIDlast].x=newx;
    trees[treeIDlast].y=newy;
    trees[treeIDlast].id=treeIDlast;
    treeIDlast++;
  }
}

void *tree(void* args)
{
  struct treeStruct *positionInfo = (struct treeStruct *) args;

  int state=state_seed;
  int day=0;
  int daysNoWater=0;
  int branch=0;
  int branchCount=0;
  int bloomDays=0;
  int fruitDays=0;
  int fruit=0;
  int flower=0;
  int havingFruit=0;
  int leaves=0;
  int maxLeaves=0;
  bool dying=false;
  bool goNextState=false;

  //randomly set days plant requires to enter next state
  srand(time(0)-pthread_self());
  int toSeedling = pickRandomNum(seedToSeedling,floatingRange);
  int toTree=pickRandomNum(seedlingToTree,floatingRange);
  int toFruiting=pickRandomNum(bloomToFruiting,floatingRange);
  int backtoTree=pickRandomNum(fruitingToTree,floatingRange);
  int lifeTime=pickRandomNum(plantLifeTime,floatingRange);

  while(1)
    {
      //wait a new day begins
      sem_wait(&dayBEGINs);

      day++;
      if(todayInfo.todayIsRainy)
      daysNoWater=0;
      else  daysNoWater++;

      //state seed 0
      if(state==state_seed)
      {
        //die without enough water
        if(daysNoWater>=treeDieNoWater)
        {
          dying=true;
          break;
          }
        //to next state
        if(day>=toSeedling)
        {
          goNextState=true;
          branch=1;
          maxLeaves+=branch*pickRandomNum(leavesPBranch,floatingRange);
        }
      }
      //state seedling 1
      else if(state==state_seedling)
      {
        //die without enough water
        if(daysNoWater>treeDieNoWater)
        {
          dying=true;
          break;
          }
        //each #days has prob to grow new branch and maxleaves
        if((day-toSeedling)/timeToBranch>branchCount)
        {
          if( ((double)rand() / RAND_MAX) <= 1.0/(double)branch )
          {
            maxLeaves+=branch*pickRandomNum(leavesPBranch,floatingRange);
            branch=branch*2;
          }
          branchCount++;
        }

        //grow new leaves every #days
        if(day%timesNewLeaves==0)
        {
          if(leaves+branch*newLeaves<=maxLeaves)
            leaves+=branch*newLeaves;
          else leaves=maxLeaves;
        }

        //to next state
        if(day>=toSeedling+toTree)
          goNextState=true;

      }
      //state tree 2
      else if(state==state_tree)
      {
        if(daysNoWater>treeDieNoWater)
        {
          dying=true;
          break;
        }
        //leaves are growing if less than max
        if(day%timesNewLeaves==0)
        {
          if(leaves+branch*newLeaves<=maxLeaves)
            leaves+=branch*newLeaves;
          else leaves=maxLeaves;
        }

        //every month 2,4,6,8 is time to bloom. move to next state
        if(todayInfo.globalMonth==2 || todayInfo.globalMonth==4 || todayInfo.globalMonth==6 || todayInfo.globalMonth==8)
        {
          goNextState=true;
          bloomDays=0;
          for(int i=0;i<branch;i++)
            flower+=pickRandomNum(flowersPBranch,floatingRange);
        }
      }
      //state bloom 3
      else if(state==state_bloom)
      {
        bloomDays++;

        if(daysNoWater>treeDieNoWater)
        {
          dying=true;
          break;
          }

        //leaves are growing if less than max
        if(day%timesNewLeaves==0)
        {
          if(leaves+branch*newLeaves<=maxLeaves)
            leaves+=branch*newLeaves;
          else leaves=maxLeaves;
        }

        //every windy day,each flower has prob pollinate
        if(flower>0 && todayInfo.todayIsWindy)
        {
          for(int i=0;i<flower;i++)
          {
            if(rand()%101<=windPolliProb*100)
            {
              flower--;
              havingFruit++;
            }
          }
        }

        //to next state
        if(bloomDays>=toFruiting)
        {
          fruitDays=0;
          goNextState=true;
          fruit=havingFruit;
          havingFruit=0;
          flower=0;
        }
      }
      //state fruiting 4
      else
      {
        fruitDays++;

        if(daysNoWater>treeDieNoWater)
        {
          dying=true;
          break;
          }

        //leaves are growing if less than max
        if(day%timesNewLeaves==0)
        {
          if(leaves+branch*newLeaves<=maxLeaves)
            leaves+=branch*newLeaves;
          else leaves=maxLeaves;
        }

        if(fruitDays>=backtoTree)
        {
          goNextState=true;
          for(int i=0;i<fruit;i++)
          {
            if(rand()%101<=newSeedProb)
              newTree(positionInfo->x,positionInfo->y);
          }
          fruit=0;
        }
      }

      if(day>=lifeTime)
        dying=true;

      //print today tree's information
      pthread_mutex_lock(&protectPrint);
      cout<<setw(5)<<positionInfo->id<<setw(8)<<day<<setw(10);
      switch(state)
      {
        case 0:
          cout<<"seed"; break;
        case 1:
          cout<<"seedling"; break;
        case 2:
          cout<<"tree"; break;
        case 3:
          cout<<"bloom"; break;
        case 4:
          cout<<"fruiting"; break;
      }
      cout<<setw(8)<<flower<<setw(9)<<fruit<<setw(9)<<dying<<setw(9)<<positionInfo->x<<"."<<positionInfo->y<<endl;
      pthread_mutex_unlock(&protectPrint);

      //update tree state
      if(goNextState)
      {
        goNextState=false;
        if(state==state_fruiting)
          state=state_tree;
        state++;
      }

      sem_post(&endReading);
      sem_wait(&endReadingCheck);
      if(dying)
      {
        treeDying++;
        sem_post(&dayENDs);
        pthread_exit(NULL);
      }
      sem_post(&dayENDs);
  }
}
