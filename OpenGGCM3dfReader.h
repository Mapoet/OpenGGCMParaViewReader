#ifndef OpenGGCM3dfReader_h
#define OpenGGCM3dfReader_h

#include <vector>
using std::vector;

#include <string>
using std::string;

int getArraysFromFile(
      const char *fileName,
      vector<string> &arrays);

int getTimesFromFile(
      const char *fileName,
      vector<double> &times);

int getCoordinatesFromFile(
      const char *fileName,
      vector<double> &x,
      vector<double> &y,
      vector<double> &z);

int readArrayFromFile(
      const char *fileName,
      const char *arrayName,
      double time,
      int *decomp,
      float *buffer);

#endif

