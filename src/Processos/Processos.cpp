#include "../includes/Processos.hpp"

Processos::Processos(int PCB_ID, const std::string &files) {

    pcb.ID = PCB_ID;
    pcb.state = READY;
    pcb.quantum = 12;
    pcb.actual_Instruction = 0;
    filename = files;
    pcb.quantumTotal = 0;
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
        if (opcodeStr == "ADD") opcode = ADD;
        else if (opcodeStr == "SUB") opcode = SUB;
        else if (opcodeStr == "AND") opcode = AND;
        else if (opcodeStr == "OR") opcode = OR;
        else if (opcodeStr == "STORE") opcode = STORE;
        else if (opcodeStr == "LOAD") opcode = LOAD;
        else if (opcodeStr == "ENQ") opcode = ENQ;
        else if (opcodeStr == "IF_igual") opcode = IF_igual;
        else {
            cerr << "Instrução inválida ignorada: " << opcodeStr << endl;
            continue;
        }

        Instruction instrucao(opcode, reg1, reg2, reg3);
        instrucoes.push_back(instrucao);
    }

    arquivo.close();
}

void Processos::execute(RAM &ram, Disco &disco, int &clock)
{   
    pcb.state = RUNNING;

    int PC = pcb.actual_Instruction * 4;

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
                cout << endl << "--- PROCESSO BLOQUEADO, QUANTUM EXCEDIDO ---";
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
            cout <<  endl << endl << "----------- Processo " << pcb.ID << " concluído" << "-----------"<< endl << endl << endl;
            
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