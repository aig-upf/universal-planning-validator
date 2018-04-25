
#include <fstream>
#include <sstream>

#include <minicppunit/MiniCppUnit.h>
#include <parser/Instance.h>
#include <validator/PlanValidator.h>
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

    void callValidator( Domain * d, Instance * ins, Plan * p ) {
        PlanValidator pv;
        pv.validate( d, ins, p );
    }

    void elevatorTest() {
        parser::pddl::Domain d( "tests/classical/elevator/dom.pddl");
        parser::pddl::Instance ins( d, "tests/classical/elevator/ins.pddl" );
        Plan p( "tests/classical/elevator/plan.1" );
        callValidator( &d, &ins, &p );
    }

    void existsTest() {
        parser::pddl::Domain d( "tests/classical/exists/dom.pddl");
        parser::pddl::Instance ins( d, "tests/classical/exists/ins.pddl" );
        Plan p( "tests/classical/exists/plan.1" );
        callValidator( &d, &ins, &p );
    }

    void forallTest1() {
        parser::pddl::Domain d( "tests/classical/forall/dom.pddl");
        parser::pddl::Instance ins( d, "tests/classical/forall/ins.pddl" );
        Plan p( "tests/classical/forall/plan.1" );
        callValidator( &d, &ins, &p );
    }

    void forallTest2() {
        parser::pddl::Domain d( "tests/classical/forall/dom.pddl");
        parser::pddl::Instance ins( d, "tests/classical/forall/ins.pddl" );
        Plan p( "tests/classical/forall/plan.2" );
        callValidator( &d, &ins, &p );
    }

    void orTest() {
        parser::pddl::Domain d( "tests/classical/or/dom.pddl");
        parser::pddl::Instance ins( d, "tests/classical/or/ins.pddl" );
        Plan p( "tests/classical/or/plan.1" );
        callValidator( &d, &ins, &p );
    }

    void whenTest() {
        parser::pddl::Domain d( "tests/classical/when/dom.pddl");
        parser::pddl::Instance ins( d, "tests/classical/when/ins.pddl" );
        Plan p( "tests/classical/when/plan.1" );
        callValidator( &d, &ins, &p );
    }
};

REGISTER_FIXTURE( ClassicalPlanningTests )
