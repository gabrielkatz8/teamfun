/*
 Copyright (c) 2011, Micael Hildenborg
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of Micael Hildenborg nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY Micael Hildenborg ''AS IS'' AND ANY
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL Micael Hildenborg BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 Contributors:
 Gustav
 Several members in the gamedev.se forum.
 Gregory Petrosyan
 */

/*
* dcharters12: editing the original file to allow for a parallel implementation
*/

#include "sha1_parallel.h"
#include <omp.h>

namespace sha1_parallel
{
    namespace // local
    {
        // Rotate an integer value to left.
        inline const unsigned int Rol(const unsigned int value,
                const unsigned int steps)
        {
            return ((value << steps) | (value >> (32 - steps)));
        }

        // Sets the first 16 integers in the buffert to zero.
        // Used for clearing the W buffert.
        inline void ClearWBuffert(unsigned int* buffert)
        {
            for (int pos = 16; --pos >= 0;)
            {
                buffert[pos] = 0;
            }
        }

        void InnerHash(unsigned int* result, unsigned int* w)
        {
            unsigned int a = result[0];
            unsigned int b = result[1];
            unsigned int c = result[2];
            unsigned int d = result[3];
            unsigned int e = result[4];

            int round = 0;

            #define sha1macro(func,val) \
			{ \
                const unsigned int t = Rol(a, 5) + (func) + e + val + w[round]; \
				e = d; \
				d = c; \
				c = Rol(b, 30); \
				b = a; \
				a = t; \
			}

            while (round < 16)
            {
                sha1macro((b & c) | (~b & d), 0x5a827999)
                ++round;
            }
            while (round < 20)
            {
                w[round] = Rol((w[round - 3] ^ w[round - 8] ^ w[round - 14] ^ w[round - 16]), 1);
                sha1macro((b & c) | (~b & d), 0x5a827999)
                ++round;
            }
            while (round < 40)
            {
                w[round] = Rol((w[round - 3] ^ w[round - 8] ^ w[round - 14] ^ w[round - 16]), 1);
                sha1macro(b ^ c ^ d, 0x6ed9eba1)
                ++round;
            }
            while (round < 60)
            {
                w[round] = Rol((w[round - 3] ^ w[round - 8] ^ w[round - 14] ^ w[round - 16]), 1);
                sha1macro((b & c) | (b & d) | (c & d), 0x8f1bbcdc)
                ++round;
            }
            while (round < 80)
            {
                w[round] = Rol((w[round - 3] ^ w[round - 8] ^ w[round - 14] ^ w[round - 16]), 1);
                sha1macro(b ^ c ^ d, 0xca62c1d6)
                ++round;
            }

            #undef sha1macro

            result[0] += a;
            result[1] += b;
            result[2] += c;
            result[3] += d;
            result[4] += e;
        }
    } // namespace

    void Calc(const void* src, size_t bytelength, unsigned char* hash)
    {
        // Init the result array.
        unsigned int result[5] = { 0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0 };

        // Cast the void src pointer to be the byte array we can work with.
        const unsigned char* sarray = reinterpret_cast<const unsigned char*>(src);

        // The reusable round buffer
        unsigned int w[80];

        // Loop through all complete 64byte blocks.
        const int endOfFullBlocks = static_cast<int>(bytelength) - 64;
        int endCurrentBlock;
        int currentBlock = 0;
        int currentBlock_temp = currentBlock; //allows initialization of currentBlock to satisfy OpenMP constraints

        //set the number of threads
        omp_set_num_threads(2);

        while (currentBlock <= endOfFullBlocks)
        {
            endCurrentBlock = currentBlock + 64;

            // Init the round buffer with the 64 byte block data.
            int roundPos = 0;
            #pragma omp parallel for
            for (currentBlock = currentBlock_temp; currentBlock < endCurrentBlock; currentBlock += 4)
            {
                // This line will swap endian on big endian and keep endian on little endian.
                w[roundPos++] = static_cast<unsigned int>(sarray[currentBlock + 3])
                        | ((static_cast<unsigned int>(sarray[currentBlock + 2]) << 8))
                        | ((static_cast<unsigned int>(sarray[currentBlock + 1]) << 16))
                        | ((static_cast<unsigned int>(sarray[currentBlock]) << 24));
            }
            currentBlock_temp = currentBlock;
            InnerHash(result, w);
        }

        // Handle the last and not full 64 byte block if existing.
        endCurrentBlock = static_cast<int>(bytelength) - currentBlock;
        ClearWBuffert(w);
        int lastBlockBytes = 0;
        #pragma omp parallel for
        for (lastBlockBytes=0; lastBlockBytes < endCurrentBlock; ++lastBlockBytes)
        {
            w[lastBlockBytes >> 2] |= static_cast<unsigned int>(sarray[lastBlockBytes + currentBlock]) << ((3 - (lastBlockBytes & 3)) << 3);
        }
        w[lastBlockBytes >> 2] |= 0x80 << ((3 - (lastBlockBytes & 3)) << 3);
        if (endCurrentBlock >= 56)
        {
            InnerHash(result, w);
            ClearWBuffert(w);
        }
        w[15] = static_cast<int>(bytelength) << 3;
        InnerHash(result, w);

        // Store hash in result pointer, and make sure we get in in the correct order on both endian models.
        #pragma omp parallel for
        for (int hashByte = 20; hashByte >= 0; --hashByte)
        {
            hash[hashByte] = (result[hashByte >> 2] >> (((3 - hashByte) & 0x3) << 3)) & 0xff;
        }
    }

    void ToHexString(const unsigned char* hash, char* hexstring)
    {
        const char hexDigits[] = { "0123456789abcdef" };

        for (int hashByte = 20; --hashByte >= 0;)
        {
            hexstring[hashByte << 1] = hexDigits[(hash[hashByte] >> 4) & 0xf];
            hexstring[(hashByte << 1) + 1] = hexDigits[hash[hashByte] & 0xf];
        }
        hexstring[40] = 0;
    }
} // namespace sha1
