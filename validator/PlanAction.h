#pragma once

#include "State.h"
#include "Message.h"

using namespace parser::pddl;

typedef std::pair< std::string, StringVec > GroundedObj;
typedef std::vector< GroundedObj > GroundedObjVec;
std::string getGroundedObjectStr( const GroundedObj& go );

class PlanAction {
public:
    std::string name;
    StringVec params;

    PlanAction( const std::string& name, const StringVec& params )
        : name( name ), params( params ) {}

    bool holds( State * s, Domain * d ) {
        if ( d->actions.index( name ) >= 0 ) {
            Action * a = d->actions.get( name );
            return holdsRec( s, d, a->pre );
        }

        showErrorMsg( "Unknown action " + getActionName() );
        return false;
    }

    void apply( State * s, Domain * d ) {
        Action * a = d->actions.get( name );
        if ( a ) {
            GroundedObjVec addList, deleteList;
            applyRec( s, d, a->eff, addList, deleteList );

            applyDeleteList( s, deleteList );
            applyAddList( s, addList );
        }
    }

    std::string getActionName() const {
        std::stringstream ss;
        ss << "(" << name;
        for ( unsigned i = 0; i < params.size(); ++i ) {
            ss << " " << params[i];
        }
        ss << ")";
        return ss.str();
    }

    friend std::ostream& operator<<( std::ostream& os, const PlanAction& pa ) {
        return pa.print( os );
    }

    virtual std::ostream& print( std::ostream& stream ) const {
        stream << getActionName() << '\n';
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
            if ( !s->holds( false, g->name, getObjectParameters( d, g ) ) ) {
                return false;
            }
        }

        Not * n = dynamic_cast< Not * >( c );
        if ( n ) {
            Ground * ng = n->cond;
            if ( !s->holds( true, ng->name, getObjectParameters( d, ng ) ) ) {
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

        Or * o = dynamic_cast< Or * >( c );
        if ( o ) {
            return holdsRec( s, d, o->first ) || holdsRec( s, d, o->second );
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
            addList.push_back( std::make_pair( g->name, getObjectParameters( d, g ) ) );
        }

        Not * n = dynamic_cast< Not * >( c );
        if ( n ) {
            Ground * ng = n->cond;
            deleteList.push_back( std::make_pair( ng->name, getObjectParameters( d, ng ) ) );
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

    StringVec getObjectParameters( Domain * d, Ground * g ) const {
        IntVec& groundParams = g->params;
        StringVec objParams;
        for ( unsigned i = 0; i < groundParams.size(); ++i ) {
            if ( groundParams[i] >= 0 ) {
                objParams.push_back( params[groundParams[i]] );
            }
            else { // constant
                Type * constantType = d->types[g->lifted->params[i]];
                objParams.push_back( constantType->object( groundParams[i] ).first );
            }
        }
        return objParams;
    }

    void applyDeleteList( State * s, const GroundedObjVec& deleteList ) const {
        for ( auto it = deleteList.begin(); it != deleteList.end(); ++it ) {
            showMsg( "Deleting " + getGroundedObjectStr( *it ) );
            s->removeFluent( it->first, it->second );
        }
    }

    void applyAddList( State * s, const GroundedObjVec& addList ) const {
        for ( auto it = addList.begin(); it != addList.end(); ++it ) {
            showMsg( "Adding " + getGroundedObjectStr( *it ) );
            s->addFluent( it->first, it->second );
        }
    }
};

std::string getGroundedObjectStr( const GroundedObj& go ) {
    std::stringstream ss;
    ss << "(" << go.first;
    for ( unsigned i = 0; i < go.second.size(); ++i ) {
        ss << " " << go.second[i];
    }
    ss << ")";
    return ss.str();
}
