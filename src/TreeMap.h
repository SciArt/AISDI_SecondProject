#ifndef AISDI_MAPS_TREEMAP_H
#define AISDI_MAPS_TREEMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <queue>

namespace aisdi
{

template <typename KeyType, typename ValueType>
class TreeMap
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

    TreeMap() : root(nullptr), size_of_tree(0) {}

    TreeMap( std::initializer_list<value_type> list ) : TreeMap()
    {
        for ( auto it = list.begin(); it != list.end(); ++it )
            addNode( new Node(*it) );
    }

    TreeMap( const TreeMap& other ) : TreeMap()
    {
        *this = other;
    }

    TreeMap(TreeMap&& other) //: TreeMap()
    {
        root = other.root;
        size_of_tree = other.size_of_tree;

        other.root = nullptr;
        other.size_of_tree = 0;
    }

    ~TreeMap()
    {
        deleteAll();
    }

    TreeMap& operator=( const TreeMap& other )
    {
        if(this != &other)
        {
            deleteAll();
            for( auto it = other.begin(); it != other.end(); ++it )
                addNode( new Node(*it) );
        }
        return *this;
    }

    TreeMap& operator=( TreeMap&& other )
    {
        if(this != &other)
        {
            deleteAll();

            root = other.root;
            size_of_tree = other.size_of_tree;

            other.root = nullptr;
            other.size_of_tree = 0;
        }
        return *this;
    }

    bool isEmpty() const
    {
        return (size_of_tree == 0);
    }

    mapped_type& operator[]( const key_type& key )
    {
        // Find node by the key
        Node* node = findNodeByKey( key );

        // If it does not exist create it
        if( node == nullptr )
        {
            node = new Node( key, mapped_type() );
            addNode( node );
        }
        return node->data.second; // return value
    }

    const mapped_type& valueOf(const key_type& key) const
    {
        const Node* node = findNodeByKey( key );
        if( node == nullptr )
            throw std::out_of_range("valueOf() const");
        return node->data.second;
    }

    mapped_type& valueOf(const key_type& key)
    {
        Node* node = findNodeByKey( key );
        if( node == nullptr )
            throw std::out_of_range("valueOf()");
        return node->data.second;
    }

    const_iterator find(const key_type& key) const
    {
        return const_iterator( this, findNodeByKey(key) );
    }

    iterator find(const key_type& key)
    {
        return iterator( this, findNodeByKey(key) );
    }

    void remove(const key_type& key)
    {
        remove( find(key) );
    }

    void remove( const const_iterator& it )
    {
        if( this != it.tree || it == end() )
            throw std::out_of_range("remove()");


        if( it.node->right == nullptr ) // One child - left child (or none)
        {
            replace( it.node, it.node->left );
            balanceTree( it.node->left );
        }
        else if( it.node->left == nullptr ) // One child - right child
        {
            replace( it.node, it.node->right );
            balanceTree( it.node->right );
        }
        else // Two children
        {
            iterator tmp = it;
            ++tmp;
            Node* tmp_parent = tmp.node->parent;
            replace( tmp.node, tmp.node->right );
            replace( it.node, tmp.node );
            balanceTree( tmp_parent );
        }

        delete it.node;
        --size_of_tree;
        return;
    }

    size_type getSize() const
    {
        return size_of_tree;
    }

    bool operator==(const TreeMap& other) const
    {
        if( size_of_tree != other.size_of_tree )
            return false;

        for(auto it0 = begin(), it1 = other.begin(); it0 != end(); ++it0, ++it1)
        {
            if(*it0 != *it1)
                return false;
        }
        return true;
    }

    bool operator!=(const TreeMap& other) const
    {
        return !(*this == other);
    }

    iterator begin()
    {
        return iterator(this, findSmallest(root));
    }

    iterator end()
    {
        return iterator(this, nullptr);
    }

    const_iterator cbegin() const
    {
        return const_iterator(this, findSmallest(root));
    }

