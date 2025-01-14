#include "../includes/Core.hpp"

mutex coutMutex;

Core::Core(int id, RAM& ram, Disco& disco) : ID(id), ram(ram), disco(disco), PC(0), Clock(0), is_busy(false){}

void Core::executeProcess(Processos* process, queue<Processos*>& processQueue, RAM& ram, Disco& disco) 
{
    lock_guard<mutex> lock(coutMutex);
    try {
        if (process == nullptr) 
        {
            cerr << "Erro: Tentativa de executar processo nulo no Core " << ID << endl;
            return;
        }

        process->execute(ram, disco, Clock);

        switch (process->pcb.state) 
        {
            case Processos::TERMINATED:
                cout << endl <<  "Processo " << process->pcb.ID << " finalizado no Core " << ID << endl;
                cout << "Clock <" << Clock << ">" << endl;
                break;
            case Processos::BLOCK:
                cout << endl << "Processo " << process->pcb.ID << " bloqueado no Core " << ID << endl;
                break;
        }

        if (process->pcb.state == Processos::BLOCK) 
        {
            processQueue.push(process); 
        }
    }
    catch (const exception& e) 
    {
        cerr << "Erro na execução do core " << ID << ": " << e.what() << endl;
    }

    setBusy(false);
}

void Core::executeProcess_SJF(Processos* process, priority_queue<Processos*, vector<Processos*>, Processos::SJFComparator>& processQueue, RAM& ram, Disco& disco)
{
    lock_guard<mutex> lock(coutMutex);

    try {
        if (process == nullptr) 
        {
            cerr << "Erro: Tentativa de executar processo nulo no Core " << ID << endl;
            return;
        }

        process->execute(ram, disco, Clock);

        switch (process->pcb.state) 
        {
            case Processos::TERMINATED:
                cout << endl <<  "Processo " << process->pcb.ID << " finalizado no Core " << ID << endl;
                cout << "Clock <" << Clock << ">" << endl;
                break;
            case Processos::BLOCK:
                cout << endl << "Processo " << process->pcb.ID << " bloqueado no Core " << ID << endl;
                break;
        }

        if (process->pcb.state == Processos::BLOCK) 
        {
            processQueue.push(process); 
        }
    }
    catch (const exception& e) 
    {
        cerr << "Erro na execução do core " << ID << ": " << e.what() << endl;
    }

    setBusy(false);
}

void Core::executeProcess_Lottery(Processos* process, vector<Processos*>& lottery_queue, RAM& ram, Disco& disco)
{
    lock_guard<mutex> lock(coutMutex);

    try {
        if (process == nullptr) 
        {
            cerr << "Erro: Tentativa de executar processo nulo no Core " << ID << endl;
            return;
        }

        process->execute(ram, disco, Clock);

        switch (process->pcb.state) 
        {
            case Processos::TERMINATED:
                cout << endl <<  "Processo " << process->pcb.ID << " finalizado no Core " << ID << endl;
                cout << "Clock <" << Clock << ">" << endl;
                break;
            case Processos::BLOCK:
                cout << endl << "Processo " << process->pcb.ID << " bloqueado no Core " << ID << endl;
                break;
        }

        if (process->pcb.state == Processos::BLOCK) 
        {
            lottery_queue.push_back(process); 
        }
    }
    catch (const exception& e) 
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