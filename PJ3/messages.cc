#include "messages.h"


#if defined(GENERIC)
ostream &RoutingMessage::Print(ostream &os) const
{
  os << "RoutingMessage()";
  return os;
}
#endif


#if defined(LINKSTATE)

int RoutingMessage::IDCounter = 0;

ostream &RoutingMessage::Print(ostream &os) const {
  unsigned i;

  os << endl;
  os << "Message ID:" << messageID << endl;
  os << "Source node: " << srcNum << endl;
  os << "Destination nodes: ";

  for (i = 0; i < dests.size(); i++) {
    os << "(" << dests[i].num << ", "  <<dests[i].lat << ") ";
  }
  os << endl;
  return os;
}

RoutingMessage::RoutingMessage(): messageID(RoutingMessage::IDCounter++), markedNodes(set<unsigned>()) {}

RoutingMessage::RoutingMessage(const RoutingMessage &rhs): messageID(rhs.messageID), srcNum(rhs.srcNum), dests(rhs.dests), markedNodes(rhs.markedNodes) {}

void RoutingMessage::SetDestsInfo(const VL& d) {
  dests = d;
}

VL RoutingMessage::GetDestsInfo() const {
  return dests;
}

void RoutingMessage::SetSrcNum(unsigned s) {
  srcNum = s;
}

unsigned RoutingMessage::GetSrcNum() const{
  return srcNum;
}

unsigned RoutingMessage::GetMessageID() const {
  return messageID;
}

bool RoutingMessage::isMarked(unsigned nodeNum) const {
  return markedNodes.find(nodeNum) != markedNodes.end();
}

void RoutingMessage::mark(unsigned nodeNum) {
  markedNodes.insert(nodeNum);
}



#endif


#if defined(DISTANCEVECTOR)
int RoutingMessage::IDCounter = 0;

ostream &RoutingMessage::Print(ostream &os) const
{
  unsigned i;

  os << endl;
  os << "=======" << endl;
  os << "Message ID:" << messageID << endl;
  os << "Source node: " << srcNum << " length: " << length <<  endl;
  os << "Type: " << type << endl;

  for (i = 0 ; i < distance.size(); i++) {
    os << i << " " << distance[i] << endl;
  }
  os << "=======" << endl;
  return os;
}

RoutingMessage::RoutingMessage() {}

RoutingMessage::RoutingMessage(RoutingMessageType t, unsigned s, unsigned l): messageID(IDCounter++)
                              , type(t), srcNum(s), length(l), distance(VD()) {}

RoutingMessage::RoutingMessage(RoutingMessageType t, unsigned s, unsigned l, const VD& d): messageID(IDCounter++)
                              , type(t), srcNum(s), length(l), distance(d) {}

RoutingMessage::RoutingMessage(const RoutingMessage &rhs): messageID(rhs.messageID), type(rhs.type), srcNum(rhs.srcNum)
                              , length(rhs.length), distance(rhs.distance) {}

RoutingMessage & RoutingMessage::operator=(const RoutingMessage &rhs) {
   return *(new(this)RoutingMessage(rhs));
}

int RoutingMessage::GetMessageID() const {return messageID;}
RoutingMessageType RoutingMessage::GetType() const {return type;}

unsigned RoutingMessage::GetSrcNum() const {return srcNum;}

unsigned RoutingMessage::GetLength() const {return length;}

VD RoutingMessage::GetDistance() const {return distance;}

#endif

