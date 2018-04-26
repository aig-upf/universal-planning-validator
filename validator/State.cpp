#include "State.h"

State::State( Domain * d, Instance * ins ) {
    set( d, ins );
}

void State::set( Domain * d, Instance * ins ) {
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

bool State::holds( bool neg, const std::string& name, const StringVec& params ) {
    std::set< StringVec >& activeFluents = getActiveFluents( name );
    bool fluentExists = activeFluents.find( params ) != activeFluents.end();
    return ( neg && !fluentExists ) || ( !neg && fluentExists );
}

void State::addFluent( const std::string& name, const StringVec& params ) {
    std::set< StringVec >& activeFluents = getActiveFluents( name );
    if ( activeFluents.find( params ) == activeFluents.end() ) {
        activeFluents.insert( params );
    }
}

void State::removeFluent( const std::string& name, const StringVec& params ) {
    std::set< StringVec >& activeFluents = getActiveFluents( name );
    std::set< StringVec >::iterator af = activeFluents.find( params );
    if ( af != activeFluents.end() ) {
        activeFluents.erase( af );
    }
}

bool State::satisfiesGoal( Domain * d, Instance * ins ) {
    const GroundVec& goalConditions = ins->goal;
    for ( unsigned i = 0; i < goalConditions.size(); ++i ) {
        if ( !holds( false, goalConditions[i]->name, d->objectList( goalConditions[i] ) ) ) {
            return false;
        }
    }

    return true;
}

std::set< StringVec >& State::getActiveFluents( const std::string& name ) {
    auto fluentSet = fluents.find( name );

    if ( fluentSet != fluents.end() ) {
        return fluentSet->second;
    }
    else {
        throw FluentNotFound( name );
    }
}
