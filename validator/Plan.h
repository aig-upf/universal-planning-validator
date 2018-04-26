#pragma once

#include <parser/Instance.h>
#include <parser/Filereader.h>

#include "PlanAction.h"

using namespace parser::pddl;

class Plan {
public:
    std::string name;
    std::vector< PlanAction > actions;

    Plan();
    Plan( const std::string & s );

    void parse( const std::string & s );

    friend std::ostream& operator<<( std::ostream& os, const Plan& p ) {
        return p.print( os );
    }

    virtual std::ostream& print( std::ostream& stream ) const {
        for ( unsigned i = 0; i < actions.size(); ++i ) {
            stream << actions[i];
        }
        return stream;
    }
};
