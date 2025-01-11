#include "../includes/UnidadeControle.hpp"

void UnidadeControle::loadRegister(Registers& regs, RAM& ram, Disco& disco, const string& instrFilename) 
{
    pipeline.setRegistersFromFile(regs, instrFilename, ram);
}

Instruction UnidadeControle::fetch(RAM& ram, int endereco)
{
    Instruction inst = pipeline.InstructionFetch(ram, endereco);
    return inst;
}

void UnidadeControle:: executePipeline(const DecodedInstruction& decoded, Registers& regs, RAM& ram, int& PC, Disco& disco, int& Clock)
{
    pipeline.Execute(decoded, regs, ram, PC, disco, Clock);
}