#include <semaphore.h>

//constant for world use
#define usecOf1day 100 //1s=1000000usec
#define rainProb 0.55
#define windProb 0.44

//define structures
struct dayInfoStruct{
  bool todayIsRainy;
  bool todayIsWindy;
  int globalDay;
  int globalMonth;
  int globalYear;
};

int main();
