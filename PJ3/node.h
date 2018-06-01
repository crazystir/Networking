#ifndef _node
#define _node

#include <new>
#include <iostream>
#include <deque>
#include <set>


class RoutingMessage;
class Link;
class Table;
class SimulationContext;

#include "table.h"

using namespace std;

class Node {
 private:
  unsigned number;
  SimulationContext    *context;
  double   bw;
  double   lat;

#if defined(LINKSTATE)
  typedef pair<unsigned, int> ID;

  Table routingTable;
  bool sent;
  int IDCounter;
  set<ID> messagesID;

  bool hasReceived(const RoutingMessage*);
  void receive(const RoutingMessage*);
#endif

#if defined(DISTANCEVECTOR)
  typedef pair<int, unsigned> ID;

  Table routingTable;
  int IDCounter;
  set<ID> messagesID;

  bool hasReceived(const RoutingMessage*);
  void receive(const RoutingMessage*);
#endif

  // students will add protocol-specific data here

 public:
  Node(const unsigned n, SimulationContext *c, double b, double l);
  Node();
  Node(const Node &rhs);
  Node & operator=(const Node &rhs);
  virtual ~Node();

  virtual bool Matches(const Node &rhs) const;

  virtual void SetNumber(const unsigned n);
  virtual unsigned GetNumber() const;

  virtual void SetLatency(const double l);
  virtual double GetLatency() const;
  virtual void SetBW(const double b);
  virtual double GetBW() const;

  virtual void SendToNeighbors(const RoutingMessage *m);
  virtual void SendToNeighbor(const Node *n, const RoutingMessage *m);
  virtual deque<Node*> *GetNeighbors();
  virtual void SetTimeOut(const double timefromnow);

  //
  // Students will WRITE THESE
  //
  virtual void LinkHasBeenUpdated(const Link *l);
  virtual void ProcessIncomingRoutingMessage(const RoutingMessage *m);
  virtual void TimeOut();
  virtual Node *GetNextHop(const Node *destination) const;
  virtual Table *GetRoutingTable() const;

  virtual ostream & Print(ostream &os) const;

};

inline ostream & operator<<(ostream &os, const Node &n) { return n.Print(os);}


#endif
