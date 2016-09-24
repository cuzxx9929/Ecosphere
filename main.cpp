#include "worldManager.h"
#include "tree.h"
#include <iostream>

using namespace std;

int main()
{
  pthread_t world;

  //create world thread
  pthread_create(&world,NULL,worldmanager,NULL);
  //add 1 three to list
  add_tree_to_list(0,0);

  pthread_join(world,NULL);
  cout<<"No more life!! world terminates!!"<<endl;

  return 0;
}
