#include "dummyalgorithm.h"
#include "routingalgorithmregistry.h"
#include "robot.h"

#include <math.h>

static const StaticAlgorithmRegistration registrar( "Dummy", [](){
    return std::unique_ptr< RoutingAlgorithm >( new DummyAlgorithm() );
} );

DummyAlgorithm::DummyAlgorithm()
{
}

DummyAlgorithm::~DummyAlgorithm()
{
}

void DummyAlgorithm::initialize( const Robot& robot )
{

}

float DummyAlgorithm::run( const Robot& robot, const float elapsed )
{
    const float x = robot.x() + robot.frac_x();
    const float y = robot.y() + robot.frac_y();

    const float goal_x = robot.goal_x() + 0.5f;
    const float goal_y = robot.goal_y() + 0.5f;

    const float dx = goal_x - x;
    const float dy = goal_y - y;

    const float angle = atan2f( dy, dx );
    return angle;
}
