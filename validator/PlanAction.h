#pragma once

#include "State.h"

using namespace parser::pddl;

typedef std::pair< std::string, StringVec > GroundedObj;
typedef std::vector< GroundedObj > GroundedObjVec;

class PlanAction {
public:
    std::string name;
    StringVec params;

    PlanAction( const std::string& name, const StringVec& params );

    bool holds( State * s, Domain * d );

    void apply( State * s, Domain * d );

    std::string getActionName() const;

    friend std::ostream& operator<<( std::ostream& os, const PlanAction& pa ) {
        return pa.print( os );
    }

    virtual std::ostream& print( std::ostream& stream ) const {
        stream << getActionName() << '\n';
        return stream;
    }

protected:
    bool holdsRec( State * s, Domain * d, Condition * c );

    bool forallHoldsRec( const StringVec& forallParams, unsigned paramIndex, State * s, Domain * d, Condition * c );

    bool existsHoldsRec( const StringVec& existsParams, unsigned paramIndex, State * s, Domain * d, Condition * c );

    void applyRec( State * s, Domain * d, Condition * c, GroundedObjVec& addList, GroundedObjVec& deleteList );

    void forallApplyRec( const StringVec& forallParams, unsigned paramIndex, State * s, Domain * d, Condition * c, GroundedObjVec& addList, GroundedObjVec& deleteList );

    StringVec getObjectParameters( Domain * d, Ground * g ) const;

    void applyDeleteList( State * s, const GroundedObjVec& deleteList ) const;

    void applyAddList( State * s, const GroundedObjVec& addList ) const;

    bool showVerbose();
};

std::string getGroundedObjectStr( const GroundedObj& go );
