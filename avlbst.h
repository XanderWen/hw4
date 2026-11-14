#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void rotateLeft(AVLNode<Key,Value>*node);
    void rotateRight(AVLNode<Key,Value>*node);
    void insertFix(AVLNode<Key,Value>*parent,AVLNode<Key,Value>*node);
    void removeFix(AVLNode<Key,Value>*node,int diff);


};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    if(this->root_ == NULL){
      this->root_ = new AVLNode<Key,Value>(
        new_item.first,new_item.second,NULL);
      return;
    }

    AVLNode<Key,Value>* current = static_cast<AVLNode<Key,Value>*>(this->root_);

    AVLNode<Key,Value>* parent =NULL;
    while(current !=NULL){
      parent = current;
      if(new_item.first == current->getKey()){
        current->setValue(new_item.second);
        return;
      }else if(new_item.first < current->getKey())
      {
        current = current->getLeft();
      }else{
        current = current->getRight();
      }
    }

    AVLNode<Key,Value> * newNode = new AVLNode<Key,Value>(
      new_item.first, new_item.second,parent
    );
    if(new_item.first<parent->getKey()){
      parent->setLeft(newNode);
    }else{
      parent->setRight(newNode);
    }

    insertFix(parent,newNode);
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    AVLNode<Key,Value>*target = static_cast<AVLNode<Key,Value>*>(this->internalFind(key));

    if(target == NULL){
      return;
    }
    if(target->getLeft() != NULL && target->getRight() != NULL){
      AVLNode<Key,Value>*pred=static_cast<AVLNode<Key,Value>*>(this->predecessor(target));
      this->nodeSwap(target,pred);
    }

    AVLNode<Key,Value>* child = target->getLeft();

    if(child == NULL){
      child = target->getRight();
    }

    AVLNode<Key,Value>*parent = target->getParent();
    int diff =0;
    
    if(child != NULL){
      child->setParent(parent);
    }

    if(parent == NULL){
      this->root_=child;
    }else if(parent->getLeft()==target){
      parent->setLeft(child);
      diff=1;
    }else{
      parent->setRight(child);
      diff = -1;
    }
    delete target;
    removeFix(parent,diff);
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

template<typename Key,typename Value>
void AVLTree<Key,Value>::rotateLeft(AVLNode<Key,Value>* node){
  if(node == NULL||node->getRight() == NULL){
    return;
  }

  AVLNode<Key,Value>* right = node->getRight();
  AVLNode<Key,Value>* parent = node->getParent();
  AVLNode<Key,Value>* middle = right->getLeft();

  node->setRight(middle);

  if(middle!=NULL){
    middle ->setParent(node);
  }
  right->setParent(parent);
  if(parent==NULL){
    this->root_ = right;
  }else if(parent->getLeft() == node){
    parent->setLeft(right);
  }else{
    parent->setRight(right);
  }
  right->setLeft(node);
  node->setParent(right);
}

template<typename Key,typename Value>
void AVLTree<Key,Value>:: rotateRight(AVLNode<Key,Value>*node){
  if(node == NULL || node->getLeft() ==NULL){
    return;
  }

  AVLNode<Key,Value> * left = node->getLeft();
  AVLNode<Key,Value> * parent = node->getParent();
  AVLNode<Key,Value> * middle = left->getRight();

  node->setLeft(middle);

  if(middle != NULL){
    middle->setParent(node);
  }

  left->setParent(parent);

  if(parent==NULL){
    this->root_ = left;
  }else if(parent->getLeft()==node){
    parent->setLeft(left);
  }else{
    parent->setRight(left);
  }
  left->setRight(node);
  node->setParent(left);
}

