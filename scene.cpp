#include "scene.h"
#include "robot.h"
#include "routingalgorithm.h"

#include <assert.h>
#include <string.h>

Scene::Scene( const unsigned width, const unsigned height ) :
    m_obstacle_map( width, height ),
    m_last_robot_id( 0 )
{
}

Scene::~Scene()
{
}

unsigned Scene::width() const
{
    return m_obstacle_map.width();
}

unsigned Scene::height() const
{
    return m_obstacle_map.height();
}

std::list< Robot >& Scene::robot_list()
{
    return m_robot_list;
}

const std::list< Robot >& Scene::robot_list() const
{
    return m_robot_list;
}

void Scene::add_wall( const unsigned x, const unsigned y )
{
    set_wall( x, y, true );
}

void Scene::remove_wall( const unsigned x, const unsigned y )
{
    set_wall( x, y, false );
}

void Scene::set_wall( const unsigned x, const unsigned y, const bool block )
{
    auto& cell = m_obstacle_map.at( x, y );

    if( block )
    {
        if( cell == ObstacleType::None )
            cell = ObstacleType::Wall;
    }
    else
    {
        if( cell == ObstacleType::Wall )
            cell = ObstacleType::None;
    }
}

Robot& Scene::add_robot( const unsigned x, const unsigned y )
{
    ObstacleType obstacle_type = at( x, y );

    if( obstacle_type == ObstacleType::Robot )
        m_robot_list.remove_if( [x, y]( const Robot& robot ) { return robot.x() == x && robot.y() == y; } );

    m_obstacle_map.at( x, y ) = ObstacleType::Robot;
    m_robot_list.emplace_back( m_last_robot_id, x, y, *this );
    m_last_robot_id++;

    return m_robot_list.back();
}

Robot * Scene::get_robot( const unsigned x, const unsigned y )
{
    return const_cast< Robot * >( const_cast< const Scene * >( this )->get_robot( x, y ) );
}

const Robot * Scene::get_robot( const unsigned x, const unsigned y ) const
{
    if( x > width() || y > height() )
        return nullptr;

    ObstacleType obstacle_type = at( x, y );

    if( obstacle_type != ObstacleType::Robot )
        return nullptr; /* There is no robot there. */

    for( const Robot& robot: m_robot_list )
    {
        if( robot.x() == x && robot.y() == y )
            return &robot;
    }

    /*
     * This shouldn't happen; if a robot exists in the obstacle map
     * then it should also exist in the list.
     */
    assert( false );
    return nullptr;
}

void Scene::remove_robot( Robot& robot )
{
    ObstacleType obstacle_type = at( robot.x(), robot.y() );

    if( obstacle_type != ObstacleType::Robot )
        return;

    m_obstacle_map.at( robot.x(), robot.y() ) = ObstacleType::None;
    m_robot_list.remove_if( [&robot]( const Robot& i ) { return &i == &robot; } );

    if( m_robot_list.empty() )
        m_last_robot_id = 0;
}

ObstacleType Scene::at( const unsigned x, const unsigned y ) const
{
    return m_obstacle_map.at( x, y );
}

const Array2d< ObstacleType >& Scene::obstacle_map() const
{
    return m_obstacle_map;
}

bool Scene::is_blocked( const unsigned x, const unsigned y ) const
{
    if( x >= m_obstacle_map.width() || y >= m_obstacle_map.height() )
        return true;

    return m_obstacle_map.at( x, y ) != ObstacleType::None;
}

void Scene::calculate_visibility_for( Robot& robot ) const
{
    /* Maximum view distance. */
    const int view_distance = 4;

    Array2d< bool >& visibility_map = robot.visibility_map();

    /* Mark everything as invisible. */
    visibility_map.clear_with( false );

    int rx = (int)robot.x();
    int ry = (int)robot.y();

    int minx = rx;
    int maxx = rx;
    int miny = ry;
    int maxy = ry;

    /* FIXME: Implement real visibility algorithm. */
    for( int i = -view_distance; i <= view_distance; ++i )
    {
        int y = ry + i;
        if( y < 0 || y >= (int)visibility_map.height() )
            continue;

        for( int j = -view_distance; j <= view_distance; ++j )
        {
            int x = rx + j;
            if( x < 0 || x >= (int)visibility_map.width() )
                continue;

            if( x < minx )
                minx = x;
            if( x > maxx )
                maxx = x;

            if( y < miny )
                miny = y;
            if( y > maxy )
                maxy = y;

            visibility_map.at( x, y ) = true;
        }
    }

    /* Update robot's view of the world. */
    auto& obstacle_map = robot.obstacle_map();
    for( int y = miny; y <= maxy; ++y )
    {
        if( y < 0 || y >= (int)visibility_map.height() )
            continue;

        for( int x = minx; x <= maxx; ++x )
        {
            if( x < 0 || x >= (int)visibility_map.width() )
                continue;

            if( visibility_map.at( x, y ) == false )
                continue;

            obstacle_map.at( x, y ) = m_obstacle_map.at( x, y );
        }
    }

}

/* Increase this number after modifying the serialization format. */
const static uint8_t file_format_version = 2;

void Scene::serialize( QDataStream& stream ) const
{
    stream.setByteOrder( QDataStream::LittleEndian );
    stream << (uint8_t)file_format_version;
    stream << (uint32_t)width();
    stream << (uint32_t)height();
    stream.writeRawData( (const char *)m_obstacle_map.vector().data(), width() * height() );
    stream << (uint32_t)m_robot_list.size();
    for( const Robot& robot: m_robot_list )
    {
        stream << (uint32_t)robot.id();
        stream << (uint32_t)robot.x();
        stream << (uint32_t)robot.y();
        stream << (uint32_t)robot.goal_x();
        stream << (uint32_t)robot.goal_y();
    }
    stream << (uint32_t)m_last_robot_id;
}

void Scene::deserialize( QDataStream& stream )
{
    stream.setByteOrder( QDataStream::LittleEndian );

    uint8_t version;
    stream >> version;

    if( version != file_format_version )
        return;

    uint32_t width, height;
    stream >> width;
    stream >> height;

    if( width > 0xffff || height > 0xffff || width == 0 || height == 0 )
        return;

    m_obstacle_map = Array2d< ObstacleType >( width, height );
    stream.readRawData( (char *)m_obstacle_map.vector().data(), width * height );

    m_robot_list.clear();
    uint32_t robot_count;
    stream >> robot_count;

    for( ; robot_count > 0; --robot_count )
    {
        uint32_t id, x, y, goal_x, goal_y;

        stream >> id;
        stream >> x;
        stream >> y;
        stream >> goal_x;
        stream >> goal_y;

        m_robot_list.emplace_back( id, x, y, *this );
        Robot& robot = m_robot_list.back();
        if( goal_x >= width || goal_y >= height )
            robot.clear_goal();
        else
            robot.set_goal( goal_x, goal_y );
    }

    uint32_t last_robot_id;
    stream >> last_robot_id;
    m_last_robot_id = last_robot_id;
}