    const_iterator cend() const
    {
        return const_iterator(this, nullptr);
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
    struct Node
    {
        value_type data;
        Node *left, *right, *parent;
        int height; // Height of the subtree
        Node( key_type key, mapped_type mapped )
        : data( std::make_pair( key, mapped ) ), left(nullptr), right(nullptr), parent(nullptr), height(1) {}

        Node( value_type it ) : Node( it.first,it.second ) {}

        ~Node() { delete left; delete right; }
    };
    Node* root;
    size_type size_of_tree;

    void deleteAll()
    {
        delete root;
        root = nullptr;
        size_of_tree = 0;
    }

    void addNode( Node* node )
    {
        if( root == nullptr )
        {
            root = node;
            ++size_of_tree;
            return;
        }
        Node* tmp = root;

        while( node->data.first != tmp->data.first )
        {
            if( node->data.first > tmp->data.first )
            {
                if( tmp->right == nullptr )
                {
                    tmp->right = node;
                    node->parent = tmp;
                    break;
                }
                else
                    tmp = tmp->right;
            }
            else //if (node->data.first < tmp->data.first)
            {
                if( tmp->left == nullptr )
                {
                    tmp->left = node;
                    node->parent = tmp;
                    break;
                }
                else
                    tmp = tmp->left;
            }
        }

        if( node->data.first == tmp->data.first )
        {
            delete node;
            return;
        }

        balanceTree( tmp );

        size_of_tree++;
        return;
    }

    void replace( Node* x, Node* y )
    {
        if( x->parent == nullptr )
            root = y;
        else if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;

        if( y != nullptr )
        {
            y->parent = x->parent;

            if( x->right != nullptr && x->right != y )
                y->right = x->right;

            if( x->left != nullptr && x->left != y )
                y->left = x->left;
        }

        // Prepering 'x' to be deleted without deleting subtrees
        x->parent = nullptr;
        x->left   = nullptr;
        x->right  = nullptr;
    }

    Node* findNodeByKey( const key_type& key ) const
    {
        Node* node = root;
        while( node != nullptr && key != node->data.first )
        {
            if( key > node->data.first )
                node = node->right;

            else if( key < node->data.first )
                node = node->left;
        }
        return node;
    }

    Node* findSmallest( Node* node ) const
    {
        if( node != nullptr )
            while( node->left != nullptr )
                node = node->left;

        return node;
    }

    void balanceTree( Node* node )
    {
        while( node != nullptr )
        {
            if( isBalanced( node ) )
            {
                updateHeight( node );
                node = node->parent;
                continue;
            }

            if( getHeight(node->left) <= getHeight(node->right) ) // Right subtree height is bigger
            {
                if( getHeight(node->right->left) <= getHeight(node->right->right) )
                {
                    rotateLeft( node );
                }
                else
                {
                    rotateRight( node->right );
                    updateHeight( node->right );
                    rotateLeft( node );
                }
            }
            else // Left subtree height is bigger
            {
                if( getHeight(node->left->left) >= getHeight(node->left->right) )
                {
                    rotateRight( node );
                }
                else
                {
                    rotateLeft( node->left );
                    updateHeight( node->left );
                    rotateRight( node );
                }
            }

            updateHeight( node );
            node = node->parent;
        }
    }

    int getHeight( Node* node ) const
    {
        if (node == nullptr)
            return 0;

        return node->height;
    }

    void updateHeight( Node* node )
    {
        node->height = std::max( getHeight( node->left ), getHeight( node->right ) ) + 1;
    }

    bool isBalanced( Node* node ) const
    {
        if( node == nullptr )
            return false;

        if( std::abs( getHeight(node->left) - getHeight(node->right) ) <= 1 )
            return true;

        return false;
    }

    void rotateLeft( Node* node )
    {
        Node *parent, *right, *right_left;
        parent = node->parent;
        right = node->right;
        right_left = node->right->left;

        if( parent != nullptr )
        {
            if( parent->right == node )
                parent->right = right;
            else
                parent->left = right;
        }
        else
            root = right;

        right->parent = parent;

        right->left = node;
        node->parent = right;

        node->right = right_left;
        if( right_left != nullptr )
            right_left->parent = node;

        //updateHeight( node );
        //updateHeight( R );
    }

    void rotateRight( Node* node )
    {
        Node *parent, *left, *left_right;
        parent = node->parent;
        left = node->left;
        left_right = node->left->right;

        if( parent != nullptr )
        {
            if( parent->right == node )
                parent->right = left;
            else
                parent->left = left;
        }
        else
            root = left;

        left->parent = parent;

        left->right = node;
        node->parent = left;

        node->left = left_right;
        if( left_right != nullptr )
            left_right->parent = node;

        //updateHeight( node );
        //updateHeight( L );
    }
};

template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::ConstIterator
{
    const TreeMap *tree;
    Node *node;
    friend class TreeMap;

public:
    using reference = typename TreeMap::const_reference;
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = typename TreeMap::value_type;
    using pointer = const typename TreeMap::value_type*;

    explicit ConstIterator(const TreeMap *tree = nullptr, Node *node = nullptr) : tree(tree), node(node)
    {}

    ConstIterator(const ConstIterator& other) : ConstIterator(other.tree, other.node)
    {}

    ConstIterator& operator++()
    {
        if(tree == nullptr || node == nullptr)
            throw std::out_of_range("operator++");

        // We want the bigger one, but as little as possible

        // If right node exist we are going there, because there are bigger values
        if( node->right != nullptr )
        {
            node = node->right;

            // We need as small difference as possible so, we go left all the way
            while( node->left != nullptr )
                node = node->left;

            return *this;
        }

        // If we do not have right node, we search in parents
        // It is possible, that we are the left child
        // and some parent is bigger then we are

        // We are searching for parent to whom we are left child
        // If it does not exist we return nullptr
        while( node->parent != nullptr && node->parent->left != node )
            node = node->parent;

        node = node->parent;

        return *this;
    }

    ConstIterator operator++(int)
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    ConstIterator& operator--()
    {
        if( tree == nullptr || tree->root == nullptr )
            throw std::out_of_range("operator--");

        // If node == nullptr we are in the end of tree
        if(node == nullptr)
        {
            // We are searching for the highest value
            node = tree->root;
            while( node->right != nullptr )
              node = node->right;

            return *this;
        }

        // If we have left child, then we should search
        // there for the lower value
        if(node->left != nullptr )
        {
            node = node->left;

            // We want the smallest difference so
            // we go now all the way right
            while(node->right != nullptr)
                node = node->right;

            return *this;
        }

        // If we do not have left child
        // We must search in parents
        // Parent to whom we are the right child
        // is smaller then we are
        while( node->parent != nullptr && node->parent->right != node )
            node = node->parent;

        // node == nullptr is reserved for the end of tree
        if( node->parent == nullptr )
            throw std::out_of_range("operator--");

        node = node->parent;

        return *this;
    }

    ConstIterator operator--(int)
    {
        auto tmp = *this;
        --(*this);
        return tmp;
    }

    pointer operator->() const
    {
        return &this->operator*();
    }

    reference operator*() const
    {
        if(tree == nullptr || node == nullptr)
            throw std::out_of_range("operator*");
        return node->data;
    }

    bool operator==(const ConstIterator& other) const
    {
        return ( (tree == other.tree) && (node == other.node) );
    }

    bool operator!=(const ConstIterator& other) const
    {
        return !(*this == other);
    }
};


template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::Iterator : public TreeMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename TreeMap::reference;
  using pointer = typename TreeMap::value_type*;

  explicit Iterator(TreeMap *tree = nullptr, Node *node = nullptr) : ConstIterator(tree, node) {}

  Iterator(const ConstIterator& other) : ConstIterator(other) {}

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

#endif /* AISDI_MAPS_MAP_H */
