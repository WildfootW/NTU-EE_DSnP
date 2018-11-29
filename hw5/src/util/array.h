/****************************************************************************
  FileName     [ array.h ]
  PackageName  [ util ]
  Synopsis     [ Define dynamic array package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>
#include <algorithm>

using namespace std;

// NO need to implement class ArrayNode
//
template <class T>
class Array
{
public:
   Array() : _data(0), _size(0), _capacity(0), _isSorted(true) {}
   ~Array() { delete []_data; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class Array;

   public:
      iterator(T* n = 0): _node(n) {}
      iterator(const iterator& i): _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // implement these overloaded operators
      // this is a "pointer" to the object, so *this is just the reference to the object
      const T& operator * () const { return (*_node); }     // while use iterator as a pointer
      T& operator * () { return (*_node); }

      bool operator != (const iterator& i) const { return _node != i._node; }
      bool operator == (const iterator& i) const { return !((*this) != i); }

      bool operator < (const iterator& i) const { return _node < i._node; }
      bool operator > (const iterator& i) const { return i < (*this); }
      bool operator <= (const iterator& i) const { return ((*this) < i || (*this) == i); }
      bool operator >= (const iterator& i) const { return ((*this) > i || (*this) == i); }

      iterator& operator = (const iterator& i) { _node = i._node; return (*this); }

      iterator operator + (int i) const { return _node + i; }
      iterator& operator += (int i) { _node += i; return (*this); }

      iterator& operator ++ () { ++_node; return (*this); }                         // prefix ++
      iterator operator ++ (int) { iterator ret(*this); ++(*this); return ret; }    // postfix ++
      iterator& operator -- () { --_node; return (*this); }
      iterator operator -- (int) { iterator ret(*this); --(*this); return ret; }

   private:
      T*    _node;
   };

   iterator begin() const { return iterator(_data); }
   iterator end() const { return iterator(_data + _size); }
   bool empty() const { return (_size == 0); }
   size_t size() const { return _size; }

   T& operator [] (size_t i) { assert(i < _size); return _data[i]; }
   const T& operator [] (size_t i) const { assert(i < _size); return _data[i]; }

   void push_back(const T& x) {
       _isSorted = false;
       if(_size == _capacity)
           assert(extend_size());
       _data[_size++] = x;
   }
   void pop_front() {
       erase(iterator(_data));
   }
   void pop_back() {
       if(empty())
           return;
       --_size;
   }

   bool erase(iterator pos) {
       if(empty())
           return false;

       // won't keep order
       _isSorted = false;
       *(pos._node) = _data[--_size];
       return true;
   }
   bool erase(const T& x) {
       iterator pos = find(x);
       if(pos == end())
           return false;
       return erase(pos);
   }

   iterator find(const T& x) {
       for(iterator it = begin();it < end();++it)
       {
           if(*it == x)
               return it;
       }
       return end();
   }

   void clear() { _size = 0; }

   // DO NOT change ::sort()
   void sort() const {
       if(empty() || _isSorted)
           return;
       ::sort(_data, _data+_size);
       _isSorted = true;
   }

   // Nice to have, but not required in this homework...
   // void reserve(size_t n) { ... }
   // void resize(size_t n) { ... }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   T*            _data;
   size_t        _size;       // number of valid elements
   size_t        _capacity;   // max number of elements
   mutable bool  _isSorted;   // (optionally) to indicate the array is sorted

    bool extend_size(size_t new_cap = 0)
    {
        if(new_cap == 0)
            new_cap = _capacity * 2 + 1;
        if(new_cap < _capacity)
            return false;
        assert(new_cap >= _size);

        T* new_data = new T[new_cap];
        if(new_data == NULL)
            return false;

        _capacity = new_cap;
        for(size_t i = 0;i < _size;++i)
            new_data[i] = _data[i];

        if(_data != NULL)
            delete [] _data;
        _data = new_data;
        return true;
    }

};

#endif // ARRAY_H
