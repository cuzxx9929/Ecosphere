#include "main.h"
#include <iostream>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <iomanip>

#define newSeedProb 0.1
#define windPolliProb 0.5

#define seedDieNoWater 3
#define seedlingDieNoWater 6
#define treeDieNoWater 10
#define treeLifeTime 1000

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

extern sem_t dayBEGINs;
extern sem_t createDone;
extern sem_t dayUpdate;
extern sem_t dayends;
extern pthread_mutex_t protectPrint;
extern struct dayInfoStruct todayInfo;

struct treeStruct{
  int position;
  int state;
  int prestate;
  int day;
  int daysNoWater;
  int branch;
  int branchCount;
  int bloomDays;
  int fruitDays;
  int fruit;
  int flower;
  int havingFruit;
  int toTree;
  int toSeedling;
  int toFruiting;
  int backtoTree;
  int lifeTime;
  bool alive;
  bool goNextState;
};

struct treeStruct treesManage[maxTree];
int newTree[maxTree];
int newTreeCount=0;

int pick_rand_num(int num, double floating)
{
  int randomNum = rand()%(int)(num*(1+floating)-num*(1-floating)+1)+(int)(num*(1-floating));
  return randomNum;
}

void draw_tree()
{
  for(int i=0;i<maxTree;i++)
  {
    if(!treesManage[i].alive)
    continue;
    pthread_mutex_lock(&protectPrint);
    std::cout<<" "<<treesManage[i].day;
    switch(treesManage[i].state)
    {
      case 0:
          std::cout<<"seed"; break;
      case 1:
          std::cout<<"seedling"; break;
      case 2:
          std::cout<<"tree"; break;
      case 3:
          std::cout<<"bloom"; break;
      case 4:
          std::cout<<"fruiting"; break;
    }
    std::cout<<std::endl;
    pthread_mutex_unlock(&protectPrint);
    }
}

void create_tree()
{
  for(newTreeCount;newTreeCount>0;newTreeCount--)
  {
    int freeSpace;

    for(int i=0;i<maxTree;i++)
    {
      if(!treesManage[i].alive)
      {
        freeSpace=i;
        break;
      }
    }

    treesManage[freeSpace].position=newTree[newTreeCount-1];
    //randomly set days plant requires to enter next state
    treesManage[freeSpace].toTree=pick_rand_num(seedlingToTree,floatingRange);
    treesManage[freeSpace].toSeedling=pick_rand_num(seedToSeedling,floatingRange);
    treesManage[freeSpace].toFruiting=pick_rand_num(bloomToFruiting,floatingRange);
    treesManage[freeSpace].backtoTree=pick_rand_num(fruitingToTree,floatingRange);
    treesManage[freeSpace].lifeTime=pick_rand_num(treeLifeTime,floatingRange);
    treesManage[freeSpace].state=state_seed;
    treesManage[freeSpace].prestate=-1;
    treesManage[freeSpace].day=0;
    treesManage[freeSpace].daysNoWater=0;
    treesManage[freeSpace].branch=1;
    treesManage[freeSpace].branchCount=0;
    treesManage[freeSpace].bloomDays=0;
    treesManage[freeSpace].fruitDays=0;
    treesManage[freeSpace].fruit=0;
    treesManage[freeSpace].flower=0;
    treesManage[freeSpace].havingFruit=0;
    treesManage[freeSpace].alive=true;
    treesManage[freeSpace].goNextState=false;
  }
}

void add_tree_to_list(int position)
{
  bool availableSpace=false;
  //randomly pick x,y ; use this position if no other threes use it

    for(int i=0;i<maxTree;i++)
    {
      if(!treesManage[i].alive)
      {
        availableSpace=true;
        break;
      }
    }
    if(!availableSpace)
    return;

  int newP=rand()%100;
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

  for(int i=0;i<maxTree;i++)
  {
    if(newP==treesManage[i].position && !treesManage[i].alive)
    {
      //std::cout<<"no more space to plant this seed"<<std::endl;
      return;
    }
  }
  for(int i=0;i<newTreeCount;i++)
  {
    if(newP==newTree[i])
    {
      //std::cout<<"no more space to plant this seed"<<std::endl;
      return;
    }
  }
  newTree[newTreeCount]=newP;
  newTreeCount++;

}

