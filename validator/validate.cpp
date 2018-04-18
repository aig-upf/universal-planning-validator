#include <iostream>
#include <parser/Instance.h>
#include "Plan.h"

using namespace parser::pddl;

int main( int argc, char * argv[] ) {
    if ( argc < 4 ) {
        std::cout << "Usage: ./validate <domain.pddl> <task.pddl> <plan>\n";
        exit(1);
    }

    Domain * d = new Domain( argv[1] );
    Instance * ins = new Instance( *d, argv[2] );
    Plan p( argv[3] );

    p.validate( d, ins );

    delete ins;
    delete d;

    return 0;
}
