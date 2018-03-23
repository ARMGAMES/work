
#include "stdafx.h"

using namespace std;

class TestClass
{
public:
  TestClass()
  {
    cout << "TestClass constructor!"<< endl;
    count = 10;
  }

   TestClass(const TestClass&)
   {
     cout << "TestClass assignment constructor!"<< endl;
     count = 5;
   }

  ~TestClass(){cout << "~~~~TestClass destructor!"<<endl;}

  void setcount(int i) {count = i;}
private:
 // TestClass(const TestClass&){}
  int count;
};

void test_adtobject()
{
  TestClass tc;
  stack<TestClass> s;

  // when push object into container, assignment constructor is called
  // to create a new object
  s.push(tc);

  tc.setcount(3);
  TestClass *pa = &s.top();

  pa->setcount(5);

  s.pop();

  if (pa != NULL)
  {
    pa->setcount(6);
  }
}

