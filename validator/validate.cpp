#include <iostream>
#include <cstring>
#include <parser/Instance.h>
#include "Plan.h"
#include "PlanValidator.h"

using namespace parser::pddl;

void showHelp() {
    std::cout << "AUV: An Unnamed Validator\n";
    std::cout << "Usage: ./validate [options] <domain.pddl> <task.pddl> <plan>\n";
    std::cout << "Options:\n";
    std::cout << "\t-h\t-- Print this message.\n";
    std::cout << "\t-v\t-- Verbose reporting of plan check progress.\n";
    exit( 1 );
}

typedef struct ProgramParams {
    bool verbose;
    bool help;
    std::string domain, ins, plan;

    ProgramParams( int argc, char * argv[] ) : verbose( false ), help( false ) {
        parseInputParameters( argc, argv );
    }

    void parseInputParameters( int argc, char * argv[] ) {
        if ( argc < 4 ) {
            showHelp();
        }

        int domainParam = 0;
        for ( int i = 1; i < argc; ++i ) {
            if ( argv[i][0] == '-') {
                if ( !strcmp( argv[i], "-h" ) ) {
                    help = true;
                }
                else if ( !strcmp( argv[i], "-v" ) ) {
                    verbose = true;
                }
            }
            else {
                domainParam = i;
                break;
            }
        }

        if ( argc > domainParam + 2 ) {
            domain = argv[domainParam];
            ins = argv[domainParam + 1];
            plan = argv[domainParam + 2];
        }
        else {
            showHelp();
        }
    }

} ProgramParams;

int main( int argc, char * argv[] ) {
    ProgramParams pp( argc, argv );

    if ( pp.help ) {
        showHelp();
    }

    Domain d( pp.domain );
    Instance ins( d, pp.ins );
    Plan p( pp.plan );

    PlanValidator pv( pp.verbose );
    pv.validate( &d, &ins, &p );

    return 0;
}
