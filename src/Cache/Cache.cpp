#include "../includes/Cache.hpp"

Cache::Cache(int capacidade) : capacidade(capacidade) {}

bool Cache::contains(int address) 
{
    return cache.find(address) != cache.end();
}

Instruction Cache::get(int address) {
    if (contains(address)) 
    {
        return cache[address].data;
    } else 
    {
        cerr << "Erro: Endereço " << address << " não está no cache!" << endl;
        return Instruction(); // Retorna uma instrução padrão
    }
}

void Cache::set(int address, const Instruction& value, bool writeBack) {
    if (cache.size() >= capacidade) 
    {
        evict();
    }
    
    cache[address] = {value, writeBack};
    fifoQueue.push(address);
}

void Cache::evict() 
{
    if (fifoQueue.empty()) return;

    int oldAddress = fifoQueue.front();
    fifoQueue.pop();

    if (cache[oldAddress].dirty) 
    {
        cout << "Escrevendo dado modificado de R" << oldAddress << " na memória principal (Write-back)." << endl;
    }
    
    cache.erase(oldAddress);
    cout << "Removendo R" << oldAddress << " do cache." << endl;
}

void Cache::markDirty(int address) 
{
    if (contains(address)) 
    {
        cache[address].dirty = true;
    }
}