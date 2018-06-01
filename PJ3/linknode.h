//
// Created by gqt on 5/17/18.
//

#ifndef ROUTESIM_LINKNODE_H
#define ROUTESIM_LINKNODE_H
struct LinkNode {
  unsigned num;
  double lat;
  LinkNode(): num(0), lat(0) {}
  LinkNode(unsigned n, double l): num(n), lat(l) {}
  LinkNode(const LinkNode& b): num(b.num), lat(b.lat) {}
  bool operator()(const LinkNode& a, const LinkNode& b) {return a.lat > b.lat;}
  bool operator== (const LinkNode& b) {return num == b.num;}
  LinkNode& operator=(const LinkNode& b) {return *(new(this)LinkNode(b));}
};

typedef vector<LinkNode> VL;
typedef vector<vector<LinkNode> > VVL;

#endif //ROUTESIM_LINKNODE_H
