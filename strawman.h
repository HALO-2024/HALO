#ifndef _STRAWMAN_H_
#define _STRAWMAN_H_

#include "murmur3.h"
#include "utils.h"

template <int n_attr> class Base {
public:
  virtual void clear() = 0;
  virtual void insert(Item<n_attr> &) = 0;
  virtual void topk(Vect<n_attr> &, vector<Item<n_attr>> &, int) = 0;
  virtual void query(Item<n_attr> &) = 0;
};

template <int n_attr, int c_heap, int d, int w>
class Sketch1Heap : public Base<n_attr> {
public:
  CSketch<d, w> sketch[n_attr];
  Heap<c_heap> hp;

public:
  void clear() {
    for (int _ = 0; _ < n_attr; _++) {
      sketch[_].clear();
    }
    hp.clear();
  }

  void insert(Item<n_attr> &item) {
    Vect<n_attr> ret;
    for (int _ = 0; _ < n_attr; _++) {
      sketch[_].insert(item.key, item.attr[_]);
      ret[_] = sketch[_].query(item.key);
    }
    hp.insert(item.key, ret.norm2());
  }

  void query(Item<n_attr> &item) {
    for (int _ = 0; _ < n_attr; _++) {
      item.attr[_] = sketch[_].query(item.key);
    }
  }

  void topk(Vect<n_attr> &util, vector<Item<n_attr>> &ret, int K) {
    hp.query(ret);
    for (auto &item : ret) {
      for (int _ = 0; _ < n_attr; _++) {
        item.attr[_] = sketch[_].query(item.key);
      }
    }
    sort(ret.begin(), ret.end(), [=](Item<n_attr> a, Item<n_attr> b) {
      return a.attr * util > b.attr * util;
    });
    if (ret.size() < K) {
      cerr << "ret.size = " << ret.size() << ", K = " << K << endl;
      exit(0);
    }
    assert(ret.size() >= K);
    ret.resize(K);
  }
};

#endif
