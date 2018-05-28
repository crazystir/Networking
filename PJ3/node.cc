#include "node.h"
#include "context.h"
#include "error.h"
#include <deque>

using namespace std;


Node::Node(const unsigned n, SimulationContext *c, double b, double l) :
    number(n), context(c), bw(b), lat(l)
#if defined(LINKSTATE)
    , routingTable(n), sent(false), messagesID()
#endif

#if defined(DISTANCEVECTOR)
    , routingTable(n), messagesID()
#endif
{
}

Node::Node() 
{ throw GeneralException(); }

Node::Node(const Node &rhs) : 
  number(rhs.number), context(rhs.context), bw(rhs.bw), lat(rhs.lat)
#if defined(LINKSTATE)
    , routingTable(rhs.routingTable), sent(false), messagesID(rhs.messagesID)
#endif

#if defined(DISTANCEVECTOR)
    , routingTable(rhs.routingTable), messagesID()
#endif
{
}

Node & Node::operator=(const Node &rhs) 
{
  return *(new(this)Node(rhs));
}

void Node::SetNumber(const unsigned n) 
{ number=n;}

unsigned Node::GetNumber() const 
{ return number;}

void Node::SetLatency(const double l)
{ lat=l;}

double Node::GetLatency() const 
{ return lat;}

void Node::SetBW(const double b)
{ bw=b;}

double Node::GetBW() const 
{ return bw;}

Node::~Node()
{}

// Implement these functions  to post an event to the event queue in the event simulator
// so that the corresponding node can recieve the ROUTING_MESSAGE_ARRIVAL event at the proper time
void Node::SendToNeighbors(const RoutingMessage *m)
{
//  deque<Link*> *ll=context -> GetOutgoingLinks(this);
//  for (deque<Link*>::const_iterator i=ll->begin();i!=ll->end();++i) {
//    Node x = Node((*i)->GetDest(),0,0,0);
//    context -> PostEvent(new Event(context -> GetTime()+(*i)->GetLatency(),
//                        ROUTING_MESSAGE_ARRIVAL,
//                        context -> FindMatchingNode(&x),
//                        (void*)m));
//  }
//  delete ll;
    context -> SendToNeighbors(this, m);
}

void Node::SendToNeighbor(const Node *n, const RoutingMessage *m)
{
//  Link x = Link(number,n->GetNumber(),0,0,0);
//  Link *l = context -> FindMatchingLink(&x);
//
//  if (l != 0) {
//    context -> PostEvent(new Event(context -> GetTime() + l->GetLatency(),
//                        ROUTING_MESSAGE_ARRIVAL,
//                        context -> FindMatchingNode(n),
//                        (void *) m));
//  }
    context -> SendToNeighbor(this, n, m);
}

deque<Node*> *Node::GetNeighbors()
{
  return context->GetNeighbors(this);
}

void Node::SetTimeOut(const double timefromnow)
{
  context->TimeOut(this,timefromnow);
}


bool Node::Matches(const Node &rhs) const
{
  return number==rhs.number;
}


#if defined(GENERIC)
void Node::LinkHasBeenUpdated(const Link *l)
{
  cerr << *this << " got a link update: "<<*l<<endl;
  //Do Something generic:
  SendToNeighbors(new RoutingMessage);
}


void Node::ProcessIncomingRoutingMessage(const RoutingMessage *m)
{
  cerr << *this << " got a routing messagee: "<<*m<<" Ignored "<<endl;
}


void Node::TimeOut()
{
  cerr << *this << " got a timeout: ignored"<<endl;
}

Node *Node::GetNextHop(const Node *destination) const
{
  return 0;
}

Table *Node::GetRoutingTable() const
{
  return new Table;
}


ostream & Node::Print(ostream &os) const
{
  os << "Node(number="<<number<<", lat="<<lat<<", bw="<<bw<<")" << endl;
  os << *GetRoutingTable() << endl;
  return os;
}

#endif

#if defined(LINKSTATE)

bool Node::hasReceived(const RoutingMessage* m) {
  return messagesID.find(m -> GetMessageID()) != messagesID.end();
}

void Node::receive(const RoutingMessage* m) {
  messagesID.insert(m -> GetMessageID());
}

void Node::LinkHasBeenUpdated(const Link *l)
{
  cerr << *this<<": Link Update: "<<*l<<endl;
  routingTable.AddLink(l -> GetSrc(), l -> GetDest(), l -> GetLatency());
  sent = true;
  SetTimeOut(10);
}


