#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <filesystem>
#include <iostream>
#include <algorithm>
#include <random>
#include <bitset>
#include <unordered_map>

namespace fs = std::filesystem;

#include "Processos.hpp"
#include "Core.hpp"

class Scheduler
{
public:
    Scheduler(RAM& ram, Disco& disco, vector<unique_ptr<Core>>& cores, Cache& cache);


    void createAndAddProcess(int PCB_ID, const string &arquivoInstrucoes, const string &arquivoRegistros, RAM &ram, Disco &disco);
    void createAndAddProcesses(const vector<string>& arquivosInstrucoes, const string& arquivoRegistros, RAM& ram, Disco& disco);
    void debugProcessQueue();
    
private:
    queue<Processos*> process_queue;
    priority_queue<Processos*, vector<Processos*>, Processos::SJFComparator> sjf_queue;
    vector<Processos*> lottery_queue;
    unordered_map<int, Processos*> binary_process_map;
    vector<int> binary_indices;

    Cache& cache;
    RAM& ram;
    Disco& disco;
    vector<unique_ptr<Core>>& cores;

    mutex scheduler_mutex;

    void schedule_FCFS(RAM& ram, Disco& disco, Cache& cache);
    void schedule_SJF(RAM& ram, Disco& disco, Cache& cache, float& durationTotal);
    void schedule_Lottery(RAM& ram, Disco& disco, Cache& cache);
};

#endif