#pragma once

#include <parser/Instance.h>
#include <parser/Filereader.h>

#include "Plan.h"

using namespace parser::pddl;

class PlanningProgram : public Plan {
public:
    // prefixes of the different kinds of instructions in a planning program
    const std::string INSTR_PREFIX = "INS-";
    const std::string GOTO_PREFIX = "GOTO-";
    const std::string COND_ASSIGN_PREFIX = "COND-ASSIGNMENT-";
    const std::string END_PREFIX = "INS-END-";

    PlanningProgram();
    PlanningProgram( const std::string & s );

    void parse( const std::string & s );
    bool run( Domain * d, Instance * ins, State * currentState );

    unsigned getNumActions() const;

    virtual std::ostream& print( std::ostream& stream ) const {
        return stream;
    }
};

bool startsWith( const std::string& testStr, const std::string& prefix );
