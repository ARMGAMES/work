
#ifndef CONSTANTS_H
#define CONSTANTS_H

typedef int Key;

static const int SEARCH_MAX = 100;
static const int SORT_MAX = 15;
static const int maxKey = 1000;
static const int maxWord = 100;

class Item
{
public:
  Item(){ keyval = maxKey;}

  Item(Key k){ keyval = k;}

  Key key() {return keyval;}

  int null() { return keyval == maxKey;}

  void rand()
  {
    keyval = 1000 * ::rand()/RAND_MAX;
    info = 1.0 * ::rand()/RAND_MAX;
  }

  int scan(istream& is = cin)
  {
    return (is >> keyval >> info) != 0;
  }

  void show()
  {
    cout << keyval << " " << info << endl;
  }

private:
  Key keyval;
  float info;
};


#endif