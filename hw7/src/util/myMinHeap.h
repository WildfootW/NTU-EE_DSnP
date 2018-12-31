/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
public:
    MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s + 1); _data.push_back(Data()); }
    ~MinHeap() {}

    void clear() { _data.erase(_data.begin() + 1, _data.end()); }

    size_t size() const { return _data.size() - 1; }
    bool empty() const { return size() == 0; }

    // For the following member functions,
    // We don't respond for the case vector "_data" is empty!
    const Data& operator [] (size_t i) const { return _data[i + 1]; }
    Data& operator [] (size_t i) { return _data[i + 1]; }

    const Data& min() const { return size() ? _data[1] : _data[0]; } // TODO if size() == 0

    void insert(const Data& d)
    {
        _data.push_back(d);
        size_t current = size();
        while(parent(current) != 0)
        {
            if(_data[current] < _data[parent(current)])
            {
                iter_swap(_data.begin() + current, _data.begin() + parent(current));
                current = parent(current);
            }
            else
                break;
        }
    }
    void delMin() { delData(0); }
    void delData(size_t i)
    {
        if(size() <= i)
            return;

        size_t current = i + 1;

        iter_swap(_data.begin() + current, _data.end() - 1);
        _data.pop_back();


        // if have rchild and rchild is smaller, swap
        while(lchild(current) < _data.size())
        {
            size_t child = lchild(current);

            // if have rchild and rchild is smaller than lchild, child = rchild
            if(rchild(current) < _data.size())
            {
                if(_data[rchild(current)] < _data[lchild(current)])
                    child = rchild(current);
            }

            if(_data[child] < _data[current])
            {
                iter_swap(_data.begin() + child, _data.begin() + current);
                current = child;
            }
            else
                break;
        }
    }

#ifndef NDEBUG
    void print_all() const
    {
        for(size_t i = 0;i < _data.size();++i)
        {
            cout << i << " : " << _data[i] << endl;
        }
    }
#endif // NDEBUG

private:
    // DO NOT add or change data members
    vector<Data>   _data;   // _data[0] is dummy node;

    inline static size_t parent(const size_t& x) { return x / 2; }
    inline static size_t lchild(const size_t& x) { return x * 2; }
    inline static size_t rchild(const size_t& x) { return x * 2 + 1; }
};

#endif // MY_MIN_HEAP_H
