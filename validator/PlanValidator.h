#pragma once

#include "State.h"
#include "Plan.h"

using namespace parser::pddl;

class PlanValidator {
private:
    static PlanValidator * instance;
    bool verbose;

protected:
    PlanValidator();

public:
    static PlanValidator * getInstance();

    void validate( Domain * d, Instance * ins, Plan * p ) const;

    void setVerbose( bool v );
    bool getVerbose() const;

private:
    bool runActionSequence( Domain * d, Instance * ins, Plan * p, State * currentState ) const;

    void checkGoal( Domain * d, Instance * ins, Plan * p, State * finalState ) const;

    double getPlanCost( Domain * d, Plan * p, State * finalState ) const;
};

bool showVerbose();
