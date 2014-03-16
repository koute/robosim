#include "simulation.h"
#include "scene.h"
#include "robot.h"
#include "routingalgorithm.h"

#include <math.h>

Simulation::Simulation( const std::shared_ptr< Scene >& scene ) :
    m_scene( scene )
{
}

Simulation::~Simulation()
{
}

void Simulation::run( const float elapsed )
{
    const float speed = 1.0f;

    for( Robot& robot: m_scene->robot_list() )
    {
        if( !robot.has_goal() )
            continue;

        RoutingAlgorithm * algorithm = robot.routing_algorithm();
        if( algorithm == nullptr )
            continue;

        const float angle = algorithm->run( robot, elapsed );
        const float dx = cosf( angle ) * elapsed * speed;
        const float dy = sinf( angle ) * elapsed * speed;
        bool moved = false;

        robot.calculate_visibility();

        if( robot.x() == robot.goal_x() && robot.y() == robot.goal_y() )
        {
            bool x_done = fabsf( robot.frac_x() - 0.5 ) <= dx;
            bool y_done = fabsf( robot.frac_y() - 0.5 ) <= dy;

            if( x_done )
                robot.m_frac_x = 0.5;
            else
                robot.m_frac_x += dx;

            if( y_done )
                robot.m_frac_y = 0.5;
            else
                robot.m_frac_y += dy;

            if( x_done && y_done )
            {
                robot.clear_goal();
                continue;
            }
        }
        else
        {
            robot.m_frac_x += dx;
            robot.m_frac_y += dy;
        }

        if( robot.frac_x() >= 1.0f )
        {
            if( m_scene->is_blocked( robot.x() + 1, robot.y() ) )
                robot.m_frac_x = 0.99f;
            else
            {
                robot.m_frac_x = 0.0f;
                robot.move_to( robot.x() + 1, robot.y() );
                moved = true;
            }
        }
        else if( robot.frac_x() < 0.0f )
        {
            if( m_scene->is_blocked( robot.x() - 1, robot.y() ) )
                robot.m_frac_x = 0.0f;
            else
            {
                robot.m_frac_x = 0.99f;
                robot.move_to( robot.x() - 1, robot.y() );
                moved = true;
            }
        }

        if( robot.frac_y() >= 1.0f )
        {
            if( m_scene->is_blocked( robot.x(), robot.y() + 1 ) )
                robot.m_frac_y = 0.99f;
            else
            {
                robot.m_frac_y = 0.0f;
                robot.move_to( robot.x(), robot.y() + 1 );
                moved = true;
            }
        }
        else if( robot.frac_y() < 0.0f )
        {
            if( m_scene->is_blocked( robot.x(), robot.y() - 1 ) )
                robot.m_frac_y = 0.0f;
            else
            {
                robot.m_frac_y = 0.99f;
                robot.move_to( robot.x(), robot.y() - 1 );
                moved = true;
            }
        }

        if( moved )
            robot.calculate_visibility();
    }
}

const std::shared_ptr< Scene >& Simulation::scene() const
{
    return m_scene;
}

std::shared_ptr< Scene >& Simulation::scene()
{
    return m_scene;
}
