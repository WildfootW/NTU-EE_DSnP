/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   // [NOTE] DO NOT ADD or REMOVE any data member
   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   DList() : _isSorted(true) {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      iterator& operator = (const iterator& i) { this->_node = i._node; return (*this); }

      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () { (*this) = iterator(this->_node->_next); return (*this); }      // prefix
      iterator operator ++ (int) { iterator ret(*this); ++(*this); return ret; }    // postfix
      iterator& operator -- () { (*this) = iterator(this->_node->_prev); return (*this); }
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

      bool operator == (const iterator& i) const { return (this->_node == i._node); }
      bool operator != (const iterator& i) const { return !((*this) == i); }

   private:
      DListNode<T>* _node;
   };

   iterator begin() const {
//       if(empty())
//           return NULL;
       return end() + 1;
   }
   iterator end() const { return iterator(_head); }
   iterator back() const {
//       if(empty())
//           return NULL;
       return end() - 1;
   }

   bool empty() const { return (_head == _head->_next); }
   size_t size() const {
       if(empty())
           return 0;
       size_t count = 0;
       for(iterator i = begin();i != end();++i)
           ++count;
       return count;
   }

   void push_back(const T& x) {
       DListNode<T>* new_node = new DListNode<T>(x, back()._node, end()._node);
       back()._node->_next = new_node;
       end()._node->_prev = new_node;
       _isSorted = false;
   }
   void pop_front() { erase(begin()); }
   void pop_back() { erase(back()); }
   void clear() { erase(begin(), back()); }  // delete all nodes except for the dummy node

   // return false if nothing to erase
   bool erase(iterator pos) { return erase(pos, pos); }
   bool erase(const T& x) {
       iterator it = find(x);
       //clog << "it: " << *it << endl;
       if(it == end())
           return false;
       return erase(it);
   }

   iterator find(const T& x) {
       iterator ret = begin();
       do
       {
           if(*ret == x)
               return ret;
           ++ret;
       }
       while(ret != end());
       return end();
   }

   void sort() const {
       if(empty() || _isSorted)
           return;
       for(iterator i = end();i != back();++i)
       {
           bool done = true;
           for(iterator j = end();i != j - 2;--j)
           {
               //clog << *(j - 2) << " " << *(j - 1) << endl;
               if(*(j - 2) > *(j - 1))
               {
                   done = false;
                   swap_node(j - 2, j - 1);
               }
           }
           if(done)
               break;
       }
       _isSorted = true;
   }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

    bool check_lhs_less_rhs(iterator lhs, iterator rhs)
    {
        //clog << "lhs: " << *lhs << " rhs: " << *rhs << endl;
        if(lhs == end() || rhs == end())
            return false;
        while(lhs != end())
        {
            if(lhs++ == rhs)
                return true;
        }
        return false;
    }
    bool erase(iterator lhs, iterator rhs)  // erase lhs to rhs (include lhs and rhs)
    {
        //clog << "erase: " << "lhs: " << *lhs << " rhs: " << *rhs << endl;
        if(empty())     // return false if nothing to erase
            return false;
        assert(check_lhs_less_rhs(lhs, rhs)); // command(q -f) would call erase(end(), end()) while empty()
        iterator prev = lhs - 1;
        iterator next = rhs + 1;
        prev._node->_next = next._node;
        next._node->_prev = prev._node;

        while(lhs != rhs)
        {
            iterator tmp = lhs;
            ++lhs;
            delete tmp._node;
        }
        delete rhs._node;
        return true;
    }
    bool swap_node(iterator lhs, iterator rhs) const
    {
        //clog << "swap_node: " << *lhs << " " << *rhs << endl;
        assert(lhs != end());
        assert(rhs != end());

        // WTF is this
        //swap(lhs._node->_data, rhs._node->_data);
        //return true;

        // normal dlink implement
        if(rhs + 1 == lhs)
            swap(lhs, rhs);
        if(lhs + 1 == rhs)  // lhs is near to rhs
        {
            // prev -> lhs -> rhs -> next
            // prev -> rhs -> lhs -> next
            DListNode<T>* prev = lhs._node->_prev;
            DListNode<T>* next = rhs._node->_next;
            prev->_next = rhs._node;
            rhs._node->_prev = prev;
            rhs._node->_next = lhs._node;
            lhs._node->_prev = rhs._node;
            lhs._node->_next = next;
            next->_prev = lhs._node;
        }
        else
        {
            // lprev -> lhs -> lnext -> ... -> rprev -> rhs -> rnext
            // lprev -> rhs -> lnext -> ... -> rprev -> lhs -> rnext
            DListNode<T>* lprev = lhs._node->_prev;
            DListNode<T>* lnext = lhs._node->_next;
            DListNode<T>* rprev = rhs._node->_prev;
            DListNode<T>* rnext = rhs._node->_next;
            lprev->_next = rhs._node;
            rhs._node->_prev = lprev;
            rhs._node->_next = lnext;
            lnext->_prev = rhs._node;
            rprev->_next = lhs._node;
            lhs._node->_prev = rprev;
            lhs._node->_next = rnext;
            rnext->_prev = lhs._node;
        }
        return true;
    }
};

#endif // DLIST_H
