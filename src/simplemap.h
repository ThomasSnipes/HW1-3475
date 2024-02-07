// CSE 375/475 Assignment #1
// Spring 2024
//
// Description: This file specifies a custom map implemented using two vectors.
// << use templates for creating a map of generic types >>
// One vector is used for storing keys. One vector is used for storing values.
// The expectation is that items in equal positions in the two vectors correlate.
// That is, for all values of i, keys[i] is the key for the item in values[i].
// Keys in the map are not necessarily ordered.
//
// The specification for each function appears as comments.
// Students are responsible for implementing the simple map as specified.

#include <vector>
#include <thread>
#include <cassert>
#include <list>
#include <atomic>

// ----- This struct was found online ----- //
template <typename T>
struct atomwrapper
{
  std::atomic<T> _a;

  atomwrapper()
    :_a()
  {}

  atomwrapper(const std::atomic<T> &a)
    :_a(a.load())
  {}

  atomwrapper(const atomwrapper &other)
    :_a(other._a.load())
  {}

  atomwrapper &operator=(const atomwrapper &other)
  {
    _a.store(other._a.load());
    return *this;
  }
};
// ---------------------------------------- //

template <class K, class V, class F>
class simplemap_t {

  public:

    std::vector<K> keys;
    std::vector<V> values;
    std::vector<F> flags;

    // The constructor should just initialize the vectors to be empty
    simplemap_t()  {
        keys.reserve(10000);
        values.reserve(10000);
        flags.reserve(10000);
    }


    // Insert (key, val) if and only if the key is not currently present in
    // the map.  Returns true on success, false if the key was
    // already present.
    bool insert(K key, V val, F flag) {

        for (auto i = keys.begin(); i != keys.end(); ++i){
            if (*i == key){
                return false;
            }
        }

        // insert the key and value.  Note that indices should, by default, match
        // between these two vectors.
        keys.push_back(key);
        values.push_back(val);
        flags.push_back(flag);

        return true;
    }

    // If key is present in the data structure, replace its value with val
    // and return true; if key is not present in the data structure, return
    // false.
    bool update(K key, V val) {
        
        for (auto i = keys.begin(); i != keys.end(); ++i){
            if (*i == key){
                values.push_back(val);
                return true;
            }
        }
        return false;
    }

    // Remove the (key, val) pair if it is present in the data structure.
    // Returns true on success, false if the key was not already present.
    bool remove(K key) {

        auto it = std::remove(keys.begin(), keys.end(), key);

        if (it != keys.end()) {
            size_t index = std::distance(keys.begin(), it);

            // Erase elements from the vector based on the new end iterator
            keys.erase(it, keys.end());
            values.erase(values.begin() + index);
            flags.erase(flags.begin() + index);

            return true;
        }
        
        return false;
    }

    // If key is present in the map, return a pair consisting of
    // the corresponding value and true. Otherwise, return a pair with the
    // boolean entry set to false.
    // Be careful not to share the memory of the map with application threads, you might
    // get unexpected race conditions
    std::pair<V, bool> lookup(K key) {

        for (auto i = keys.begin(); i != keys.end(); ++i){
            if (*i == key){
                size_t index = std::distance(keys.begin(), i);
                return std::make_pair(values[index], true);
            }
        }
        return std::make_pair(0, false);
    }

    // Apply a function to each key in the map
    void apply(void (*f)(K, V)) {
    	for (auto i : keys) {
    		f(i, values.at(i));
    	}
    }
};
