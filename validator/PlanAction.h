#pragma once

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

        Forall * f = dynamic_cast< Forall * >( c );
        if ( f ) {
            StringVec forallParams = d->typeList( f );
            return forallHoldsRec( forallParams, 0, s, d, f->cond );
        }

        Exists * e = dynamic_cast< Exists * >( c );
        if ( e ) {
            // TODO
        }

        return true;
    }

    bool forallHoldsRec( const StringVec& forallParams, unsigned paramIndex, State * s, Domain * d, Condition * c ) {
        if ( paramIndex < forallParams.size() ) {
            Type * type = d->getType( forallParams[paramIndex] );

            for ( unsigned i = 0; i < type->noObjects(); ++i ) {
                std::pair< std::string, int > typeObj = type->object( i );
                params.push_back( typeObj.first );  // expand action params
                bool result = forallHoldsRec( forallParams, paramIndex + 1, s, d, c );
                params.pop_back();  // remove previously added params

                if ( !result ) {
                    return false;
                }
            }
        }
        else {
            // all forall parameters instantiated and added to action params,
            // continue checking satisfiability
            return holdsRec( s, d, c );
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

        Forall * f = dynamic_cast< Forall * >( c );
        if ( f ) {
            StringVec forallParams = d->typeList( f );
            forallApplyRec( forallParams, 0, s, d, f->cond );
        }
    }

    void forallApplyRec( const StringVec& forallParams, unsigned paramIndex, State * s, Domain * d, Condition * c ) {
        if ( paramIndex < forallParams.size() ) {
            Type * type = d->getType( forallParams[paramIndex] );

            for ( unsigned i = 0; i < type->noObjects(); ++i ) {
                std::pair< std::string, int > typeObj = type->object( i );
                params.push_back( typeObj.first );  // expand action params
                forallApplyRec( forallParams, paramIndex + 1, s, d, c );
                params.pop_back();  // remove previously added params
            }
        }
        else {
            // all forall parameters instantiated and added to action params,
            // continue checking satisfiability
            applyRec( s, d, c );
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
