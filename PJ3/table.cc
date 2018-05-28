#include "table.h"
#include "error.h"
#include <algorithm>

using namespace std;

#define max(a,b) ((a) > (b) ? (a) : (b))

#if defined(GENERIC)
ostream & Table::Print(ostream &os) const
{
  // WRITE THIS
  os << "Table()";
  return os;
}
#endif

#if defined(LINKSTATE)

Table::Table() {
  throw LinkStateException();
}

Table::Table(unsigned num): nodeNum(num), nodeLength(0), updated(false){}

Table::Table(const Table& table): nodeNum(table.nodeNum), nodeLength(table.nodeLength), graph(table.graph), distance(table.distance), next(table.next), updated(false) {}

bool Table::operator()(unsigned num1, unsigned num2) {
  return distance[num1] > distance[num2];
}

Table& Table::operator=(const Table& t) {
  return *(new (this)Table(t));
}

VVL Table::GetGraph() const {
  return graph;
}

bool Table::isUpdated() {
  return updated;
}

void Table::UpdateCheck() {
  if (updated) {
    Dijkstra();
    updated = false;
  }
}

void Table::Dijkstra() {
  VI pre(nodeLength, -1);
  priority_queue<LinkNode, vector<LinkNode>, LinkNode> q;
  LinkNode mark;
  unsigned num, length, src, node, lat, i;

  distance.clear();
  next.clear();
  distance = VD(nodeLength, INF);
  next = VI(nodeLength, -1);
  src = nodeNum;
  distance[src] = 0;
  pre[src] = src;
  length = nodeLength;
  q.push(LinkNode(src, 0));

  while (!q.empty()) {
    mark = q.top();
    q.pop();
    num = mark.num;
    lat = mark.lat;
    if (distance[num] < lat)
      continue;
    for (i = 0; i < graph[num].size(); i++) {
      node = graph[num][i].num;
      lat = graph[num][i].lat;
      if (distance[num] + lat < distance[node]) {
        distance[node] = distance[num] + lat;
        pre[node] = num;
        q.push(LinkNode(node, distance[node]));
      }
    }
  }

  for (i = 0; i < length; i++) {
    num = i;
    while (pre[num] != -1 && pre[num] != src) {
      num = pre[num];
    }
    next[i] = pre[num] == -1 ? -1 : num;
  }
}

//void Table::AddNode(const Node* node) {
//  nodeLength++;
//  updated = true;
//}

void Table::AddLinks(unsigned src, const VL& dests) {
  unsigned i;

  for (i = 0; i < dests.size(); i++) {
    AddLink(src, dests[i].num, dests[i].lat);
  }
}

void Table::AddLink(unsigned src, unsigned dest, double lat) {
  unsigned size = max(src, dest) + 1;
  VL::iterator it;

  if (graph.size() < size) {
    graph.resize(size, VL());
    nodeLength = size;
  }

  if ((it = find(graph[src].begin(), graph[src].end(), LinkNode(dest, 0))) != graph[src].end()) {
    it -> lat = lat;
  } else {
    graph[src].push_back(LinkNode(dest, lat));
  }
  updated = true;

}

unsigned Table::GetNext(unsigned dest) const {
  return (unsigned)next[dest];
}

ostream & Table::Print(ostream &os) const {
  unsigned i;

  os << "ID: " << nodeNum << endl;
  os << "Destination" << " " << "Distance" << " " << "Next" << endl;
  for (i = 0; i < next.size(); i++) {
    os << i << " " << distance[i] << " " << next[i] << endl;
  }
  os << endl;
  return os;
}

#endif

#if defined(DISTANCEVECTOR)

Table::Table() {
  throw DistanceVectorException();
}
Table::Table(unsigned num): nodeNum(num) {
  resize(num + 1);
  distance[num] = 0;
  neighborsCost[num] = 0;
  next[num] = num;
}

Table::Table(const Table& table): nodeNum(table.nodeNum)
  , nodeLength(table.nodeLength), next(table.next), distance(table.distance), neighborsCost(table.neighborsCost) {}

void Table::resize(unsigned size) {
  map<unsigned, VD>::iterator it;

  next.resize(size, -1);
  distance.resize(size, INF);
  neighborsCost.resize(size, INF);
  nodeLength = size;

  for (it = neighborsDistance.begin(); it != neighborsDistance.end(); it++) {
    (it -> second).resize(size, INF);
  }
}

bool Table::update() {
  unsigned i, j, neighbor;
  map<unsigned, VD>::iterator it;
  VD neighborDistance;
  VD oldDistance = distance;
  VI oldNext = next;

  fill(distance.begin(), distance.end(), INF);
  fill(next.begin(), next.end(), -1);

  distance[nodeNum] = 0;
  next[nodeNum] = nodeNum;

  for (it = neighborsDistance.begin(); it != neighborsDistance.end(); it++) {
    neighbor = it -> first;
    neighborDistance = it -> second;
    for (i = 0; i < neighborDistance.size(); i++) {
      if (neighborsCost[neighbor] + neighborDistance[i] < distance[i]) {
        distance[i] = neighborsCost[neighbor] + neighborDistance[i];
        next[i] = neighbor;
      }
    }
  }

  return oldDistance != distance || oldNext != next;
}

bool Table::NodeChange(unsigned neighbor, unsigned nLength, const VD& nDistance) {
  CheckLength(nLength);
  neighborsDistance[neighbor] = nDistance;
  if (nLength < nodeLength) {
    neighborsDistance[neighbor].resize(nodeLength, INF);
  }
  return update();
}

bool Table::AddLink(unsigned dest, double lat) {
  unsigned i;
  VD tmp;

  CheckLength(dest + 1);
  if (neighborsCost[dest] == lat) {
    return false;
  }
  neighborsCost[dest] = lat;
  if (neighborsDistance.find(dest) == neighborsDistance.end()) {
    tmp = VD(nodeLength, INF);
    tmp[dest] = 0;
    neighborsDistance[dest] = tmp;
  }
  return update();
}

VD Table::FilteredDistance(unsigned destNum) const {
  VD resDistance = distance;
  unsigned i;

  for (i = 0; i < nodeLength; i++) {
    if (next[i] == destNum) {
      resDistance[i] = INF;
    }
  }
  return resDistance;
}

VD Table::GetDistance(unsigned nLength) {
  CheckLength(nLength);
  return distance;
}

void Table::CheckLength(unsigned length) {
  if (length > nodeLength) {
    resize(length);
  }
}

unsigned Table::GetLength() const {
  return nodeLength;
}

unsigned Table::GetNext(unsigned dest) const {
  return (unsigned)next[dest];
}

ostream & Table::Print(ostream &os) const {
  unsigned i;

  os << "ID: " << nodeNum << endl;
  os << "Destination Next Distance" << endl;
  for (i = 0; i < next.size(); i++) {
    os << i << " " << next[i] << " " << distance[i] << endl;
  }
  os << endl;
  return os;

}

#endif
