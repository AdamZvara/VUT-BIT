//======== Copyright (c) 2021, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     Test Driven Development - priority queue code
//
// $NoKeywords: $ivs_project_1 $tdd_code.cpp
// $Author:     Adam Zvara <xzvara01@stud.fit.vutbr.cz>
// $Date:       $2021-02-21
//============================================================================//
/**
 * @file tdd_code.cpp
 * @author Adam Zvara
 * 
 * @brief Implementace metod tridy prioritni fronty.
 */

#include <stdlib.h>
#include <stdio.h>

#include "tdd_code.h"

//============================================================================//
// ** ZDE DOPLNTE IMPLEMENTACI **
//
// Zde doplnte implementaci verejneho rozhrani prioritni fronty (Priority Queue)
// 1. Verejne rozhrani fronty specifikovane v: tdd_code.h (sekce "public:")
//    - Konstruktor (PriorityQueue()), Destruktor (~PriorityQueue())
//    - Metody Insert/Remove/Find a GetHead
//    - Pripadne vase metody definovane v tdd_code.h (sekce "protected:")
//
// Cilem je dosahnout plne funkcni implementace prioritni fronty implementovane
// pomoci tzv. "single-linked list", ktera bude splnovat dodane testy 
// (tdd_tests.cpp).
//============================================================================//

PriorityQueue::PriorityQueue()
{
    m_pHead = NULL;
    size = 0;
}

PriorityQueue::~PriorityQueue()
{
    while (m_pHead != NULL)
    {
        //Postupne posuvanie hlavicky fronty a mazanie prejdenych prvkov
        Element_t *to_delete = m_pHead;
        m_pHead = m_pHead->pNext;

        delete to_delete;
    }
    size = 0;
}

void PriorityQueue::Insert(int value)
{
    if (m_pHead == nullptr) //vkladanie do prazdneho fronty
    {
        m_pHead = CreateElement(value, nullptr);
        size++;
    } 
    //vkladanie na prvu poziciu do (neprazdneho) fronty
    else if (m_pHead->value <= value)     
    {
        Element_t *to_insert = CreateElement(value, m_pHead);
        m_pHead = to_insert;
        size++;
    }
    else 
    {
        /* Pri vkladani prvku do vnutra fronty porovnavam hodnoty 
         * s nasledujucim prvkom za pointerom tmp pretoze 
         * potrebujem zmenit pointer pNext predchadzajuceho prvku
         *
         * napr. queue={100,75,50,25}; insert(60);
         * 1. tmp:{->100,75,50,25} porovnavam so 75;
         * 2. tmp:{100,->75,50,25} porovavam s 50 - za tento prvok budem vkladat
         *                         prepis pNext: novy prvok -> 50, 75 -> novy prvok
         */

        Element_t *tmp = m_pHead;
        while (tmp->pNext != nullptr)
        {
            if (tmp->pNext->value <= value)
            {
                Element_t *to_insert = CreateElement(value, tmp->pNext);
                tmp->pNext = to_insert;
                size++;
                break;
            }
            tmp = tmp->pNext; 
        }
        //pridanie prvku na koniec fronty
        if (tmp->pNext == nullptr)
        {
            Element_t *to_insert = CreateElement(value, nullptr);
            tmp->pNext = to_insert;
            size++;
        }
    }
}

bool PriorityQueue::Remove(int value)
{
    Element_t *tmp = m_pHead;
    if (tmp == nullptr) return false; 

    if (tmp->value == value) //odstranenie prvej hodnoty fronty (m_pHead)
    {
        m_pHead = tmp->pNext;
        delete tmp;
        size--;
        return true;
    }
    else 
    {
        /* Pri mazani prvku z vnutra fronty porovnavam hodnoty s nasledujucim prvkom,
         * pretoze potrebujem zmenit pointer pNext predchadzajuceho prvku
         * 
         * napr. queue = {100,75,50,25}; remove(50);
         * 1. tmp:{->100,75,50,25}; porovnavam so 75
         * 2. tmp:{100,->75,50,25}; porovnavam s 50 -> zhoda 
         *             ulozim si prvok na vymazanie
         *             prepisem pNext 75->25 
         */

        while (tmp->pNext != nullptr)
        {
            if (tmp->pNext->value == value)
            {
                Element_t *to_delete = tmp->pNext;
                tmp->pNext = to_delete->pNext;
                delete to_delete;
                size--;
                return true;
            }
            tmp = tmp->pNext;
        }
    }
    return false;
}

PriorityQueue::Element_t *PriorityQueue::Find(int value)
{
    Element_t *tmp = m_pHead;

    while (tmp != nullptr)
    {
        if (tmp->value == value)
        {
            return tmp;
        }
        tmp = tmp->pNext;
    }

    return nullptr;
}

size_t PriorityQueue::Length()
{
    return size;
}

PriorityQueue::Element_t *PriorityQueue::GetHead()
{
    return m_pHead;
}

PriorityQueue::Element_t *PriorityQueue::CreateElement(
        int value, 
        PriorityQueue::Element_t *pNext)
{
    return new Element_t 
    {
        .pNext = pNext,
        .value = value
    };
}

/*** Konec souboru tdd_code.cpp ***/
