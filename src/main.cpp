#include <cstddef>
#include <cstdlib>
#include <string>

// For calculating elapsed time
#include <chrono>

#include <iostream>

#include <iomanip>

#include "TreeMap.h"
#include "HashMap.h"

using ns = std::chrono::nanoseconds;
using get_time = std::chrono::steady_clock;

namespace
{

//template <typename K, typename V>
//using Hash_Map = aisdi::HashMap<K, V>;
//template <typename K, typename V>
//using Tree_Map = aisdi::TreeMap<K, V>;


} // namespace

ns testAddRandomNumberHashMap( std::size_t number_of_elements, std::size_t size_of_table )
{
    aisdi::HashMap< int, int > x(size_of_table);

    srand( 0 );
    auto start = get_time::now();

    for( std::size_t i = 0; i < number_of_elements; ++i )
        x[i] = rand()%number_of_elements;

    return std::chrono::duration_cast<ns>(get_time::now() - start);
}

ns testAddRandomNumberTreeMap( std::size_t number_of_elements )
{
    aisdi::TreeMap< int, int > x;

    srand( 0 );
    auto start = get_time::now();

    for( std::size_t i = 0; i < number_of_elements; ++i )
        x[i] = rand()%number_of_elements;

    return std::chrono::duration_cast<ns>(get_time::now() - start);
}

ns testSearchRandomNumberHashMap( std::size_t number_of_elements, std::size_t size_of_table )
{
    aisdi::HashMap< int, int > x(size_of_table);

    for( std::size_t i = 0; i < number_of_elements; ++i )
        x[i] = rand()%number_of_elements;

    srand( 0 );
    auto start = get_time::now();

    for( std::size_t i = 0; i < number_of_elements; ++i )
        x[rand()%number_of_elements];

    return std::chrono::duration_cast<ns>(get_time::now() - start);
}

ns testSearchRandomNumberTreeMap( std::size_t number_of_elements )
{
    aisdi::TreeMap< int, int > x;

    for( std::size_t i = 0; i < number_of_elements; ++i )
        x[i] = rand()%number_of_elements;

    srand( 0 );
    auto start = get_time::now();

    for( std::size_t i = 0; i < number_of_elements; ++i )
        x[rand()%number_of_elements];

    return std::chrono::duration_cast<ns>(get_time::now() - start);
}

ns testIterationRandomNumberHashMap( std::size_t number_of_elements, std::size_t size_of_table )
{
    aisdi::HashMap< int, int > x(size_of_table);

    for( std::size_t i = 0; i < number_of_elements; ++i )
        x[i] = rand()%number_of_elements;

    srand( 0 );
    auto start = get_time::now();

    for( auto i = x.begin(); i != x.end(); ++i );

    return std::chrono::duration_cast<ns>(get_time::now() - start);
}

ns testIterationRandomNumberTreeMap( std::size_t number_of_elements )
{
    aisdi::TreeMap< int, int > x;

    for( std::size_t i = 0; i < number_of_elements; ++i )
        x[i] = rand()%number_of_elements;

    srand( 0 );
    auto start = get_time::now();

    for( auto i = x.begin(); i != x.end(); ++i );

    return std::chrono::duration_cast<ns>(get_time::now() - start);
}

ns testDeleteAllHashMap( std::size_t number_of_elements, std::size_t size_of_table )
{
    aisdi::HashMap< int, int > *x = new aisdi::HashMap< int, int >(size_of_table);

    for( std::size_t i = 0; i < number_of_elements; ++i )
        (*x)[i] = rand()%number_of_elements;

    srand( 0 );
    auto start = get_time::now();

    delete x;

    return std::chrono::duration_cast<ns>(get_time::now() - start);
}

ns testDeleteAllTreeMap( std::size_t number_of_elements )
{
    aisdi::TreeMap< int, int > *x = new aisdi::TreeMap< int, int >;

    for( std::size_t i = 0; i < number_of_elements; ++i )
        (*x)[i] = rand()%number_of_elements;

    srand( 0 );
    auto start = get_time::now();

    delete x;

    return std::chrono::duration_cast<ns>(get_time::now() - start);
}

ns testAddingInOrderHashMap( std::size_t number_of_elements, std::size_t size_of_table )
{
    aisdi::HashMap< int, int > x(size_of_table);

    srand( 0 );
    auto start = get_time::now();

    for( std::size_t i = 0; i < number_of_elements; ++i )
        x[i] = i;

    return std::chrono::duration_cast<ns>(get_time::now() - start);
}

