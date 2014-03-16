#ifndef DUMMYALGORITHM_H
#define DUMMYALGORITHM_H

#include "routingalgorithm.h"

class DummyAlgorithm : public RoutingAlgorithm
{
    public:
        explicit DummyAlgorithm();
        virtual ~DummyAlgorithm();

        virtual void initialize( const Robot& robot ) override;
        virtual float run( const Robot& robot, const float elapsed ) override;
};

#endif // DUMMYALGORITHM_H
