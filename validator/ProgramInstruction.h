#pragma once

#include "PlanAction.h"

typedef std::pair< bool, long > InstructionResult;

class ProgramInstruction {
public:
    unsigned line;
    unsigned procedureId;

    virtual ~ProgramInstruction() {}

    virtual InstructionResult run( Domain * d, Instance * ins, State * currentState ) = 0;
};

class ActionInstruction : public ProgramInstruction {
public:
    std::string name;

    ActionInstruction( const std::string& instr ) {
        long firstDash = instr.find( '-' );
        long lastDash = instr.rfind( '-' );

        name = instr.substr( firstDash + 1, lastDash - firstDash - 1 );
        line = std::stoi( instr.substr( lastDash + 1, instr.length() - lastDash ) );
    }

    InstructionResult run( Domain * d, Instance * ins, State * currentState ) {
        PlanAction pa( name, StringVec() );

        bool stateHolds = pa.holds( currentState, d );
        if ( stateHolds ) {
            pa.apply( currentState, d, ins );
        }

        return InstructionResult( stateHolds, line + 1 );
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
        return InstructionResult( result, line );
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
        if ( !res.first ) {
            nextLine = jumpLine;
        }

        return InstructionResult( true, nextLine );
    }
};

class EndInstruction : public ProgramInstruction {
public:
    EndInstruction( const std::string& instr ) {
        long lastDash = instr.rfind( '-' );
        line = std::stoi( instr.substr( lastDash + 1, instr.length() - lastDash - 1 ) );
    }

    InstructionResult run( Domain * d, Instance * ins, State * currentState ) {
        return InstructionResult( true, -1 );
    }
};
