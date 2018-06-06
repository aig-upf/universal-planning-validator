#pragma once

#include "ProgramInstruction.h"

using namespace parser::pddl;

class ProgramProcedure {
public:
    InstructionVec instructions;

    ~ProgramProcedure() {
        for ( unsigned i = 0; i < instructions.size(); ++i ) {
            delete instructions[i];
        }
    }

    void addInstruction( ProgramInstruction * pi ) {
        instructions.push_back( pi );
    }

// se puede hacer igual que una instruccion: al llamarlo pasar como parametros
// el procedimiento de origen y linea
};
