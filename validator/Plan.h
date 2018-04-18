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

    bool holds( State& s, Domain * d ) {
        Action * action = d->actions.get( name );
        Condition * actionPre = action->pre;

        // hacer analisis recursivo de las condiciones!

        And * a = dynamic_cast< And * >( actionPre );

        for ( unsigned i = 0; i < a->conds.size(); ++i ) {
            Ground * g = dynamic_cast< Ground * >( a->conds[i] );
            if ( g ) {
                const std::string& groundName = g->name;
                const IntVec& groundParams = g->params;

                StringVec groundParamsObj;
                for ( unsigned j = 0; j < groundParams.size(); ++j ) {
                    groundParamsObj.push_back( params[groundParams[j]] );
                }

                if ( !s.holds( false, groundName, groundParamsObj ) ) {
                    return false;
                }
            }
        }

        return true;
    }

    void apply( State& s, Domain * d ) {
        Action * action = d->actions.get( name );
        Condition * actionEff = action->eff;

        And * a = dynamic_cast< And * >( actionEff );

        for ( unsigned i = 0; i < a->conds.size(); ++i ) {
            Ground * g = dynamic_cast< Ground * >( a->conds[i] );
            if ( g ) {
                const std::string& groundName = g->name;
                const IntVec& groundParams = g->params;

                StringVec groundParamsObj;
                for ( unsigned j = 0; j < groundParams.size(); ++j ) {
                    groundParamsObj.push_back( params[groundParams[j]] );
                }

                s.addFluent( groundName, groundParamsObj );
            }

            Not * n = dynamic_cast< Not * >( a->conds[i] );
            if ( n ) {
                Ground * ng = n->cond;
                const std::string& groundName = ng->name;
                const IntVec& groundParams = ng->params;

                StringVec groundParamsObj;
                for ( unsigned j = 0; j < groundParams.size(); ++j ) {
                    groundParamsObj.push_back( params[groundParams[j]] );
                }

                s.removeFluent( groundName, groundParamsObj );
            }
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
        State initialState( d, ins );

        // validar each action depending on the current state and create new state if preconditions hold
        // throw exception if validation fails
        bool validSeq = true;

        for ( unsigned i = 0; i < actions.size(); ++i ) {
            if ( actions[i].holds( initialState, d ) ) {
                actions[i].apply( initialState, d );
            }
            else {
                std::cout << "invalid plan\n";
                validSeq = false;
                break;
            }
        }

        //comprobar si se cumplen las condiciones de la meta!
        if ( validSeq ) {
            if ( initialState.satisfiesGoal( d, ins ) ) {
                std::cout << "SAT!\n";
            }
            else {
                std::cout << "UNSAT\n";
            }
        }
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
