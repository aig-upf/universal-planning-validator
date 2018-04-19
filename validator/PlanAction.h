#pragma once

#include "State.h"

using namespace parser::pddl;

typedef std::vector< std::pair< std::string, StringVec > > GroundedObjVec;

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
            GroundedObjVec addList, deleteList;
            applyRec( s, d, a->eff, addList, deleteList );

            for ( auto it = addList.begin(); it != addList.end(); ++it ) {
                s->addFluent( it->first, it->second );
            }

            for ( auto it = deleteList.begin(); it != deleteList.end(); ++it ) {
                s->removeFluent( it->first, it->second );
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
            StringVec existsParams = d->typeList( e );
            return existsHoldsRec( existsParams, 0, s, d, e->cond );
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

    bool existsHoldsRec( const StringVec& existsParams, unsigned paramIndex, State * s, Domain * d, Condition * c ) {
        if ( paramIndex < existsParams.size() ) {
            Type * type = d->getType( existsParams[paramIndex] );

            for ( unsigned i = 0; i < type->noObjects(); ++i ) {
                std::pair< std::string, int > typeObj = type->object( i );
                params.push_back( typeObj.first );  // expand action params
                bool result = existsHoldsRec( existsParams, paramIndex + 1, s, d, c );
                params.pop_back();  // remove previously added params

                if ( result ) {
                    return true;
                }
            }
        }
        else {
            return holdsRec( s, d, c );
        }

        return false;
    }

    void applyRec( State * s, Domain * d, Condition * c, GroundedObjVec& addList, GroundedObjVec& deleteList ) {
        And * a = dynamic_cast< And * >( c );
        if ( a ) {
            for ( unsigned i = 0; i < a->conds.size(); ++i ) {
                applyRec( s, d, a->conds[i], addList, deleteList );
            }
        }

        Ground * g = dynamic_cast< Ground * >( c );
        if ( g ) {
            addList.push_back( std::make_pair( g->name, getObjectParameters( g->params ) ) );
        }

        Not * n = dynamic_cast< Not * >( c );
        if ( n ) {
            Ground * ng = n->cond;
            deleteList.push_back( std::make_pair( ng->name, getObjectParameters( ng->params ) ) );
        }

        Forall * f = dynamic_cast< Forall * >( c );
        if ( f ) {
            StringVec forallParams = d->typeList( f );
            forallApplyRec( forallParams, 0, s, d, f->cond, addList, deleteList );
        }

        When * w = dynamic_cast< When * >( c );
        if ( w ) {
            bool conditionHolds = holdsRec( s, d, w->pars );
            if ( conditionHolds ) {
                applyRec( s, d, w->cond, addList, deleteList );
            }
        }
    }

    void forallApplyRec( const StringVec& forallParams, unsigned paramIndex, State * s, Domain * d, Condition * c, GroundedObjVec& addList, GroundedObjVec& deleteList ) {
        if ( paramIndex < forallParams.size() ) {
            Type * type = d->getType( forallParams[paramIndex] );

            for ( unsigned i = 0; i < type->noObjects(); ++i ) {
                std::pair< std::string, int > typeObj = type->object( i );
                params.push_back( typeObj.first );  // expand action params
                forallApplyRec( forallParams, paramIndex + 1, s, d, c, addList, deleteList );
                params.pop_back();  // remove previously added params
            }
        }
        else {
            // all forall parameters instantiated and added to action params,
            // continue checking satisfiability
            applyRec( s, d, c, addList, deleteList );
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
