#ifndef _EXPERIMENT_H_
#define _EXPERIMENT_H_

#include "dataloader.h"
#include "strawman.h"
#include "utils.h"

template <int n_attr, int n_item, int n_expr> class Experimentor {
public:
  int evalCache(double alpha, double thresh, Base<n_attr> *base = nullptr) {
    attr.clear();

    vector<pair<uint32_t, uint32_t>> cache;
    cache.clear();
    float capacity = 100000;

    int nb_hit = 0;
    int nb_new = 0;
    double AAE = 0;
    for (int i = 0; i < n_item; i++) {
      uint32_t key = items[i].key;
      attr[key] = attr[key] + items[i].attr;

      if (base) {
        base->insert(items[i]);
      }

      assert(attr[key][1] >= 0);

      bool hit = false;
      for (int j = 0; j < cache.size(); j++) {
        if (cache[j].second == key) {
          cache[j].first = i;
          hit = true;

          capacity -= items[i].attr[1];
          if (capacity < 0) {
            sort(cache.rbegin(), cache.rend());
            while (!cache.empty() && capacity < 0) {
              capacity += attr[cache[cache.size() - 1].second][1];
              cache.pop_back();
            }
          }
        }
      }

      if (hit == true) {
        if (i > n_item / 2) {
          nb_hit += 1;
        }
      } else {
        if (i > n_item / 2 && attr[key][0] == 1) {
          nb_new += 1;
        }

        bool admission = false;
        if (base) {
          Item<n_attr> item = items[i];
          item.attr[0] = 0;
          item.attr[1] = 0;
          base->query(item);
          if (item.attr[0] - item.attr[1] * alpha > thresh) {
            admission = true;
          }

          double esti = item.attr[0] - item.attr[1] * alpha;
          double real = attr[key][0] - attr[key][1] * alpha;
          AAE += fabs(real - esti);

          // cout << admission << " " << item.attr[0] << " " << item.attr[1] <<
          // " "
          //      << attr[key][0] << " " << attr[key][1] << endl;
        } else {
          if (attr[key][0] - attr[key][1] * alpha > thresh) {
            admission = true;
          }
        }

        if (admission) {
          sort(cache.rbegin(), cache.rend());
          while (!cache.empty() && capacity < attr[key][1]) {
            capacity += attr[cache[cache.size() - 1].second][1];
            cache.pop_back();
          }
          if (capacity >= attr[key][1]) {
            cache.push_back(make_pair(i, key));
            capacity -= attr[key][1];
          }
        }
      }
    }
    cout << alpha << " " << thresh << " " << nb_hit << "/"
         << n_item / 2 - nb_new << " " << AAE / n_item << endl;

    return nb_hit;
  }

  void prefetch(vector<uint32_t> &keys,
                unordered_map<uint32_t, Vect<n_attr>> &attr, Vect<n_attr> util,
                vector<Item<n_attr>> &ret, int K) {
    Item<n_attr> item;
    for (auto key : keys) {
      item.key = key;
      item.attr = attr[key];
      ret.push_back(item);
    }

    sort(ret.begin(), ret.end(), [=](Item<n_attr> a, Item<n_attr> b) {
      return a.attr * util > b.attr * util;
    });
    ret.resize(K);
  }

  void experiment(Dataloader<n_attr> *data, Base<n_attr> *base,
                  Evaluator<n_attr> *eval, string algoname) {
    data->clear();
    base->clear();

    for (int i = 0; i < n_item; i++) {
      Item<n_attr> item;
      data->next(item);
      items.push_back(item);
    }

    // webpage
    // double chooseAlpha = 0.24;
    // double chooseThresh = -2;

    // zipfian 1.0
    double chooseAlpha = 0.14;
    double chooseThresh = 0.0;

    // zipfian 1.1
    // double chooseAlpha = 0.35;
    // double chooseThresh = -8;

    // CAIDA
    // double chooseAlpha = 0.06;
    // double chooseThresh = -2;

    // int chooseHit = 0;
    // for (double alpha = 0.2; alpha <= 0.3 + 1e-6; alpha += 0.02) {
    //   for (double thresh = -5.0; thresh <= 0.0 + 1e-6; thresh += 1.0) {
    //     int nb_hit = evalCache(alpha, thresh);
    //     if (nb_hit > chooseHit) {
    //       chooseHit = nb_hit;
    //       chooseAlpha = alpha;
    //       chooseThresh = thresh;
    //     }
    //   }
    // }

    cout << algoname << endl;
    evalCache(chooseAlpha, chooseThresh);
    evalCache(chooseAlpha, chooseThresh, base);

    keys.clear();
    freq.clear();
    attr.clear();

    for (int i = 0; i < n_item; i++) {
      if (freq[items[i].key] == 0) {
        keys.push_back(items[i].key);
      }

      freq[items[i].key] += 1;
      attr[items[i].key] = attr[items[i].key] + items[i].attr;
    }

    int K = 100;
    double score = 0;
    vector<Item<n_attr>> gTopk, qTopk;

    Vect<n_attr> util;
    util[0] = 1.0;
    util[1] = -chooseAlpha;
    prefetch(keys, attr, util, gTopk, K);
    base->topk(util, qTopk, K);

    unordered_set<uint32_t> topKey;
    for (int i = 0; i < K; i++) {
      topKey.insert(gTopk[i].key);
    }
    for (int i = 0; i < K; i++) {
      if (topKey.count(qTopk[i].key) != 0) {
        score += 1.0 / K;
      }
    }
    cout << keys.size() << " " << score << endl;
  }

public:
  vector<uint32_t> keys;
  vector<Item<n_attr>> items;

  unordered_map<uint32_t, uint32_t> freq;
  unordered_map<uint32_t, Vect<n_attr>> attr;
};

#endif
