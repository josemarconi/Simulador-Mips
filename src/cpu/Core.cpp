#include "../includes/Core.hpp"

mutex coutMutex;

Core::Core(int id, RAM &ram, Disco &disco, Cache &cache) : ID(id), ram(ram), disco(disco), cache(cache), PC(0), Clock(0), is_busy(false) {}

void Core::executeProcess(Processos *process, queue<Processos *> &processQueue, RAM &ram, Disco &disco, Cache &cache)
{
    lock_guard<mutex> lock(coutMutex);
    try
    {
        if (process == nullptr)
        {
            cerr << "Erro: Tentativa de executar processo nulo no Core " << ID << endl;
            return;
        }

        process->execute(ram, disco, Clock);

        switch (process->pcb.state)
        {
        case Processos::TERMINATED:
            cout << endl
                 << "Processo " << process->pcb.ID << " finalizado no Core " << ID << endl;
            cout << "Clock <" << Clock << ">" << endl;
            break;
        case Processos::BLOCK:
            cout << endl
                 << "Processo " << process->pcb.ID << " bloqueado no Core " << ID << endl;
            break;
        }

        if (process->pcb.state == Processos::BLOCK)
        {
            processQueue.push(process);
        }
    }
    catch (const exception &e)
    {
        cerr << "Erro na execução do core " << ID << ": " << e.what() << endl;
    }

    setBusy(false);
}

void Core::executeProcess_SJF(Processos *process, vector<int> &binary_indices, unordered_map<int, Processos *> &binary_process_map,
                              RAM &ram, Disco &disco, Cache &cache, 
                              float& durationTotal) 
{
    lock_guard<mutex> lock(coutMutex);

    try
    {
        if (process == nullptr)
        {
            cerr << "Erro: Tentativa de executar processo nulo no Core " << ID << endl;
            return;
        }

        process->execute(ram, disco, Clock);

        switch (process->pcb.state)
        {
        case Processos::TERMINATED:
            cout << endl
                 << "Processo " << process->pcb.ID << " finalizado no Core " << ID << endl;
            cout << "Clock <" << Clock << ">" << endl;
            break;

        case Processos::BLOCK:
            cout << endl
                 << "Processo " << process->pcb.ID << " bloqueado no Core " << ID << endl;

            // Encontrar índice binário correspondente ao processo bloqueado
            int blocked_index = -1;
            for (const auto &pair : binary_process_map)
            {
                if (pair.second == process)
                {
                    blocked_index = pair.first;
                    break;
                }
            }

            if (blocked_index != -1)
            {
                auto swap = chrono::high_resolution_clock::now();
  
                binary_indices.push_back(blocked_index);

                // Reordenar vetor de índices binários com base no quantum do processo
                sort(binary_indices.begin(), binary_indices.end(), [&](int a, int b)
                     { return binary_process_map[a]->pcb.quantum < binary_process_map[b]->pcb.quantum; });
                
                auto end_swap = chrono::high_resolution_clock::now();
                chrono::duration<double> duration_swap = end_swap - swap;
                durationTotal += duration_swap.count();
                cout << "Tempo de execução da busca no map: " << duration_swap.count() << " segundos" << endl;
            }
            break;
        }
    }
    catch (const exception &e)
    {
        cerr << "Erro na execução do core " << ID << ": " << e.what() << endl;
    }

    setBusy(false);
}

void Core::executeProcess_Lottery(Processos *process, vector<Processos *> &lottery_queue, RAM &ram, Disco &disco, Cache &cache)
{
    lock_guard<mutex> lock(coutMutex);

    try
    {
        if (process == nullptr)
        {
            cerr << "Erro: Tentativa de executar processo nulo no Core " << ID << endl;
            return;
        }

        process->execute(ram, disco, Clock);

        switch (process->pcb.state)
        {
        case Processos::TERMINATED:
            cout << endl
                 << "Processo " << process->pcb.ID << " finalizado no Core " << ID << endl;
            cout << "Clock <" << Clock << ">" << endl;
            break;
        case Processos::BLOCK:
            cout << endl
                 << "Processo " << process->pcb.ID << " bloqueado no Core " << ID << endl;
            break;
        }

        if (process->pcb.state == Processos::BLOCK)
        {
            lottery_queue.push_back(process);
        }
    }
    catch (const exception &e)
    {
        cerr << "Erro na execução do core " << ID << ": " << e.what() << endl;
    }

    setBusy(false);
}

bool Core::isBusy() const
{
    lock_guard<mutex> lock(coreSecurity);
    return is_busy;
}

void Core::setBusy(bool busy)
{
    lock_guard<mutex> lock(coreSecurity);
    is_busy = busy;
}