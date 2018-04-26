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

    State( Domain * d, Instance * ins );

    // sets state from initial fluents in an instance
    void set( Domain * d, Instance * ins );

    // checks if a condition holds
    bool holds( bool neg, const std::string& name, const StringVec& params );

    void addFluent( const std::string& name, const StringVec& params );

    void removeFluent( const std::string& name, const StringVec& params );

    bool satisfiesGoal( Domain * d, Instance * ins );

protected:
    std::set< StringVec >& getActiveFluents( const std::string& name );
};
