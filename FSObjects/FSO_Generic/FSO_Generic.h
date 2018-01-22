// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the FSOGENERIC_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// FSOGENERIC_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef FSO_GENERIC_EXPORTS
#define FSO_GENERIC_API __declspec(dllexport)
#else
#define FSO_GENERIC_API __declspec(dllimport)
#endif

#include "FSUtils.h"
#include "FSObject.h"

// include the needed library files
#ifdef _DEBUG
#pragma comment(lib, "FireStorm_debug.lib")
#else
#pragma comment(lib, "FireStorm.lib")
#endif

using namespace FS;

extern "C"
{
	FSO_GENERIC_API bool	queryIsObject();
	FSO_GENERIC_API bool	queryIsObjectType(stringc objecttype);
	FSO_GENERIC_API void*	createObject(stringc objecttype);
	FSO_GENERIC_API void 	queryAllObjectTypes(list<stringc> *list);
	FSO_GENERIC_API stringc	queryObjectDescription(stringc objecttype);
}