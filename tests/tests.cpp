
#include <fstream>
#include <sstream>

#include <minicppunit/MiniCppUnit.h>
#include <parser/Instance.h>
#include <validator/Plan.h>

class ClassicalPlanningTests : public TestFixture<ClassicalPlanningTests>
{
public:
    TEST_FIXTURE( ClassicalPlanningTests )
    {
        TEST_CASE( elevatorTest );
        TEST_CASE( existsTest );
        TEST_CASE( forallTest1 );
        TEST_CASE( forallTest2 );
        TEST_CASE( orTest );
        TEST_CASE( whenTest );
    }

    template < typename T >
    void checkEqual( T & prob, const std::string & file ) {
        std::ifstream f(file.c_str());
        if (!f) throw std::runtime_error(std::string("Failed to open file '") + file + "'");
        std::string s, t;

        while(std::getline(f, s)){
            t += s + "\n";
        }

        std::ostringstream ds;
        ds << prob;
        ASSERT_EQUALS( t, ds.str() );
        std::ofstream of("ins.txt");
        of<<ds.str();
    }

    void elevatorTest() {
        parser::pddl::Domain * d = new parser::pddl::Domain( "tests/classical/elevator/dom.pddl");
        parser::pddl::Instance * ins = new parser::pddl::Instance( *d, "tests/classical/elevator/ins.pddl" );
        Plan p( "tests/classical/elevator/plan.1" );
        p.validate( d, ins );
    }

    void existsTest() {
        parser::pddl::Domain * d = new parser::pddl::Domain( "tests/classical/exists/dom.pddl");
        parser::pddl::Instance * ins = new parser::pddl::Instance( *d, "tests/classical/exists/ins.pddl" );
        Plan p( "tests/classical/exists/plan.1" );
        p.validate( d, ins );
    }

    void forallTest1() {
        parser::pddl::Domain * d = new parser::pddl::Domain( "tests/classical/forall/dom.pddl");
        parser::pddl::Instance * ins = new parser::pddl::Instance( *d, "tests/classical/forall/ins.pddl" );
        Plan p( "tests/classical/forall/plan.1" );
        p.validate( d, ins );
    }

    void forallTest2() {
        parser::pddl::Domain * d = new parser::pddl::Domain( "tests/classical/forall/dom.pddl");
        parser::pddl::Instance * ins = new parser::pddl::Instance( *d, "tests/classical/forall/ins.pddl" );
        Plan p( "tests/classical/forall/plan.2" );
        p.validate( d, ins );
    }

    void orTest() {
        parser::pddl::Domain * d = new parser::pddl::Domain( "tests/classical/or/dom.pddl");
        parser::pddl::Instance * ins = new parser::pddl::Instance( *d, "tests/classical/or/ins.pddl" );
        Plan p( "tests/classical/or/plan.1" );
        p.validate( d, ins );
    }

    void whenTest() {
        parser::pddl::Domain * d = new parser::pddl::Domain( "tests/classical/when/dom.pddl");
        parser::pddl::Instance * ins = new parser::pddl::Instance( *d, "tests/classical/when/ins.pddl" );
        Plan p( "tests/classical/when/plan.1" );
        p.validate( d, ins );
    }
};

REGISTER_FIXTURE( ClassicalPlanningTests )
