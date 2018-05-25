#include "PlanningProgram.h"

PlanningProgram::PlanningProgram() {}

PlanningProgram::PlanningProgram( const std::string & s ) {
    parse( s );
}

PlanningProgram::~PlanningProgram() {
    for ( unsigned i = 0; i < instructions.size(); ++i ) {
        delete instructions[i];
    }
}

void PlanningProgram::parse( const std::string & s ) {
    Filereader f( s );

    std::map< unsigned, GotoInstruction * > pendingGotos; // <line, goto>
    std::map< unsigned, GotoConditionInstruction * > pendingGotoConds;

    while ( !f.f.eof() && !f.s.empty() ) {
        f.c = 0;
        f.next();

        std::string instrName = f.getToken();

        if ( startsWith( instrName, END_PREFIX ) ) {
            f.next();
            f.getToken(); // ignore stack row, useless for validation

            ProgramInstruction * instr = new EndInstruction( instrName );
            instructions.push_back( instr );
        }
        else if ( startsWith( instrName, INSTR_PREFIX ) ) {
            f.next();
            f.getToken(); // ignore stack row, useless for validation

            ProgramInstruction * instr = new ActionInstruction( instrName );
            instructions.push_back( instr );
        }
        else if ( startsWith( instrName, GOTO_PREFIX ) ) {
            GotoInstruction * instr = new GotoInstruction( instrName );
            instructions.push_back( instr );

            auto pendingCond = pendingGotoConds.find( instr->line );
            if ( pendingCond == pendingGotoConds.end() ) {
                pendingGotos[instr->line] = instr;
            }
            else {
                instr->condition = pendingCond->second;
                pendingGotoConds.erase( pendingCond );
            }
        }
        else if ( startsWith( instrName, GOTO_COND_PREFIX ) ) {
            StringVec instrParams;

            bool allParamsRead = false;
            while ( !allParamsRead ) {
                f.next();
                std::string currentParam = f.getToken();

                if ( startsWith( currentParam, "ROW-" ) ) {
                    allParamsRead = true;
                }
                else {
                    instrParams.push_back( currentParam );
                }
            }

            GotoConditionInstruction * instr = new GotoConditionInstruction( instrName, instrParams );

            auto pendingGoto = pendingGotos.find( instr->line );
            if ( pendingGoto == pendingGotos.end() ) {
                pendingGotoConds[instr->line] = instr;
            }
            else {
                pendingGoto->second->condition = instr;
                pendingGotos.erase( pendingGoto );
            }
        }

        ++f.r;
        getline( f.f, f.s );
    }

    std::sort( instructions.begin(), instructions.end(), ProgramInstructionCmp() );

    name = s;
}

bool PlanningProgram::run( Domain * d, Instance * ins, State * currentState ) {
    if ( instructions.empty() ) {
        // error
    }

    unsigned currentLine = 0;

    while ( currentLine >= 0 && currentLine < instructions.size() ) {
        ProgramInstruction * pi = instructions[currentLine];
        InstructionResult result = pi->run( d, ins, currentState );
        if ( result.first ) {
            currentLine = result.second;
        }
        else {
            return false;
        }
    }

    return true;
}

unsigned PlanningProgram::getNumActions() const {
    return 0;
}

bool startsWith( const std::string& testStr, const std::string& prefix ) {
    if ( prefix.length() <= testStr.length() ) {
        return testStr.substr( 0, prefix.length() ) == prefix;
    }

    return false;
}
