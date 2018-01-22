// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the FSO_DEMOOBJECTS_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// FSO_DEMOOBJECTS_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef FSO_DEMOOBJECTS_EXPORTS
#define FSO_DEMOOBJECTS_API __declspec(dllexport)
#else
#define FSO_DEMOOBJECTS_API __declspec(dllimport)
#endif

#include "FSUtils.h"
#include "FSObject.h"

// include the needed library files
#ifdef _DEBUG
#pragma comment(lib, "FireStorm_debug.lib")
#pragma comment(lib, "FSO_Generic_Debug.lib")
#else
#pragma comment(lib, "FireStorm.lib")
#pragma comment(lib, "FSO_Generic.lib")
#endif
using namespace FS;
