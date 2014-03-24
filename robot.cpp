#include "robot.h"
#include "scene.h"
#include "routingalgorithm.h"

Robot::Robot( const unsigned id, const unsigned x, const unsigned y, Scene& scene ) :
    m_scene( scene ),
    m_visibility_map( scene.width(), scene.height(), false ),
    m_obstacle_map( scene.width(), scene.height() ),
    m_id( id ),
    m_x( x ), m_y( y ),
    m_goal_x( x ), m_goal_y( y ),
    m_frac_x( 0.5f ), m_frac_y( 0.5f )
{
    assert( x < m_obstacle_map.width() );
    assert( y < m_obstacle_map.height() );
}

Robot::~Robot()
{
}

unsigned Robot::id() const
{
    return m_id;
}

unsigned Robot::x() const
{
    return m_x;
}

unsigned Robot::y() const
{
    return m_y;
}

float Robot::frac_x() const
{
    return m_frac_x;
}

float Robot::frac_y() const
{
    return m_frac_y;
}

unsigned Robot::goal_x() const
{
    return m_goal_x;
}

unsigned Robot::goal_y() const
{
    return m_goal_y;
}

bool Robot::has_goal() const
{
    return m_goal_x < m_scene.width() && m_goal_y < m_scene.height();
}

void Robot::set_goal( const unsigned x, const unsigned y )
{
    assert( x < m_obstacle_map.width() );
    assert( y < m_obstacle_map.height() );

    m_goal_x = x;
    m_goal_y = y;
}

void Robot::clear_goal()
{
    m_goal_x = -1;
    m_goal_y = -1;
}

RoutingAlgorithm * Robot::routing_algorithm()
{
    return m_routing_algorithm.get();
}

const RoutingAlgorithm * Robot::routing_algorithm() const
{
    return m_routing_algorithm.get();
}

void Robot::set_routing_algorithm( std::unique_ptr< RoutingAlgorithm > algorithm )
{
    m_routing_algorithm = std::move( algorithm );
    if( m_routing_algorithm )
        m_routing_algorithm->initialize( *this );
}

Array2d< bool >& Robot::visibility_map()
{
    return m_visibility_map;
}

Array2d< ObstacleType >& Robot::obstacle_map()
{
    return m_obstacle_map;
}

const Array2d< ObstacleType >& Robot::obstacle_map() const
{
    return m_obstacle_map;
}

void Robot::calculate_visibility()
{
    m_scene.calculate_visibility_for( *this );
}

bool Robot::can_see( const unsigned x, const unsigned y )
{
    if( x >= m_visibility_map.width() || y >= m_visibility_map.height() )
        return false;

    return m_visibility_map.at( x, y );
}

bool Robot::move_to( const unsigned x, const unsigned y )
{
    if( x == m_x && y == m_y )
        return true;

    if( m_scene.is_blocked( x, y ) )
        return false;

    m_scene.m_obstacle_map.at( m_x, m_y ) = ObstacleType::None;
    m_scene.m_obstacle_map.at( x, y ) = ObstacleType::Robot;

    m_x = x;
    m_y = y;

    return true;
}
