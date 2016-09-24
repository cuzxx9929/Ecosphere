struct dayInfoStruct{
  bool todayIsRainy;
  bool todayIsWindy;
  int globalDay;
  int globalMonth;
  int globalYear;
};

#define usecOf1day 100 //1s=1000000usec
#define windProb 0.44
#define rainProb 1
#define maxTree 100
#define floatingRange 0.15

void *worldmanager(void* args);
