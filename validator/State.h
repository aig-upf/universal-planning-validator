#pragma once

#include <stdexcept>
#include <parser/Instance.h>

using namespace parser::pddl;

class FluentNotFound : public std::runtime_error {
public:
    FluentNotFound( const std::string& fluent )
        : std::runtime_error( "Fluent " + fluent + " is undefined" ) {}
};

class InitialFunctionValueUndefined : public std::runtime_error {
public:
    InitialFunctionValueUndefined( const std::string& name )
        : std::runtime_error( "Function " + name + " is not defined in the initial state" ) {}
};

class TotalCostFunctionUndefined : public std::runtime_error {
public:
    TotalCostFunctionUndefined()
        : std::runtime_error( "Undefined TOTAL-COST function" ) {};
};

class State {
public:
    std::map< std::string, std::set< StringVec > > fluents;  // map of active fluents
    std::map< std::string, std::map< StringVec, double > > functions; // map of function values

    State( Domain * d, Instance * ins );

    // sets state from initial fluents in an instance
    void set( Domain * d, Instance * ins );

    // checks if a condition holds
    bool holds( bool neg, const std::string& name, const StringVec& params );

    void addFluent( const std::string& name, const StringVec& params );

    void removeFluent( const std::string& name, const StringVec& params );

    void modifyFunctionValue( const std::string& name, const StringVec& params, double changeValue );

    bool satisfiesGoal( Domain * d, Instance * ins );

    double getTotalCostValue() const;

protected:
    std::set< StringVec >& getActiveFluents( const std::string& name );
};
