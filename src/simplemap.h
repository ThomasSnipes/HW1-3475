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
#include <mutex>


template <class K, class V>
class simplemap_t {

  public:

    // Define the two vectors of types K and V
	// << use std::vector<K> >>
    std::vector<K> keys;
    std::vector<V> values;
    //std::vector<std::mutex> lockStripes;

    // The constructor should just initialize the vectors to be empty
    simplemap_t(): keys(), values() {
        //const size_t numLockStripes = 16;
        //lockStripes.resize(numLockStripes);
        //assert(keys.empty());
        //assert(values.empty());
    }


    // Insert (key, val) if and only if the key is not currently present in
    // the map.  Returns true on success, false if the key was
    // already present.
    bool insert(K key, V val) {
        //std::lock_guard<std::mutex> lock(getLock(key));

        for (auto i = keys.begin(); i != keys.end(); ++i){
            if (*i == key){
                return false;
            }
        }
          
        // insert the key and value.  Note that indices should, by default, match
        // between these two vectors.
        keys.push_back(key);
        values.push_back(val);
        return true;
    }

    // If key is present in the data structure, replace its value with val
    // and return true; if key is not present in the data structure, return
    // false.
    bool update(K key, V val) {
        //std::lock_guard<std::mutex> lock(getLock(key));

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
        //std::lock_guard<std::mutex> lock(getLock(key));

        for (auto i = keys.begin(); i != keys.end(); ++i){
            if (*i == key){
                size_t index = std::distance(keys.begin(), i);
                keys.erase(key);
                values.erase(values[index]);
                return true;
            }
        }
        return false;
    }

    // If key is present in the map, return a pair consisting of
    // the corresponding value and true. Otherwise, return a pair with the
    // boolean entry set to false.
    // Be careful not to share the memory of the map with application threads, you might
    // get unexpected race conditions
    std::pair<V, bool> lookup(K key) {
        //std::lock_guard<std::mutex> lock(getLock(key));

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

    // Step 4
	// Define a function "balance" that sums the amount of all the
	// bank accounts in the map. In order to have a consistent result,
	// the execution of this function should happen atomically:
	// no other deposit operations should interleave.
	float balance(){
        float total = 0.0;

		
        return total;
	}

    // private:
    //     bool containsKey(K key) {
    //         return std::find(keys.begin(), keys.end(), key) != keys.end();
    //     }

    //     typename std::vector<K>::iterator findKey(K key) {
    //         return std::find(keys.begin(), keys.end(), key);
    //     }

    //     std::mutex& getLock(K key) {
    //         // Use a hash function to determine the lock stripe
    //         size_t stripeIndex = std::hash<K>{}(key) % lockStripes.size();
    //         return lockStripes[stripeIndex];
    //     }
};
