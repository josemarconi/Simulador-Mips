#ifndef PROCESSOS_HPP
#define PROCESSOS_HPP

#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <condition_variable>

#include "Instruction.hpp"
#include "Registers.hpp"
#include "Opcode.hpp"
#include "RAM.hpp"
#include "Disco.hpp"
#include "UnidadeControle.hpp"

class Processos
{
public:

    enum State
    {
        READY,
        RUNNING,
        BLOCK,
        TERMINATED
    };

    struct PCB
    {
        int ID;
        State state;
        int quantum;
        int actual_Instruction;
        Registers regs;
        string files;
        int quantumTotal;
    };

    UnidadeControle uc;

    PCB pcb;
    vector<Instruction> instrucoes;
    string filename;

    Processos(int PCB_ID, const string &files);

    void RegistersLoad(const string& arquivoRegistros, RAM& ram, Disco& disco);

    void StructionsLoad(const string& arquivoInstrucoes);

    void execute(RAM &ram, Disco &disco, int &clock);
    void block();
    void unblock();

    struct SJFComparator 
    {
        bool operator()(const Processos* a, const Processos* b) 
            {
                return a->pcb.quantum > b->pcb.quantum; 
            }
    };

private:
    mutex process_mutex;
    condition_variable cond_process;
    bool ready_to_run = false;
    mutex output_mutex;
};

#endif