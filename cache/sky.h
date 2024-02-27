#ifndef _SKY_H_
#define _SKY_H_

#include "strawman.h"

template <int n_attr, int n_samp, int w, int d, int scoreType>
class SkySketch : public Base<n_attr> {
public:
  SkySketch() : e(0), dist(0, 1) {}

  void clear() {
    for (int i = 0; i < w; i++) {
      for (int j = 0; j < d; j++) {
        bucket[i][j].clear();
      }
    }
    for (int i = 0; i < n_samp; i++) {
      sample[i].clear();
    }
  }

  Vect<n_attr> getCenter() {
    Vect<n_attr> center;

    int np = 0;
    for (int _ = 0; _ < n_samp; _++) {
      if (sample[_].key > 0) {
        center = center + sample[_].attr;
        np += 1;
      }
    }

    if (np > 0) {
      center = center * (1.0 / np);
    }
    return center;
    // return sumAttr * (1.0 / n_item);
  }

  float calScore1(Item<n_attr> item) {
    if (item.key > 0) {
      return item.attr.norm2();
    } else {
      return -1.0;
    }
  }

  float calScore2(Item<n_attr> item) {
    if (item.key > 0) {
      Vect<n_attr> dir = getCenter() + item.attr * -1.0;
      return dir.norm2();
    } else {
      return -1.0;
    }
  }

  float calScore3(Item<n_attr> item) {
    if (item.key > 0) {
      Vect<n_attr> dir = getCenter() + item.attr * -1.0;

      float ret = 0;
      for (int _ = 0; _ < n_samp; _++) {
        if (sample[_].key > 0) {
          Vect<n_attr> curDir = sample[_].attr + item.attr * -1.0;
          if (curDir * dir > 0) {
            ret += 1;
          }
          if (curDir * dir < 0) {
            ret -= 1;
          }
        }
      }
      return max(ret, -ret);
    } else {
      return -1.0;
    }
  }

  float calScore4_2D_1(Item<2> item) {

    static const float pi = acos(-1);
    vector<float> angles;
    for (int i = 0; i < n_samp; i++) {
      if (sample[i].key == 0)
        continue;
      Vect<n_attr> dir = sample[i].attr + item.attr * -1.0;
      angles.push_back(atan2(dir.attr[1], dir.attr[0]));
    }

    int n = angles.size();

    sort(angles.begin(), angles.end());
    for (int i = 0; i < n; i++) {
      angles.push_back(angles[i] + 2 * pi);
    }

    int max_one_direction = 0;
    for (int i = 0, l = 0; i < 2 * n; i++) {
      while (l <= i && angles[l] + pi <= angles[i]) {
        l++;
      }
      max_one_direction = max(max_one_direction, i - l);
    }

    return abs(2 * max_one_direction - n);
  }

  float calScore4_2D_2(Item<2> item) {

    static const float pi = acos(-1);
    vector<float> angles;
    for (int i = 0; i < n_samp; i++) {
      if (sample[i].key == 0)
        continue;
      Vect<n_attr> dir = sample[i].attr + item.attr * -1.0;
      angles.push_back(atan2(dir.attr[1], dir.attr[0]));
    }

    int n = angles.size();

    sort(angles.begin(), angles.end());
    for (int i = 0; i < n; i++) {
      angles.push_back(angles[i] + 2 * pi);
    }

    float sum_scores = 0;
    float sum_angles = 0;
    for (int i = 0, l = 0; i < 2 * n; i++) {
      while (l <= i && angles[l] + pi <= angles[i]) {
        l++;
      }
      if (i > 0) {
        // this is not 100% correct. but it's roughly there.
        // to correct it, use "add(angles[i])" and "delete(angles[i] + pi)"
        // events.
        sum_scores += (angles[i] - angles[i - 1]) * abs(2 * (i - l) - n);
        sum_angles += angles[i] - angles[i - 1];
      }
    }

    return sum_scores / sum_angles;
  }

  float calScore4(Item<n_attr> item) {
    assert(n_attr == 3 || n_attr == 2);

    if (item.key > 0) {
      float ret = 0;
      if (n_attr == 2) {
        ret = calScore4_2D_1(item);
        /*
        for (int i = 0; i < n_samp; i++) {
          Vect<n_attr> dir = ~(sample[i].attr + item.attr * -1.0);
          float curRet = 0;
          for (int k = 0; k < n_samp; k++) {
            if (sample[k].key > 0) {
              Vect<n_attr> curDir = sample[k].attr + item.attr * -1.0;
              if (curDir * dir > 0) {
                curRet += 1;
              }
              if (curDir * dir < 0) {
                curRet -= 1;
              }
            }
          }
          ret = max(ret, curRet);
          ret = max(ret, -curRet);
        }
        */
      }
      if (n_attr == 3) {
        for (int i = 0; i < n_samp; i++) {
          for (int j = 0; j < n_samp; j++) {
            Vect<n_attr> dir = (sample[i].attr + item.attr * -1.0) ^
                               (sample[j].attr + item.attr * -1.0);
            float curRet = 0;
            for (int k = 0; k < n_samp; k++) {
              if (sample[k].key > 0) {
                Vect<n_attr> curDir = sample[k].attr + item.attr * -1.0;
                if (curDir * dir > 0) {
                  curRet += 1;
                }
                if (curDir * dir < 0) {
                  curRet -= 1;
                }
              }
            }
            ret = max(ret, curRet);
          }
        }
      }
      return ret;
    } else {
      return -1.0;
    }
  }

