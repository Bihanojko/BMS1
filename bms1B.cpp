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
#include <vector>
#include <algorithm>

#include "sndfile.hh"

#define AMPLITUDE (1.0 * 0x7F000000)
#define FREQ (1000.0)


// create and open output file
std::ofstream CreateOutputFile(std::string filename)
{
    std::ofstream outputFile;

    if (filename.substr(filename.length() - std::strlen(".wav")) == ".wav")
        filename = filename.substr(0, filename.length() - std::strlen(".wav"));

    outputFile.open((filename + ".txt").c_str());
    return outputFile;
}


// load input sequence from input file
std::vector<int> LoadInput(SndfileHandle &inputFile)
{
	int sample = 0;
    std::vector<int> samples; 

	while (inputFile.read(&sample, 1) == 1)
		samples.push_back(sample);

    return samples;
}


// get the number of samples in one section of input
unsigned int GetSampleCount(std::vector<int> &samples)
{
	unsigned int sampleCount = 0;

	while (sampleCount < samples.size())
    {
        if (samples.at(sampleCount) != 0)
            break;

        ++sampleCount;
    }

    return sampleCount;
}


// get the maximum absolute value of a subvector
int GetMaxAbsValue(std::vector<int>::const_iterator &first, std::vector<int>::const_iterator &last)
{
    int min = *std::min_element(first, last);
    int max = *std::max_element(first, last);

    return std::abs(min) > max ? std::abs(min) : max;
}


// MAIN
int main(int argc, char** argv)
{
    // check the number of arguments
    if (argc != 2)
    {
		std::cerr <<  "Invalid argument count! Usage: ./bms1B filename.wav" << std::endl;
		return EXIT_FAILURE;
    }

    // open input and output files
    std::string filename = argv[1];
    SndfileHandle inputFile = SndfileHandle(filename);
    std::ofstream outputFile = CreateOutputFile(filename);

    // check if output file was opened successfully
    if (!outputFile.is_open())
    {
        std::cerr <<  "Unable to create output file!" << std::endl;
        return EXIT_FAILURE;
    }

    int sampleRate = inputFile.samplerate();
    std::vector<int> samples = LoadInput(inputFile);
    unsigned int sampleCount = GetSampleCount(samples);

    // check if the last read number was already a beginning of the next sequence or not 
    bool decreaseSampleCount = sin(FREQ / sampleRate * M_PI * 2 * (sampleCount - 1)) == 0.0 ? true : false;
    // if so, decrease the number of samples per sequence accordingly
    if (decreaseSampleCount)
        --sampleCount;

    int currentAmplitude = 0;
    int value = 0;
    std::vector<int>::const_iterator first = samples.begin() + sampleCount + 1;
    std::vector<int>::const_iterator last = samples.begin() + 2 * sampleCount;

    // TODO is this ok??? check first and last iterator setting and decreaseSampleCount as is it
    if (decreaseSampleCount)
    {
        --first;
        --last;
    }

    // skip and check synchronization sequence
    for (int i = 0; i < 3, last <= samples.end(); first = last, last += sampleCount, ++i)
    {
        // get value of amplitude in current section
        currentAmplitude = GetMaxAbsValue(first, last);

        // demodulation
        if (currentAmplitude < AMPLITUDE / 6)
            value = 0;
        else if (currentAmplitude < AMPLITUDE / 2)
            value = 1;
        else if (currentAmplitude < AMPLITUDE / 6 * 5)
            value = 2;
        else
            value = 3;

        // check if the synchronization sequence is valid
        if ((value != 3 && i % 2 == 0) || (value != 0 && i % 2 == 1))
        {
            std::cerr << "Invalid synchronization sequence!" << std::endl;
            outputFile.close();
            EXIT_FAILURE;
        }
    }

    // demodulate input sequence and output
    for (; last <= samples.end(); first = last, last += sampleCount)
    {
        // get value of amplitude in current section
        currentAmplitude = GetMaxAbsValue(first, last);

        // demodulation
        // less than 16 % of amplitude value
        if (currentAmplitude < AMPLITUDE / 6)
            outputFile << "00";
        // less than 50 % of amplitude value
        else if (currentAmplitude < AMPLITUDE / 2)
            outputFile << "01";
        // less than 83 % of amplitude value
        else if (currentAmplitude < AMPLITUDE / 6 * 5)
            outputFile << "10";
        // otherwise
        else
            outputFile << "11";
    }

    outputFile << std::endl;

    outputFile.close();
    return EXIT_SUCCESS;
}
