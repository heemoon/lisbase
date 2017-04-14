/*
 * WQueue.h
 *
 *  Created on: 2016年8月5日
 *      Author: 58
 */

#ifndef WEDIS_BASE_CONTAINER_WQUEUE_H_
#define WEDIS_BASE_CONTAINER_WQUEUE_H_

/**
 * 这是一个无锁化queue实现，使用CAS
 */
namespace wRedis {

class WQueue {
};

} /* namespace wRedis */

#endif /* WEDIS_BASE_CONTAINER_WQUEUE_H_ */

//
//#include <iostream>
//#include <pthread.h>
//#include <stdlib.h>
//
//#include "PrintError.h"
//
//class CLMutex;
//
//CLMutex* g_pMutex;
//
//#define CAS(ptr , oldvalue , newvalue) __sync_bool_compare_and_swap(ptr , oldvalue , newvalue)
//
//template<typename T>
//struct Node
//{
//    T* data;
//    Node<T>* next;
//};
//
//template<typename T>
//struct Queue
//{
//    Node<T>* head;
//    Node<T>* tail;
//};
//
//template<typename T>
//Queue<T>*
//queueNew(void)
//{
//    Node<T>* tmp = new Node<T>;
//    Queue<T>* queue = new Queue<T>;
//    queue->head = queue->tail = tmp;
//    return queue;
//}
//
//Queue<int>* myqueue;
//
//template<typename T>
//void
//queuePush(Queue<T>* queue , T* data)
//{
//    Node<T>* onenode= new Node<T>;
//    onenode->data = data;
//    onenode->next = NULL;
//    Node<T>* tail;
//    Node<T>* next;
//
//    while(true)
//    {
//        tail = queue->tail;
//        next = tail->next;
//
//        if(tail != queue->tail)
//            continue;
//        if(next != NULL)
//        {
//            CAS(&queue->tail , tail , next);
//            continue;
//        }
//        if(CAS(&tail->next , next , onenode))
//            break;
//    }
//    CAS(&queue->tail , tail , onenode);
//}
//
//template<typename T>
//T*
//queuePop(Queue<T>* queue)
//{
//    Node<T>* head;
//    Node<T>* tail;
//    Node<T>* next;
//    T* data = NULL;
//    while(true)
//    {
//        head = queue->head;
//        tail = queue->tail;
//        next = head->next;
//
//        if(head != queue->head)
//            continue;
//        if(next == NULL)
//            return NULL;
//        if(head == tail)//prevent head exceed tail
//        {
//            CAS(&queue->tail , tail , next);
//            continue;
//        }
//        data = next->data;
//        if(CAS(&queue->head , head , next))
//            break;
//    }
//    delete head;
//    return data;
//}
