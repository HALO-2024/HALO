#ifndef _UTILS_H_
#define _UTILS_H_

#include "murmur3.h"
#include <bits/stdc++.h>

using namespace std;

template <int n_attr> class Vect {
public:
  Vect() { memset(attr, 0, sizeof(attr)); }

  void clear() { memset(attr, 0, sizeof(attr)); }

  float &operator[](int _) { return attr[_]; }

  Vect<n_attr> operator+(const Vect<n_attr> &other) const {
    Vect<n_attr> ret;
    for (int _ = 0; _ < n_attr; _++) {
      ret[_] = attr[_] + other.attr[_];
    }
    return ret;
  }

  float operator*(const Vect<n_attr> &other) const {
    float ret = 0;
    for (int _ = 0; _ < n_attr; _++) {
      ret += attr[_] * other.attr[_];
    }
    return ret;
  }

  Vect<n_attr> operator~() const {
    assert(n_attr == 2);

    Vect<n_attr> ret;
    ret[0] = attr[1] * 1.0;
    ret[1] = attr[0] * -1.0;
    return ret;
  }

  Vect<n_attr> operator^(const Vect<n_attr> &other) const {
    assert(n_attr == 3);

    Vect<n_attr> ret;
    ret[0] = attr[1] * other.attr[2] - other.attr[1] * attr[2];
    ret[1] = attr[2] * other.attr[0] - other.attr[2] * attr[0];
    ret[2] = attr[0] * other.attr[1] - other.attr[0] * attr[1];
    return ret;
  }

  Vect<n_attr> operator*(const float other) const {
    Vect<n_attr> ret;
    for (int _ = 0; _ < n_attr; _++) {
      ret[_] = attr[_] * other;
    }
    return ret;
  }

  float norm2() { return pow((*this) * (*this), 1.0 / 2.0); }

  static Vect<n_attr> randVec() {
    static default_random_engine e(time(0));
    static normal_distribution<float> ndist(0, 1);

    Vect<n_attr> ret;
    do {
      for (int _ = 0; _ < n_attr; _++) {
        ret[_] = ndist(e);
      }
    } while (ret.norm2() == 0.0);
    return ret * (1.0 / ret.norm2());
  }

public:
  float attr[n_attr];
};

template <int n_attr> class Item {
public:
  // float &operator[](int _) { return attr[_]; }

  void clear() {
    key = 0;
    attr.clear();
  }

public:
  uint32_t key = 0;
  Vect<n_attr> attr;
};

template <int d, int w> class CSketch {
public:
  CSketch() { memset(counter, 0, sizeof(counter)); }

  void clear() { memset(counter, 0, sizeof(counter)); }

  void insert(uint32_t key, float val) {
    uint32_t idx, sgn;
    for (int _ = 0; _ < d; _++) {
      MurmurHash3_x86_32(&key, sizeof(key), _ + idxSeed, &idx);
      MurmurHash3_x86_32(&key, sizeof(key), _ + sgnSeed, &sgn);
      counter[_][idx % w] += val * (sgn % 2 == 0 ? 1.0 : -1.0);
    }
  }

  float query(uint32_t key) {
    float ret[d];
    uint32_t idx, sgn;
    for (int _ = 0; _ < d; _++) {
      MurmurHash3_x86_32(&key, sizeof(key), _ + idxSeed, &idx);
      MurmurHash3_x86_32(&key, sizeof(key), _ + sgnSeed, &sgn);
      ret[_] = counter[_][idx % w] * (sgn % 2 == 0 ? 1.0 : -1.0);
    }

    sort(ret, ret + d);
    return (ret[d / 2] + ret[(d - 1) / 2]) / 2;
  }

public:
  float counter[d][w];

  const uint32_t idxSeed = 0x0000;
  const uint32_t sgnSeed = 0x1000;
};

template <int c_heap> class Heap {
public:
  void clear() {
    q.clear();
    iter.clear();
  }

  void insert(uint32_t key, float val) {
    if (iter.count(key) != 0) {
      q.erase(iter[key]);
      iter[key] = q.insert(make_pair(val, key));
    } else if (q.size() < c_heap) {
      iter[key] = q.insert(make_pair(val, key));
    } else if (q.begin()->first < val) {
      iter.erase(q.begin()->second);
      q.erase(q.begin());
      iter[key] = q.insert(make_pair(val, key));
    }
  }

  template <int n_attr> void query(vector<Item<n_attr>> &ret) {
    assert(ret.size() == 0);

    Item<n_attr> item;
    for (auto [key, _] : iter) {
      item.key = key;
      ret.push_back(item);
    }
  }

public:
  multimap<float, uint32_t> q;
  unordered_map<uint32_t, multimap<float, uint32_t>::iterator> iter;
};

template <int n_attr> ostream &operator<<(ostream &out, Vect<n_attr> vec) {
  out << "(";
  for (int _ = 0; _ < n_attr; _++) {
    out << vec[_] << (_ == n_attr - 1 ? ")" : ", ");
  }
  return out;
}

#endif
