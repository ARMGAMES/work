
#ifndef SEH_TEST_H
#define SEH_TEST_H

#include "stdafx.h"
/************************************************************************/
/*   Test code from Win c++ C23 - C26                                                                   */
/************************************************************************/

/*
__finally always execute after try block
*/
int test_tryfinal();

/*
Best practice to close any resources in finally block
*/
BOOL tryfinal_best_practice();

struct CaseInsensitiveCompare
{
  bool operator() (const std::string& left, const std::string& right) const
  {
    {
      std::string lstr = left;
      std::string rstr = right;
      std::transform(lstr.begin(), lstr.end(), lstr.begin(), ::toupper);
      std::transform(rstr.begin(), rstr.end(), rstr.begin(), ::toupper);
      return lstr.compare(rstr);
    }
  }
};

typedef std::map<std::string, std::set<std::string>> cs_string_set_map;
typedef std::map<std::string, std::set<std::string, CaseInsensitiveCompare>, CaseInsensitiveCompare> ci_string_set_map;

template <bool CS>
struct comparator
{
  typedef std::less<std::string> type;      
};

template <>
struct comparator<false>
{
  typedef CaseInsensitiveCompare type;
};

template <bool CS = true>
struct string_set_map
{
  typedef std::map<std::string, std::set<std::string, typename comparator<CS>::type>, typename comparator<CS>::type> type;
};

template <typename T>
int fun1(const T& p);

#endif