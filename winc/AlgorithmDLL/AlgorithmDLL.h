#ifndef ALGORIGHTM_DLL_H
#define ALGORIGHTM_DLL_H

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the ALGORITHMDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// ALGORITHMDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef ALGORITHMDLL_EXPORTS
#define ALGORITHMDLL_API  __declspec(dllexport)
#else
#define ALGORITHMDLL_API  __declspec(dllimport)
#endif

// This class is exported from the AlgorithmDLL.dll
class ALGORITHMDLL_API CAlgorithmDLL {
public:
	CAlgorithmDLL(void);
	// TODO: add your methods here.
};

// declar functions here
extern "C"
{
  ALGORITHMDLL_API int AddTwo(int nleft, int nright);
  ALGORITHMDLL_API int GetInt();
}
#endif
