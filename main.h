#include <semaphore.h>

//constant for world use
//Constants unit:day
#define usecOf1day  100 //1s=1000000usec
#define newSeedProb 0.1
#define rainProb 1
#define plantLifeTime 3650
#define maxTree 100

#define windProb 0.44
#define windPolliProb 0.15
#define floatingRange 0.15

#define seedDieNoWater 3
#define seedlingDieNoWater 5
#define treeDieNoWater 7

#define seedToSeedling 14
#define seedlingToTree 180
#define bloomToFruiting 30
#define fruitingToTree 14

#define flowersPBranch 5
#define leavesPBranch 20
#define timesNewLeaves 2
#define newLeaves 2
#define timeToBranch 10
#define newSeedSpace 3
//plant states
#define state_seed 0
#define state_seedling 1
#define state_tree 2
#define state_bloom 3
#define state_fruiting 4


//define structures
struct dayInfoStruct{
  bool todayIsRainy;
  bool todayIsWindy;
  int globalDay;
  int globalMonth;
  int globalYear;
};
struct treeStruct{
  int id;
  int x;
  int y;
};

int main();
