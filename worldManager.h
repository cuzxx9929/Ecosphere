struct dayInfoStruct{
  bool todayIsRainy;
  bool todayIsWindy;
  int globalDay;
  int globalMonth;
  int globalYear;
};

#define usecOf1day 50000 //1s=1000000usec
#define windProb 0.44
#define rainProb 0.66
#define maxTree 100
#define floatingRange 0.15
#define positionAdjust 50
#define ground 21

void *worldmanager(void* args);
