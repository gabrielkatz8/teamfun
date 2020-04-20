#include "sha1.h"
#include "sha1_parallel.h"
#include <vector>
#include <string>
#include <time.h>

using namespace std;

int main(void) {
    //get list of strings to hash
    vector<string> strings_to_hash;

    //initialize timing variables to compare the times of the serial and parallel algorithms
    struct timespec start, stop;
    double time;

    for (unsigned int i = 0; i < strings_to_hash.size(); i++) {
        string s = strings_to_hash[i];
        
        //test serial time and print results
        if(clock_gettime(CLOCK_REALTIME, &start) == -1) { perror("clock gettime");}
        unsigned char hash[20];
        sha1::Calc(s.c_str(), s.length(), hash);
        if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) { perror("clock gettime");}		
		time = (stop.tv_sec - start.tv_sec)+ (double)(stop.tv_nsec - start.tv_nsec)/1e9;
        printf("Execution time = %f sec\n", time);	

        //test parallel time and print results
        if(clock_gettime(CLOCK_REALTIME, &start) == -1) { perror("clock gettime");}
        //EXECUTION
        if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) { perror("clock gettime");}		
		time = (stop.tv_sec - start.tv_sec)+ (double)(stop.tv_nsec - start.tv_nsec)/1e9;
        printf("Execution time = %f sec\n", time);	
    }
}