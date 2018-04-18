#pragma once

#include <parser/Instance.h>
#include <parser/Filereader.h>
#include "State.h"

using namespace parser::pddl;

class PlanAction {
public:
    std::string name;
    StringVec params;

    PlanAction( const std::string& name, const StringVec& params )
        : name( name ), params( params ) {}

    bool holds( State * s, Domain * d ) {
        Action * a = d->actions.get( name );
        if ( a ) {
            return holdsRec( s, d, a->pre );
        }

        return false; // add warning -> the action does not exist!
    }

    void apply( State * s, Domain * d ) {
        Action * a = d->actions.get( name );
        if ( a ) {
            applyRec( s, d, a->eff );
        }
    }

    friend std::ostream& operator<<( std::ostream& os, const PlanAction& pa ) {
        return pa.print( os );
    }

    virtual std::ostream& print( std::ostream& stream ) const {
        stream << "(" << name;
        for ( unsigned i = 0; i < params.size(); ++i ) {
            stream << " " << params[i];
        }
        stream << ")\n";
        return stream;
    }

protected:
    bool holdsRec( State * s, Domain * d, Condition * c ) {
        And * a = dynamic_cast< And * >( c );
        if ( a ) {
            for ( unsigned i = 0; i < a->conds.size(); ++i ) {
                if ( !holdsRec( s, d, a->conds[i] ) ) {
                    return false;
                }
            }
        }

        Ground * g = dynamic_cast< Ground * >( c );
        if ( g ) {
            if ( !s->holds( false, g->name, getObjectParameters( g->params ) ) ) {
                return false;
            }
        }

        Not * n = dynamic_cast< Not * >( c );
        if ( n ) {
            Ground * ng = n->cond;
            if ( !s->holds( true, ng->name, getObjectParameters( ng->params ) ) ) {
                return false;
            }
        }

        return true;
    }

    void applyRec( State * s, Domain * d, Condition * c ) {
        And * a = dynamic_cast< And * >( c );
        if ( a ) {
            for ( unsigned i = 0; i < a->conds.size(); ++i ) {
                applyRec( s, d, a->conds[i] );
            }
        }

        Ground * g = dynamic_cast< Ground * >( c );
        if ( g ) {
            s->addFluent( g->name, getObjectParameters( g->params ) );
        }

        Not * n = dynamic_cast< Not * >( c );
        if ( n ) {
            Ground * ng = n->cond;
            s->removeFluent( ng->name, getObjectParameters( ng->params ) );
        }
    }

    StringVec getObjectParameters( IntVec& groundParams ) {
        StringVec objParams;
        for ( unsigned j = 0; j < groundParams.size(); ++j ) {
            objParams.push_back( params[groundParams[j]] );
        }
        return objParams;
    }
};

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
