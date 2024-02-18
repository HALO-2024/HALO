#ifndef _EVAL_H_
#define _EVAL_H_

#include "strawman.h"
#include "utils.h"

template <int n_attr> class Evaluator {
public:
  virtual Vect<5> eval(vector<uint32_t> &,
                      unordered_map<uint32_t, Vect<n_attr>> &,
                      Base<n_attr> *) = 0;
};

template <int n_attr, int n_eval, int K>
class TopKEvaluator : public Evaluator<n_attr> {
public:
  void topk(vector<uint32_t> &keys, unordered_map<uint32_t, Vect<n_attr>> &attr,
            Vect<n_attr> util, vector<Item<n_attr>> &ret) {
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

  /**
   * @param
   * @return Vect<3> of {F1, AAE, ARE, Kendall}
  */

  Vect<5> eval(vector<uint32_t> &keys,
              unordered_map<uint32_t, Vect<n_attr>> &attr, Base<n_attr> *base) {
    
    // calculate precision
    double precision = 0;

    #ifndef p_aimed
    #define p_aimed 0.5
    #endif



    for (int _ = 0; _ < n_eval; _++) {
      double score = 0;
      vector<Item<n_attr>> gTopk, qTopk;

      Vect<n_attr> util = Vect<n_attr>::randVec();


      // 0.5+1/2p'=p_aimed

      if (((double)(rand() % 1024) / 1024) < p_aimed * 2 - 1) {
        assert(n_attr == 2);
        util.attr[1] = abs(util.attr[1]);
      }

      topk(keys, attr, util, gTopk);
      base->topk(util, qTopk, K);

      unordered_set<uint32_t> topKey;
      for (int i = 0; i < K; i++) {
        topKey.insert(gTopk[i].key);
      }

      // printf("util = (%f, %f)\n", util.attr[0], util.attr[1]);

      // for (int i = 0; i < K; i++) {
      //   printf("ground truth: (%.3f, %.3f)\n", gTopk[i].attr[0], gTopk[i].attr[1]);
      // }

      // for (int i = 0; i < K; i++) {
      //   printf("query: (%.3f, %.3f)\n", qTopk[i].attr[0], qTopk[i].attr[1]);
      // }

      for (int i = 0; i < K; i++) {
        if (topKey.count(qTopk[i].key) != 0) {
          score += 1.0 / K;
        }
      }
      precision += score / n_eval;
    }

    // calculate recall
    double recall = 0;
    // for (int _ = 0; _ < n_eval; _++) {
    //   double score = 0;
    //   vector<Item<n_attr>> gTopk, qTopk;

    //   Vect<n_attr> util = Vect<n_attr>::randVec();

    //   if ((double)rand() % 1024 / 1024 < p_aimed) {
    //     assert(n_attr == 2);
    //     util.attr[0] = -abs(util.attr[0]);
    //     util.attr[1] = -abs(util.attr[1]);
    //   }

    //   topk(keys, attr, util, gTopk);
    //   base->topk(util, qTopk, K);

    //   unordered_set<uint32_t> topKey;
    //   for (int i = 0; i < K; i++) {
    //     topKey.insert(qTopk[i].key);
    //   }
    //   for (int i = 0; i < K; i++) {
    //     if (topKey.count(gTopk[i].key) != 0) {
    //       score += 1.0 / K;
    //     }
    //   }
    //   recall += score / n_eval;
    // }

    // calculate AAE and ARE
    // note that this is "like" recall, not precision
    double AAE = 0, ARE = 0;
    for (int _ = 0; _ < n_eval; _++) {
      double tmp_AAE = 0, tmp_ARE = 0;
      vector<Item<n_attr>> gTopk, qTopk;

      
      
      Vect<n_attr> util = Vect<n_attr>::randVec();
      if (((double)(rand() % 1024) / 1024) < p_aimed * 2 - 1) {
        assert(n_attr == 2);
        util.attr[1] = abs(util.attr[1]);
      }

      topk(keys, attr, util, gTopk);
      base->topk(util, qTopk, K);


      // map key to index of qTopk
      unordered_map<uint32_t, uint32_t> topKeyMap;
      for (int i = 0; i < K; i++) {
        topKeyMap[qTopk[i].key] = i;
      }


      int counter_gF_nonzero = 0;
      for (int i = 0; i < K; i++) {
        double qF = 0;
        if (topKeyMap.count(gTopk[i].key) != 0) {
          qF = util * gTopk[topKeyMap[gTopk[i].key]].attr;
        }
        double gF = util * gTopk[i].attr;
        double diff = abs(gF - qF);
        tmp_AAE += diff / K;
        if (abs(gF) > 1e-7) {
          tmp_ARE += diff / abs(gF);
          counter_gF_nonzero++;
        }
      }
      AAE += tmp_AAE / n_eval;
      if (counter_gF_nonzero > 0)
        ARE += tmp_ARE / counter_gF_nonzero / n_eval;
    }


    // calculate Kendall
    double Kendall = 0;
    for (int _ = 0; _ < n_eval; _++) {
      vector<Item<n_attr>> gTopk, qTopk;

      Vect<n_attr> util = Vect<n_attr>::randVec();

      if (((double)(rand() % 1024) / 1024) < p_aimed * 2 - 1) {
        assert(n_attr == 2);
        util.attr[1] = abs(util.attr[1]);
      }

      topk(keys, attr, util, gTopk);
      base->topk(util, qTopk, K);

      


      map<uint32_t, int> pos;
      for (int i = 0; i < K; i++) {
        pos[gTopk[i].key] = i;
      }
      int concordant_pairs = 0;
      int total_pairs = 0;
      for (int i = 0; i < K; i++) {
        for (int j = i + 1; j < K; j++) {
          if (pos.find(qTopk[i].key) == pos.end() || pos.find(qTopk[j].key) == pos.end())
            continue;
          total_pairs++;
          if (pos[qTopk[i].key] < pos[qTopk[j].key])
            concordant_pairs++;
        }
      }
      if (total_pairs == 0)
        Kendall += (double)-1 / n_eval;
      else
        Kendall += ((double)2 * concordant_pairs / total_pairs - 1) / n_eval;

    }


    Vect<5> ret;
    ret[0] = precision;
    ret[1] = recall;
    ret[2] = AAE;
    ret[3] = ARE;
    ret[4] = Kendall;
    return ret;
  }

};

#endif
