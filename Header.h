#pragma once
#include<iostream>

using namespace std;
template<typename T>
class Node
{
public:
T data;
Node* next;
Node()
{
next = NULL;
}
Node(T val)
{
data = val;
next = NULL;
}
};  
template<typename T>
class SLinkedList
{
public:
Node<T>* head;
int count = 0;
SLinkedList()
{
head = NULL;
}
void insert(T val)
{
if (head== NULL && count==0)
{
head = new Node<T>;
head->data = val;
head->next = NULL;
count++;
}
else
{
Node<T>* current = head;
while (current->next != NULL)
{
current = current->next;
}
current->next = new Node<T>;
current = current->next;
current->next=NULL;
current->data = val;
count++;
}

}
void insertAtHead(T val)
{
Node<T>* current = new Node<T>;
current->next = head;
current->data = val;
head = current;
count++;

}
void InsertAtIndex(T val, int index)
{
if (index < 0 || index>count)
{
return;
}

int currIdx = 0;
Node<T>* current = head;
while (current && index > currIdx) {
current = current->next;
currIdx++;
}
if (index > 0 && current == NULL) return ;

Node<T>* temp = new Node<T>;
temp->data = val;
if (index == 0) {
temp->next = head;
head = temp;
}
else {
temp->next = current->next;
current->next = temp;
}

}
int search(T val)
{
int var = 0;
Node<T>* current = head;
while (current && current->data != val)
{
current = current->next;
var++;
}
return var;
}
void update(int idx, T val)
{
int temp = 0;
Node<T>* current = head;
while (current && temp != idx)
{
temp++;
current = current->next;
}
current->data = val;
}
void remove(T val)
{
Node<T>* priv = NULL;
Node<T>* current = head;
int idx = 0;
while (current&& current->data != val) {
priv = current;
current = current->next;
idx++;
}
if (current) {
if (priv) {
priv->next = current->next;
delete current;
}
else {
head = current->next;
delete current;
}
}

}
bool isEmpty()
{
if (head->next == NULL)
{
return 1;
}
return 0;
}
void print()
{
Node<T>* current = head;
cout << endl;
while (current!=NULL)
{
cout << current->data<<",";
current=current->next;
}
cout << "NULL\n";
}
};
template <typename T>
Node<T>* swapAdjacentNodes(Node<T> *head)
{
    
    Node<T> *newhead = head->next;
    Node<T> *current = head;
    
    while(current && current->next)
    {
	Node<T>* temp=current->next;
	current->next=temp->next;
	temp->next=current;
	
	
    }
    return newhead;
}
