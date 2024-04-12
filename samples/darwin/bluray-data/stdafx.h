#pragma once

// Disable various CRT warnings 
#define _CRT_NON_CONFORMING_SWPRINTFS
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_CPP_OVERLOAD_SECURE_NAMES 1

#define _BIND_TO_CURRENT_CRT_VERSION 1

//  Workaround for the unfortunate min/max macros defined by some platform headers
#define PRIMO_NO_MIN_MAX

#include <cstdio>
#include <cassert>

#include <string>
#include <vector>

#include <filesystem>

#include <primo/burner/pb.h>
using namespace primo::burner;

#include <primo/platform/ustring.h>
namespace p = primo;

// convert to wide string 
#define _W(a) p::ustring(a).wstr().c_str()
#define _N(a) p::ustring(a).str().c_str()

// Unicode std::string definition
#if defined(_UNICODE) || defined(UNICODE)
    typedef std::wstring tstring;
    
    #define _TEXT(q) L##q
    #define _T(q) L##q
#else
    typedef std::string tstring;

    #define _TEXT(q) ##q
    #define _T(q) ##q
#endif


#include <fmt/format.h>

#include <argparse/argparse.hpp>
#include <tabulate/table.hpp>

#include <indicators/cursor_control.hpp>
#include <indicators/progress_bar.hpp>
#include <indicators/multi_progress.hpp>
using namespace indicators;
