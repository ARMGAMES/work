// AlgorithmDLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "AlgorithmDLL.h"



// This is an example of an exported function.
int AddTwo(int nleft, int nright)
{
	return nleft + nright;
}

int GetInt()
{
  return 100;
}

// This is the constructor of a class that has been exported.
// see AlgorithmDLL.h for the class definition
CAlgorithmDLL::CAlgorithmDLL()
{
	return;
}
