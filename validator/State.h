#pragma once

#include <stdexcept>
#include <parser/Instance.h>

using namespace parser::pddl;

class FluentNotFound : public std::runtime_error {
public:
    FluentNotFound( const std::string& fluent ) : std::runtime_error( "Fluent " + fluent + " is undefined" ) {}
};

class State {
public:
    std::map< std::string, std::set< StringVec > > fluents;  // map of active fluents

    State( Domain * d, Instance * ins ) {
        set( d, ins );
    }

    // sets state from initial fluents in an instance
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
    bool holds( bool neg, const std::string& name, const StringVec& params ) {
        std::set< StringVec >& activeFluents = getActiveFluents( name );
        bool fluentExists = activeFluents.find( params ) != activeFluents.end();
        return ( neg && !fluentExists ) || ( !neg && fluentExists );
    }

    void addFluent( const std::string& name, const StringVec& params ) {
        std::set< StringVec >& activeFluents = getActiveFluents( name );
        if ( activeFluents.find( params ) == activeFluents.end() ) {
            activeFluents.insert( params );
        }
    }

    void removeFluent( const std::string& name, const StringVec& params ) {
        std::set< StringVec >& activeFluents = getActiveFluents( name );
        std::set< StringVec >::iterator af = activeFluents.find( params );
        if ( af != activeFluents.end() ) {
            activeFluents.erase( af );
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

protected:
    std::set< StringVec >& getActiveFluents( const std::string& name ) {
        auto fluentSet = fluents.find( name );

        if ( fluentSet != fluents.end() ) {
            return fluentSet->second;
        }
        else {
            throw FluentNotFound( name );
        }
    }
};
