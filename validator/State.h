#pragma once

#include <parser/Instance.h>

using namespace parser::pddl;

class State {
public:
    std::map< std::string, std::set< StringVec > > fluents; // map of active fluents

    State( Domain * d, Instance * ins ) {
        set( d, ins );
    }

    void set( Domain * d, Instance * ins ) {
        for ( unsigned i = 0; i < d->preds.size(); ++i ) {
            const std::string& pname = d->preds[i]->name;
            fluents[pname] = std::set< StringVec >();
        }

        const GroundVec& initialState = ins->init;
        for ( unsigned i = 0; i < initialState.size(); ++i ) {
            const std::string& groundName = initialState[i]->name;

            if ( fluents.find( groundName ) != fluents.end() ) {
                fluents[groundName].insert( d->objectList( initialState[i] ) );
            }
        }
    }

    // checks if a condition holds
    bool holds( bool neg, const std::string& name, const StringVec& params ) const {

        const std::set< StringVec >& posFluents = fluents.find(name)->second;

        bool fluentExists = posFluents.find( params ) != posFluents.end();

        return ( neg && !fluentExists ) || ( !neg && fluentExists );
    }

    void addFluent( const std::string& name, const StringVec& params ) {
        auto fluentSet = fluents.find( name );

        if ( fluentSet != fluents.end() ) {
            std::set< StringVec >& activeFluents = fluentSet->second;
            if ( activeFluents.find( params ) == activeFluents.end() ) {
                activeFluents.insert( params );
            }
        }
        else {
            // throw exception
        }
    }

    void removeFluent( const std::string& name, const StringVec& params ) {
        auto fluentSet = fluents.find( name );

        if ( fluentSet != fluents.end() ) {
            std::set< StringVec >& activeFluents = fluentSet->second;
            std::set< StringVec >::iterator af = activeFluents.find( params );
            if ( af != activeFluents.end() ) {
                activeFluents.erase( af );
            }
        }
        else {
            // throw exception
        }
    }

    bool satisfiesGoal( Domain * d, Instance * ins ) {
        const GroundVec& goalConditions = ins->goal;
        for ( unsigned i = 0; i < goalConditions.size(); ++i ) {
            if ( !holds( false, goalConditions[i]->name, d->objectList( goalConditions[i] ) ) ) {
                return false;
            }
        }

        return true;
    }
};
