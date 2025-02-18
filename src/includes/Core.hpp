#include "Opcode.hpp"
#include "Instruction.hpp"
#include "Registers.hpp"
#include "ULA.hpp"
#include "RAM.hpp"
#include "InstructionDecode.hpp"
#include "Pipeline.hpp"
#include "Disco.hpp"
#include "Pipeline.hpp"
#include "Processos.hpp"
#include <mutex>
#include <queue>

class Core {
public:
    int ID;
    Registers regs;
    int PC;
    RAM& ram;
    Disco& disco;
    int Clock = 0;
    bool is_busy;
    mutable mutex coreSecurity;

    Core(int id, RAM& ram, Disco& disco);
    void executeProcess(Processos* process, queue<Processos*>& processQueue, RAM& ram, Disco& disco);
    bool isBusy() const;
    void setBusy(bool busy);
    void executeProcess_SJF(Processos *process, vector<int> &binary_indices, unordered_map<int, Processos *> &binary_process_map,
        RAM &ram, Disco &disco, Cache &cache, 
        float& durationTotal);
    void executeProcess_Lottery(Processos* process, vector<Processos*>& lottery_queue, RAM& ram, Disco& disco, Cache& cache);
};