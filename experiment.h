#ifndef _EXPERIMENT_H_
#define _EXPERIMENT_H_

#include "dataloader.h"
#include "strawman.h"
#include "utils.h"

template <int n_attr, int n_item, int n_expr> class Experimentor {
public:
  void experiment(Dataloader<n_attr> *data, Base<n_attr> *base,
                  Evaluator<n_attr> *eval, string base_name = "") {
    double precision = 0.0, var_precision = 0.0;
    double recall = 0.0, var_recall = 0.0;
    double AAE = 0.0, var_AAE = 0.0;
    double ARE = 0.0, var_ARE = 0.0;
    double Kendall = 0.0, var_Kendall = 0.0;

    double duration = 0.0;

    for (int expr = 0; expr < n_expr; expr++) {
      data->clear();
      base->clear();

      keys.clear();
      attr.clear();
      freq.clear();

      int start = clock();

      for (int i = 0; i < n_item; i++) {
        Item<n_attr> item;
        if (!data->next(item))
          break;

        if (freq[item.key] == 0) {
          keys.push_back(item.key);
        }

        freq[item.key] += 1;
        attr[item.key] = attr[item.key] + item.attr;

        base->insert(item);
      }

      duration += (double)(clock() - start) / CLOCKS_PER_SEC / n_expr;
      
      Vect<5> result = eval->eval(keys, attr, base);

      // result : {precision, recall, AAE, ARE, Kendall}

      precision += result[0] / n_expr;
      var_precision += result[0] * result[0] / n_expr;

      AAE += result[2] / n_expr;
      var_AAE += result[2] * result[2] / n_expr;

      ARE += result[3] / n_expr;
      var_ARE += result[3] * result[3] / n_expr;

      Kendall += result[4] / n_expr;
      var_Kendall += result[4] * result[4] / n_expr;

      cerr << "\r"
           << "Expr. [";
      for (int j = 0; j < expr + 1; j++) {
        cerr << "=";
      }
      if (expr + 1 < n_expr) {
        cerr << ">";
      }
      for (int j = expr + 2; j < n_expr; j++) {
        cerr << " ";
      }
      cerr << "]: " << base_name;
      cerr.flush();
    }
    cerr << endl;

    cout << base_name << endl;
    cout << "over " << n_expr << " expr.:" << endl;
    cout << "precision: " << precision << ", std.: " << sqrt(var_precision - precision * precision) << endl;
    cout << "AAE: " << AAE << ", std.: " << sqrt(var_AAE - AAE * AAE) << endl;
    cout << "ARE: " << ARE << ", std.: " << sqrt(var_ARE - ARE * ARE) << endl;
    cout << "Kendall: " << Kendall << ", std.: " << sqrt(var_Kendall - Kendall * Kendall) << endl;
    cout << "Throughput: " << (int)(n_item / duration) << " items per second" << endl;
    cout << endl;
  }

public:
  vector<uint32_t> keys;
  unordered_map<uint32_t, uint32_t> freq;
  unordered_map<uint32_t, Vect<n_attr>> attr;
};

#endif
