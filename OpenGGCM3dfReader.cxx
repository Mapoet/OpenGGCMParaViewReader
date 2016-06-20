#include "OpenGGCM3dfReader.h"
#include <cmath>

//*****************************************************************************
int getArraysFromFile(
      const char *fileName,
      vector<string> &arrays)
{
  // TODO -- Call subroutine to get a list of available arrays
  // use the following for demonstration
  // return -1 to indicate an error

  const int nN=6;
  const char *names[nN] = {"P","T","Rho","V","B","E"};

  arrays.resize(nN);
  for (int i=0; i<nN; ++i)
    {
    arrays[i]=names[i];
    }

  return 0;
}

//*****************************************************************************
int getTimesFromFile(
      const char *fileName,
      vector<double> &times)
{
  // TODO -- Call subroutine to get a list of available times
  // use the following for demonstration
  // return -1 to indicate an error

  const int nT=4;
  double t[nT] = {0.2502, 0.51, 0.769999, 1.1};

  times.resize(nT);
  for (int i=0; i<nT; ++i)
    {
    times[i]=t[i];
    }

  return 0;
}

//*****************************************************************************
int getCoordinatesFromFile(
      const char *fileName,
      vector<double> &x,
      vector<double> &y,
      vector<double> &z)
{

  // TODO -- Call subroutine to get the coordinate arrays and
  // grid dimensions. use the following for demonstration
  // return -1 to indicate an error

  const int nx=17;
  const int ny=9;
  const int nz=9;

  x.resize(nx);
  y.resize(ny);
  z.resize(nz);

  #define fsign(t) (t<0.0?-1.0:1.0)
  for (int i=0; i<nx; ++i)
    {
    double w=nx/2;
    double t=(i-w)/w;
    x[i]=sin(t);
    }

  for (int i=0; i<ny; ++i)
    {
    double w=ny/2;
    double t=(i-w)/w;
    y[i]=sin(t)*sin(t)*fsign(t)/3.0;
    z[i]=y[i];
    }

  return 0;
}

//*****************************************************************************
int readArrayFromFile(
      const char *fileName,
      const char *arrayName,
      double time,
      int *decomp,
      float *buffer)
{
  // TODO -- Call subroutine to actually read the data on this
  // subset at this time. use the following for demonstration
  // return -1 to indicate an error

  size_t ni=decomp[1]-decomp[0]+1;
  size_t nj=decomp[3]-decomp[2]+1;
  size_t nk=decomp[5]-decomp[4]+1;
  size_t nij=ni*nj;

  for (size_t k=0; k<nk; ++k)
    {
    for (size_t j=0; j<nj; ++j)
      {
      for (size_t i=0; i<ni; ++i)
        {
        size_t q=k*nij+j*ni+i;
        buffer[q]=time;
        }
      }
    }

  return 0;
}

