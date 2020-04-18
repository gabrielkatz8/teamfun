#include "SrcMain.h"
#include "HashTable.hpp"
#include "sha1.h"
#include <tbb/parallel_for_each.h>
#include <iostream>
#include <fstream>
#include <ostream>
#include <vector>

void ProcessCommandArgs(int argc, const char* argv[])
{
	// TODO: Implement
    std::string type = argv[1];
    int size = atoi(argv[2]);
    if (type == "dictionary") {
        HashTable<std::string> h(size);
        std::string dictFile = argv[3];
        std::string passFile = argv[4];
        
        std::ifstream ifile(dictFile);
        std::string line;
        while (static_cast<bool>(getline(ifile, line))) {
            unsigned char hash[20];
            sha1::Calc(line.c_str(), line.length(), hash);
            
            std::string hexStr;
            hexStr.resize(40);
            sha1::ToHexString(hash, &hexStr[0]);
            
            h.Insert(hexStr, line);
        }
        ifile.close();
        
        std::ifstream pfile(passFile);
        std::ofstream ofile("solved.txt");
        std::string word;
        while (static_cast<bool>(getline(pfile, word))) {
            ofile << word << "," << *h.Find(word) << "\n";
        }
        pfile.close();
        ofile.close();
    }
    else if (type == "phrases") {
        std::string phrasesFile = argv[3];
        std::string pFile = argv[4];
        
        HashTable<int> h(size);
        
        std::ifstream phraseFile(phrasesFile);
        std::string p;
        std::vector<std::string> allPhrases;
        while (static_cast<bool>(getline(phraseFile, p))) {
            allPhrases.emplace_back(p);
        }
        
        std::ifstream ifile(pFile);
        std::string line;
        int i = 0;
        
        while (static_cast<bool>(getline(ifile, line))) {
            
            h.Insert(line, i);
            i++;
        }
        ifile.close();
        
        std::vector<std::pair<std::string, std::string>> solved(h.Size());
        
        std::string fourPhrases;

        tbb::parallel_for_each(allPhrases.begin(), allPhrases.end(), [&solved, &h, &allPhrases](std::string word1) {
            tbb::parallel_for_each(allPhrases.begin(), allPhrases.end(), [&solved, &h, &allPhrases, &word1](std::string word2) {
                tbb::parallel_for_each(allPhrases.begin(), allPhrases.end(), [&solved, &h, &allPhrases, &word1, &word2](std::string word3) {
                    unsigned int allPhrasesSize = allPhrases.size();
                    std::string test;
                    std::string hexStr;
                    int* val;
                    unsigned char hash[20];
                    unsigned int j;
                    for (j = 0; j < allPhrasesSize; j++) {
                        //test = word1 + word2 + allPhrases[k] + allPhrases[j];
                        test = word1 + word2 + word3 + allPhrases[j];

                        sha1::Calc(test.c_str(), test.length(), hash);

                        
                        hexStr.resize(40);
                        sha1::ToHexString(hash, &hexStr[0]);
                        val = h.Find(hexStr);
                        if (val != nullptr) {
                            solved[*val].first = hexStr;
                            solved[*val].second = test;
                        }
                    }
                //}
                });
            });
        });
        
        std::ofstream ofile("solved.txt");
        
        unsigned int solvedSize = solved.size();
        for (unsigned int i = 0; i < solvedSize; i++) {
            ofile << solved[i].first << "," << solved[i].second << "\n";
        }
        
        ofile.close();
        
    }
    
}
