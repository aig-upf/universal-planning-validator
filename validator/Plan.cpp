#include "Plan.h"

Plan::Plan() {}

Plan::Plan( const std::string & s ) {
    parse( s );
}

void Plan::parse( const std::string & s ) {
    actions.clear();

    Filereader f( s );

    while ( !f.f.eof() && !f.s.empty() ) {
        f.c = 0;

        if ( f.getChar() != ';' ) {
            f.next();
            f.assert_token( "(" );
            f.next();

            std::string actionName = f.getToken();
            f.next();

            StringVec actionParams;
            while ( f.getChar() != ')' ) {
                actionParams.push_back( f.getToken() );
                f.next();
            }

            PlanAction pa( actionName, actionParams);
            actions.push_back( pa );
        }

        ++f.r;
        getline( f.f, f.s );
    }

    name = s;
}
