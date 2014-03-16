#ifndef SIMULATION_H
#define SIMULATION_H

#include <memory>

class Scene;
class Robot;

class Simulation
{
    Simulation( const Simulation& ) = delete;
    void operator =( const Simulation& ) = delete;
    void operator =( Simulation&& ) = delete;

    std::shared_ptr< Scene > m_scene;

    public:
        explicit Simulation( const std::shared_ptr< Scene >& scene );
        ~Simulation();

        void run( const float elapsed );

        const std::shared_ptr< Scene >& scene() const;
        std::shared_ptr< Scene >& scene();
};

#endif // SIMULATION_H