template<typename Key,typename Value>
void AVLTree<Key,Value>::insertFix(
  AVLNode<Key,Value> *parent,
  AVLNode<Key,Value> *node){
    while(parent!=NULL){
      if(node == parent->getLeft()){
        parent->updateBalance(-1);
      }else{
        parent->updateBalance(1);
      }

      int balance = parent->getBalance();

      if(balance == 0){
        return;
      }
      if(balance == -2){
        AVLNode<Key,Value>* left = parent->getLeft();
        if(left->getBalance() == -1){
          rotateRight(parent);
          parent->setBalance(0);
          left->setBalance(0);
        }else{
          AVLNode<Key,Value>* middle = left->getRight();
          int middleBalance = middle->getBalance();

          rotateLeft(left);
          rotateRight(parent);

          if(middleBalance == -1){
            parent->setBalance(1);
            left->setBalance(0);
          }else if(middleBalance == 0){
            parent->setBalance(0);
            left->setBalance(0);
          }else{
            parent->setBalance(0);
            left->setBalance(-1);
          }
          middle->setBalance(0);
        }
        return;
      }
      if(balance == 2){
        AVLNode<Key,Value>* right = parent->getRight();

        if(right->getBalance() == 1){
          rotateLeft(parent);
          parent->setBalance(0);
          right->setBalance(0);
        }else{
          AVLNode<Key,Value>* middle = right->getLeft();
          int middleBalance = middle->getBalance();

          rotateRight(right);
          rotateLeft(parent);

          if(middleBalance == -1){
            parent->setBalance(0);
            right->setBalance(1);
          }else if(middleBalance == 0){
            parent->setBalance(0);
            right->setBalance(0);
          }else{
            parent->setBalance(-1);
            right->setBalance(0);
          }
          middle->setBalance(0);
        }
        return;
      }
      node = parent;
      parent = parent->getParent();
    }
  }

template<typename Key,typename Value>
void AVLTree<Key,Value>::removeFix(AVLNode<Key,Value>*node, int diff){
  while(node != NULL){
    AVLNode<Key,Value>* parent = node->getParent();
    int nextDiff = 0;

    if(parent!=NULL){
      if(parent->getLeft() == node){
        nextDiff = 1;
      }
      else{
        nextDiff = -1;
      }
    }

    node -> updateBalance(diff);
    int balance = node->getBalance();

    if(balance == -1||balance == 1){
      return;
    }
    if(balance==-2){
      AVLNode<Key,Value>*left = node->getLeft();
      int leftBalance = left->getBalance();
      if(leftBalance <= 0){
        rotateRight(node);
        if(leftBalance==0){
          node->setBalance(-1);
          left->setBalance(1);
          return;
        }

        node->setBalance(0);
        left->setBalance(0);
      }else{
        AVLNode<Key,Value>* middle = left->getRight();
        int middleBalance = middle->getBalance();

        rotateLeft(left);
        rotateRight(node);

        if(middleBalance == -1){
          node->setBalance(1);
          left->setBalance(0);
        }else if(middleBalance == 0){
          node->setBalance(0);
          left->setBalance(0);
        }
        else{
          node->setBalance(0);
          left->setBalance(-1);
        }
        middle->setBalance(0);
      }
    }else if(balance == 2){
      AVLNode<Key,Value>*right=node->getRight();
      int rightBalance = right->getBalance();

      if(rightBalance>=0){
        rotateLeft(node);
        if(rightBalance == 0){
          node->setBalance(1);
          right->setBalance(-1);
          return;
        }
        node->setBalance(0);
        right->setBalance(0);
      }else{
        AVLNode<Key,Value> * middle = right->getLeft();
        int middleBalance=middle->getBalance();

        rotateRight(right);
        rotateLeft(node);
        if(middleBalance == -1){
          node->setBalance(0);
          right->setBalance(1);
        }else if(middleBalance == 0){
          node->setBalance(0);
          right->setBalance(0);
        }else{
          node->setBalance(-1);
          right->setBalance(0);
        }
        middle->setBalance(0);
      }
    }
    node = parent;
    diff = nextDiff;
  }
}

#endif
