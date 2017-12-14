#ifndef AISDI_MAPS_HASHMAP_H
#define AISDI_MAPS_HASHMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>

#include <iostream>

#include <functional>

namespace aisdi
{

template <typename KeyType, typename ValueType>
class HashMap
{
public:
    using key_type = KeyType;
    using mapped_type = ValueType;
    using value_type = std::pair<const key_type, mapped_type>;
    using size_type = std::size_t;
    using reference = value_type&;
    using const_reference = const value_type&;

    class ConstIterator;
    class Iterator;
    using iterator = Iterator;
    using const_iterator = ConstIterator;

public:
    HashMap( size_type tableSize = 1000 ) : size_of_table(tableSize), table(nullptr), count(0)
    {
        table = new HashNode* [size_of_table];
        for( size_type i = 0; i < size_of_table; ++i )
            table[i] = nullptr;
    }

    HashMap( std::initializer_list<value_type> list ) : HashMap( list.size() ) // HashMap()
    {
        for( auto it = list.begin(); it != list.end(); ++it )
            (*this)[(*it).first] = (*it).second;
    }

    HashMap( const HashMap& other ) : HashMap( other.size_of_table )
    {
        *this = other;
    }

    HashMap( HashMap&& other ) : size_of_table( other.size_of_table ), table(other.table), count(other.count)
    {
        other.size_of_table = 0;
        other.table = nullptr;
        other.count = 0;
    }

    ~HashMap()
    {
        deleteAll();
        delete[] table;
    }

    HashMap& operator=( const HashMap& other )
    {
        if( this != &other )
        {
            deleteAll();
            for( auto it = other.begin(); it != other.end(); ++it )
                (*this)[(*it).first] = (*it).second;
        }
        return *this;
    }

    HashMap& operator=( HashMap&& other )
    {
        if( this != &other )
        {
            deleteAll();
            delete[] table;

            size_of_table = other.size_of_table;
            table = other.table;
            count = other.count;

            other.size_of_table = 0;
            other.table = nullptr;
            other.count = 0;
        }

        return *this;
    }

    bool isEmpty() const
    {
        return (count == 0);
    }

    mapped_type& operator[]( const key_type& key )
    {
        size_type hash_key = hashFunction( key );
        HashNode* node = findNode( key );

        if( node == nullptr )
        {
            node = new HashNode( key, mapped_type() );
            ++count;

            if( table[ hash_key ] == nullptr )
            {
                table[ hash_key ] = node;
            }
            else
            {
                HashNode* tmp = table[ hash_key ];
                while( tmp->next != nullptr )
                    tmp = tmp->next;

                tmp->next = node;
                node->prev = tmp;
            }
        }
        return node->data.second;
    }

    const mapped_type& valueOf( const key_type& key ) const
    {
        HashNode* node = findNode(key);
        if( node == nullptr )
            throw std::out_of_range("valueOf");
        return node->data.second;
    }

    mapped_type& valueOf( const key_type& key )
    {
        HashNode* node = findNode( key );
        if( node == nullptr )
            throw std::out_of_range("valueOf");
        return node->data.second;
    }

    const_iterator find( const key_type& key ) const
    {
        return const_iterator( this, findNode(key), hashFunction(key) );
    }

    iterator find( const key_type& key )
    {
        return iterator( this, findNode(key), hashFunction(key) );
    }

    void remove( const key_type& key )
    {
        remove( find( key ) );
    }

    void remove(const const_iterator& it)
    {
        if(this != it.base_map || it == end())
            throw std::out_of_range("remove");
        remove(it.node, it.node->data.first);
    }

    size_type getSize() const
    {
        return count;
    }

    bool operator==(const HashMap& other) const
    {
        if(count != other.count)
            return false;

        for(auto it = begin(), it2 = other.begin() ; it!=end(); ++it, ++it2)
        {
            if(*it != *it2)
                return false;
        }

        return true;
    }

    bool operator!=(const HashMap& other) const
    {
        return !(*this == other);
    }

    iterator begin()
    {
        auto node_index = findFirstNode();
        return iterator (this, node_index.first, node_index.second);
    }

    iterator end()
    {
        iterator it( this );
        return it;
    }

    const_iterator cbegin() const
    {
        auto node_index = findFirstNode();
        return const_iterator( this, node_index.first, node_index.second );
    }

    const_iterator cend() const
    {
        const_iterator it( this );
        return it;
    }

    const_iterator begin() const
    {
        return cbegin();
    }

    const_iterator end() const
    {
        return cend();
    }

private:
    //const size_type size_of_table;
    size_type size_of_table;

