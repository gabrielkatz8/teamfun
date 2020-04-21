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

#define P 4

#pragma GCC diagnostic ignored "-Wsign-compare"

int main()
{
        //std::string phrasesFile = argv[3];
        std::string pFile = "test-hex.txt";
        
        std::string ch = "0123456789QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm";

        std::unordered_map<std::string, std::string> h(20);

        std::ifstream ifile(pFile);
        std::string line;
        std::vector<std::pair<std::string, std::string>> solved;
        //int numPasswords = 0;
        //std::vector<std::string> words;

       
        	//h[hexStr] = line;
        	//numPasswords++;
        	//words.push_back(hexStr);
    
        
        // std::ifstream phraseFile(phrasesFile);
        // std::string p;
        // std::vector<std::string> allPhrases;
        // while (static_cast<bool>(getline(phraseFile, p))) {
        //     allPhrases.emplace_back(p);
        // }
        
        // std::ifstream ifile(pFile);
        // std::string line;
        // int i = 0;
        
        // while (static_cast<bool>(getline(ifile, line))) {
            
        //     h.Insert(line, i);
        //     i++;
        // }
        // ifile.close();
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
        	//unsigned char arr[20];
        	//sha1::Calc(passcodes[i].c_str(), passcodes[i].length(), arr);
        	//std::string hexString;
        	//hexString.resize(40);
        	//sha1::ToHexString((unsigned char*)(passcodes[i].c_str()), &hexString[0]);
        	h.insert({passcodes[i], passcodes[i]});
        	//std::cout << words[i] <<  " " << h[words[i]] << std::endl;
	        
	        std::vector<int> indices(1);
	        indices[0] = 0;
	        
	        // std::string fourPhrases;

	        // tbb::parallel_for_each(allPhrases.begin(), allPhrases.end(), [&solved, &h, &allPhrases](std::string word1) {
	        //     tbb::parallel_for_each(allPhrases.begin(), allPhrases.end(), [&solved, &h, &allPhrases, &word1](std::string word2) {
	        //         tbb::parallel_for_each(allPhrases.begin(), allPhrases.end(), [&solved, &h, &allPhrases, &word1, &word2](std::string word3) {
	        //             unsigned int allPhrasesSize = allPhrases.size();
	        //             std::string test;
	        //             std::string hexStr;
	        //             int* val;
	        //             unsigned char hash[20];
	        //             unsigned int j;
	        //             for (j = 0; j < allPhrasesSize; j++) {
	        //                 test = word1 + word2 + word3 + allPhrases[j];

	        //                 sha1::Calc(test.c_str(), test.length(), hash);

	                        
	        //                 hexStr.resize(40);
	        //                 sha1::ToHexString(hash, &hexStr[0]);
	        //                 val = h.Find(hexStr);
	        //                 if (val != nullptr) {
	        //                     solved[*val].first = hexStr;
	        //                     solved[*val].second = test;
	        //                 }
	        //             }
	        //         //}
	        //         });
	        //     });
	        // });

	        bool done = false;
	        while (!done) {
	        	// if (numPasswords == 0) {
	        	// 	done = true;
	        	// }
	        	// else {
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
	        		
		        	//indices[indices.size() - 1]++;
		        	if (indices[indices.size() - 1] == ch.size()) {

		        		indices[indices.size() - 1] = 0;
		        		bool finished = false;
		        		int cnt = 2;
		        		while (!finished) {
		        			
		        			if (static_cast<int>(indices.size()) - cnt < 0) {
		        				
		        				finished = true;
		        				// for (int i = 0; i < indices.size(); i++) {
		        				// 	indices[i] = 0;
		        				// }
		        				//std::cout << "here " << attempt << std::endl;
		        				//if (attempt == "mm") std::cout << 1 << std::endl;
		        				indices.push_back(0);
		        			}
		        			else if (indices[indices.size() - cnt] == ch.size() - 1) {
		        				//if (attempt == "mm") std::cout << 2 << std::endl;
		        				indices[indices.size() - cnt] = 0;
		        				cnt++;
		        			}
		        			else {
		        				//if (attempt == "mm") std::cout << 3 << std::endl;
		        				indices[indices.size() - cnt]++;
		        				finished = true;
		        			}
		        		}
		        	}
	        	//}
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
        
    //}
    
}
