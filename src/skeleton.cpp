#include <iostream>
#include <string>
using namespace std;

int main() {
  const int M = 5;
  string names[M]   = {"ananya", "arjun", "raheesh", "rahul", "sneha"};
  string records[M] = {"159032239100, JB", "159032239103, LR",
                       "159032239099, JB", "159032239101, KP",
                       "159032239102, KP"};

  const int CMAX = 10;
  string cacheKey[CMAX];
  string cacheVal[CMAX];
  int cacheN = 0;

  cout << "=== QUERY OPTIMISER (skeleton) ===\n";
  cout << "Known names: ananya, arjun, raheesh, rahul, sneha\n";
  cout << "Search a student name: ";
  string q;
  getline(cin, q);
  if (q.empty()) q = "raheesh";
  for (char& c : q)
    if (c >= 'A' && c <= 'Z') c = char(c - 'A' + 'a');

  cout << "\n[1] Result Cache  : checking... ";
  for (int i = 0; i < cacheN; ++i)
    if (cacheKey[i] == q) {
      cout << "HIT -> " << cacheVal[i] << "\n";
      return 0;
    }
  cout << "MISS\n";

  cout << "[2] Query Index   : binary searching... ";
  int lo = 0, hi = M - 1, found = -1;
  while (lo <= hi) {
    int mid = (lo + hi) / 2;
    if (names[mid] == q) { found = mid; break; }
    else if (names[mid] < q) lo = mid + 1;
    else hi = mid - 1;
  }
  if (found < 0) {
    cout << "STUDENT NOT FOUND\n";
    return 0;
  }
  string record = records[found];
  cout << "found\n";

  int cost[3] = {120, 40, 10};
  string plan[3] = {"Scan all records", "Use student index", "Cache + index"};
  int best = 0;
  for (int i = 1; i < 3; ++i)
    if (cost[i] < cost[best]) best = i;
  cout << "[3] Plan Sorter   : best plan = " << plan[best] << " (cost "
       << cost[best] << ")\n";

  const int N = 3;
  const int INF = 1000000000;
  string node[N] = {"Student", "Courses", "Results"};
  int w[N][N] = {{0, 2, 5}, {2, 0, 1}, {5, 1, 0}};
  int dist[N];
  bool done[N];
  for (int i = 0; i < N; ++i) { dist[i] = INF; done[i] = false; }
  dist[0] = 0;
  for (int i = 0; i < N; ++i) {
    int u = -1;
    for (int v = 0; v < N; ++v)
      if (!done[v] && (u == -1 || dist[v] < dist[u])) u = v;
    done[u] = true;
    for (int v = 0; v < N; ++v)
      if (w[u][v] && dist[u] + w[u][v] < dist[v]) dist[v] = dist[u] + w[u][v];
  }
  cout << "[4] Join Map      : " << node[0] << " - " << node[1] << " - "
       << node[2] << "\n";
  cout << "    Fastest Path  : Student -> Courses -> Results (cost " << dist[2]
       << ")\n";

  string jobs[2] = {"rebuild index", "update stats"};
  cout << "[5] Worker Queue  : ";
  for (int i = 0; i < 2; ++i) cout << jobs[i] << (i < 1 ? " -> " : "\n");

  string stk[10];
  int top = 0;
  stk[top++] = "Load Student";
  stk[top++] = "Load Courses";
  stk[top++] = "Load Results";
  cout << "[6] Execution Log : pushed " << top << " operations\n";
  cout << "    ERROR! rolling back (pop in reverse):\n";
  while (top > 0) cout << "      undo " << stk[--top] << "\n";
  cout << "    -> restored to last stable state\n";

  if (cacheN < CMAX) {
    cacheKey[cacheN] = q;
    cacheVal[cacheN] = record;
    cacheN++;
  }

  cout << "\n=== RESULT ===\n";
  cout << "Name   : " << q << "\n";
  cout << "Record : " << record << "\n";
  cout << "Cache  : MISS (now stored)\n";
  cout << "Plan   : " << plan[best] << "  (cost " << cost[best] << ")\n";
  return 0;
}
