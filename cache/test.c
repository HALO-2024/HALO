#include <bits/stdc++.h>

using namespace std;

template <int c_heap> class Heap {
public:
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

public:
  multimap<float, uint32_t> q;
  unordered_map<uint32_t, multimap<float, uint32_t>::iterator> iter;
};

int main() {
  Heap<10> h;
  map<int, int> m;

  for (int i = 0; i < 1000; i++) {
    int key = rand() % 100;
    int val = rand() % 100;

    m[key] += val;
    h.insert(key, m[key]);
  }

  for (auto [k, v] : h.iter) {
    cout << k << " " << v->first << " " << v->second << " " << m[k] << endl;
  }
}
