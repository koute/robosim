#include "routingalgorithmregistry.h"
#include "routingalgorithm.h"

RoutingAlgorithmRegistry::RoutingAlgorithmRegistry()
{
}

RoutingAlgorithmRegistry& RoutingAlgorithmRegistry::instance()
{
    static RoutingAlgorithmRegistry instance;
    return instance;
}

void RoutingAlgorithmRegistry::register_algorithm(
        const std::string& name,
        const FactoryMethodWrapper& factory_method )
{
    m_algorithm_map.insert( std::make_pair( name, factory_method ) );
}

const RoutingAlgorithmRegistry::FactoryMethodMap& RoutingAlgorithmRegistry::algorithm_map() const
{
    return m_algorithm_map;
}

std::unique_ptr< RoutingAlgorithm > RoutingAlgorithmRegistry::instantiate_algorithm(
        const std::string& name ) const
{
    auto i = m_algorithm_map.find( name );
    if( i == m_algorithm_map.end() )
        return std::unique_ptr< RoutingAlgorithm >( nullptr );

    return std::unique_ptr< RoutingAlgorithm >( i->second() );
}
