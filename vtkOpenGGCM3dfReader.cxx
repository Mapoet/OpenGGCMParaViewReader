#include "vtkOpenGGCM3dfReader.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"

#include "vtkStreamingDemandDrivenPipeline.h"
typedef vtkStreamingDemandDrivenPipeline vtkSDDP;
#include "vtkRectilinearGridAlgorithm.h"
#include "vtkRectilinearGrid.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "vtkFloatArray.h"

#include "OpenGGCM3dfReader.h"

#define vtkOpenGGCM3dfReaderDEBUG

//-----------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkOpenGGCM3dfReader, "$Revision: 1.0 $");

//-----------------------------------------------------------------------------
vtkStandardNewMacro(vtkOpenGGCM3dfReader);

//-----------------------------------------------------------------------------
vtkOpenGGCM3dfReader::vtkOpenGGCM3dfReader()
{
  #ifdef vtkOpenGGCM3dfReaderDEBUG
  cerr << "=======vtkOpenGGCM3dfReader::vtkOpenGGCM3dfReader" << endl;
  #endif

  this->SetNumberOfInputPorts(0);
  this->SetNumberOfOutputPorts(1);

}

//-----------------------------------------------------------------------------
vtkOpenGGCM3dfReader::~vtkOpenGGCM3dfReader()
{
  #ifdef vtkOpenGGCM3dfReaderDEBUG
  cerr << "=======vtkOpenGGCM3dfReader::~vtkOpenGGCM3dfReader" << endl;
  #endif

  this->SetFileName(0);
}

//-----------------------------------------------------------------------------
int vtkOpenGGCM3dfReader::CanReadFile(const char *fname)
{
  #ifdef vtkOpenGGCM3dfReaderDEBUG
  cerr << "=======vtkOpenGGCM3dfReader::Canreadfile" << endl;
  #endif

  // at a minimum check the file name for our extension.
  int can=0;
  if (fname)
    {
    string fileName=fname;
    if (fileName.find(".ggcm")!=string::npos)
      {
      can=1;
      }
    }

  #ifdef vtkOpenGGCM3dfRaederDEBUG
  cerr "can=" << can << endl;
  #endif

  return can;
}

//-----------------------------------------------------------------------------
void vtkOpenGGCM3dfReader::SetFileName(const char *fname)
{
  #ifdef vtkOpenGGCM3dfReaderDEBUG
  cerr << "=======vtkOpenGGCM3dfReader::SetFileName" << endl;
  #endif
  if (fname && this->FileName==fname)
    {
    return;
    }

  // free up resources
  this->FileName="";
  this->Arrays.clear();
  this->X.clear();
  this->Y.clear();
  this->Z.clear();

  if (fname==0)
    {
    return;
    }

  this->FileName=fname;

  int iErr;

  // read coordinates
  iErr=getCoordinatesFromFile(fname,this->X,this->Y,this->Z);
  if (iErr)
    {
    vtkErrorMacro("Failed to read coordinate arrays from " << fname);
    return;
    }

  // read array names
  vector<string> arrays;
  iErr=getArraysFromFile(fname,arrays);
  if (iErr)
    {
    vtkErrorMacro("Failed to array names from " << fname);
    return;
    }
  size_t nArrays=arrays.size();
  for (size_t i=0; i<nArrays; ++i)
    {
    this->Arrays[arrays[i]]=0;
    }

  // read times
  iErr=getTimesFromFile(fname,this->Times);
  if (iErr)
    {
    vtkErrorMacro("Failed to times from " << fname);
    return;
    }

  this->Modified();
}

//-----------------------------------------------------------------------------
void vtkOpenGGCM3dfReader::SetPointArrayStatus(const char *name, int status)
{
  #if defined vtkOpenGGCM3dfReaderDEBUG
  cerr
    << "=======vtkOpenGGCM3dfReader::SetPointArrayStatus "
    << (const char*)(name?name:"NULL") << " " << status
    << endl;
  #endif

  if (name==0) return;

  this->Arrays[name]=status;

  this->Modified();
}

