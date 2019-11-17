////////////////////////////////////////////////////////////////////////////////
// Module Name:  skip_list.h/hpp
// Authors:      Leonid Dworzanski, Sergey Shershakov
// Version:      2.0.0
// Date:         28.10.2018
//
// This is a part of the course "Algorithms and Data Structures"
// provided by  the School of Software Engineering of the Faculty
// of Computer Science at the Higher School of Economics.
////////////////////////////////////////////////////////////////////////////////

// !!! DO NOT include skip_list.h here, 'cause it leads to circular refs. !!!

#include <cstdlib>
#include <array>
#include <stdexcept>

//==============================================================================
// class NodeSkipList
//==============================================================================

template <class Value, class Key, int numLevels>
void NodeSkipList<Value, Key, numLevels>::clear(void)
{
    for (int i = 0; i < numLevels; ++i)
        Base::nextJump[i] = 0;

    Base::levelHighest = -1;
}

//------------------------------------------------------------------------------

template <class Value, class Key, int numLevels>
NodeSkipList<Value, Key, numLevels>::NodeSkipList(void)
{
    clear();
}

//------------------------------------------------------------------------------

template <class Value, class Key, int numLevels>
NodeSkipList<Value, Key, numLevels>::NodeSkipList(const Key& tkey)
{
    clear();

    Base::Base::key = tkey;
}

//------------------------------------------------------------------------------

template <class Value, class Key, int numLevels>
NodeSkipList<Value, Key, numLevels>::NodeSkipList(const Key& tkey, const Value& val)
{
    clear();

    Base::Base::key = tkey;
    Base::Base::value = val;
}


//==============================================================================
// class SkipList
//==============================================================================

template <class Value, class Key, int numLevels>
SkipList<Value, Key, numLevels>::SkipList(double probability)
{
    _probability = probability;

    // Lets use _preHead as a final sentinel element
    for (int i = 0; i < numLevels; ++i)
        Base::_preHead->nextJump[i] = Base::_preHead;

    Base::_preHead->levelHighest = numLevels - 1;
}



template <class Value, class Key, int numLevels>

std::array<typename SkipList<Value, Key, numLevels>::Node*, numLevels + 1>

find(SkipList<Value, Key, numLevels> list, Key kkey)
{
    typename SkipList<Value, Key, numLevels>::Node* position = list.getPreHead();

    std::array<typename SkipList<Value, Key, numLevels>::Node*, numLevels + 1> jumps;

    for (int i = list.getPreHead() -> levelHighest; i >= 0; --i)
    {
        while (position -> nextJump[i] != list.getPreHead() && position -> nextJump[i] -> key < kkey)
        {
            position = position -> nextJump[i];
        }

        jumps[i] = position;
    }

    while (position -> next != list.getPreHead() && position -> next -> key < kkey)
    {
        position = position -> next;
    }

    jumps[numLevels] = position;

    return jumps;
}

template <class Value, class Key, int numLevels>
SkipList<Value, Key, numLevels>::~SkipList()
{

}

template <class Value, class Key, int numLevels>
void SkipList<Value, Key, numLevels>::insert(const Value& val, const Key& key)
{
    std::array<typename SkipList<Value, Key, numLevels>::Node*, numLevels + 1> jumps = find(*this, key);

    Node* toInsert = new Node(key, val);

    toInsert -> next = jumps[numLevels] -> next;

    jumps[numLevels] -> next = toInsert;

    for (int i = 0; i <= this -> _preHead -> levelHighest; ++i)
    {
        if (((double) (rand()) / RAND_MAX) <= this -> _probability)
        {
            toInsert -> nextJump[i] = jumps[i] -> nextJump[i];
            jumps[i] -> nextJump[i] = toInsert;
        }

        else break;
    }
}

template <class Value, class Key, int numLevels>
void SkipList<Value, Key, numLevels>::removeNext(Node* node)
{
    if (node == this -> _preHead)
    {
        throw std::invalid_argument("Ошибка с _preHead");
    }


    typename SkipList<Value, Key, numLevels>::Node* position = this -> _preHead;

    std::array<typename SkipList<Value, Key, numLevels>::Node*, numLevels + 1> jumps;

    for (int i = this -> _preHead->levelHighest; i >= 0; --i)
    {
        while (position -> nextJump[i] != this -> _preHead && position -> nextJump[i] -> key <= node -> key && position ->nextJump[i] != node)
        {
            position = position -> nextJump[i];
        }

        jumps[i] = position;
    }

    while (position -> next != this -> _preHead && position -> next -> key <= node -> key && position -> next != node)
    {
        position = position -> next;
    }

    if (position -> next != node)
    {
        throw std::invalid_argument("Узел не найден");
    }

    position -> next = position -> next -> next;

    for (int i = 0; i <= this -> _preHead -> levelHighest; ++i)
        if (jumps[i] -> nextJump[i] == node)
        {
            jumps[i] -> nextJump[i] = jumps[i] -> nextJump[i] -> nextJump[i];
        }

    delete node;
}


template <class Value, class Key, int numLevels>
typename SkipList<Value, Key, numLevels>::Node* SkipList<Value, Key, numLevels>::findLastLessThan(const Key& key) const
{
    return find(*this, key)[numLevels];
}


template <class Value, class Key, int numLevels>
typename SkipList<Value, Key, numLevels>::Node* SkipList<Value, Key, numLevels>::findFirst(const Key& kkey) const
{
    Node* run = findLastLessThan(kkey);

    if (run -> next != this -> _preHead && run -> next -> key == kkey)
    {
        return run -> next;
    }

    return nullptr;
}