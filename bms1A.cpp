/* 
 * Project 1
 * Subject: Wireless and Mobile Networks
 * Author:  Nikola Valesova
 * Date:    14. 10. 2018
 * Faculty of Information Technology
 * Brno University of Technology
 * File:    bms1A.cpp
 */

#include <cstdlib>
#include <math.h>
#include <iostream>

#include "sndfile.hh"

#define SAMPLE_RATE 18000
#define CHANELS 1
#define FORMAT (SF_FORMAT_WAV | SF_FORMAT_PCM_24)
#define AMPLITUDE (1.0 * 0x7F000000)
#define FREQ (1000.0 / SAMPLE_RATE)


int main(int argc, char** argv) {

    if (argc != 2)
    {
        std::cerr <<  "Invalid argument count! Usage: ./bms1A filename.txt" << std::endl;
        return EXIT_FAILURE;
    }

    std::string filename = argv[1];

    if (filename.find('.') != std::string::npos)
        filename = filename.substr(0, filename.find_last_of('.'));

    SndfileHandle outputFile;
    int *buffer = new int[SAMPLE_RATE];
    
    for (int i = 0; i < SAMPLE_RATE; i++)
        buffer [i] = AMPLITUDE * sin(FREQ * 2 * i * M_PI);

    outputFile = SndfileHandle(filename + ".waw", SFM_WRITE, FORMAT, CHANELS, SAMPLE_RATE);

    outputFile.write(buffer, SAMPLE_RATE);

    delete [] buffer;
    return EXIT_SUCCESS;
}
