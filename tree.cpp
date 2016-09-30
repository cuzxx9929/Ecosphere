#include "main.h"
#include <iostream>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <iomanip>

#define newSeedProb 0.1
#define windPolliProb 0.3

#define seedDieNoWater 3
#define seedlingDieNoWater 6
#define treeDieNoWater 10
#define treeLifeTime 500

#define seedToSeedling 14
#define seedlingToTree 180
#define bloomToFruiting 30
#define fruitingToTree 14

#define flowersPBranch 5
#define timeToBranch 10

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
  bool alive;
  int day;
  int state;
  int toSeedling;
  bool goNextState;
  int branch;
  int toTree;
  int flower;
  int bloomDays;
  int havingFruit;
  int fruitDays;
  int backtoTree;
  int toFruiting;
  int fruit;
  int position;
  int prestate;
  int daysNoWater;
  int lifeTime;
};

struct treeStruct treesManage[maxTree];
int newTree[maxTree];
int newTreeCount=0;
bool treeExtinct=false;

int pick_rand_num(int num, double floating)
{
  int randomNum = rand()%(int)(num*(1+floating)-num*(1-floating)+1)+(int)(num*(1-floating));
  return randomNum;
}

void draw_tree()
{
  //clear line
  std::cout<<"\33["<<ground<<";1H"<<"\33[K";
  //draw every alive tree
  for(int i=0;i<maxTree;i++)
  {
    if(!treesManage[i].alive || treesManage[i].position>positionAdjust
      || treesManage[i].position<-positionAdjust)
    continue;

    pthread_mutex_lock(&protectPrint);
    std::cout<<"\33["<<ground<<";"<<positionAdjust+treesManage[i].position<<"H";
    switch(treesManage[i].state)
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
    treesManage[freeSpace].toTree=pick_rand_num(seedlingToTree,floatingRange);
    treesManage[freeSpace].toSeedling=pick_rand_num(seedToSeedling,floatingRange);
    treesManage[freeSpace].toFruiting=pick_rand_num(bloomToFruiting,floatingRange);
    treesManage[freeSpace].backtoTree=pick_rand_num(fruitingToTree,floatingRange);
    treesManage[freeSpace].lifeTime=pick_rand_num(treeLifeTime,floatingRange);
    treesManage[freeSpace].state=state_seed;
    treesManage[freeSpace].prestate=-1;
    treesManage[freeSpace].day=0;
    treesManage[freeSpace].alive=true;
    treesManage[freeSpace].goNextState=false;
  }
}

void add_tree_to_list(int position)
{
  bool freeSpace=false;

  for(int i=0;i<maxTree;i++)
  {
    if(!treesManage[i].alive)
    {
      freeSpace=true;
      break;
    }
  }

  if(!freeSpace)
  return;

  int newP=rand()%100+1;
  if(newP>50)
  {
    newP-=50;
    newP=newP/25+1;
    newP=position+newP;
  }
  else{
    newP=newP/25+1;
    newP=position-newP;
  }
  //check if the position is being used, if it is ,return

  for(int i=0;i<maxTree;i++)
  {
    if(newP==treesManage[i].position && treesManage[i].alive)
    return;
  }
  for(int i=0;i<newTreeCount;i++)
  {
    if(newP==newTree[i])
    return;
  }
  newTree[newTreeCount]=newP;
  newTreeCount++;
}

void tree_activity()
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
      {
        treesManage[i].goNextState=true;
        treesManage[i].branch=1;
      }
    }
    //state seedling 1
    else if(treesManage[i].state==state_seedling)
    {
      //each #days has prob to grow new branch
      if(treesManage[i].day%timeToBranch==0)
      if( ((double)rand() / RAND_MAX) <= 1.0/(double)treesManage[i].branch )
      treesManage[i].branch=treesManage[i].branch*2;

      if(treesManage[i].day>=treesManage[i].toTree+treesManage[i].toSeedling)
      {
        treesManage[i].goNextState=true;
        treesManage[i].flower=0;
      }
    }
    //state seedling 2
    else if(treesManage[i].state==state_tree)
    { //to next state
      if(todayInfo.globalMonth==1 || todayInfo.globalMonth==4 || todayInfo.globalMonth==8)
      {
        treesManage[i].goNextState=true;
        treesManage[i].bloomDays=0;
        treesManage[i].havingFruit=0;
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
        treesManage[i].havingFruit+=treesManage[i].flower*windPolliProb;
        treesManage[i].flower *=(1-windPolliProb);
      }
      //to next state
      if(treesManage[i].bloomDays>=treesManage[i].toFruiting)//
      {
        treesManage[i].goNextState=true;
        treesManage[i].fruitDays=0;
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
        for(;treesManage[i].fruit>0;treesManage[i].fruit--)
        {
          if(rand()%101<newSeedProb*100)
          add_tree_to_list(treesManage[i].position);
        }
      }
    }

     //update tree state
     treesManage[i].prestate=treesManage[i].state;
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

    //die without enough water,exceed lifetime....
    if(treesManage[i].day>=treesManage[i].lifeTime||
      (treesManage[i].state==state_seed && treesManage[i].daysNoWater>=seedDieNoWater)||
      (treesManage[i].state==state_seedling && treesManage[i].daysNoWater>=seedlingDieNoWater)||
      (treesManage[i].state!=state_seedling && treesManage[i].state!=state_seed && treesManage[i].daysNoWater>=treeDieNoWater))
    treesManage[i].alive=false;
  }
}

void check_tree_N()
{
  for(int i=0;i<maxTree;i++)
  {
    if(treesManage[i].alive)
    return;
  }
  treeExtinct=true;
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

    tree_activity();
    draw_tree();
    check_tree_N();
    sem_post(&dayends);
  }
}
