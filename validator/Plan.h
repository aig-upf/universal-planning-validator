#pragma once

#include <parser/Instance.h>
#include <parser/Filereader.h>

#include "PlanAction.h"

using namespace parser::pddl;

class Plan {
public:
    std::vector< PlanAction > actions;

    Plan() {}

    Plan( const std::string & s ) {
        parse( s );
    }

    void parse( const std::string & s ) {
        Filereader f( s );

        while ( !f.f.eof() && !f.s.empty() ) {
            f.c = 0;

            if ( f.getChar() != ';' ) {
                f.next();
                f.assert_token( "(" );
                f.next();

                std::string actionName = f.getToken();
                f.next();

                StringVec actionParams;
                while ( f.getChar() != ')' ) {
                    actionParams.push_back( f.getToken() );
                    f.next();
                }

                PlanAction pa( actionName, actionParams);
                actions.push_back( pa );
            }

            ++f.r;
            getline( f.f, f.s );
        }
    }

    void validate( Domain * d, Instance * ins ) {
        // build initial state
        State * currentState = new State( d, ins );

        bool validSeq = true;

        for ( unsigned i = 0; i < actions.size() && validSeq; ++i ) {
            if ( actions[i].holds( currentState, d ) ) {
                actions[i].apply( currentState, d );
            }
            else {
                std::cout << "Plan failed to execute\n";
                validSeq = false;
            }
        }

        //comprobar si se cumplen las condiciones de la meta!
        if ( validSeq ) {
            std::cout << "Plan executed successfully - checking goal\n";

            if ( currentState->satisfiesGoal( d, ins ) ) {
                std::cout << "Plan valid\n";
            }
            else {
                std::cout << "Goal not satisifed\nPlan invalid\n";
            }
        }

        delete currentState;
    }

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
