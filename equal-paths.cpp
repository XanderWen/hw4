#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#include <iostream>
#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
bool checkDepth(Node* node, int depth, int& leafDepth);

bool equalPaths(Node * root)
{
  // Add your code below
  int leafDepth = -1;
  return checkDepth(root,0,leafDepth);
}

bool checkDepth(Node* node, int depth, int& leafDepth){
  if(node == nullptr){
    return true;
  }

  if(node->left == nullptr && node->right==nullptr){
    if(leafDepth == -1){
      leafDepth = depth;
      return true;
    }
    return depth == leafDepth;
  }

  return checkDepth(node->left, depth+1,leafDepth) && checkDepth(node->right, depth+1,leafDepth);
}
