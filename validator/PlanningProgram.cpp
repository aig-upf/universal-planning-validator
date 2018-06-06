#include "PlanningProgram.h"

PlanningProgram::PlanningProgram() {}

PlanningProgram::PlanningProgram( const std::string & s ) {
    parse( s );
}

PlanningProgram::~PlanningProgram() {
    for ( auto it = procedures.begin(); it != procedures.end(); ++it ) {
        delete it->second;
    }
}

void PlanningProgram::parse( const std::string & s ) {
    Filereader f( s );

    InstructionVec instructions;

    std::map< std::pair< long, unsigned >, GotoInstruction * > pendingGotos; // <(procedure, line), goto>
    std::map< std::pair< long, unsigned >, GotoConditionInstruction * > pendingGotoConds;

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

            std::pair< long, unsigned > procLinePair( instr->procedureId, instr->line );

            auto pendingCond = pendingGotoConds.find( procLinePair );
            if ( pendingCond == pendingGotoConds.end() ) {
                pendingGotos[procLinePair] = instr;
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

            std::pair< long, unsigned > procLinePair( instr->procedureId, instr->line );

            auto pendingGoto = pendingGotos.find( procLinePair );
            if ( pendingGoto == pendingGotos.end() ) {
                pendingGotoConds[procLinePair] = instr;
            }
            else {
                pendingGoto->second->condition = instr;
                pendingGotos.erase( pendingGoto );
            }
        }
        else if ( startsWith( instrName, CALL_PREFIX ) ) {
            f.next();
            f.getToken(); // ignore stack row, useless for validation

            ProgramInstruction * instr = new CallInstruction( instrName );
            instructions.push_back( instr );
        }

        ++f.r;
        getline( f.f, f.s );
    }

    addInstructionsToProcedures( instructions );

    name = s;
}

bool PlanningProgram::run( Domain * d, Instance * ins, State * currentState ) {
    // if ( instructions.empty() ) {
        // error
    // }

    std::stack< std::pair< long, long > > callStack; // (procedure, line) stack

    ProgramProcedure * currentProcedure = procedures[mainProcedureId];
    long currentLine = 0;

    bool executionCompleted = false;

    while ( !executionCompleted ) {
        ProgramInstruction * pi = currentProcedure->getInstruction( currentLine );
        InstructionResult result = pi->run( d, ins, currentState );
        if ( result.success ) {
            if ( result.targetLine < 0 ) {
                if ( callStack.empty() ) {
                    executionCompleted = true;
                }
                else {
                    std::pair< long, long > call = callStack.top();
                    currentProcedure = procedures[call.first];
                    currentLine = call.second;
                    callStack.pop();
                }
            }
            else {
                // change of procedure
                if ( currentProcedure->procedureId != result.targetProcedureId ) {
                    callStack.push( std::make_pair( currentProcedure->procedureId, currentLine + 1 ) );
                    currentProcedure = procedures[result.targetProcedureId];
                }

                currentLine = result.targetLine;
            }
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

void PlanningProgram::addInstructionsToProcedures( InstructionVec& instructions ) {
    // sort them in decreasing order of procedure, and increasing line number
    std::sort( instructions.begin(), instructions.end(), ProgramInstructionCmp() );

    long maxProcedureId = -1;

    for ( auto instr = instructions.begin(); instr != instructions.end(); ++instr ) {
        ProgramInstruction * pi = *instr;

        if ( pi->procedureId < 0 ) { // global end instruction
            pi->procedureId = maxProcedureId;
        }

        maxProcedureId = std::max( maxProcedureId, pi->procedureId );

        addInstructionToProcedure( pi );
    }

    // the main procedure has always the highest procedure id
    mainProcedureId = maxProcedureId;
}

void PlanningProgram::addInstructionToProcedure( ProgramInstruction * pi ) {
    if ( procedures.find( pi->procedureId ) == procedures.end() ) {
        procedures[pi->procedureId] = new ProgramProcedure( pi->procedureId );
    }

    procedures[pi->procedureId]->addInstruction( pi );
}

bool startsWith( const std::string& testStr, const std::string& prefix ) {
    if ( prefix.length() <= testStr.length() ) {
        return testStr.substr( 0, prefix.length() ) == prefix;
    }

    return false;
}
