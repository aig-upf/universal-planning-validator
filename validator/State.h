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
};
