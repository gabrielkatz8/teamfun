#include "SrcMain.h"
#include "HashTable.hpp"
#include "sha1.cpp"
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <ostream>
#include <vector>

#include <time.h>
#include <omp.h>

#define P 2

#pragma GCC diagnostic ignored "-Wsign-compare"

int main()
{
        
        std::string pFile = "common-pass.txt";
        
        std::string ch = "0123456789QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm";

        std::unordered_map<std::string, std::string> h(20);

        std::ifstream ifile(pFile);
        std::string line;
        std::vector<std::pair<std::string, std::string>> solved;
        
        int wordNum = 1;
        std::vector<std::string> passcodes;
        while (getline(ifile, line)) {
        	passcodes.push_back(line);
        }

        // measure the start time here
		struct timespec start, stop; 
		double time;
		if( clock_gettime(CLOCK_REALTIME, &start) == -1) { perror("clock gettime");}

        for (size_t i = 0; i < passcodes.size(); i++) {
        	unsigned char arr[20];
        	sha1::Calc(passcodes[i].c_str(), passcodes[i].length(), arr);
        	std::string hexString;
        	hexString.resize(40);
        	sha1::ToHexString(arr, &hexString[0]);
        	h.insert({hexString, passcodes[i]});
	        
	        std::vector<int> indices(1);
	        indices[0] = 0;
	        
	        

	        bool done = false;
	        while (!done) {
	        	
	        		std::string attempt = "";
	        		for (size_t i = 0; i < indices.size()-1; i++) {
	        			attempt += ch[indices[i]];
	        		}
	        		//parallelize checking symbols
	        		omp_set_num_threads(P);
					#pragma omp parallel for schedule(static)
	        		for (size_t i = 0; i < ch.size(); i++) {
	        			std::string temp_attempt = attempt + ch[i];

		        		//uncomment line below for animation
		        		//std::cout << "\r" << temp_attempt;
		        		//std::cout << wordNum << " " << attempt << std::endl;
		        		unsigned char hash[20];
			        	sha1::Calc(temp_attempt.c_str(), temp_attempt.length(), hash);
			        	std::string hexStr;
			        	hexStr.resize(40);
			        	sha1::ToHexString(hash, &hexStr[0]);
			        	if (h.find(hexStr) != h.end()) {
			        		
			        		std::pair<std::string, std::string> newPair = std::make_pair(temp_attempt, hexStr);
			        		solved.push_back(newPair);
			        		h.erase(hexStr);
			        		done = true;
			        	}

			        	int tid = omp_get_thread_num();
			        	if (tid == P-1) {
			        		//set last value in indices so that we go into next if statement
			        		indices[indices.size()-1] = ch.size();
			        	}
	        		}
	        		
		        	
		        	if (indices[indices.size() - 1] == ch.size()) {

		        		indices[indices.size() - 1] = 0;
		        		bool finished = false;
		        		int cnt = 2;
		        		while (!finished) {
		        			
		        			if (static_cast<int>(indices.size()) - cnt < 0) {
		        				
		        				finished = true;
		        				
		        				indices.push_back(0);
		        			}
		        			else if (indices[indices.size() - cnt] == ch.size() - 1) {
		        				
		        				indices[indices.size() - cnt] = 0;
		        				cnt++;
		        			}
		        			else {
		        				
		        				indices[indices.size() - cnt]++;
		        				finished = true;
		        			}
		        		}
		        	}
	        	
	        }
	        wordNum++;
	    }

	    // measure the end time here
		if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) { perror("clock gettime");}		
		time = (stop.tv_sec - start.tv_sec)+ (double)(stop.tv_nsec - start.tv_nsec)/1e9;
	
		// print out the execution time here
		printf("Execution time = %f sec\n", time);
        
        std::ofstream ofile("solved_ParallelSym.txt");
        
        unsigned int solvedSize = solved.size();
        for (unsigned int i = 0; i < solvedSize; i++) {
            ofile << solved[i].first << "," << solved[i].second << "\n";
        }

         //put execution time in file
        ofile << "Execution time = " << time << " sec\n";
        
        ofile.close();
        
    
}
