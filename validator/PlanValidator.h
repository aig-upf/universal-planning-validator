#pragma once

#include "State.h"
#include "Message.h"
#include "Plan.h"

using namespace parser::pddl;

class PlanValidator {
private:
    bool verbose;

public:
    PlanValidator( bool verbose=false ) : verbose( verbose ) {}

    void validate( Domain * d, Instance * ins, Plan * p ) {
        State * currentState = new State( d, ins ); // initial state

        bool isValidSeq = runActionSequence( d, p, currentState );
        if ( isValidSeq ) {
            showMsg( "Plan executed successfully - checking goal" );
            checkGoal( d, ins, currentState );
        }

        delete currentState;
    }

private:
    bool runActionSequence( Domain * d, Plan * p, State * currentState ) {
        bool isValidSeq = true;

        unsigned actionNum = 1;
        auto action = p->actions.begin();

        for ( ; isValidSeq && action != p->actions.end(); ++action, ++actionNum ) {
            showMsg( "Checking next happening (time " + std::to_string( actionNum ) + ")" );

            if ( action->holds( currentState, d ) ) {
                action->apply( currentState, d );
                showMsg( "" );
            }
            else {
                showMsg( "Plan failed because of unsatisfied precondition in:" );
                showMsg( action->getActionName() );
                showErrorMsg( "Plan failed to execute" );
                isValidSeq = false;
            }
        }

        return isValidSeq;
    }

    void checkGoal( Domain * d, Instance * ins, State * finalState ) {
        if ( finalState->satisfiesGoal( d, ins ) ) {
            showSuccessMsg( "Plan valid" );
        }
        else {
            showErrorMsg( "Goal not satisifed - Plan invalid" );
        }
    }
};
