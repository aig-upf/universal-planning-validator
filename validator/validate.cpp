#include <iostream>
#include <parser/Instance.h>
#include "Plan.h"

using namespace parser::pddl;

int main( int argc, char * argv[] ) {
    if ( argc < 4 ) {
        std::cout << "Usage: ./validate <domain.pddl> <task.pddl> <plan>\n";
        exit(1);
    }

    Domain d( argv[1] );
    Instance ins( d, argv[2] );
    Plan p( argv[3] );

    std::cout << d << "\n";
    std::cout << ins << "\n";
    std::cout << p << "\n";

	return 0;
}
