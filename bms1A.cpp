/* 
 * Project 1
 * Subject: Wireless and Mobile Networks
 * Author:  Nikola Valesova
 * Date:    3. 11. 2018
 * Faculty of Information Technology
 * Brno University of Technology
 * File:    bms1A.cpp
 */

#include <cstdlib>
#include <math.h>
#include <cstring>
#include <iostream>
#include <fstream>

#include "sndfile.hh"

#define SAMPLE_RATE 18000
#define CHANELS 1
#define FORMAT (SF_FORMAT_WAV | SF_FORMAT_PCM_24)
#define AMPLITUDE (1.0 * 0x7F000000)
#define FREQ (1000.0 / SAMPLE_RATE)

// number of modulated values used to represent one digram
#define SAMPLES_PER_DIGRAM (SAMPLE_RATE / 600)


// create and open output file
SndfileHandle CreateOutputFile(std::string filename)
{
    SndfileHandle outputFile;

    if (filename.substr(filename.length() - std::strlen(".txt")) == ".txt")
        filename = filename.substr(0, filename.length() - std::strlen(".txt"));
    else
    {
        std::cerr <<  "Invalid input filename! Expecting a file ending with \'.txt\'." << std::endl;
        exit(EXIT_FAILURE);
    }

    return SndfileHandle(filename + ".wav", SFM_WRITE, FORMAT, CHANELS, SAMPLE_RATE);
}


// create synchronization sequence and return it
int* CreateSynchSequence(int& index)
{
    int* buffer = new int[SAMPLES_PER_DIGRAM * 4];

    // modulate synchronization sequence "0011"
    for (int i = 0; i < 2; ++i)
    {
        // modulate "00"
        for (int j = 0; j < SAMPLES_PER_DIGRAM; ++j)
        {
            buffer[index] = 0.0;
            ++index;
        }

        // modulate "11"
        for (int j = 0; j < SAMPLES_PER_DIGRAM; ++j)
        {
            buffer[index] = AMPLITUDE * sin(FREQ * 2 * index * M_PI);
            ++index;
        }
    }

    return buffer;
}


// create modulated sequence for current digram
int* ApplyModulation(int signalStrength, int& index, int* buffer)
{
    for (int i = 0; i < SAMPLES_PER_DIGRAM; ++i, ++index)
        buffer[i] = signalStrength * sin(FREQ * 2 * index * M_PI);

    return buffer;
}


// MAIN
int main(int argc, char** argv)
{
    // check the number of arguments
    if (argc != 2)
    {
        std::cerr <<  "Invalid argument count! Usage: ./bms1A filename.txt" << std::endl;
        return EXIT_FAILURE;
    }

    // open input file
    std::string filename = argv[1];
    std::ifstream inputFile;
    inputFile.open(filename.c_str());

    // check if input file was opened successfully
    if (!inputFile.is_open())
    {
        std::cerr <<  "Unable to open input file: " << filename << std::endl;
        return EXIT_FAILURE;
    }

    int index = 0;
    // modulate synchronization sequence
    int* buffer = CreateSynchSequence(index);
    SndfileHandle outputFile = CreateOutputFile(filename);
    outputFile.write(buffer, SAMPLES_PER_DIGRAM * 4);

    // for storing digram that is being modulated
    char currentChar, nextChar;

    // digram modulation
    while (inputFile >> currentChar && inputFile >> nextChar)
    {
        if (currentChar == '0' && nextChar == '0')
            buffer = ApplyModulation(0.0, index, buffer);
        else if (currentChar == '0' && nextChar == '1')
            buffer = ApplyModulation(AMPLITUDE / 3.0, index, buffer);
        else if (currentChar == '1' && nextChar == '0')
            buffer = ApplyModulation(AMPLITUDE / 3.0 * 2, index, buffer);
        else if (currentChar == '1' && nextChar == '1')
            buffer = ApplyModulation(AMPLITUDE, index, buffer);
        else
        {
            std::cerr << "Input file contains invalid data!" << std::endl;
            inputFile.close();
            delete [] buffer;
            return EXIT_FAILURE;
        }

        // write modulated digram to output file
        outputFile.write(buffer, SAMPLES_PER_DIGRAM);
    }

    inputFile.close();
    delete [] buffer;
    return EXIT_SUCCESS;
}