void Node::ProcessIncomingRoutingMessage(const RoutingMessage *m)
{
  unsigned src;
  VL dests;
  RoutingMessage* message;
  deque<Node*> neighbors;

  cerr << *this << " Routing Message: "<<*m << endl;
  if (hasReceived(m)) {
    cerr << *this << "Already receive such message!" << endl;
    return;
  }
  receive(m);
  src = m -> GetSrcNum();
  dests = m -> GetDestsInfo();
  routingTable.AddLinks(src, dests);

  message = new RoutingMessage(*m);
  message -> mark(this -> number);
  neighbors = *GetNeighbors();
  for (deque<Node*>::iterator it = neighbors.begin();
        it != neighbors.end(); it++)
  {
     if (!message -> isMarked((*it) -> number)) {
       SendToNeighbor(*it, message);
     }
  }
  SetTimeOut(10);
}

void Node::TimeOut()
{
  cerr << *this << " got a timeout: send data and execute Dijkstra"<<endl;

  if (sent) {
    RoutingMessage* message = new RoutingMessage();
    message -> SetSrcNum(number);
    message -> SetDestsInfo(routingTable.GetGraph()[number]);
    message -> mark(number);
    sent = false;
    SendToNeighbors(message);
  }
  routingTable.UpdateCheck();
}

Node *Node::GetNextHop(const Node *destination) const
{
  return new Node(routingTable.GetNext(destination -> number), context, 0, 0);
}

Table *Node::GetRoutingTable() const
{
  return new Table(routingTable);
}


ostream & Node::Print(ostream &os) const
{
  os << "Node(number="<<number<<", lat="<<lat<<", bw="<<bw<<")" << endl;
  os << routingTable << endl;
  return os;
}
#endif


#if defined(DISTANCEVECTOR)

bool Node::hasReceived(const RoutingMessage* m) {
  return messagesID.find(m -> GetMessageID()) != messagesID.end();
}

void Node::receive(const RoutingMessage* m) {
  messagesID.insert(m -> GetMessageID());
}

void Node::LinkHasBeenUpdated(const Link *l)
{
  // update our table
  // send out routing mesages
  cerr << *this<<": Link Update: "<<*l<<endl;
  if (routingTable.AddLink(l -> GetDest(), l -> GetLatency())) {
    deque<Node*> neighbors = *GetNeighbors();
    for (deque<Node*>::iterator it = neighbors.begin();
          it != neighbors.end(); it++)
    {
       SendToNeighbor(*it, new RoutingMessage(response
          , number, routingTable.GetLength(), routingTable.FilteredDistance((**it).GetNumber())));
    }
  }
}


void Node::ProcessIncomingRoutingMessage(const RoutingMessage *m)
{
  cerr << *this << " Routing Message: "<<*m << endl;
  if (hasReceived(m)) {
    cerr << *this << "Already receive such message!" << endl;
    return;
  }
  receive(m);
  RoutingMessageType type = m -> GetType();
  unsigned srcNum = m -> GetSrcNum();
  unsigned length = m -> GetLength();
  if (type == request) {
    VD distance = routingTable.GetDistance(length);
    routingTable.CheckLength(length);
    RoutingMessage* message = new RoutingMessage(response, number, routingTable.GetLength(), routingTable.FilteredDistance(srcNum));
    SendToNeighbor(new Node(srcNum,0,0,0), message);
  } else {
    if (routingTable.NodeChange(srcNum, length, m -> GetDistance())) {
      deque<Node*> neighbors = *GetNeighbors();
      for (deque<Node*>::iterator it = neighbors.begin();
            it != neighbors.end(); it++)
      {
         SendToNeighbor(*it, new RoutingMessage(response
            , number, routingTable.GetLength(), routingTable.FilteredDistance((**it).GetNumber())));
      }
    }
  }
  cout << routingTable << endl;
}

void Node::TimeOut()
{
  cerr << *this << " got a timeout: ignored"<<endl;
}


Node *Node::GetNextHop(const Node *destination) const
{
  return new Node(routingTable.GetNext(destination -> GetNumber()), 0, 0, 0);
}

Table *Node::GetRoutingTable() const
{
  return new Table(routingTable);
}


ostream & Node::Print(ostream &os) const
{
  os << "Node(number="<<number<<", lat="<<lat<<", bw="<<bw << endl;
  os << routingTable << endl;
  return os;
}
#endif
