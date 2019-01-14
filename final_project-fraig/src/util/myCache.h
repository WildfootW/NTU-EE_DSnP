/****************************************************************************
  FileName     [ myCache.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashMap and Cache ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2009-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_CACHE_H
#define MY_CACHE_H

using namespace std;

//---------------------
// Define Cache classes
//---------------------
// To use Cache ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//
// class CacheKey
// {
// public:
//    CacheKey() {}
//    size_t operator() () const { return 0; }
//    bool operator == (const CacheKey&) const { return true; }
// private:
// };
//

template <class CacheKey, class CacheData>
class Cache
{
typedef pair<CacheKey, CacheData> CacheNode;

public:
    Cache() : _size(0), _cache(0) {}
    Cache(size_t s) : _size(0), _cache(0) { init(s); }
    ~Cache() { reset(); }

    // NO NEED to implement Cache::iterator class

    // Initialize _cache with size s
    void init(size_t s) { reset(); _size = s; _cache = new CacheNode[s]; }
    void reset() {  _size = 0; if (_cache) { delete [] _cache; _cache = 0; } }

    size_t size() const { return _size; }

    CacheNode& operator [] (size_t i) { return _cache[i]; }
    const CacheNode& operator [] (size_t i) const { return _cache[i]; }

    // return false if cache miss
    bool read(const CacheKey& k, CacheData& d) const
    {
        size_t i = k() % _size;
        if (k == _cache[i].first)
        {
            d = _cache[i].second;
            return true;
        }
        return false;
    }
    // If k is already in the Cache, overwrite the CacheData
    void write(const CacheKey& k, const CacheData& d)
    {
        size_t i = k() % _size;
        _cache[i].first = k;
        _cache[i].second = d;
    }

private:
    size_t         _size;
    CacheNode*     _cache;
};

#endif // MY_CACHE_H
