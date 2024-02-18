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
      : e(time(0)), rdist(0, 1), Dataloader<n_attr>(filename) {}

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

      // if (sgn[item.key] * attr > 0) {
      //   item.attr = item.attr + attr * rdist(e);
      // } else {
      //   item.attr = item.attr + attr * -rdist(e);
      // }

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

template <int n_attr> class CriteoDataLoader : public Dataloader<n_attr> {
public:
  CriteoDataLoader(string filename, vector<int> _columnIndices)
      : e(time(0)), rdist(0, 1), Dataloader<n_attr>(filename), columnIndices(_columnIndices) {}
  
  void clear() {
    sgn.clear();
    Dataloader<n_attr>::data.close();
    Dataloader<n_attr>::data.open(Dataloader<n_attr>::filename);
  }

  

  bool next(Item<n_attr> &item) {

    string line;
    if (getline(Dataloader<n_attr>::data, line)) {

      stringstream liness(line);
      
      vector<int> lineData;
      for (int i = 0, x; i < 14; i++) {
        liness >> x;
        lineData.push_back(x);
      }

      liness >> hex >> item.key;
      
      for (int _ = 0; _ < n_attr; _++) {
        item.attr[_] = lineData[columnIndices[_]];
      }

      return true;
    }
    return false;
  }

      
public:
  default_random_engine e;
  uniform_real_distribution<float> rdist;

  unordered_map<uint32_t, Vect<n_attr>> sgn;

  vector<int> columnIndices;// data column indices
};

template <int n_attr> class NBADataLoader : public Dataloader<n_attr> {
public:
  NBADataLoader(string filename)
      : e(time(0)), rdist(0, 1), Dataloader<n_attr>(filename) {}
  
  void clear() {
    sgn.clear();
    Dataloader<n_attr>::data.close();
    Dataloader<n_attr>::data.open(Dataloader<n_attr>::filename);
  }

  

  bool next(Item<n_attr> &item) {

    string line;
    while (getline(Dataloader<n_attr>::data, line)) {

      stringstream liness(line);

      uint32_t id;
      string MIN;
      int PTS;
      
      liness >> id >> MIN >> PTS;
      
      if (count(MIN.begin(), MIN.end(), ':') == 0)
        continue;
      
      if (count(MIN.begin(), MIN.end(), ':') == 2) {
        MIN = MIN.substr(0, MIN.length() - 3);
      }

      MIN[MIN.find(':')] = ' ';

      stringstream MINss(MIN);
      double minutes, seconds;
      MINss >> minutes >> seconds;

      item.key = id;
      item.attr[0] = (int)(minutes * 60 + seconds + 0.001);
      item.attr[1] = PTS;
      
      return true;
    }
    return false;
  }

      
public:
  default_random_engine e;
  uniform_real_distribution<float> rdist;

  unordered_map<uint32_t, Vect<n_attr>> sgn;

};



#endif
