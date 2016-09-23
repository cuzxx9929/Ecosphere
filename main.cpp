#include "globalManager.h"
#include<iostream>

int main()
{
  pthread_t manager;

  pthread_create(&manager,NULL,globalmanager,NULL);

  pthread_join(manager,NULL);

  return 0;
}
