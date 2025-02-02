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

namespace fs = std::filesystem;

#include "Processos.hpp"
#include "Core.hpp"

class Scheduler
{
public:
    Scheduler(RAM& ram, Disco& disco, vector<unique_ptr<Core>>& cores, Cache& cache);

    void createAndAddProcess(int PCB_ID, const std::string& arquivoInstrucoes, const std::string& arquivoRegistros, RAM& ram, Disco& disco);    
    void createAndAddProcesses(const vector<string>& arquivosInstrucoes, const string& arquivoRegistros, RAM& ram, Disco& disco);
    void debugProcessQueue();
    
private:
    queue<Processos*> process_queue;
    priority_queue<Processos*, vector<Processos*>, Processos::SJFComparator> sjf_queue;
    vector<Processos*> lottery_queue;

    RAM& ram;
    Disco& disco;
    vector<unique_ptr<Core>>& cores;
    Cache& cache;

    mutex scheduler_mutex;

    void schedule_FCFS(RAM& ram, Disco& disco, Cache& cache);
    void schedule_SJF(RAM& ram, Disco& disco, Cache& cache);
    void schedule_Lottery(RAM& ram, Disco& disco, Cache& cache);
};

#endif