//-----------------------------------------------------------------------------
int vtkOpenGGCM3dfReader::GetPointArrayStatus(const char *name)
{
  #if defined vtkOpenGGCM3dfReaderDEBUG
  //cerr << "=======vtkOpenGGCM3dfReader::GetPointArrayStatus" << endl;
  #endif

  if (name==0) return 0;

  return this->Arrays[name];
}

//-----------------------------------------------------------------------------
int vtkOpenGGCM3dfReader::GetNumberOfPointArrays()
{
  #if defined vtkOpenGGCM3dfReaderDEBUG
  //cerr << "=======vtkOpenGGCM3dfReader::GetNumberOfPointArrays" << endl;
  #endif

  return this->Arrays.size();
}

//-----------------------------------------------------------------------------
const char* vtkOpenGGCM3dfReader::GetPointArrayName(int idx)
{
  #if defined vtkOpenGGCM3dfReaderDEBUG
  //cerr << "=======vtkOpenGGCM3dfReader::GetArrayName " << idx << endl;
  #endif

  map<string,int>::iterator it=this->Arrays.begin();
  map<string,int>::iterator itEnd=this->Arrays.end();

  for (int i=0; it!=itEnd && i<idx; ++it,++i);

  if (it==itEnd)
    {
    vtkErrorMacro(<< idx << " out of bounds");
    return 0;
    }

  return (*it).first.c_str();
}

//-----------------------------------------------------------------------------
void vtkOpenGGCM3dfReader::ClearPointArrayStatus()
{
  #if defined vtkOpenGGCM3dfReaderDEBUG
  //cerr << "=======vtkOpenGGCM3dfReader::ClearPointArrayStatus" << endl;
  #endif

  map<string,int>::iterator it=this->Arrays.begin();
  map<string,int>::iterator itEnd=this->Arrays.end();

  for (; it!=itEnd; ++it)
    {
    (*it).second=0;
    }

  this->Modified();
}

//-----------------------------------------------------------------------------
int vtkOpenGGCM3dfReader::RequestInformation(
      vtkInformation *request,
      vtkInformationVector **inInfos,
      vtkInformationVector *outInfos)
{
  #ifdef vtkOpenGGCM3dfReaderDEBUG
  cerr << "=======vtkOpenGGCM3dfReader::RequestInformation" << endl;
  #endif

  vtkInformation *info=outInfos->GetInformationObject(0);

  int wholeExt[6]={0,0,0,0,0,0};
  wholeExt[1]=this->X.size()-1;
  wholeExt[3]=this->Y.size()-1;
  wholeExt[5]=this->Z.size()-1;
  info->Set(vtkSDDP::WHOLE_EXTENT(),wholeExt,6);

  info->Set(vtkSDDP::TIME_STEPS(),&this->Times[0],this->Times.size());

  double timeRange[2]={this->Times[0],this->Times[this->Times.size()-1]};
  info->Set(
    vtkStreamingDemandDrivenPipeline::TIME_RANGE(),
    timeRange,
    2);

  #if defined vtkOpenGGCM3dfReaderDEBUG
  cerr << "WHOLE_EXTENT=";
  for (int i=0; i<6; ++i) cerr << wholeExt[i] << " ";
  cerr << endl;
  cerr << "TIME_STEPS=";
  for (int i=0; i<this->Times.size(); ++i) cerr << this->Times[i] << " ";
  cerr << endl;
  #endif

  return 1;
}

