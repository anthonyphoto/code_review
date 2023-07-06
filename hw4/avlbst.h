#ifndef RBBST_H
#define RBBST_H

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
* An explicit constructor to initialize the elements by calling the base class constructor and setting
* the color to red since every new node will be red when it is first inserted.
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
    void rotateRight(AVLNode<Key,Value>* parent);
    void rotateLeft(AVLNode<Key,Value>* parent);
    void insertFix(AVLNode<Key,Value>* parent, AVLNode<Key,Value>* child);
    // removeFix


};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    AVLNode<Key, Value>* avlNode = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
    if (this->root_ == nullptr) {
        this->root_ = avlNode;
    }
    else {
        AVLNode<Key, Value>* temp = static_cast<AVLNode<Key, Value>*> (this->root_);
        Key avlKey = avlNode->getKey();
        while(temp != NULL) {
            if (temp->getKey() < avlKey) {
                if (temp->getRight() == NULL) {
                    temp->setRight(avlNode);
                    avlNode->setParent(temp);
                    avlNode->getParent()->updateBalance(1);
                    break;
                }
                temp = temp->getRight();
            }
            else if (temp->getKey() > avlKey) {
                if (temp->getLeft() == NULL) {
                    temp->setLeft(avlNode);
                    avlNode->setParent(temp);
                    avlNode->getParent()->updateBalance(-1);
                    break;
                }
                temp = temp->getLeft();
            }
            else { // already existing key
                temp->setValue(avlNode->getValue());
                delete avlNode;
                break;
            }
        }   
        AVLNode<Key, Value>* avlParent = avlNode->getParent();
        if (avlParent->getBalance() != 0) {
            insertFix(avlParent, avlNode);
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key,Value>* parent, AVLNode<Key,Value>* child) {
    AVLNode<Key,Value>* grandP = parent->getParent();
    if (grandP == nullptr) return;
    // when p is left child of grandparent
    if (grandP->getLeft() == parent) {
        grandP->updateBalance(-1);
        if (grandP->getBalance() == 0) {
            return;
        }
        else if (grandP->getBalance() == -1) {
            insertFix(grandP, parent);
        }
        else {
            // zig-zig
            if ((grandP->getBalance() < 0 && parent->getBalance() < 0) || (grandP->getBalance() > 0 && parent->getBalance() > 0)) {
                // rotate right (p)
                rotateRight(parent);
                parent->setBalance(0);
                grandP->setBalance(0);
            }
            // zig-zag
            else {
                // rotate left (p)
                rotateLeft(parent);
                // rotate right (g)
                rotateRight(grandP);
                if (child->getBalance() == -1) {
                    parent->setBalance(0);
                    grandP->setBalance(1);
                    child->setBalance(0);
                } 
                else if (child->getBalance() == 0) {
                    parent->setBalance(0);
                    grandP->setBalance(0);
                    child->setBalance(0);
                }
                else {
                    parent->setBalance(-1);
                    grandP->setBalance(0);
                    child->setBalance(0);
                }
            }
        }
    }
    // when p is right child of grandparent
    else {
        grandP->updateBalance(1);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key,Value>* parent) {
    AVLNode<Key,Value>* temp = parent;
    AVLNode<Key,Value>* grandTemp = parent->getParent();
    AVLNode<Key,Value>* leftChild = parent->getLeft();
    // if left child does not have right child, parent becomes right child of left child. left child becomes child of parent's parent
    // if left child has right child, right child becomes left child of parent and parent becomes right child of left child. left child becomes child of parent's parent
    if (leftChild->getRight() != nullptr) parent->setLeft(leftChild->getRight());
    leftChild->setRight(parent);
    parent->setParent(leftChild);
    leftChild->setParent(grandTemp);
    if (grandTemp->getLeft() == temp) {
        grandTemp->setLeft(leftChild);
    }
    else {
        grandTemp->setRight(leftChild); 
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key,Value>* parent) {
    AVLNode<Key,Value>* temp = parent;
    AVLNode<Key,Value>* grandTemp = parent->getParent();
    AVLNode<Key,Value>* rightChild = parent->getRight();
    // if right child does not have left child, parent becomes left child of right child. right child becomes child of parent's parent
    // if right child has left child, left child becomes right child of parent and parent becomes left child of right child. right child becomes child of parent's parent
    if (rightChild->getLeft() != nullptr) parent->setRight(rightChild->getLeft());
    rightChild->setLeft(parent);
    parent->setParent(rightChild);
    rightChild->setParent(grandTemp);
    if (grandTemp->getLeft() == temp) {
        grandTemp->setLeft(rightChild);
    }
    else {
        grandTemp->setRight(rightChild); 
    }
}


/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