void tree_update()
{
  for(int i=0;i<maxTree;i++)
  {
    if(!treesManage[i].alive)
    continue;

    treesManage[i].day++;
    //state seed 0
    if(treesManage[i].state==state_seed)
    {
      if(treesManage[i].day>=treesManage[i].toSeedling)
        treesManage[i].goNextState=true;
    }
    //state seedling 1
    else if(treesManage[i].state==state_seedling)
    {
      //each #days has prob to grow new branch
      if((treesManage[i].day-treesManage[i].toSeedling)/timeToBranch>treesManage[i].branchCount)
      {
        if( ((double)rand() / RAND_MAX) <= 1.0/(double)treesManage[i].branch )
        {
          treesManage[i].branch=treesManage[i].branch*2;
        }
        treesManage[i].branchCount++;
      }

      if(treesManage[i].day>=treesManage[i].toTree+treesManage[i].toSeedling)
        treesManage[i].goNextState=true;
    }
    //state seedling 2
    else if(treesManage[i].state==state_tree)
    {
      //to next state
      if(todayInfo.globalMonth==1 || todayInfo.globalMonth==4 || todayInfo.globalMonth==8)
      {
        treesManage[i].goNextState=true;
        treesManage[i].bloomDays=0;
        for(int j=0;j<treesManage[i].branch;j++)
        treesManage[i].flower+=pick_rand_num(flowersPBranch,floatingRange);
      }

    }
    //state bloom 3
    else if(treesManage[i].state==state_bloom)//
    {
      treesManage[i].bloomDays++;

      //every windy day,each flower has prob pollinate
      if(treesManage[i].flower>0 && todayInfo.todayIsWindy)
      {
        for(int j=0;j<treesManage[i].flower;j++)
        {
          if(rand()%101<=windPolliProb*100)
          {
            treesManage[i].flower--;
            treesManage[i].havingFruit++;
          }
        }
      }

      //to next state
      if(treesManage[i].bloomDays>=treesManage[i].toFruiting)//
      {
        treesManage[i].fruitDays=0;
        treesManage[i].goNextState=true;
        treesManage[i].fruit=treesManage[i].havingFruit;
        treesManage[i].havingFruit=0;
        treesManage[i].flower=0;
      }
    }
    //state fruiting 4
    else
    {
      treesManage[i].fruitDays++;

      if(treesManage[i].fruitDays>=treesManage[i].backtoTree)
      {
        treesManage[i].goNextState=true;
        for(int j=0;j<treesManage[i].fruit;j++)
        {
          if(rand()%101<=newSeedProb)
          add_tree_to_list(treesManage[i].position);
        }
        treesManage[i].fruit=0;
      }
    }

    //update tree state
     if(treesManage[i].goNextState)
     {
       treesManage[i].goNextState=false;
       if(treesManage[i].state==state_fruiting)
       treesManage[i].state=state_tree;
       else treesManage[i].state++;
     }

    if(todayInfo.todayIsRainy)
    treesManage[i].daysNoWater=0;
    else treesManage[i].daysNoWater++;
    //die without enough water,exceed lifetime
    if(treesManage[i].day>=treesManage[i].lifeTime||
      (treesManage[i].state==state_seed && treesManage[i].daysNoWater>=seedDieNoWater)||
      (treesManage[i].state==state_seedling && treesManage[i].daysNoWater>=seedlingDieNoWater)||
      (treesManage[i].state!=state_seedling && treesManage[i].state!=state_seed && treesManage[i].daysNoWater>=treeDieNoWater))
    treesManage[i].alive=false;
  }
}

void *tree(void* args)
{
  srand(time(0)-pthread_self());

  while(1)
  {
    //wait to create trees
    sem_wait(&dayBEGINs);

    create_tree();

    sem_post(&createDone);
    sem_wait(&dayUpdate);

    tree_update();
    draw_tree();
    sem_post(&dayends);
  }
}
