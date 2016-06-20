#ifndef _vtkOpenGGCM3dfReader_h
#define _vtkOpenGGCM3dfReader_h

#include "vtkRectilinearGridAlgorithm.h"

#include <vector>
using std::vector;
#include <map>
using std::map;
#include <string>
using std::string;

class VTK_EXPORT vtkOpenGGCM3dfReader : public vtkRectilinearGridAlgorithm
{
public:
  static vtkOpenGGCM3dfReader *New();
  vtkTypeRevisionMacro(vtkOpenGGCM3dfReader,vtkRectilinearGridAlgorithm);

  // Description:
  // Return 1 if the reader can read the named file.
  int CanReadFile(const char* fname);

  // Description:
  // Set the filename to read.
  void SetFileName(const char *fname);
  const char *GetFileName(){ return this->FileName.c_str(); }

  // Description:
  // Array selection.
  void SetPointArrayStatus(const char *name, int status);
  int GetPointArrayStatus(const char *name);
  int GetNumberOfPointArrays();
  const char* GetPointArrayName(int idx);
  void ClearPointArrayStatus();


  void PrintSelf(ostream& os, vtkIndent indent);

protected:
  vtkOpenGGCM3dfReader();
  ~vtkOpenGGCM3dfReader();

  int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  int Canreadfile(const char *fname);

private:
  vtkOpenGGCM3dfReader(const vtkOpenGGCM3dfReader&); // not implmented
  void operator = (const vtkOpenGGCM3dfReader&); // not implemeneted

private:
  string FileName;

  map<string,int> Arrays;

  vector<double> X;
  vector<double> Y;
  vector<double> Z;

  vector<double> Times;
};
#endif
