#ifndef ROUTINGALGORITHMREGISTRY_H
#define ROUTINGALGORITHMREGISTRY_H

#include <string>
#include <functional>
#include <memory>
#include <map>

class RoutingAlgorithm;

class RoutingAlgorithmRegistry
{
    RoutingAlgorithmRegistry( const RoutingAlgorithmRegistry& ) = delete;
    RoutingAlgorithmRegistry& operator =( const RoutingAlgorithmRegistry& ) = delete;
    void operator =( RoutingAlgorithmRegistry&& ) = delete;

    public:

        typedef std::unique_ptr< RoutingAlgorithm > (*FactoryMethodType)();
        typedef std::function< std::remove_pointer< FactoryMethodType >::type > FactoryMethodWrapper;
        typedef std::map< std::string, FactoryMethodWrapper > FactoryMethodMap;

    private:

        FactoryMethodMap m_algorithm_map;

    public:

        explicit RoutingAlgorithmRegistry();

        void register_algorithm( const std::string& name, const FactoryMethodWrapper& factory_method );
        const FactoryMethodMap& algorithm_map() const;
        std::unique_ptr< RoutingAlgorithm > instantiate_algorithm( const std::string& name ) const;

        static RoutingAlgorithmRegistry& instance();
};

class StaticAlgorithmRegistration
{
    public:
        StaticAlgorithmRegistration( const std::string& name,
                                     const RoutingAlgorithmRegistry::FactoryMethodWrapper& factory_method )
        {
            RoutingAlgorithmRegistry::instance().register_algorithm( name, factory_method );
        }
};

#endif // ROUTINGALGORITHMREGISTRY_H
