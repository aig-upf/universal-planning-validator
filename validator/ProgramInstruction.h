#pragma once

#include <stdexcept>

#include "PlanAction.h"

// typedef std::pair< bool, long > InstructionResult;

class InstructionResult {
public:
    bool success;
    long targetProcedureId;
    long targetLine;

    InstructionResult( bool psuccess, long ptargetProcedureId, long ptargetLine):
        success( psuccess ), targetProcedureId( ptargetProcedureId ), targetLine( ptargetLine )
    {

    }
};

class ProgramInstruction {
public:
    long line;
    long procedureId;

    virtual ~ProgramInstruction() {}

    virtual InstructionResult run( Domain * d, Instance * ins, State * currentState ) = 0;
};

typedef std::vector< ProgramInstruction * > InstructionVec;

struct ProgramInstructionCmp {
    inline bool operator() (const ProgramInstruction * lhs, const ProgramInstruction * rhs) {
        if ( lhs->procedureId == rhs->procedureId ) {
            return lhs->line < rhs->line;
        }

        return lhs->procedureId > rhs->procedureId;
    }
};

class ActionInstruction : public ProgramInstruction {
public:
    std::string name;

    ActionInstruction( const std::string& instr ) {
        long firstDash = instr.find( '-' );
        long lastDash = instr.rfind( '-' );
        long penultimateDash = instr.substr( 0, lastDash ).rfind( '-' );

        bool existsProcedureId = false;
        std::string procedureIdStr = instr.substr( penultimateDash + 1, lastDash - penultimateDash - 1 );

        try {
            procedureId = std::stoi( procedureIdStr );
            existsProcedureId = true;
        }
        catch ( const std::invalid_argument& ia ) {
            procedureId = 0;  // only one procedure exists!
        }

        if ( existsProcedureId ) {
            name = instr.substr( firstDash + 1, penultimateDash - firstDash - 1 );
        }
        else {
            name = instr.substr( firstDash + 1, lastDash - firstDash - 1 );
        }

        line = std::stoi( instr.substr( lastDash + 1, instr.length() - lastDash ) );
    }

    InstructionResult run( Domain * d, Instance * ins, State * currentState ) {
        PlanAction pa( name, StringVec() );

        bool stateHolds = pa.holds( currentState, d );
        if ( stateHolds ) {
            pa.apply( currentState, d, ins );
        }

        return InstructionResult( stateHolds, procedureId, line + 1 );
    }
};

class GotoConditionInstruction : public ProgramInstruction {
public:
    std::string predicateName;
    StringVec predicateParams;

    GotoConditionInstruction( const std::string& instr, const StringVec& instrParams )
        : predicateParams( instrParams )
    {
        long firstDash = instr.find( '-' );
        long lastDash = instr.rfind( '-' );
        long penultimateDash = instr.substr( 0, lastDash ).rfind( '-' );

        predicateName = instr.substr( firstDash + 1, penultimateDash - firstDash - 1 );
        procedureId = std::stoi( instr.substr( penultimateDash + 1, lastDash - penultimateDash ) );
        line = std::stoi( instr.substr( lastDash + 1, instr.length() - lastDash - 1 ) );
    }

    InstructionResult run( Domain * d, Instance * ins, State * currentState ) {
        bool result = currentState->holds( false, predicateName, predicateParams );
        return InstructionResult( result, procedureId, line );
    }
};

class GotoInstruction : public ProgramInstruction {
public:
    long jumpLine;
    GotoConditionInstruction * condition;

    GotoInstruction( const std::string& instr ) {
        long firstDash = instr.find( '-' );
        long secondDash = -1;
        long thirdDash = instr.rfind( '-' );

        for ( unsigned i = firstDash + 1; i < thirdDash; ++i ) {
            if ( instr[i] == '-' ) {
                secondDash = i;
                break;
            }
        }

        procedureId = std::stoi( instr.substr( firstDash + 1, secondDash - firstDash) );
        line = std::stoi( instr.substr( secondDash + 1, thirdDash - secondDash) );
        jumpLine = std::stoi( instr.substr( thirdDash + 1, instr.length() - thirdDash - 1 ) );
    }

    virtual ~GotoInstruction() {
        delete condition;
    }

    InstructionResult run( Domain * d, Instance * ins, State * currentState ) {
        long nextLine = line + 1;

        InstructionResult res = condition->run( d, ins, currentState );
        if ( !res.success ) {
            nextLine = jumpLine;
        }

        return InstructionResult( true, procedureId, nextLine );
    }
};

class EndInstruction : public ProgramInstruction {
public:
    EndInstruction( const std::string& instr ) {
        long firstDash = instr.find( '-' );
        long lastDash = instr.rfind( '-' );
        long penultimateDash = instr.substr( 0, lastDash ).rfind( '-' );

        if ( firstDash == penultimateDash ) {
            procedureId = -1; // the end for the global program, not necessarily 0!!!
        }
        else {
            procedureId = std::stoi( instr.substr( penultimateDash + 1, lastDash - penultimateDash - 1 ) );
        }

        line = std::stoi( instr.substr( lastDash + 1, instr.length() - lastDash - 1 ) );
    }

    InstructionResult run( Domain * d, Instance * ins, State * currentState ) {
        return InstructionResult( true, procedureId, -1 );
    }
};

class CallInstruction : public ProgramInstruction {
public:
    long targetProcedureId;

    CallInstruction( const std::string& instr ) {
        long firstDash = instr.find( '-' );
        std::string callParams = instr.substr( firstDash + 1, instr.length() - firstDash - 1 );

        long subFirstDash = callParams.find( '-' );
        long subLastDash = callParams.rfind( '-' );

        targetProcedureId = std::stoi( callParams.substr( 0, subFirstDash ) );
        procedureId = std::stoi( callParams.substr( subFirstDash + 1, subLastDash - subFirstDash - 1 ) );
        line = std::stoi( callParams.substr( subLastDash + 1, callParams.length() - subLastDash - 1 ) );
    }

    InstructionResult run( Domain * d, Instance * ins, State * currentState ) {
        return InstructionResult( true, targetProcedureId, 0 );
    }
};
