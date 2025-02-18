#ifndef CACHE_HPP
#define CACHE_HPP

#include <unordered_map>
#include <queue>
#include <iostream>
#include "Instruction.hpp"

using namespace std;

class Cache {
private:
    struct CacheLine 
    {
        Instruction data;
        bool dirty;  
    };

    unordered_map<int, CacheLine> cache;
    queue<int> fifoQueue;
    int capacidade;

public:
    Cache(int capacidade);

    bool contains(int address);
    Instruction get(int address);
    void set(int address, const Instruction& value, bool writeBack = false);
    void evict();
    void markDirty(int address);
};

#endif