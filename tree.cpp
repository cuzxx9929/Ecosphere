#include "worldManager.h"
#include <iostream>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <iomanip>

#define newSeedProb 0.1
#define windPolliProb 0.2

#define seedDieNoWater 3
#define seedlingDieNoWater 6
#define treeDieNoWater 10
#define treeLifeTime 3650

#define seedToSeedling 14
#define seedlingToTree 180
#define bloomToFruiting 30
#define fruitingToTree 14

#define flowersPBranch 5
#define timeToBranch 10
#define newSeedSpace 2

#define state_seed 0
#define state_seedling 1
#define state_tree 2
#define state_bloom 3
#define state_fruiting 4

extern sem_t endReading;
extern sem_t dayBEGINs;
extern sem_t endReadingCheck;
extern sem_t dayENDs;
extern pthread_mutex_t protectPrint;
extern struct dayInfoStruct todayInfo;

pthread_t tree_thread[maxTree];
int treeID=0;
int treesAlive=0;
int treeIDlast=0;
int treeDying=0;
int trees[maxTree];


int pick_rand_num(int num, double floating)
{
  int randomNum = rand()%(int)(num*(1+floating)-num*(1-floating)+1)+(int)(num*(1-floating));
  return randomNum;
}

void add_tree_to_list(int position)
{
  int newP;
  bool availableSpace=true;

  if(treeIDlast>=maxTree)
  std::cout<<"no more tree can be planted"<<std::endl;
  else if(treeIDlast==0)
  {
    trees[treeIDlast]=0;
    treeIDlast++;
  }
  else{
      //randomly pick x,y ; use this position if no other threes use it
      newP=rand()%100;
      if(newP<13)
      newP=position+newSeedSpace;
      else if(newP<25)
      newP=position-newSeedSpace;
      else if(newP<38)
      newP=position+newSeedSpace+1;
      else if(newP<50)
      newP=position-newSeedSpace-1;
      else if(newP<63)
      newP=position+newSeedSpace+2;
      else if(newP<75)
      newP=position-newSeedSpace-2;
      else if(newP<87)
      newP=position+newSeedSpace+3;
      else newP=position-newSeedSpace-3;

      for(int i=0;i<treeIDlast;i++)
      {
        if(newP==trees[i])
        {
          //std::cout<<"no more space to plant this seed"<<std::endl;
          return;
        }
      }
    trees[treeIDlast]=newP;
    treeIDlast++;
  }
}

void *tree(void* args)
{
  int* position=(int*) args;

  int state=state_seed;
  int prestate=-1;
  int day=0;
  int daysNoWater=0;
  int branch=1;
  int branchCount=0;
  int bloomDays=0;
  int fruitDays=0;
  int fruit=0;
  int flower=0;
  int havingFruit=0;
  bool dying=false;
  bool goNextState=false;

  //randomly set days plant requires to enter next state
  srand(time(0)-pthread_self());
  int toTree=pick_rand_num(seedlingToTree,floatingRange);
  int toSeedling=pick_rand_num(seedToSeedling,floatingRange);
  int toFruiting=pick_rand_num(bloomToFruiting,floatingRange);
  int backtoTree=pick_rand_num(fruitingToTree,floatingRange);
  int lifeTime=pick_rand_num(treeLifeTime,floatingRange);

  while(1)
    {
      //wait a new day begins
      sem_wait(&dayBEGINs);
      day++;

      //state seed 0
      if(state==state_seed)
      {
        if(day>=toSeedling)
          goNextState=true;
      }
      //state seedling 1
      else if(state==state_seedling)
      {
        //each #days has prob to grow new branch
        if((day-toSeedling)/timeToBranch>branchCount)
        {
          if( ((double)rand() / RAND_MAX) <= 1.0/(double)branch )
          {
            branch=branch*2;
          }
          branchCount++;
        }

        if(day>=toTree+toSeedling)
          goNextState=true;
      }
      //state seedling 2
      else if(state==state_tree)
      {
        //to next state
        if(todayInfo.globalMonth==1 || todayInfo.globalMonth==4 || todayInfo.globalMonth==8)
        {
          goNextState=true;
          bloomDays=0;
          for(int i=0;i<branch;i++)
          flower+=pick_rand_num(flowersPBranch,floatingRange);
        }

      }
      //state bloom 3
      else if(state==state_bloom)//
      {
        bloomDays++;

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
        if(bloomDays>=toFruiting)//
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

        if(fruitDays>=backtoTree)
        {
          goNextState=true;
          for(int i=0;i<fruit;i++)
          {
            if(rand()%101<=newSeedProb)
            add_tree_to_list(*position);
          }
          fruit=0;
        }
      }

      if(todayInfo.todayIsRainy)
      daysNoWater=0;
      else daysNoWater++;
      //die without enough water,exceed lifetime
      if(day>=lifeTime||
        (state==state_seed && daysNoWater>=seedDieNoWater)||
        (state==state_seedling && daysNoWater>=seedlingDieNoWater)||
        (state!=state_seedling && state!=state_seed && daysNoWater>=treeDieNoWater))
      dying=true;


      //draw tree
      if(prestate==-1 || (prestate>-1 && prestate!=state))
      {
        pthread_mutex_lock(&protectPrint);
        std::cout<<"\33["<<ground<<";"<<positionAdjust+*position<<"H";
        switch(state)
        {
          case 0://seed
          std::cout<<"ᨔ"<<std::flush; break;
          case 1://seedling
          std::cout<<"🌱"<<std::flush; break;
          case 2://tree
          std::cout<<"🌳"<<std::flush; break;
          case 3://bloom
          std::cout<<"🌻"<<std::flush; break;
          case 4://fruiting
          std::cout<<"🍐"<<std::flush; break;
        }
        pthread_mutex_unlock(&protectPrint);
      }

      prestate=state;

      //update tree state
      if(goNextState)
      {
        goNextState=false;
        if(state==state_fruiting)
        state=state_tree;
        else state++;
      }

      sem_post(&endReading);
      sem_wait(&endReadingCheck);
      if(dying)
      {
        pthread_mutex_lock(&protectPrint);
        std::cout<<"\33["<<ground<<";"<<positionAdjust+*position<<"H"<<" "<<std::flush;
        pthread_mutex_unlock(&protectPrint);
        treeDying++;
        sem_post(&dayENDs);
        for(int i=0;i<treeIDlast;i++)
        {
          if(trees[i]==*position)
          {
            trees[i]=2147483647;
            break;
          }
        }
        return NULL;
      }
      sem_post(&dayENDs);
  }
}

int tree_manager()
{
  for(treeID;treeID<treeIDlast;treeID++)
  {
    pthread_create(&tree_thread[treeID], NULL, tree, &trees[treeID]);
    treesAlive++;
  }
  for(treeDying;treeDying>0;treeDying--)
  treesAlive--;
  return treesAlive;
}
