// Copyright (C) Amaya Software Ltd. All rights reserved.

#ifndef CG_h_included
#define CG_h_included

#include "targetver.h"
//#include <crtdbg.h>
#include <memory>
#include <string>
#include <set>
#include <map>
#include <vector>
#include <deque>
#include <cstdint>
#include <iostream>
#include <tuple>
#include <exception>
#include <sstream>
#include <fstream> 

using namespace std;

class CGfield;
class CGstruct;
class CGdocument;
class CGmain;
class CGloader;

#include "CGdefs.h"
#include "CGfile.h"
#include "CGlog.h"
#include "CGbaseobj.h"
#include "CGbaseparsecompose.h"
#include "CGstruct.h"
#include "CGfieldtype.h"
#include "CGfield.h"
#include "CGstruct.h"
#include "CGdocument.h"
#include "CGplugin.h"
#include "CGwindynplugin.h"
#include "CGwinstatplugin.h"
#include "CGmain.h"

#endif	// CG_h_included
