#include "PlanningProgram.h"

PlanningProgram::PlanningProgram() {}

PlanningProgram::PlanningProgram( const std::string & s ) {
    parse( s );
}

void PlanningProgram::parse( const std::string & s ) {
    Filereader f( s );

    while ( !f.f.eof() && !f.s.empty() ) {
        f.c = 0;
        f.next();

        std::string instrName = f.getToken();

        if ( startsWith( instrName, END_PREFIX ) ) {
            f.next();
            std::string stackRow = f.getToken();
            std::cout << instrName << " " << stackRow << "\n";
        }
        else if ( startsWith( instrName, INSTR_PREFIX ) ) {
            f.next();
            std::string stackRow = f.getToken();
            std::cout << instrName << " " << stackRow << "\n";
        }
        else if ( startsWith( instrName, GOTO_PREFIX ) ) {
            std::cout << instrName << "\n";
        }
        else if ( startsWith( instrName, COND_ASSIGN_PREFIX ) ) {
            f.next();

            std::string variableName = f.getToken();
            f.next();

            std::string variableVal = f.getToken();
            f.next();

            std::string stackRow = f.getToken();

            std::cout << instrName << " " << variableName << " " << variableVal << " " << stackRow << "\n";
        }

        ++f.r;
        getline( f.f, f.s );
    }

    name = s;
}

bool PlanningProgram::run( Domain * d, Instance * ins, State * currentState ) {
    return false;
}

unsigned PlanningProgram::getNumActions() const {
    return 0;
}

bool startsWith( const std::string& testStr, const std::string& prefix ) {
    if ( prefix.length() <= testStr.length() ) {
        return testStr.substr( 0, prefix.length() ) == prefix;
    }

    return false;
}
