#ifndef ROUTINGALGORITHM_H
#define ROUTINGALGORITHM_H

class Robot;

/**
 * @brief Base class for every routing algorithm. A new
 *        instance is created for every Robot, so it's
 *        safe to store arbitrary data in here for the
 *        purpose of pathfinding.
 */
class RoutingAlgorithm
{
    RoutingAlgorithm( const RoutingAlgorithm& ) = delete;
    RoutingAlgorithm& operator =( const RoutingAlgorithm& ) = delete;
    void operator =( RoutingAlgorithm&& ) = delete;

    public:
        explicit RoutingAlgorithm();
        virtual ~RoutingAlgorithm();

        /**
         * @brief Initializes the algorithm for a given robot.
         *        Called before starting the simulation.
         */
        virtual void initialize( const Robot& robot ) = 0;

        /**
         * @brief Runs the pathfinding algorithm.
         * @param elapsed Time elapsed since the last call.
         *
         * @return Angle in radians.
         */
        virtual float run( const Robot& robot, const float elapsed ) = 0;
};

#endif // ROUTINGALGORITHM_H
