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


std::ofstream CreateOutputFile(std::string filename)
{
    std::ofstream outputFile;

    if (filename.substr(filename.length() - std::strlen(".wav")) == ".wav")
        filename = filename.substr(0, filename.length() - std::strlen(".wav"));

    outputFile.open((filename + ".txt").c_str());
    return outputFile;
}


std::vector<int> LoadInput(SndfileHandle &inputFile)
{
    std::vector<int> samples; 
	int sample = 0;

	while (inputFile.read(&sample, 1) == 1)
		samples.push_back(sample);
    
    return samples;
}


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


int GetMaxAbsValue(std::vector<int>::const_iterator &first, std::vector<int>::const_iterator &last)
{
    int min = *std::min_element(first, last);
    int max = *std::max_element(first, last);

    return std::abs(min) > max ? std::abs(min) : max;
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
    std::vector<int> samples = LoadInput(inputFile);
    unsigned int sampleCount = GetSampleCount(samples);

    bool decreaseSampleCount = sin(FREQ / sampleRate * M_PI * 2 * (sampleCount - 1)) == 0.0 ? true : false;
    if (decreaseSampleCount)
        --sampleCount;

    int currentAmplitude = 0;
    int value = 0;
    std::vector<int>::const_iterator first = samples.begin() + sampleCount + 1;
    std::vector<int>::const_iterator last = samples.begin() + 2 * sampleCount;

    if (decreaseSampleCount)
    {
        --first;
        --last;
    }

    for (int i = 0; i < 3, last <= samples.end(); first = last, last += sampleCount, ++i)
    {
        currentAmplitude = GetMaxAbsValue(first, last);

        if (currentAmplitude < AMPLITUDE / 6)
            value = 0;
        else if (currentAmplitude < AMPLITUDE / 2)
            value = 1;
        else if (currentAmplitude < AMPLITUDE / 6 * 5)
            value = 2;
        else
            value = 3;

        if ((value != 3 && i % 2 == 0) || (value != 0 && i % 2 == 1))
        {
            std::cerr << "Invalid synchronization sequence!" << std::endl;
            break;
        }
    }

    for (; last <= samples.end(); first = last, last += sampleCount)
    {
        currentAmplitude = GetMaxAbsValue(first, last);

        if (currentAmplitude < AMPLITUDE / 6)
            outputFile << "00";
        else if (currentAmplitude < AMPLITUDE / 2)
            outputFile << "01";
        else if (currentAmplitude < AMPLITUDE / 6 * 5)
            outputFile << "10";
        else
            outputFile << "11";
    }

    outputFile << std::endl;

    outputFile.close();
    return EXIT_SUCCESS;
}
