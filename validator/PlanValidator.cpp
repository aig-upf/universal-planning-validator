#include "PlanValidator.h"
#include "Message.h"

PlanValidator * PlanValidator::instance = nullptr;

PlanValidator::PlanValidator() : verbose( false ) {}

PlanValidator * PlanValidator::getInstance() {
    if ( instance == nullptr ) {
        instance = new PlanValidator();
    }
    return instance;
}

void PlanValidator::validate( Domain * d, Instance * ins, Plan * p ) {
    State * currentState = new State( d, ins ); // initial state

    bool isValidSeq = runActionSequence( d, p, currentState );
    if ( isValidSeq ) {
        showMsg( "Plan executed successfully - checking goal" );
        checkGoal( d, ins, currentState );
    }

    delete currentState;
}

void PlanValidator::setVerbose( bool v ) {
    verbose = v;
}

bool PlanValidator::getVerbose() const {
    return verbose;
}

bool PlanValidator::runActionSequence( Domain * d, Plan * p, State * currentState ) {
    bool isValidSeq = true;

    unsigned actionNum = 1;
    auto action = p->actions.begin();

    for ( ; isValidSeq && action != p->actions.end(); ++action, ++actionNum ) {
        if ( verbose ) {
            showMsg( "Checking next happening (time " + std::to_string( actionNum ) + ")" );
        }

        if ( action->holds( currentState, d ) ) {
            action->apply( currentState, d );
            if ( verbose ) {
                showMsg( "" );    
            }
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

void PlanValidator::checkGoal( Domain * d, Instance * ins, State * finalState ) {
    if ( finalState->satisfiesGoal( d, ins ) ) {
        showSuccessMsg( "Plan valid" );
    }
    else {
        showErrorMsg( "Goal not satisifed - Plan invalid" );
    }
}
