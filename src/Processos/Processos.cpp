#include "../includes/Processos.hpp"

Processos::Processos(int PCB_ID, const std::string &files) {

    pcb.ID = PCB_ID;
    pcb.state = READY;
    pcb.actual_Instruction = 0;
    filename = files;
    pcb.quantumTotal = 0;
    pcb.lottery_tickets = 0;
    pcb.priority = 0;
}

void Processos::RegistersLoad(const string& arquivoRegistros, RAM& ram, Disco& disco){
    uc.loadRegister(pcb.regs, ram, disco, arquivoRegistros);
}

void Processos::StructionsLoad(const string& arquivoInstrucoes) {
    ifstream arquivo(filename);
    if (!arquivo.is_open()) {
        throw runtime_error("Não foi possível abrir o arquivo de instruções: " + filename);
    }

    instrucoes.clear();
    string linha;

    while (getline(arquivo, linha)) {
        istringstream ss(linha);
        string opcodeStr;
        int reg1, reg2, reg3;
        char virgula;

        getline(ss, opcodeStr, ',');
        ss >> reg1 >> virgula >> reg2 >> virgula >> reg3;

        Opcode opcode;
        if (opcodeStr == "ADD"){ opcode = ADD; pcb.priority += 3;}
        else if (opcodeStr == "SUB") {opcode = SUB; pcb.priority += 3;}
        else if (opcodeStr == "AND") {opcode = AND; pcb.priority += 3;}
        else if (opcodeStr == "OR") {opcode = OR; pcb.priority += 3;}
        else if (opcodeStr == "STORE") {opcode = STORE; pcb.priority += 2;}
        else if (opcodeStr == "LOAD") {opcode = LOAD; pcb.priority += 2;}
        else if (opcodeStr == "ENQ") {opcode = ENQ; pcb.priority += 2;}
        else if (opcodeStr == "IF_igual") {opcode = IF_igual; pcb.priority += 2;}
        else {
            cerr << "Instrução inválida ignorada: " << opcodeStr << endl;
            continue;
        }

        Instruction instrucao(opcode, reg1, reg2, reg3);
        instrucoes.push_back(instrucao);
    }

    if(pcb.priority > 0  && pcb.priority <= 10)
    {
        pcb.quantum = 10;
        pcb.lottery_tickets = 5;
    }
    else if(pcb.priority >=11 && pcb.priority <= 20)
    {
        pcb.quantum = 15;
        pcb.lottery_tickets = 3;
    }
    else if(pcb.priority >= 21)
    {
        pcb.quantum = 20;
        pcb.lottery_tickets = 1;
    }

    cout << endl << "Priority" << pcb.priority << endl;

    arquivo.close();
}

void Processos::execute(RAM &ram, Disco &disco, int &clock)
{   
    pcb.state = RUNNING;

    int PC = pcb.actual_Instruction * 4;
    clock++;

    int counter = 0;
    int aux = 0;
    int anterior_counter = 0;

    try {
        for (size_t i = 0; i < instrucoes.size(); ++i) {
            ram.writeInstruction(i, instrucoes[i]);
            clock++;
            counter++;
        }

        while (PC < instrucoes.size() * 4) 
        {
            int total = 0;
            
            if (counter >= pcb.quantum) {
                //cout << endl << "--- PROCESSO BLOQUEADO, QUANTUM EXCEDIDO ---";
                pcb.actual_Instruction = PC / 4;
                block();
                break;
            }

            Instruction instr = uc.fetch(ram, PC / 4);
            clock++;
            counter++;

            DecodedInstruction decodedInstr = InstructionDecode(instr, pcb.regs);
            clock++;
            counter++;

            aux = clock;
            
            cout << endl << "[Processo " << pcb.ID << "] Executando instrução:" 
                      << " PC=" << PC 
                      << " Opcode=" << decodedInstr.opcode 
                      << " Destino=R" << decodedInstr.destiny 
                      << " Valor1=" << decodedInstr.value1 
                      << " Valor2=" << decodedInstr.value2 << endl;

            uc.executePipeline(decodedInstr, pcb.regs, ram, PC, disco, clock);
            PC += 4;

            aux = clock - aux;
            counter += aux;
            total = counter - anterior_counter;
            pcb.quantumTotal += total;

            
            cout << "State:" << pcb.state << endl;
            cout << "Arquivo fonte: " << filename << endl;
            cout << "Quantum total utilizado: " << pcb.quantumTotal << endl;
            cout << "clock = " << clock << endl;
            
            anterior_counter = counter;
        }
        
        if (PC >= instrucoes.size() * 4) 
        {
            pcb.state = TERMINATED;
            cout << endl << "----------- Processo " << pcb.ID << " concluído" << "-----------"<< endl;
            cout << "Quantum total utilizado: " << pcb.quantumTotal;
            
        } 
    }
    catch (const exception& e) {
        cerr << "Erro na execução do processo " << pcb.ID<< ": " << e.what() << endl;
        block();
    }
}

void Processos::block()
{
    unique_lock<mutex> lock(process_mutex);
    pcb.state = BLOCK;
}

void Processos::unblock()
{
    unique_lock<mutex> lock(process_mutex);
    pcb.state = READY;
}