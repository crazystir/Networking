#ifndef _table
#define _table


#include <iostream>
#include <string>

using namespace std;


#if defined(GENERIC)
class Table {
  // Students should write this class

 public:
  ostream & Print(ostream &os) const;
};
#endif


#if defined(LINKSTATE)
#include <vector>
#include <queue>
#include "linknode.h"
typedef vector<vector<double> > VVD;
typedef vector<vector<int> > VVI;
typedef vector<double> VD;
typedef vector<int> VI;


class Table {
  // Students should write this class
 private:
  unsigned nodeNum;
  unsigned nodeLength;
  bool updated;
  VVL graph;
  VD distance;
  VI next;

  void Dijkstra();
 public:
  Table();
  Table(unsigned);
  Table(const Table&);
  bool operator()(unsigned, unsigned);
  Table& operator=(const Table&);
  void AddLinks(unsigned, const VL&);
  void AddLink(unsigned, unsigned, double);
  bool isUpdated();
  void UpdateCheck();
  unsigned GetNext(unsigned) const;
  VVL GetGraph() const;
  ostream & Print(ostream &os) const;
};
#endif

#if defined(DISTANCEVECTOR)

#include <deque>
#include <vector>
#include <map>

#define INF 99e99

typedef vector<int> VI;
typedef vector<double> VD;

class Table {
 private:
  unsigned nodeNum;
  unsigned nodeLength;
  VI next;
  VD distance;
  VD neighborsCost;
  map<unsigned, VD> neighborsDistance;

  void resize(unsigned);
  bool update();
 public:
  Table();
  Table(unsigned);
  Table(const Table&);
  void CheckLength(unsigned);
  bool AddLink(unsigned, double);
  bool NodeChange(unsigned, unsigned, const VD&);
  unsigned GetLength() const;
  VD GetDistance(unsigned);
  VD FilteredDistance(unsigned);
  unsigned GetNext(unsigned) const;
  ostream & Print(ostream &os) const;
};
#endif

inline ostream & operator<<(ostream &os, const Table &t) { return t.Print(os);}

#endif
