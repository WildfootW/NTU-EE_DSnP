/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
    friend class BSTree<T>;
    friend class BSTree<T>::iterator;

    BSTreeNode(const T& d, BSTreeNode<T>* parent = NULL, BSTreeNode<T>* rchild = NULL, BSTreeNode<T>* lchild = NULL): _data(d), _parent(parent), _rchild(rchild), _lchild(lchild){
        update_size();
    }

    void update_size()
    {
        _size = 1;
        if(_rchild != NULL)
            _size += _rchild->_size;
        if(_lchild != NULL)
            _size += _lchild->_size;
    }
    BSTreeNode<T>* get_rchild_end()    // find maximal
    {
        BSTreeNode<T>* ret(this);
        while(ret->_rchild != NULL)
            ret = ret->_rchild;
        return ret;
    }
    BSTreeNode<T>* get_lchild_end()    // find minimal
    {
        BSTreeNode<T>* ret(this);
        while(ret->_lchild != NULL)
            ret = ret->_lchild;
        return ret;
    }
    BSTreeNode<T>* get_lparent_end()
    {
        BSTreeNode<T>* ret(this);
        while(ret->_parent != NULL && ret->_parent->_rchild == ret)
            ret = ret->_parent;
        return ret;
    }
    BSTreeNode<T>* get_rparent_end()
    {
        BSTreeNode<T>* ret(this);
        while(ret->_parent != NULL && ret->_parent->_lchild == ret)
            ret = ret->_parent;
        return ret;
    }

    T               _data;
    BSTreeNode<T>*  _parent;
    BSTreeNode<T>*  _rchild;
    BSTreeNode<T>*  _lchild;
    size_t          _size;       // number of childs (include itself)
};


template <class T>
class BSTree
{
public:
    BSTree()
    {
        _maximum = new BSTreeNode<T>(T());
    }
    ~BSTree() { clear(); delete _maximum; }

    class iterator
    {
        friend class BSTree;

    public:
        iterator(BSTreeNode<T>* n = 0): _node(n) {}
        iterator(const iterator& i) : _node(i._node) {}
        ~iterator() {} // Should NOT delete _node

        const T& operator * () const { return _node->_data; }
        T& operator * () { return _node->_data; }

        iterator& operator = (const iterator& i) { this->_node = i._node; return (*this); }

        bool operator == (const iterator& i) const { return (this->_node == i._node); }
        bool operator != (const iterator& i) const { return !((*this) == i); }

        iterator& operator ++ () {                                                  // Successor (return NULL if _node is maximum)
            if(_node->_rchild != NULL)
                _node = _node->_rchild->get_lchild_end();
            else
                _node = _node->get_lparent_end()->_parent;
            return (*this);
        }
        iterator operator ++ (int) { iterator ret(*this); ++(*this); return ret; }  // postfix
        iterator& operator -- () {                                                  // Predecessor (return NULL if _node is minimum)
            if(_node->_lchild != NULL)
                _node = _node->_lchild->get_rchild_end();
            else
                _node = _node->get_rparent_end()->_parent;
            return (*this);
        }
        iterator operator -- (int) { iterator ret(*this); --(*this); return ret; }
        iterator operator + (int num)
        {
            iterator ret = (*this);
            while(num--)
                ++ret;
            return ret;
        }
        iterator operator - (int num)
        {
            iterator ret = (*this);
            while(num--)
                --ret;
            return ret;
        }

    private:
        BSTreeNode<T>* _node;
    };

    iterator begin() const { return iterator(_maximum->get_lchild_end()); }
    iterator end()   const { return iterator(_maximum); }
    iterator back()  const { return end() - 1; }

    bool empty() const { return (size() == 0); }
    size_t size() const { return (_maximum->_size - 1); }

    void insert(const T& x) {
        if(empty())
        {
            BSTreeNode<T>* new_node = new BSTreeNode<T>(x, _maximum);
            _maximum->_lchild = new_node;
            _maximum->_size = 2;
            return;
        }
        BSTreeNode<T>* current = _maximum->_lchild;
        while(true)
        {
            if(current->_data >= x)
            {
                if(current->_lchild == NULL)
                {
                    BSTreeNode<T>* new_node = new BSTreeNode<T>(x, current);
                    current->_lchild = new_node;
                    update_size_upward(new_node);
                    break;
                }
                current = current->_lchild;
            }
            else
            {
                if(current->_rchild == NULL)
                {
                    BSTreeNode<T>* new_node = new BSTreeNode<T>(x, current);
                    current->_rchild = new_node;
                    update_size_upward(new_node);
                    break;
                }
                current = current->_rchild;
            }
        }
    }

    iterator find(const T& x) {
        if(empty())
            return end();
        BSTreeNode<T>* current = _maximum->_lchild;
        while(true)
        {
            if(current->_data > x)
            {
                if(current->_lchild == NULL)
                    break;
                current = current->_lchild;
            }
            else if(current->_data < x)
            {
                if(current->_rchild == NULL)
                    break;
                current = current->_rchild;
            }
            else
                return iterator(current);
        }
        return end();
    }

    bool erase(iterator pos) {
        BSTreeNode<T>* current = pos._node;
        BSTreeNode<T>* parent  = current->_parent;
        if(current->_rchild == NULL && current->_lchild == NULL)
        {
            if(parent->_rchild == current)
                parent->_rchild = NULL;
            else if(parent->_lchild == current)
                parent->_lchild = NULL;
            delete current;
            update_size_upward(parent);
        }
        else if(current->_rchild == NULL || current->_lchild == NULL)
        {
            BSTreeNode<T>* child = current->_rchild;
            if(child == NULL)
                child = current->_lchild;

            if(parent->_rchild == current)
                parent->_rchild = child;
            else if(parent->_lchild == current)
                parent->_lchild = child;
            child->_parent = parent;
            delete current;
            update_size_upward(parent);
        }
        else
        {
            iterator successor = iterator(current) + 1;
            current->_data = successor._node->_data;    // really?
            erase(successor);
        }
        return true;
    }
    bool erase(const T& x) {
        iterator it = find(x);
        if(it == end())
            return false;
        return erase(it);
    }
    void pop_front() { erase(begin()); }
    void pop_back() { erase(back()); }
    void clear() {      // PostOrder
        if(empty())
            return;
        clear_dfs(_maximum->_lchild);
    }

    void sort() const {  }
    void print() const {  } // print -v
private:
    BSTreeNode<T>*  _maximum;   // dummy node
    void clear_dfs(BSTreeNode<T>* node)
    {
        if(node->_lchild != NULL)
            clear_dfs(node->_lchild);
        if(node->_rchild != NULL)
            clear_dfs(node->_rchild);
        erase(iterator(node));
    }
    void update_size_upward(BSTreeNode<T>* node)
    {
        do
        {
            node->update_size();
            node = node->_parent;
        }
        while(node != NULL);
    }
};

#endif // BST_H
