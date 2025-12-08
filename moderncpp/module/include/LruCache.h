#pragma once
#include <unordered_map>
#include <list>
#include <optional>

using namespace std; 



template <typename KeyType, typename ValueType, size_t N = 1000>
struct LruCache 

{


    void add(const KeyType& key, const ValueType& value)
    {

        if(size_ >= N)
        {
            //cache is full , evict the last used
            auto backEl = listCache.back(); 
            listCache.pop_back(); 
            lru_cache_.erase(backEl.key_);
            --size_;
        }
        auto it = lru_cache_.find(key); 
        if(it != lru_cache_.end())
        {
            // update the list , so that it becomes the top of the list, latest at front 
            //oldest at back 
            listCache.splice(listCache.begin(), listCache, it->second); 
            
            (it->second)->val_ = value;
            return; 
        }
        // not found ,  insert at front
        listCache.emplace_front(CacheNode{key, value});
        lru_cache_.insert(make_pair(key, listCache.begin())); 
        size_++; 
    }

    optional<ValueType> get(const KeyType& key)
    {
        auto it = lru_cache_.find(key);
        if (it == lru_cache_.end())
        {
            return nullopt; 
        }
        //found 
        //map has the value as the iterator of list where element is present
        //update the list so that this element moves to front

        listCache.splice(listCache.begin(), listCache, it->second);
        return (it->second)->val_;
    }

    private:
    struct CacheNode
    {
        KeyType key_;
        ValueType val_; 
        CacheNode(const KeyType& key, const ValueType& value): key_{key}, val_{value}
        {

        }
    }; 
    list<CacheNode> listCache; 
    using IteratorType =   typename list<CacheNode>::iterator;
    unordered_map<KeyType, IteratorType> lru_cache_; 
    size_t size_{0}; 
}; 