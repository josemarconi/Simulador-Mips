#include "../includes/Scheduler.hpp"

Scheduler::Scheduler(RAM& ram, Disco& disco, vector<unique_ptr<Core>>& cores) : ram(ram), disco(disco), cores(cores) 
{
    for (int i = 1; i < 3; i++) 
    {
        cores.emplace_back(make_unique<Core>(i, ram, disco));
    }

    createAndAddProcess(1, "data/instructions0.txt", "data/setRegisters.txt", ram, disco);
    createAndAddProcess(2, "data/instructions1.txt", "data/setRegisters.txt", ram, disco);
    createAndAddProcess(3, "data/instructions2.txt", "data/setRegisters.txt", ram, disco);
    createAndAddProcess(4, "data/instructions3.txt", "data/setRegisters.txt", ram, disco);
    createAndAddProcess(5, "data/instructions4.txt", "data/setRegisters.txt", ram, disco);
    createAndAddProcess(6, "data/instructions5.txt", "data/setRegisters.txt", ram, disco);

    //schedule_FCFS(ram, disco); 
    schedule_SJF(ram, disco);
}

void Scheduler::createAndAddProcess(int PCB_ID, const string& arquivoInstrucoes, const string& arquivoRegistros, RAM& ram, Disco& disco) 
{  
    Processos* process = new Processos(PCB_ID, arquivoInstrucoes);

    try {
        if (!fs::exists(arquivoInstrucoes)) {
            cerr << "Arquivo de instruções não encontrado: " << arquivoInstrucoes << endl;
            return;
        }

        if (!fs::exists(arquivoRegistros)) {
            cerr << "Arquivo de registros não encontrado: " << arquivoRegistros << endl;
            return;
        }

        process->StructionsLoad(arquivoInstrucoes);
        process->RegistersLoad(arquivoRegistros, ram, disco);

        process_queue.push(process);
        sjf_queue.push(process);

        }
        catch (const exception& e) {
            cerr << "Erro ao carregar processos: " << e.what() << endl;
        } 
}

void Scheduler::schedule_FCFS(RAM& ram, Disco& disco) 
{
    unique_lock<mutex> lock(scheduler_mutex);
    
    vector<thread> threads;
    
    while (!process_queue.empty() || any_of(cores.begin(), cores.end(), [](const unique_ptr<Core>& core) { return core->isBusy(); })) {
        if (!process_queue.empty())
        {
            for (auto& core : cores) {
                if (!core->isBusy() && !process_queue.empty())  
                {
                    Processos* process = process_queue.front();
                    process_queue.pop();

                    if (process_queue.empty()) 
                    {
                        process->pcb.quantum = std::numeric_limits<int>::max(); // Define um quantum muito grande
                    }

                    cout << endl << endl;
                    cout << "Processo " << process->pcb.ID << " sendo executado no core " << core->ID << endl;

                    core->setBusy(true);
                    threads.emplace_back(&Core::executeProcess, core.get(), process, std::ref(process_queue), std::ref(ram), std::ref(disco)).detach();
                    break;
                }
                this_thread::sleep_for(chrono::milliseconds(10));
            }
        }
    }
}

void Scheduler::schedule_SJF(RAM& ram, Disco& disco) 
{
    unique_lock<mutex> lock(scheduler_mutex);
    
    vector<thread> threads;
    
    while (!sjf_queue.empty() || any_of(cores.begin(), cores.end(), [](const unique_ptr<Core>& core) { return core->isBusy(); })) {
        if (!sjf_queue.empty())
        {
            for (auto& core : cores) {
                if (!core->isBusy() && !sjf_queue.empty())  
                {
                    Processos* process = sjf_queue.top();
                    sjf_queue.pop();
                    
                    if (sjf_queue.empty()) 
                    {
                        process->pcb.quantum = std::numeric_limits<int>::max(); // Define um quantum muito grande
                    }
                    
                    cout << endl << endl;
                    cout << "Processo " << process->pcb.ID << " sendo executado no core " << core->ID << endl;
                    
                    core->setBusy(true);
                    threads.emplace_back(&Core::executeProcess_SJF, core.get(), process, std::ref(sjf_queue), std::ref(ram), std::ref(disco)).detach();
                    break;
                }
                this_thread::sleep_for(chrono::milliseconds(10));
            }
        }
    }
}

void Scheduler::debugProcessQueue() 
{
    cout << "Estado da fila de processos:" << endl;
    queue<Processos*> temp_queue = process_queue;
    while (!temp_queue.empty()) {
        Processos* process = temp_queue.front();
        temp_queue.pop();
        cout << "Processo ID: " << process->pcb.ID << ", Estado: " << process->pcb.state << endl;
    }
}