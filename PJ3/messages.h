#ifndef _messages
#define _messages

#include <iostream>
#include <set>

#include "node.h"
#include "link.h"

using namespace std;

#if defined(GENERIC)
class RoutingMessage {
 public:
  ostream & Print(ostream &os) const;
};
#endif

#if defined(LINKSTATE)
#include "linknode.h"
class RoutingMessage {
private:
//  static int IDCounter;
  int messageID;
  unsigned srcNum;
  VL dests;
  set<unsigned> markedNodes;

public:
  RoutingMessage();
  RoutingMessage(int, unsigned, VL);
  RoutingMessage(const RoutingMessage &rhs);
  RoutingMessage &operator=(const RoutingMessage &rhs);
  void SetDestsInfo(const VL&);
  VL GetDestsInfo() const;
  void SetSrcNum(unsigned);
  unsigned GetSrcNum() const;
  unsigned GetMessageID() const;
  bool isMarked(unsigned) const;
  void mark(unsigned);
  ostream & Print(ostream &os) const;
};
#endif

#if defined(DISTANCEVECTOR)
typedef vector<double> VD;

enum RoutingMessageType {request, response};
class RoutingMessage {
private:
//  static int IDCounter;
  int messageID;
  RoutingMessageType type;
  unsigned srcNum;
  unsigned length;
  VD distance;

public:
  RoutingMessage();
  RoutingMessage(RoutingMessageType,int, unsigned, unsigned);
  RoutingMessage(RoutingMessageType,int, unsigned, unsigned, const VD&);
  RoutingMessage(const RoutingMessage &rhs);
  RoutingMessage &operator=(const RoutingMessage &rhs);

  int GetMessageID() const;
  RoutingMessageType GetType() const;
  unsigned GetSrcNum() const;
  unsigned GetLength() const;
  VD GetDistance() const;
  ostream & Print(ostream &os) const;
};
#endif


inline ostream & operator<<(ostream &os, const RoutingMessage &m) { return m.Print(os);}

#endif