//-----------------------------------------------------------------------------
int vtkOpenGGCM3dfReader::RequestData(
      vtkInformation *request,
      vtkInformationVector **inInfos,
      vtkInformationVector *outInfos)
{
  #ifdef vtkOpenGGCM3dfReaderDEBUG
  cerr << "=======vtkOpenGGCM3dfReader::RequestData" << endl;
  #endif

  vtkInformation *info=outInfos->GetInformationObject(0);

  // the vtk dataset to populate
  vtkRectilinearGrid *output
    = dynamic_cast<vtkRectilinearGrid *>(info->Get(vtkDataObject::DATA_OBJECT()));
  if (output==NULL)
    {
    vtkErrorMacro("DATA_OBJECT was not a rectiliner grid.");
    return 1;
    }

  // decomp is what will be loaded by this process.
  int decomp[6];
  info->Get(vtkSDDP::UPDATE_EXTENT(),decomp);
  output->SetExtent(decomp);

  // construct the dataset
  size_t nx=decomp[1]-decomp[0]+1;
  size_t ny=decomp[3]-decomp[2]+1;
  size_t nz=decomp[5]-decomp[4]+1;
  size_t nxyz=nx*ny*nz;

  vtkDoubleArray *x=vtkDoubleArray::New();
  x->SetNumberOfTuples(nx);
  x->SetName("x");
  double *pX=x->GetPointer(0);
  for (size_t i=decomp[0], q=0; i<=decomp[1]; ++i,++q)
    {
    pX[q]=this->X[i];
    }
  output->SetXCoordinates(x);
  x->Delete();

  vtkDoubleArray *y=vtkDoubleArray::New();
  y->SetNumberOfTuples(ny);
  y->SetName("y");
  double *pY=y->GetPointer(0);
  for (size_t i=decomp[2], q=0; i<=decomp[3]; ++i,++q)
    {
    pY[q]=this->Y[i];
    }
  output->SetYCoordinates(y);
  y->Delete();

  vtkDoubleArray *z=vtkDoubleArray::New();
  z->SetNumberOfTuples(nz);
  z->SetName("z");
  double *pZ=z->GetPointer(0);
  for (size_t i=decomp[4], q=0; i<=decomp[5]; ++i,++q)
    {
    pZ[q]=this->Z[i];
    }
  output->SetZCoordinates(z);
  z->Delete();

  // time we need to read
  double *time = info->Get(vtkSDDP::UPDATE_TIME_STEPS());
  info->Set(vtkDataObject::DATA_TIME_STEPS(),time,1);
  output->GetInformation()->Set(vtkDataObject::DATA_TIME_STEPS(),time,1);

  // read the selected arrays
  const char *fileName=this->FileName.c_str();

  map<string,int>::iterator it=this->Arrays.begin();
  map<string,int>::iterator itEnd=this->Arrays.end();

  for (; it!=itEnd; ++it)
    {
    if ((*it).second)
      {
      const char *arrayName=(*it).first.c_str();

      vtkFloatArray *fa=vtkFloatArray::New();
      fa->SetNumberOfTuples(nxyz);
      fa->SetName(arrayName);
      output->GetPointData()->AddArray(fa);
      fa->Delete();

      int iErr=readArrayFromFile(
            fileName,
            arrayName,
            *time,
            decomp,
            fa->GetPointer(0));
      if (iErr)
        {
        vtkErrorMacro("Failed to read array " << arrayName);
        }
      }
    }

  #ifdef vtkOpenGGCM3dfReaderDEBUG
  cerr << "UPDATE_EXTENT=";
  for (int i=0; i<6; ++i) cerr << decomp[i] << " ";
  cerr << endl;
  cerr << "UPDATE_TIME_STEP=" << *time << endl;
  this->Print(cerr);
  output->Print(cerr);
  #endif
  return 1;
}

//-----------------------------------------------------------------------------
void vtkOpenGGCM3dfReader::PrintSelf(ostream& os, vtkIndent indent)
{
  //this->Superclass::PrintSelf(os, indent);

  os << "FileName=" << this->FileName << endl;

  os << "Arrays" << endl;
  map<string,int>::iterator it;
  map<string,int>::iterator itEnd;
  for (; it!=itEnd; ++it)
    {
    os << "  " << (*it).first << "=" << (*it).second << endl;
    }

  os << "X=";
  for (int i=0; i<this->X.size(); ++i){ os << this->X[i] << " "; }
  os << endl;

  os << "Y=";
  for (int i=0; i<this->Y.size(); ++i){ os << this->Y[i] << " "; }
  os << endl;

  os << "Z=";
  for (int i=0; i<this->Z.size(); ++i){ os << this->Z[i] << " "; }
  os << endl;

  os << "Times=";
  for (int i=0; i<this->Times.size(); ++i){ os << this->Times[i] << " "; }
  os << endl;
}

