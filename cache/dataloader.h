#ifndef _DATALOADER_H_
#define _DATALOADER_H_

#include "utils.h"
#include <bits/stdc++.h>

using namespace std;

template <int n_attr> class Dataloader {
public:
  Dataloader(string filename) : filename(filename), data(filename) {}

  virtual void clear() = 0;
  virtual bool next(Item<n_attr> &item) = 0;

public:
  ifstream data;
  string filename;
};

template <int n_attr> class ZipfDataloader : public Dataloader<n_attr> {
public:
  ZipfDataloader(string filename)
      : e(0), rdist(0, 1), Dataloader<n_attr>(filename) {}

  void clear() {
    sgn.clear();
    Dataloader<n_attr>::data.close();
    Dataloader<n_attr>::data.open(Dataloader<n_attr>::filename);
  }

  bool next(Item<n_attr> &item) {
    if (Dataloader<n_attr>::data.read(reinterpret_cast<char *>(&item.key),
                                      sizeof(item.key))) {
      if (sgn.count(item.key) == 0) {
        sgn[item.key] = Vect<n_attr>::randVec();
      }
      Vect<n_attr> attr = Vect<n_attr>::randVec();

      attr = attr * rdist(e);
      for (int _ = 0; _ < n_attr; _++) {
        item.attr[_] = fabs(attr[_]);
      }

      return true;
    }
    return false;
  }

public:
  default_random_engine e;
  uniform_real_distribution<float> rdist;

  unordered_map<uint32_t, Vect<n_attr>> sgn;
};

template <int n_attr> class CacheDataloader : public Dataloader<n_attr> {
public:
  CacheDataloader(string filename)
      : e(0), rdist(0, 1), Dataloader<n_attr>(filename) {
    assert(n_attr == 2);
  }

  void clear() {
    volumn.clear();
    Dataloader<n_attr>::data.close();
    Dataloader<n_attr>::data.open(Dataloader<n_attr>::filename);
  }

  bool next(Item<n_attr> &item) {
    if (Dataloader<n_attr>::data.read(reinterpret_cast<char *>(&item.key),
                                      sizeof(item.key))) {
      item.attr[0] = 1;
      if (volumn.count(item.key) == 0) {
        item.attr[1] = rdist(e) * 100;
      } else {
        item.attr[1] = volumn[item.key] * (rdist(e) * 1e-2 - 2e-3);
      }
      volumn[item.key] += item.attr[1];

      // Dataloader<n_attr>::data.seekg(17, ios::cur);

      return true;
    }
    return false;
  }

public:
  default_random_engine e;
  uniform_real_distribution<float> rdist;

  unordered_map<uint32_t, float> volumn;
};

#endif
