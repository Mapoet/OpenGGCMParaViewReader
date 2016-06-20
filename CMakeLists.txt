cmake_minimum_required(VERSION 2.6)

PROJECT(OpenGGCM3dfReader)

FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

set(SM_CXX_SOURCES
  vtkOpenGGCM3dfReader.cxx)

set(CXX_SOURCES
  OpenGGCM3dfReader.cxx)

add_paraview_plugin(
  OpenGGCMReader "11.12"
  SERVER_MANAGER_SOURCES ${SM_CXX_SOURCES}
  SERVER_MANAGER_XML OpenGGCM3dfReaderServer.xml
  GUI_RESOURCE_FILES OpenGGCM3dfReaderClient.xml
  SOURCES ${CXX_SOURCES}
  )


