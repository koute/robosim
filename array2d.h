#ifndef ARRAY2D_H
#define ARRAY2D_H

#include <vector>
#include <assert.h>
#include <stdint.h>

template< typename T > struct vector_trait { typedef std::vector< T > type; };
template<> struct vector_trait< bool > { typedef std::vector< uint8_t > type; };

template < typename type_t >
class Array2d
{
    unsigned m_width;
    unsigned m_height;

    typename vector_trait< type_t >::type m_vector;

    public:
        explicit Array2d( const unsigned width, const unsigned height, const type_t default_value = type_t() ) :
            m_width( width ),
            m_height( height ),
            m_vector( width * height, default_value )
        {
        }

        Array2d< type_t >& operator =( const Array2d< type_t >& array )
        {
            m_width = array.width();
            m_height = array.height();
            m_vector = array.vector();

            return *this;
        }

        /**
         * @return Width of the array.
         */
        unsigned width() const
        {
            return m_width;
        }

        /**
         * @return Height of the array.
         */
        unsigned height() const
        {
            return m_height;
        }

        /**
         * @return Underlying vector used for storage.
         */
        typename vector_trait< type_t >::type& vector()
        {
            return m_vector;
        }

        /**
         * @return Underlying vector used for storage.
         */
        const typename vector_trait< type_t >::type& vector() const
        {
            return m_vector;
        }

        /**
         * @return Reference to the element of the array at given point.
         */
        type_t& at( const unsigned x, const unsigned y )
        {
            assert( x < width() );
            assert( y < height() );

            return *((type_t *)&m_vector[ y * height() + x ]);
        }

        /**
         * @return Reference to the element of the array at given point.
         */
        const type_t& at( const unsigned x, const unsigned y ) const
        {
            assert( x < width() );
            assert( y < height() );

            return *((const type_t *)&m_vector[ y * height() + x ]);
        }

        /**
         * @brief Clears the array with @a value.
         */
        void clear_with( type_t value )
        {
            const std::size_t size = m_vector.size();
            type_t * data = (type_t *)m_vector.data();
            for( std::size_t i = 0; i < size; ++i )
                *(data + i) = value;
        }
};

#endif // ARRAY2D_H
