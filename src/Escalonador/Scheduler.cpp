#include "../includes/Scheduler.hpp"

Scheduler::Scheduler(RAM &ram, Disco &disco, vector<unique_ptr<Core>> &cores) : ram(ram), disco(disco), cores(cores)
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

    
    auto start_fcfs = std::chrono::high_resolution_clock::now();
    cout << endl << "Politica FCFS: " << endl;
    schedule_FCFS(ram, disco);
    auto end_fcfs = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_fcfs = end_fcfs - start_fcfs;
    std::cout << "Tempo de execução do FCFS: " << duration_fcfs.count() << " segundos" << std::endl;
    this_thread::sleep_for(chrono::milliseconds(10000));
    
    cout << endl << "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;
    
    cout << endl << "Politica SJF: " << endl;
    auto start_sjf = std::chrono::high_resolution_clock::now();
    schedule_SJF(ram, disco);
    auto end_sjf = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_sjf = end_sjf - start_sjf;
    std::cout << "Tempo de execução do SJF: " << duration_sjf.count() << " segundos" << std::endl;
    this_thread::sleep_for(chrono::milliseconds(10000));

    
    cout << endl << "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;

    cout << endl << "Politica de Loteria: " << endl;
    auto startlottery = std::chrono::high_resolution_clock::now();
    schedule_Lottery(ram, disco);
    auto endlottery = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> durationlottery = endlottery - startlottery;
    std::cout << "Tempo de execução do Lottery: " << durationlottery.count() << " segundos" << std::endl;
    this_thread::sleep_for(chrono::milliseconds(10000));
    
}

void Scheduler::createAndAddProcess(int PCB_ID, const string &arquivoInstrucoes, const string &arquivoRegistros, RAM &ram, Disco &disco)
{
    Processos *process = new Processos(PCB_ID, arquivoInstrucoes);

    try
    {
        if (!fs::exists(arquivoInstrucoes))
        {
            cerr << "Arquivo de instruções não encontrado: " << arquivoInstrucoes << endl;
            return;
        }

        if (!fs::exists(arquivoRegistros))
        {
            cerr << "Arquivo de registros não encontrado: " << arquivoRegistros << endl;
            return;
        }

        process->StructionsLoad(arquivoInstrucoes);
        process->RegistersLoad(arquivoRegistros, ram, disco);

        process_queue.push(process);
        sjf_queue.push(process);
        lottery_queue.push_back(process);
    }
    catch (const exception &e)
    {
        cerr << "Erro ao carregar processos: " << e.what() << endl;
    }
}

void Scheduler::schedule_FCFS(RAM &ram, Disco &disco)
{
    unique_lock<mutex> lock(scheduler_mutex);

    vector<thread> threads;

    while (!process_queue.empty() || any_of(cores.begin(), cores.end(), [](const unique_ptr<Core> &core)
                                            { return core->isBusy(); }))
    {
        if (!process_queue.empty())
        {
            for (auto &core : cores)
            {
                if (!core->isBusy() && !process_queue.empty())
                {
                    Processos *process = process_queue.front();
                    process_queue.pop();

                    if (process_queue.empty())
                    {
                        process->pcb.quantum = std::numeric_limits<int>::max(); 
                    }

                    //cout << endl << "Processo " << process->pcb.ID << " sendo executado no core " << core->ID << endl;

                    core->setBusy(true);
                    threads.emplace_back(&Core::executeProcess, core.get(), process, std::ref(process_queue), std::ref(ram), std::ref(disco)).detach();
                    break;
                }
                this_thread::sleep_for(chrono::milliseconds(10));
            }
        }
    }
}

void Scheduler::schedule_SJF(RAM &ram, Disco &disco)
{
    unique_lock<mutex> lock(scheduler_mutex);

    vector<thread> threads;

    while (!sjf_queue.empty() || any_of(cores.begin(), cores.end(), [](const unique_ptr<Core> &core)
                                        { return core->isBusy(); }))
    {
        if (!sjf_queue.empty())
        {
            for (auto &core : cores)
            {
                if (!core->isBusy() && !sjf_queue.empty())
                {
                    Processos *process = sjf_queue.top();
                    sjf_queue.pop();

                    if (sjf_queue.empty())
                    {
                        process->pcb.quantum = numeric_limits<int>::max();
                    }

                    //cout << endl << "Processo " << process->pcb.ID << " sendo executado no core " << core->ID << endl;
                    
                    core->setBusy(true);
                    threads.emplace_back(&Core::executeProcess_SJF, core.get(), process, std::ref(sjf_queue), std::ref(ram), std::ref(disco)).detach();
                    break;
                }
                this_thread::sleep_for(chrono::milliseconds(10));
            }
        }
    }
}

void Scheduler::schedule_Lottery(RAM &ram, Disco &disco)
{
    unique_lock<mutex> lock(scheduler_mutex);

    vector<thread> threads;

    while (!lottery_queue.empty() || any_of(cores.begin(), cores.end(), [](const unique_ptr<Core> &core)
                                            { return core->isBusy(); }))
    {

        for (auto &core : cores)
        {
            if (!lottery_queue.empty())
            {
                int total_tickets = 0;

                for (const auto &process : lottery_queue)
                {
                    total_tickets += process->pcb.lottery_tickets;
                }

                random_device rd;
                mt19937 gen(rd());
                uniform_int_distribution<> dis(0, total_tickets - 1);
                int winning_ticket = dis(gen);

                Processos *winning_process = nullptr;
                int current_ticket = 0;
                for (auto &process : lottery_queue)
                {
                    current_ticket += process->pcb.lottery_tickets;
                    if (current_ticket > winning_ticket)
                    {
                        winning_process = process;
                        break;
                    }
                }
                if (winning_process != nullptr)
                {
                    if (!core->isBusy() && !lottery_queue.empty())
                    {
                        lottery_queue.erase(remove(lottery_queue.begin(), lottery_queue.end(), winning_process), lottery_queue.end());

                        if (lottery_queue.empty())
                        {
                            winning_process->pcb.quantum = numeric_limits<int>::max();
                        }


                        //cout << endl << "Processo " << winning_process->pcb.ID << " sendo executado no core " << core->ID << endl;
                        

                        core->setBusy(true);
                        threads.emplace_back(&Core::executeProcess_Lottery, core.get(), winning_process, ref(lottery_queue), ref(ram), ref(disco)).detach();
                        break;
                    }
                }
            }
        }
    }
    this_thread::sleep_for(chrono::milliseconds(10));
}

void Scheduler::debugProcessQueue()
{
    cout << "Estado da fila de processos:" << endl;
    queue<Processos *> temp_queue = process_queue;
    while (!temp_queue.empty())
    {
        Processos *process = temp_queue.front();
        temp_queue.pop();
        cout << "Processo ID: " << process->pcb.ID << ", Estado: " << process->pcb.state << endl;
    }
}
