all: SrcMainSerial ParallelPass ParallelSym sha-test

SrcMainSerial: SrcMainSerial.cpp sha1.h sha1.cpp
	g++ -g -Wall -std=c++11 SrcMainSerial.cpp sha1.h -o SrcMainSerial

sha-test: main_sha.cpp sha1.h sha1.cpp sha1_parallel.h sha1_parallel.cpp
	g++ -g -Wall -std=c++11 main_sha.cpp sha1.h sha1.cpp sha1_parallel.h sha1_parallel.cpp -o sha-test -fopenmp

ParallelPass: ParallelPass.cpp sha1.h sha1.cpp
	g++ -g -Wall -std=c++11 ParallelPass.cpp sha1.h -o ParallelPass -fopenmp

ParallelSym: ParallelSym.cpp sha1.h sha1.cpp
	g++ -g -Wall -std=c++11 ParallelSym.cpp sha1.h -o ParallelSym -fopenmp

clean: 
	rm -f *.o SrcMainSerial ParallelPass ParallelSym sha-test