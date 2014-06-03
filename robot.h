#ifndef ROBOT_H
#define ROBOT_H

#include "array2d.h"
#include "scene.h"
#include <memory>

class RoutingAlgorithm;
class Scene;
class Simulation;

class Robot
{
    friend class Simulation;

    Scene& m_scene;

    Array2d< bool > m_visibility_map;
    Array2d< ObstacleType > m_obstacle_map;

    std::unique_ptr< RoutingAlgorithm > m_routing_algorithm;

    unsigned m_id;
    unsigned m_x, m_y;
    unsigned m_goal_x, m_goal_y;
    float m_frac_x, m_frac_y;

    public:

        explicit Robot( const unsigned id, const unsigned x, const unsigned y, Scene& scene );
        ~Robot();

        /**
         * @return The ID of the robot.
         */
        unsigned id() const;

        /**
         * @return Current X position of the robot, in blocks.
         */
        unsigned x() const;

        /**
         * @return Current Y position of the robot, in blocks.
         */
        unsigned y() const;

        /**
         * @return Fractional X position of the robot within one block.
         */
        float frac_x() const;

        /**
         * @return Fractional X position of the robot within one block.
         */
        float frac_y() const;

        /**
         * @return X position of the robot's goal, in blocks.
         */
        unsigned goal_x() const;

        /**
         * @return Y position of the robot's goal, in blocks.
         */
        unsigned goal_y() const;

        /**
         * @return Whenever the robot has a goal.
         */
        bool has_goal() const;

        /**
         * @brief Sets the robot's goal on the map.
         */
        void set_goal( const unsigned x, const unsigned y );

        /**
         * @brief Clears the robot's goal.
         */
        void clear_goal();

        /**
         * @return Currently used routing algorithm; can be null.
         */
        RoutingAlgorithm * routing_algorithm();

        /**
         * @return Currently used routing algorithm; can be null.
         */
        const RoutingAlgorithm * routing_algorithm() const;

        /**
         * @brief Sets a routing algorithm.
         */
        void set_routing_algorithm( std::unique_ptr< RoutingAlgorithm > algorithm );

        /**
         * @return A visibility map updated every simulation tick.
         *         If a tile is visible by the robot it has 'true'
         *         set at its position; 'false' otherwise.
         */
        Array2d< bool >& visibility_map();

        /**
         * @return Obstacle map, as memorized by the robot.
         */
        Array2d< ObstacleType >& obstacle_map();

        /**
         * @return Obstacle map, as memorized by the robot.
         */
        const Array2d< ObstacleType >& obstacle_map() const;

        /**
         * @brief Recalculates the robot's visibility.
         */
        void calculate_visibility();

        /**
         * @return Whenever the given point is currently visible by the robot.
         */
        bool can_see( const unsigned x, const unsigned y ) const;

        /**
         * @brief Moves the robot to given point, if possible.
         * @return Whenever the operation was successful.
         */
        bool move_to( const unsigned x, const unsigned y );
};

#endif // ROBOT_H
