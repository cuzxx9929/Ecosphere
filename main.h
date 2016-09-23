#include <semaphore.h>

#define windProb 0.33
#define rainProb 0.55

struct dayInfoStruct{
  bool todayIsRainy;
  bool todayIsWindy;
  int globalDay;
  int globalMonth;
  int globalYear;
};

int main();