    struct HashNode
    {
        value_type data;
        HashNode *next;
        HashNode *prev;
        HashNode(key_type key, mapped_type mapped) : data(std::make_pair( key, mapped )), next(nullptr), prev(nullptr) {}
        HashNode(key_type key, mapped_type mapped, HashNode *prev) : HashNode(key, mapped) { this->prev = prev; }
        ~HashNode() { delete next; }
    };
    HashNode **table;
    size_type count;


    void deleteAll()
    {
        if( count != 0 )
        {
            for( size_type i = 0; i < size_of_table; ++i )
            {
                delete table[i];
                table[i] = nullptr;
            }
        }
        count = 0;
    }

    void remove( HashNode* node, const key_type& key )
    {
        if(node->prev == nullptr)
            table[hashFunction(key)] = node->next;
        else
            node->prev->next = node->next;

        if( node->next != nullptr )
            node->next->prev = node->prev;

        node->next = nullptr;
        delete node;
        count--;
    }

    size_type hashFunction( const key_type& key ) const
    {
        std::hash<key_type> tmp;
        return tmp(key)%size_of_table;
    }

    HashNode* findNode( const key_type& key ) const
    {
        HashNode *node = table[ hashFunction(key) ];
        while( node != nullptr )
        {
            if( node->data.first == key )
                return node;

            node = node->next;
        }
        return node;
    }

    std::pair<HashNode*, size_type> findFirstNode() const
    {
        size_type index = 0;

        while( index < size_of_table && table[index] == nullptr )
            ++index;

        HashNode *node = nullptr;
        if( index < size_of_table )
            node = table[index];

        return std::make_pair( node, index );
    }

};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::ConstIterator
{
    const HashMap *base_map;
    HashNode *node;
    size_type index;
    friend class HashMap;

public:
    using reference = typename HashMap::const_reference;
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = typename HashMap::value_type;
    using pointer = const typename HashMap::value_type*;

    explicit ConstIterator( const HashMap *base_map = nullptr, HashNode *node = nullptr, size_type index = 0 ) : base_map(base_map), node(node), index(index)
    {
        if( node == nullptr && base_map != nullptr )
            this->index = base_map->size_of_table;
    }

    ConstIterator( const ConstIterator& other ) : ConstIterator(other.base_map, other.node, other.index)
    {}

    ConstIterator& operator++()
    {
        if( base_map == nullptr || node == nullptr )
        {
            throw std::out_of_range("operator++");
        }
        else if(node->next != nullptr)
        {
            node = node->next;
        }
        else
        {
            for( ++index; index < base_map->size_of_table && base_map->table[index] == nullptr; ++index );

            if( index >= base_map->size_of_table )
                node = nullptr;
            else
                node = base_map->table[index];
        }
        return *this;
    }

    ConstIterator operator++(int)
    {
        auto result = *this;
        ++(*this);
        return result;
    }

    ConstIterator& operator--()
    {
        if( base_map == nullptr )
            throw std::out_of_range("operator--");
        else if( node == nullptr || node == base_map->table[index] )
        {
            if( index == 0 && base_map->table[index] == nullptr )
                throw std::out_of_range("operator--");

            for( --index; index > 0 && base_map->table[index] == nullptr; --index );

            if( index == 0 && base_map->table[index] == nullptr )
                throw std::out_of_range("operator--");

            node = base_map->table[index];
            while( node->next != nullptr )
                node = node->next;
        }
        else
        {
            node = node->prev;
        }

        return *this;
  }

    ConstIterator operator--(int)
    {
        auto result = *this;
        --(*this);
        return result;
    }

    reference operator*() const
    {
        if( node == nullptr )
            throw std::out_of_range("operator*");
        return node->data;
    }

    pointer operator->() const
    {
        return &this->operator*();
    }

    bool operator==( const ConstIterator& other ) const
    {
        return base_map == other.base_map && node == other.node && index == other.index;
    }

    bool operator!=( const ConstIterator& other ) const
    {
        return !(*this == other);
    }
};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::Iterator : public HashMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename HashMap::reference;
  using pointer = typename HashMap::value_type*;

  explicit Iterator(HashMap *myMap = nullptr, HashNode *myNode = nullptr, size_type index = 0) : ConstIterator(myMap, myNode, index)
  {}

  Iterator(const ConstIterator& other)
    : ConstIterator(other)
  {}

  Iterator& operator++()
  {
    ConstIterator::operator++();
    return *this;
  }

  Iterator operator++(int)
  {
    auto result = *this;
    ConstIterator::operator++();
    return result;
  }

  Iterator& operator--()
  {
    ConstIterator::operator--();
    return *this;
  }

  Iterator operator--(int)
  {
    auto result = *this;
    ConstIterator::operator--();
    return result;
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  reference operator*() const
  {
    // ugly cast, yet reduces code duplication.
    return const_cast<reference>(ConstIterator::operator*());
  }
};

}

#endif /* AISDI_MAPS_HASHMAP_H */
