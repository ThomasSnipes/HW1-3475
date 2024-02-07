// CSE 375/475 Assignment #1
// Spring 2024
//
// Description: This file implements a function 'run_custom_tests' that should be able to use
// the configuration information to drive tests that evaluate the correctness
// and performance of the map_t object.
//
#include <shared_mutex>
#include <iostream>
#include <ctime>
#include "config_t.h"
#include "tests.h"
#include <random>
#include <vector>
#include <thread>
#include <future>
#include <mutex>
#include <algorithm>
#include <omp.h>
#include <fstream>
#include <atomic>
#include "simplemap.h"

	std::mutex lock;

	// Step 1
	// Define a simplemap_t of types <int,float>
	// this map represents a collection of bank accounts:
	// each account has a unique ID of type int;
	// each account has an amount of fund of type float.
	simplemap_t<int, float, atomwrapper<bool>> map;

	
	void printer(int k, float v) {
		std::cout<<"<"<<k<<","<<v<<">"<< std::endl;
	}

	// Generate a random integer
	int rand_int(int min, int max){

		std::random_device rd;
		std::mt19937 gen(rd());

		std::uniform_int_distribution<> rand_dist(min, max);
		int rand_int = rand_dist(gen);

		return rand_int;
	}

	// Generate a random float for an account
	float rand_float(int min, int max){

		std::random_device rd;
		std::mt19937 gen(rd());

		std::uniform_int_distribution<> rand_dist_int(min, max);
		int rand_int = rand_dist_int(gen);

		std::uniform_real_distribution<> rand_dist_float(0.0, 1.0);
		float rand_float = rand_dist_float(gen);

		return rand_int + rand_float;
	}

	// Generate a random float between 0 and 1 for probability
	float rand_float_dep(){

		std::random_device rd;
		std::mt19937 gen(rd());

		std::uniform_real_distribution<> rand_dist(0.0, 1.0);
		float rand_float = rand_dist(gen);

		return rand_float;
	}

	// Step 3
	// Define a function "deposit" that selects two random bank accounts
	// and an amount. This amount is subtracted from the amount
	// of the first account and summed to the amount of the second
	// account. In practice, give two accounts B1 and B2, and a value V,
	// the function performs B1-=V and B2+=V.
	// The execution of the whole function should happen atomically:
	// no operation should happen on B1 and B2 (or on the whole map?)
	// while the function executes.
	void deposit(){

		int val_length = map.values.size();
		
		int B1 = 0;
		int B2 = 0;
		int test = 0;

		// ---------- B1 ---------- //
		bool B1_busy = true;

		while(B1_busy){
			int B1 = rand_int(0, val_length);
			B1_busy = map.flags[B1]._a.load();
		}  
		
		map.flags[B1]._a.store(true);
		// ------------------------ //


		// ---------- B2 ---------- //
		bool B2_busy = true;

		while(B2_busy){
			int B2 = rand_int(0, val_length);
			B2_busy = map.flags[B2]._a.load();
		}  

		
		map.flags[B1]._a.store(true);
		// ------------------------ //

	    float V = rand_float(1, 500);

		map.values[B1] -= V;
		map.values[B2] += V;

		map.flags[B1]._a.store(false);
		map.flags[B2]._a.store(false);
		
	}

	void deposit_one(){

		int val_length = map.values.size();
		

		// ---------- B1 ---------- //
		int B1 = rand_int(0, val_length);
		// ------------------------ //

		// ---------- B2 ---------- //
		int B2 = rand_int(0, val_length);
		// ------------------------ //

		float V = rand_float(1, 500);

		map.values[B1] -= V;
		map.values[B2] += V;

	}

	// Step 4
	// Define a function "balance" that sums the amount of all the
	// bank accounts in the map. In order to have a consistent result,
	// the execution of this function should happen atomically:
	// no other deposit operations should interleave.
	float balance(){
        float total = 0.0;
		std::unique_lock<std::mutex> b_lock(lock, std::defer_lock);

		while(true){
			b_lock.try_lock();
			if (b_lock.owns_lock()){
				
				for (int i=0; i<map.values.size(); ++i){
            		total += map.values.at(i);
        		}
				break;
			}
		}
	
        return total;
	}

	float balance_one(){
        float total = 0.0;
	
		for (int i=0; i<map.values.size(); ++i){
            total += map.values.at(i);
        }
			
        return total;
	}

	// Step 5
	// Define a function 'do_work', which has a for-loop that
	// iterates for config_t.iters times. In each iteration,
	// the function 'deposit' should be called with 95% of the probability;
	// otherwise (the rest 5%) the function 'balance' should be called.
	// The function 'do_work' should measure 'exec_time_i', which is the
	// time needed to perform the entire for-loop. This time will be shared with
	// the main thread once the thread executing the 'do_work' joins its execution
	// with the main thread.
	int do_work(config_t& config){

		// Get start time
		auto start_time = std::chrono::high_resolution_clock::now();

		// Loop config.iters number of times
		for (int i = 0; i < config.iters; ++i) {

			// Move random number generation inside the loop
			float prob = rand_float_dep();
			
			// Call deposit with 95% probability
			if (prob < 0.95) {
				if(config.threads == 1){
					deposit_one();
				} else {
					deposit();
				}
				
			} else {
				if(config.threads == 1){
					balance_one();
				} else {
					balance();
				}
			}
		}
		// Get end time
		auto end_time = std::chrono::high_resolution_clock::now();

		// Calculate total execution time
    	auto exec_time_i = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
		return exec_time_i;
	}


	void run_custom_tests(config_t& cfg) {
		
		// Step 2
		// Populate the entire map with the 'insert' function
		// Initialize the map in a way the sum of the amounts of
		// all the accounts in the map is 100000
		int curr_sum = 0;
		int target_sum = 100000;
		
		int id = 0;
		std::atomic<bool> flag(false);
		
		// Loop until we hit the sum of the accounts is 100,000
		while (curr_sum < target_sum){
			
			// Generate a random number for each key and value 
			int key = id;
			float val = rand_float(1, cfg.key_max);
	
			if (curr_sum + val > target_sum){
				val = target_sum - curr_sum;
			}

			curr_sum += val;

			// Insert the pair
			map.insert(key,val,flag);
			
			// Increment ID number for the key
			id++;
		}
	
		// Step 6
		// The evaluation should be performed in the following way:
		// - the main thread creates #threads threads (as defined in config_t)
		//   << use std:threads >>
		// - each thread executes the function 'do_work' until completion
		// - the (main) spawning thread waits for all the threads to be executed
		//   << use std::thread::join() >>
		//	 and collect all the 'exec_time_i' from each joining thread
		//   << consider using std::future for retrieving 'exec_time_i' after the thread finishes its do_work>>
		// - once all the threads have joined, the function "balance" must be called
		// Make sure evey invocation of the "balance" function returns 100000.

		std::vector<std::future<int>> futures;

		for (int i = 0; i < cfg.threads; ++i) {
			futures.emplace_back(std::async(std::launch::async, do_work, std::ref(cfg)));
		}

		// Step 6: Wait for all threads to finish and collect exec_time_i
		std::vector<int> exec_times;
		for (auto& future : futures) {
			exec_times.push_back(future.get());
		}

		balance();

		// Step 7
		// Now configure your application to perform the SAME TOTAL amount
		// of iterations just executed, but all done by a single thread.
		// Measure the time to perform them and compare with the time
		// previously collected.
		// Which conclusion can you draw?
		// Which optimization can you do to the single-threaded execution in
		// order to improve its performance?
		std::ofstream dataFile("execution_times.dat", std::ios::app);
		for (int i = 0; i < exec_times.size(); ++i) {
			dataFile << cfg.threads << "\t" << exec_times[i] << "\n";
    	}

		// Step 8
		// Remove all the items in the map by leveraging the 'remove' function of the map
		// Destroy all the allocated resources (if any)
		// Execution terminates.
		for (int i=0; i<map.keys.size(); ++i){
            map.remove(map.keys.at(i));
        }


		// Final step: Produce plot
		// I expect each submission to include at least one plot in which
		// the x-axis is the concurrent threads used {1;2;4;8}
		// the y-axis is the application execution time.
		// The performance at 1 thread must be the sequential
		// application without synchronization primitives
		// Save data to a file
		dataFile.close();

	}

void test_driver(config_t &cfg) {
	
	run_custom_tests(cfg);
}
