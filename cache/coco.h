#ifndef _COCO_H_
#define _COCO_H_

#include "strawman.h"
#include "utils.h"

template <int n_attr, int d, int w> class CocoSketch : public Base<n_attr> {
public:
  CocoSketch() : e(0), dist(0, 1) {}

  void clear() {
    for (int i = 0; i < d; i++) {
      for (int j = 0; j < w; j++) {
        bucket[i][j].clear();
      }
    }
  }

  void insert(Item<n_attr> &item) {
    uint32_t idx;

    int minRow = -1;
    float minScore;

    for (int _ = 0; _ < d; _++) {
      MurmurHash3_x86_32(&item.key, sizeof(item.key), idxSeed + _, &idx);
      idx %= w;

      if (bucket[_][idx].key == item.key) {
        bucket[_][idx].attr = bucket[_][idx].attr + item.attr;
        return;
      } else {
        float curScore = bucket[_][idx].attr.norm2();
        if (minRow < 0 || curScore < minScore) {
          minRow = _;
          minScore = curScore;
        }
      }
    }

    MurmurHash3_x86_32(&item.key, sizeof(item.key), idxSeed + minRow, &idx);
    idx %= w;

    float score = item.attr.norm2();
    if (dist(e) <= score / (score + minScore)) {
      bucket[minRow][idx] = item;
      bucket[minRow][idx].attr =
          bucket[minRow][idx].attr * ((score + minScore) / score);
    } else {
      bucket[minRow][idx].attr =
          bucket[minRow][idx].attr * ((score + minScore) / minScore);
    }
  }

  void topk(Vect<n_attr> &util, vector<Item<n_attr>> &ret, int K) {
    for (int _ = 0; _ < d; _++) {
      for (int idx = 0; idx < w; idx++) {
        if (bucket[_][idx].key > 0) {
          ret.push_back(bucket[_][idx]);
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

    for (int _ = 0; _ < d; _++) {
      MurmurHash3_x86_32(&item.key, sizeof(item.key), idxSeed + _, &idx);
      idx %= w;

      if (bucket[_][idx].key == item.key) {
        item = bucket[_][idx];
        return;
      }
    }
  }

public:
  Item<n_attr> bucket[d][w];

  const uint32_t idxSeed = 0x2000;

  default_random_engine e;
  uniform_real_distribution<float> dist;
};

#endif
