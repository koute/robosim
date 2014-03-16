#ifndef SCENE_H
#define SCENE_H

#include <stdint.h>
#include <vector>
#include <memory>
#include <list>

#include <QDataStream>

#include "array2d.h"

class Robot;

enum class ObstacleType : uint8_t
{
    None  = 0,
    Wall  = 1,
    Robot = 2
};

class Scene
{
    friend class Robot;

    Array2d< ObstacleType > m_obstacle_map;
    std::list< Robot > m_robot_list;
    unsigned m_last_robot_id;

    public:

        explicit Scene( const unsigned width, const unsigned height );
        ~Scene();

        /**
         * @return Width of the scene, in blocks.
         */
        unsigned width() const;

        /**
         * @return Height of the scene, in blocks.
         */
        unsigned height() const;

        /**
         * @return List of robots inside this Scene.
         */
        std::list< Robot >& robot_list();

        /**
         * @return List of robots inside this Scene.
         */
        const std::list< Robot >& robot_list() const;

        /**
         * @brief Adds a wall at given point; does nothing
         *        in case of an already occupied block.
         */
        void add_wall( const unsigned x, const unsigned y );

        /**
         * @brief Removes a wall from a given point; does nothing
         *        in case of a block that doesn't contain a wall.
         */
        void remove_wall( const unsigned x, const unsigned y );

        /**
         * @sa add_wall, remove_wall
         */
        void set_wall( const unsigned x, const unsigned y, const bool block );

        /**
         * @brief Adds a new robot to the scene; will replace
         *        anything that already exists in the specified block.
         * @return A handle to the newly created robot.
         */
        Robot& add_robot( const unsigned x, const unsigned y );

        /**
         * @return Robot at given point; may return null.
         */
        Robot * get_robot( const unsigned x, const unsigned y );

        /**
         * @return Robot at given point; may return null.
         */
        const Robot * get_robot( const unsigned x, const unsigned y ) const;

        /**
         * @brief Removes given robot.
         */
        void remove_robot( Robot& robot );

        /**
         * @return ObstacleType at given point.
         */
        ObstacleType at( const unsigned x, const unsigned y ) const;

        /**
         * @return Map of obstacles.
         */
        const Array2d< ObstacleType >& obstacle_map() const;

        /**
         * @return Whenever a given block is occupied.
         */
        bool is_blocked( const unsigned x, const unsigned y ) const;

        /**
         * @brief Recalculates field of view for given robot.
         */
        void calculate_visibility_for( Robot& robot ) const;

        /**
         * @brief Serializes the whole scene to a data stream.
         */
        void serialize( QDataStream& stream ) const;

        /**
         * @brief Deserializes the whole scene from a data stream.
         */
        void deserialize( QDataStream& stream );
};

#endif // SCENE_H
