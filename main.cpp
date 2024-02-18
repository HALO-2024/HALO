#include "coco.h"
#include "dataloader.h"
#include "eval.h"
#include "experiment.h"
#include "sky.h"
#include "strawman.h"
#include "utils.h"
#include <bits/stdc++.h>

using namespace std;

#ifndef n_samp
#define n_samp 32
#endif

#ifndef d_s
#define d_s 8
#endif

#ifndef w_s
#define w_s 5000
#endif



const int n_attr = 2;
const int n_item = 1000000;
const int n_eval = 50;
const int n_expr = 50;

const int K = 100;    

const int d_c = 3;
const int w_c = d_s * w_s / d_c;
const int c_heap = d_s * w_s;

unordered_map<uint32_t, uint32_t> freq;

vector<uint32_t> keys;
unordered_map<uint32_t, Vect<n_attr>> attr;

void analysis() {
  sort(keys.begin(), keys.end(), [=](uint32_t a, uint32_t b) {
    return attr[a].norm2() > attr[b].norm2();
  });

  for (int i = 0; i < K; i++) {
    cout << i << " "
         << " " << freq[keys[i]] << " " << attr[keys[i]].norm2() << endl
         << "\t" << attr[keys[i]] << endl;
  }

  Vect<n_attr> core;
  for (int i = 0; i < keys.size(); i++) {
    core = core + attr[keys[i]];
  }
  core = core * (1.0 / keys.size());
  cout << "center of gravity: " << core << endl;
}

int main(int argc, char *argv[]) {

  Dataloader<n_attr> *data =
      new ZipfDataloader<n_attr>("zipf_0.9.dat");
  string filePath = (string)"result/test/" + "samp" + to_string(n_samp) + "_d" + to_string(d_s) + "_w" + to_string(w_s) + ".txt";

  // Dataloader<n_attr> *data =
  //     new CriteoDataLoader<n_attr>("Criteo_10m", {1, 2, 8});

  // string filePath = (string)"result/cretio-3D_ours3/" + "samp" + to_string(n_samp) + "_d" + to_string(d_s) + "_w" + to_string(w_s) + ".txt";

  // Dataloader<n_attr> *data =
  //     new NBADataLoader<n_attr>("dataNBA/archive/games_details_MIN_PTS.txt");

  // string filePath = (string)"result/NBAnew/" + "samp" + to_string(n_samp) + "_d" + to_string(d_s) + "_w" + to_string(w_s) + ".txt";

  freopen(filePath.c_str(), "w", stdout);

  Base<n_attr> *sketch1Heap = new Sketch1Heap<n_attr, c_heap, d_c, w_c>();
  Base<n_attr> *cocoSketch = new CocoSketch<n_attr, d_s, w_s>();
  Base<n_attr> *skySketch1 = new SkySketch<n_attr, n_samp, w_s, d_s, 1>();
  Base<n_attr> *skySketch2 = new SkySketch<n_attr, n_samp, w_s, d_s, 2>();
  Base<n_attr> *skySketch3 = new SkySketch<n_attr, n_samp, w_s, d_s, 3>();
  Base<n_attr> *skySketch4 = new SkySketch<n_attr, n_samp, w_s, d_s, 4>();
  Base<n_attr> *skySketch5 = new SkySketch<n_attr, n_samp, w_s, d_s, 5>();

  Evaluator<n_attr> *evaluator = new TopKEvaluator<n_attr, n_eval, K>();

  Experimentor<n_attr, n_item, n_expr> *experimenter =
      new Experimentor<n_attr, n_item, n_expr>();
    
  cerr << "n_samp = " << n_samp << ", d = " << d_s << ", w = " << w_s << endl;

  experimenter->experiment(data, sketch1Heap, evaluator, "sketch1Heap");
  experimenter->experiment(data, cocoSketch, evaluator, "cocoSketch");
  experimenter->experiment(data, skySketch2, evaluator, "distance to G");
  experimenter->experiment(data, skySketch3, evaluator, "dir = G-tmp");
  experimenter->experiment(data, skySketch4, evaluator, "best dir");
  experimenter->experiment(data, skySketch5, evaluator, "weighted best dir");

  return 0;
}