  float calScore5(Item<n_attr> item) {
    assert(n_attr == 2);
    if (item.key > 0) {
      return calScore4_2D_2(item);
    } else {
      return -1.0;
    }
  }

  void insert(Item<n_attr> &item) {
    switch (scoreType) {
    case 1:
      insert(item, &SkySketch<n_attr, n_samp, w, d, scoreType>::calScore1);
      return;
    case 2:
      insert(item, &SkySketch<n_attr, n_samp, w, d, scoreType>::calScore2);
      return;
    case 3:
      insert(item, &SkySketch<n_attr, n_samp, w, d, scoreType>::calScore3);
      return;
    case 4:
      insert(item, &SkySketch<n_attr, n_samp, w, d, scoreType>::calScore4);
      return;
    case 5:
      insert(item, &SkySketch<n_attr, n_samp, w, d, scoreType>::calScore5);
      return;
    default:
      assert(0);
    }
  }

  void insert(Item<n_attr> &item,
              float (SkySketch<n_attr, n_samp, w, d, scoreType>::*calScore)(
                  Item<n_attr>)) {
    uint32_t idx, rep;
    MurmurHash3_x86_32(&item.key, sizeof(item.key), idxSeed, &idx);
    MurmurHash3_x86_32(&item.key, sizeof(item.key), repSeed, &rep);
    idx %= w;

    int minRow = -1, minSamp = -1;
    float minScore, minRep;

    for (int _ = 0; _ < d; _++) {
      if (bucket[idx][_].key == item.key) {
        bucket[idx][_].attr = bucket[idx][_].attr + item.attr;
        // sumAttr = sumAttr + item.attr;

        for (int samp = 0; samp < n_samp; samp++) {
          if (sample[samp].key == bucket[idx][_].key) {
            sample[samp].attr = sample[samp].attr + item.attr;
          }
        }
        return;
      } else {
        float curScore = (this->*calScore)(bucket[idx][_]);
        if (minRow < 0 || curScore < minScore) {
          minRow = _;
          minScore = curScore;
        }
      }
    }

    float score = (this->*calScore)(item);
    // if (minScore < 0 ||
    //     dist(e) < pow(score, 1) / (pow(score, 1) + pow(minScore, 1))) {
    // if (minScore < 0 || dist(e) < exp(score) / (exp(score) + exp(minScore)))
    // {
    if (temperature > 0.1) {
      temperature *= 0.999;
    }
    if (minScore <= 0 || score > minScore ||
        dist(e) < exp((score - minScore) / temperature)) {
      // if (minScore < 0 || dist(e) < score / (score +
      // minScore)) {
      for (int samp = 0; samp < n_samp; samp++) {
        if (sample[samp].key == bucket[idx][minRow].key) {
          sample[samp] = item;
          minRep = rep;
          break;
        } else {
          uint32_t curRep = 0;
          if (sample[samp].key > 0) {
            MurmurHash3_x86_32(&sample[samp].key, sizeof(sample[samp].key),
                               repSeed, &curRep);
          }
          if (minSamp == -1 || curRep < minRep) {
            minSamp = samp;
            minRep = curRep;
          }
        }
      }
      if (minRep < rep) {
        sample[minSamp] = item;
      }

      // if (minScore < 0) {
      //   n_item += 1;
      // }
      // sumAttr = sumAttr + bucket[idx][minRow].attr * -1.0;
      bucket[idx][minRow] = item;
      // sumAttr = sumAttr + bucket[idx][minRow].attr;
    }

    // for (int samp = 0; samp < n_samp; samp++) {
    //   cout << sample[samp].key << ", ";
    // }
    // cout << endl;
  }

  void topk(Vect<n_attr> &util, vector<Item<n_attr>> &ret, int K) {
    // cout << "n_item: " << n_item << endl;
    // cout << "center: " << getCenter() << endl;

    for (int idx = 0; idx < w; idx++) {
      for (int _ = 0; _ < d; _++) {
        if (bucket[idx][_].key > 0) {
          ret.push_back(bucket[idx][_]);
        }
      }
    }
    sort(ret.begin(), ret.end(), [=](Item<n_attr> a, Item<n_attr> b) {
      return a.attr * util > b.attr * util;
    });

    assert(ret.size() >= K);
    ret.resize(K);
  }

  void query(Item<n_attr> &item) {
    uint32_t idx;
    MurmurHash3_x86_32(&item.key, sizeof(item.key), idxSeed, &idx);
    idx %= w;

    for (int _ = 0; _ < d; _++) {
      if (bucket[idx][_].key == item.key) {
        item = bucket[idx][_];
        return;
      }
    }
  }

public:
  Item<n_attr> bucket[w][d];
  Item<n_attr> sample[n_samp];

  const uint32_t idxSeed = 0x2000;
  const uint32_t repSeed = 0x3000;

  default_random_engine e;
  uniform_real_distribution<float> dist;

  double temperature = 100;

  // int n_item = 0;
  // Vect<n_attr> sumAttr;
};

#endif
