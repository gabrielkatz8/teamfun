#include "sha1.h"
#include "sha1_parallel.h"
#include <vector>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>

using namespace std;

#define ARRAY_LENGTH 5
string strings_to_hash[5] = {
    "ghtysjkadkjf",
    "lksdfnkjsdbjkfanlksdklfnjklasdlfhjklasdfhkajlsdckjbasdjkfghjkascgaldsfhkjasdjkfhcjksadzbcjkbdfghjk",
    "alkdhfkj",
    "asodhjioasdhasklcfkjdsajfhjklsdafasdlfhkjnsadcjkbasjkdfhkjSDFGUIG",
    "lkhjIBFDWUI%&^%@kjhgfdbk"
};

int main(void) {
    //initialize timing variables to compare the times of the serial and parallel algorithms
    struct timespec start, stop;
    double time_serial, time_parallel;

    printf("BEGIN SHA-TEST:\n\n");

    for (unsigned int i = 0; i < ARRAY_LENGTH; i++) {
        string s = strings_to_hash[i];
        unsigned char hash_serial[20];
        unsigned char hash_parallel[20];
        printf("****************\nTEST #%d   string=  %s\n****************\n\n", i, s.c_str());
        
        //test serial time
        if(clock_gettime(CLOCK_REALTIME, &start) == -1) { perror("clock gettime");}
        sha1::Calc(s.c_str(), s.length(), hash_serial);
        if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) { perror("clock gettime");}		
		time_serial = (stop.tv_sec - start.tv_sec)+ (double)(stop.tv_nsec - start.tv_nsec)/1e9;	

        //test parallel time
        if(clock_gettime(CLOCK_REALTIME, &start) == -1) { perror("clock gettime");}
        sha1_parallel::Calc(s.c_str(), s.length(), hash_parallel);
        if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) { perror("clock gettime");}		
		time_parallel = (stop.tv_sec - start.tv_sec)+ (double)(stop.tv_nsec - start.tv_nsec)/1e9;

        //format the results
        char* serial_formatted, parallel_formatted;
        sha1::ToHexString((const unsigned char*) hash_serial, serial_formatted);
        sha1::ToHexString((const unsigned char*) hash_parallel, parallel_formatted);

        //print the results
        printf("Serial hash:   %s\nParallel hash: %s\n", serial_formatted, parallel_formatted);
        //printf("Serial hash:   %s\n", hash_serial);
        printf("Serial time:   %f sec\nParallel time: %f sec\n\n\n", time_serial, time_parallel);
    }
}