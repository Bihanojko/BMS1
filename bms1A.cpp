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
#include <cstring>
#include <iostream>
#include <fstream>

#include "sndfile.hh"

// TODO dafuq? proc zrovna 24000??? vvvv puvodne bylo 18000
#define SAMPLE_RATE 24000
#define CHANELS 1
#define FORMAT (SF_FORMAT_WAV | SF_FORMAT_PCM_24)
#define AMPLITUDE (1.0 * 0x7F000000)
#define FREQ (1000.0 / SAMPLE_RATE)

// TODO co to je??? vvvvv BAUDRATE???
#define BITRATE 1350


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
int* CreateSynchSequence()
{
    int index = - SAMPLE_RATE / BITRATE;
    int* buffer = new int[SAMPLE_RATE / BITRATE * 4];

    for (int i = 0; i < SAMPLE_RATE / BITRATE * 4; i += SAMPLE_RATE / BITRATE * 2)
    {
        for (int j = 0; j < SAMPLE_RATE / BITRATE; ++j)
            buffer[i + j] = 0;

        index += SAMPLE_RATE / BITRATE * 2;

        for (int j = 0; j < SAMPLE_RATE / BITRATE; ++j)
            buffer[i + j + SAMPLE_RATE / BITRATE] = AMPLITUDE * sin(FREQ * 2 * (index + j) * M_PI);
    }

    return buffer;
}


int* ApplyModulation(int signalStrength, int index, int* buffer)
{
    for (int i = 0; i < SAMPLE_RATE / BITRATE; ++i)
        buffer[i] = signalStrength * sin(FREQ * 2 * (index + i) * M_PI);

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

    int* buffer = CreateSynchSequence();
    SndfileHandle outputFile = CreateOutputFile(filename);
    outputFile.write(buffer, SAMPLE_RATE / BITRATE * 4);

    int index = 0;
    char currentChar, nextChar;

    while (inputFile >> currentChar && inputFile >> nextChar)
    {
        if (currentChar == '0' && nextChar == '0')
            buffer = ApplyModulation(0, index, buffer);
        else if (currentChar == '0' && nextChar == '1')
            buffer = ApplyModulation(AMPLITUDE / 3, index, buffer);
        else if (currentChar == '1' && nextChar == '0')
            buffer = ApplyModulation(AMPLITUDE / 3 * 2, index, buffer);
        else if (currentChar == '1' && nextChar == '1')
            buffer = ApplyModulation(AMPLITUDE, index, buffer);
        else
        {
            std::cerr << "Input file contains invalid data!" << std::endl;
            inputFile.close();
            delete [] buffer;
            return EXIT_FAILURE;
        }

        outputFile.write(buffer, SAMPLE_RATE / BITRATE);
        index += SAMPLE_RATE / BITRATE;
    }

    inputFile.close();
    delete [] buffer;
    return EXIT_SUCCESS;
}
