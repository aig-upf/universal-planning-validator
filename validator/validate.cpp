#include <iostream>
#include <parser/Instance.h>

using namespace parser::pddl;

int main( int argc, char * argv[] ) {
    if ( argc < 3 ) {
        std::cout << "Usage: ./validate <domain.pddl> <task.pddl> <plan>\n";
        exit(1);
    }

    Domain d( argv[1] );
    Instance ins( d, argv[2] );

    std::cout << d << "\n";
    std::cout << ins << "\n";

	return 0;
}
