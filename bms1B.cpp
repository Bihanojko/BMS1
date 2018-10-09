/* 
 * Project 1
 * Subject: Wireless and Mobile Networks
 * Author:  Nikola Valesova
 * Date:    14. 10. 2018
 * Faculty of Information Technology
 * Brno University of Technology
 * File:    bms1B.cpp
 */

#include <cstdlib>
#include <math.h>
#include <iostream>

#include "sndfile.hh"


int main(int argc, char** argv) {
    
    if (argc != 2)
    {
		std::cerr <<  "Invalid argument count! Usage: ./bms1B filename.wav" << std::endl;
		return EXIT_FAILURE;
    }

    std::string filename = argv[1];
    inputFile = SndfileHandle(filename);

    if (filename.find('.') != std::string::npos)
        filename = filename.substr(0, filename.find_last_of('.'));

    ofstream outputFile;
    outputFile.open(filename);

    SndfileHandle inputFile;
    int sampleRate;
    int *buffer;
        
    sampleRate = inputFile.samplerate();
    
    buffer = new int[sampleRate];

    inputFile.read(buffer, sampleRate);
    
    delete [] buffer;
    outputFile.close();
    return EXIT_SUCCESS;
}
