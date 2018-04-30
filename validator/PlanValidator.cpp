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

void PlanValidator::validate( Domain * d, Instance * ins, Plan * p ) const {
    showMsg( "Checking plan: " + p->name );

    if ( verbose ) {
        showMsg( "Plan Validation details\n-----------------------\n" );
    }

    State * currentState = new State( d, ins ); // initial state

    bool isValidSeq = runActionSequence( d, ins, p, currentState );
    if ( isValidSeq ) {
        showMsg( "Plan executed successfully - checking goal" );
        checkGoal( d, ins, p, currentState );
    }

    delete currentState;
}

void PlanValidator::setVerbose( bool v ) {
    verbose = v;
}

bool PlanValidator::getVerbose() const {
    return verbose;
}

bool PlanValidator::runActionSequence( Domain * d, Instance * ins, Plan * p, State * currentState ) const {
    bool isValidSeq = true;

    unsigned actionNum = 1;
    auto action = p->actions.begin();

    for ( ; isValidSeq && action != p->actions.end(); ++action, ++actionNum ) {
        if ( verbose ) {
            showMsg( "Checking next happening (time " + std::to_string( actionNum ) + ")" );
        }

        if ( action->holds( currentState, d ) ) {
            action->apply( currentState, d, ins );
            if ( verbose ) {
                showMsg( "" );
            }
        }
        else {
            showMsg( "Plan failed because of unsatisfied precondition in:" );
            showMsg( action->getActionName() );
            showErrorMsg( "Plan failed to execute\n" );
            isValidSeq = false;
        }
    }

    return isValidSeq;
}

void PlanValidator::checkGoal( Domain * d, Instance * ins, Plan * p, State * finalState ) const {
    if ( finalState->satisfiesGoal( d, ins ) ) {
        showSuccessMsg( "Plan valid" );
        showMsg( "Value: " + std::to_string( getPlanCost( d, p, finalState ) ) );
    }
    else {
        showErrorMsg( "Goal not satisifed - Plan invalid\n" );
    }
}

double PlanValidator::getPlanCost( Domain * d, Plan * p, State * finalState ) const {
    if ( d->costs ) {
        return finalState->getTotalCostValue();
    }

    return p->actions.size();
}

bool showVerbose() {
   return PlanValidator::getInstance()->getVerbose();
}
