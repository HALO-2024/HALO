# HALO: Identifying Top-K Items in Multi-Attribute Data Streams for On-the-Fly Linear Utility Functions

Finding top-K items in data streams with approximation algorithms is a fundamental problem in fields such as data mining. However, the vast majority of existing algorithms only focus on single-attribute data streams, a special case of multi-attribute data streams. For multi-attribute data streams, the set of top-K items changes with the utility function. In this paper, we study how to approximately store data stream in compact space when the utility function is linear and changing on-the-fly, i.e., provided at the time of query. We propose a novel sketch algorithm, HALO, to address this problem. We offer several versions of algorithm to balance between accuracy and efficiency. Experimental results show that HALO outperforms baselines based on existing algorithms across all datasets and demonstrates its optimization of end-to-end performance in a mini-simulator. 

## Compile & Run

```
g++ main.cpp murmur3.cpp -o main -std=c++17 -O2 -g -D d_s=8 -D w_s=5000 -D p_aimed=0.5
./main
```