/* 
 * Project 1
 * Subject: Wireless and Mobile Networks
 * Author:  Nikola Valesova
 * Date:    3. 11. 2018
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


// create and open output file
std::ofstream CreateOutputFile(std::string filename)
{
    std::ofstream outputFile;

    if (filename.substr(filename.length() - std::strlen(".wav")) == ".wav")
        filename = filename.substr(0, filename.length() - std::strlen(".wav"));
    else
    {
        std::cerr <<  "Invalid input filename! Expecting a file ending with \'.wav\'." << std::endl;
        exit(EXIT_FAILURE);
    }

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


// get length of zero values
int GetZerosLength(SndfileHandle inputFile, int& currentChar)
{
    int sampleCount = 0;

    // read all the zero values and count them
    while (currentChar == 0)
    {
        ++sampleCount;
        inputFile.read(&currentChar, 1);
    }

    return sampleCount;
}


// get length of non-zero values
int GetNonZerosLength(SndfileHandle inputFile, int& currentChar)
{
    int prevChar = 1.0;
    int sampleCount = 0;

    // read all the non-zero values and count them
    // checking only current character would not be enough,
    // sine can also have a zero value inside, therefore stop
    // on two consequent zero values 
    while (!(currentChar == 0 && prevChar == 0))
    {
        ++sampleCount;
        prevChar = currentChar;
        inputFile.read(&currentChar, 1);
    }

    // decrement due to the additional read zero
    return --sampleCount;
}


// get lengths of the first three parts - zeros, non-zeros and zeros
std::vector<int> GetLengths(SndfileHandle inputFile)
{
    std::vector<int> lengths;
    int currentChar;

    inputFile.read(&currentChar, 1);

    // get the number of zero samples at the beginning
    lengths.push_back(GetZerosLength(inputFile, currentChar));
    // get the number of non-zero samples
    lengths.push_back(GetNonZerosLength(inputFile, currentChar));
    // get the number of zero samples in the second zero sequence
    lengths.push_back(GetZerosLength(inputFile, currentChar));

    // increment due to the one zero read in GetNonZerosLength function
    ++lengths[2];

    return lengths;
}


// determine the number of values per single digram based on lengths of subsequences
int DetermineSamplesCount(std::vector<int> lengths)
{
    // TODO
    if (lengths[0] == lengths[2] && lengths[0] == lengths[1] + 2)
        return lengths[0] - 1;
    // all parts had the same length or TODO
    else
        return lengths[0];
}


// get the number of values per single digram
unsigned int GetSamplesPerDigram(SndfileHandle inputFile, int& readValuesCount)
{
    // get lengths of zero and non-zero parts
    std::vector<int> lengths = GetLengths(inputFile);
    int samplesPerDigram = DetermineSamplesCount(lengths);
    readValuesCount = lengths[0] + lengths[1] + lengths[2];

    return samplesPerDigram;
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

    int readValuesCount;
    unsigned int samplesPerDigram = GetSamplesPerDigram(inputFile, readValuesCount);
    unsigned int toBeSkipped = samplesPerDigram * 4 - readValuesCount - 1;
    std::vector<int> samples = LoadInput(inputFile);

    int currentAmplitude = 0;
    std::vector<int>::const_iterator first = samples.begin() + toBeSkipped;
    std::vector<int>::const_iterator last = samples.begin() + toBeSkipped + samplesPerDigram;

    // demodulate input sequence and output
    for (; last <= samples.end(); first = last, last += samplesPerDigram)
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