ns testAddingInOrderTreeMap( std::size_t number_of_elements )
{
    aisdi::TreeMap< int, int > x;

    srand( 0 );
    auto start = get_time::now();

    for( std::size_t i = 0; i < number_of_elements; ++i )
        x[i] = i;

    return std::chrono::duration_cast<ns>(get_time::now() - start);
}

int main(int argc, char** argv)
{
    const std::size_t number_of_elements    = argc > 1 ? std::atoll(argv[1]) : 100000;
    const std::size_t size_of_table         = argc > 2 ? std::atoll(argv[2]) : 100000;

    /// ADDING RANDOM ELEMENTS (NUMBERS)

    /// TEST#1 ===========================================================================
    std::cout << "Test#1: adding random elements, size_of_table == " << size_of_table << " (for HashMap)\n";

    auto diff = testAddRandomNumberHashMap( number_of_elements, size_of_table );

    std::cout << "HashMap    :" << std::setw(20) << std::right << std::chrono::duration_cast<ns>(diff).count() << " ns\n";

    auto diff2 = testAddRandomNumberTreeMap( number_of_elements );

    std::cout << "TreeMap    :" << std::setw(20) << std::right << std::chrono::duration_cast<ns>(diff2).count() << " ns\n";

    std::cout << "Difference :" << std::setw(20) << std::right << std::chrono::duration_cast<ns>(diff-diff2).count() << " ns\n\n";



    /// SEARCHING FOR RANDOM ELEMENT

    std::cout << "Test#2: searching for random elements, size_of_table == " << size_of_table << " (for HashMap)\n";

    diff = testSearchRandomNumberHashMap( number_of_elements, size_of_table );

    std::cout << "HashMap    :" << std::setw(20) << std::right << std::chrono::duration_cast<ns>(diff).count() << " ns\n";

    diff2 = testSearchRandomNumberTreeMap( number_of_elements );

    std::cout << "TreeMap    :" << std::setw(20) << std::right << std::chrono::duration_cast<ns>(diff2).count() << " ns\n";

    std::cout << "Difference :" << std::setw(20) << std::right << std::chrono::duration_cast<ns>(diff-diff2).count() << " ns\n\n";

    /// ITERATION

    std::cout << "Test#3: iterating from the begin to the end, size_of_table == " << size_of_table << " (for HashMap)\n";

    diff = testIterationRandomNumberHashMap( number_of_elements, size_of_table );

    std::cout << "HashMap    :" << std::setw(20) << std::right << std::chrono::duration_cast<ns>(diff).count() << " ns\n";

    diff2 = testIterationRandomNumberTreeMap( number_of_elements );

    std::cout << "TreeMap    :" << std::setw(20) << std::right << std::chrono::duration_cast<ns>(diff2).count() << " ns\n";

    std::cout << "Difference :" << std::setw(20) << std::right << std::chrono::duration_cast<ns>(diff-diff2).count() << " ns\n\n";

    /// DELETING

    std::cout << "Test#4: deleting all elements, size_of_table == " << size_of_table << " (for HashMap)\n";

    diff = testDeleteAllHashMap( number_of_elements, size_of_table );

    std::cout << "HashMap    :" << std::setw(20) << std::right << std::chrono::duration_cast<ns>(diff).count() << " ns\n";

    diff2 = testDeleteAllTreeMap( number_of_elements );

    std::cout << "TreeMap    :" << std::setw(20) << std::right << std::chrono::duration_cast<ns>(diff2).count() << " ns\n";

    std::cout << "Difference :" << std::setw(20) << std::right << std::chrono::duration_cast<ns>(diff-diff2).count() << " ns\n\n";

    /// ADDING IN ORDER

    std::cout << "Test#5: adding elements in order, size_of_table == " << size_of_table << " (for HashMap)\n";

    diff = testAddingInOrderHashMap( number_of_elements, size_of_table );

    std::cout << "HashMap    :" << std::setw(20) << std::right << std::chrono::duration_cast<ns>(diff).count() << " ns\n";

    diff2 = testAddingInOrderTreeMap( number_of_elements );

    std::cout << "TreeMap    :" << std::setw(20) << std::right << std::chrono::duration_cast<ns>(diff2).count() << " ns\n";

    std::cout << "Difference :" << std::setw(20) << std::right << std::chrono::duration_cast<ns>(diff-diff2).count() << " ns\n\n";


    return 0;
}
