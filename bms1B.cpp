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
#include <cstring>
#include <iostream>
#include <fstream>

#include "sndfile.hh"

#define AMPLITUDE (1.0 * 0x7F000000)
// TODO co to je??? vvvvv
#define BITRATE 1500


std::ofstream CreateOutputFile(std::string filename)
{
    std::ofstream outputFile;

    if (filename.substr(filename.length() - std::strlen(".wav")) == ".wav")
        filename = filename.substr(0, filename.length() - std::strlen(".wav"));

    outputFile.open(filename.c_str());
    return outputFile;
}


int main(int argc, char** argv)
{    
    if (argc != 2)
    {
		std::cerr <<  "Invalid argument count! Usage: ./bms1B filename.wav" << std::endl;
		return EXIT_FAILURE;
    }

    std::string filename = argv[1];
    SndfileHandle inputFile = SndfileHandle(filename);
    std::ofstream outputFile = CreateOutputFile(filename);

    if (!outputFile.is_open())
    {
        std::cerr <<  "Unable to create output file!" << std::endl;
        return EXIT_FAILURE;
    }

    int sampleRate = inputFile.samplerate();
    int* buffer;
        
    sampleRate = inputFile.samplerate();
    
    // buffer = new int[sampleRate];




    // Buffer for the read values
    buffer = new int[sampleRate/BITRATE];
    int syncParts = 4;

    while(inputFile.read(buffer, sampleRate/BITRATE) != 0){
      int partAmp = 0;
      int i = 0;

      // Find the top of the amplitude
      while(!i || partAmp < abs(buffer[i])){
        partAmp = abs(buffer[i]);
        i++;
      }

      // Get the amplitude's value
      int value = -1;
      if(partAmp == 0)
        value = 0;
      else if(partAmp >= AMPLITUDE/10*9)
        value = 3;
      else if(partAmp >= AMPLITUDE/10*6)
        value = 2;
      else if(partAmp >= AMPLITUDE/10*3)
        value = 1;
      else{
        std::cerr << "Unexpected amplitude value retreived!" << std::endl;
        break;
      }

      // Get rid of synchronization sequence
    //   if(syncParts){
    //     if((syncParts%2 == 0 && value) || (syncParts%2 && value != 3)){
    //       std::cerr << "Error in synchronization sequence!" << std::endl;
    //       break;
    //     }
    //     syncParts--;
    //     continue;
    //   }

      std::cout << value/2 << value%2;
    }






    // inputFile.read(buffer, sampleRate);
    
    delete [] buffer;
    outputFile.close();
    return EXIT_SUCCESS;
}
