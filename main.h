
 //constant for world use
 //Constants unit:day
 #define usecOf1day  50000 //1s=1000000usec
 #define newSeedProb 0.1
 #define rainProb 0.77
 #define windProb 0.44
 #define maxTree 100
 #define floatingRange 0.15
 #define ground 21
 #define positionAdjust 50

 //define structures
 struct dayInfoStruct{
   bool todayIsRainy;
   bool todayIsWindy;
   int globalDay;
   int globalMonth;
   int globalYear;
 };
