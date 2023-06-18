#include <bits/stdc++.h>

using namespace std;

class SegmentTree{
private:

    void build(int node, int l, int r, const vector<int> &arr){
        if (l == r){
            tree[node] = arr[l];
        }
        else{
            int m = (l + r) / 2;

            build(2 * node, l, m, arr);
            build(2 * node + 1, m + 1, r, arr);

            tree[node] = max(tree[2 * node], tree[2 * node + 1]);
        }
    }

    void update(int node, int l, int r, int pos, int key){
        if (l == r){
            tree[node] = key;
        }
        else{
            int m = (l + r) / 2;

            if (pos <= m)
                update(2 * node, l, m, pos, key);
            else
                update(2 * node + 1, m + 1, r, pos, key);

            tree[node] = max(tree[2 * node], tree[2 * node + 1]);
        }
    }

    int query(int node, int l, int r, int st_q, int dr_q) const{
        if (st_q <= l && r <= dr_q)
            return tree[node];
        else{
            int m = (l + r) / 2;
            int answer = 0;

            if (st_q <= m)
                answer = max(answer, query(2 * node, l, m, st_q, dr_q));

            if (m < dr_q)
                answer = max(answer, query(2 * node + 1, m + 1, r, st_q, dr_q));

            return answer;
        }
    }

    vector<int> tree;
    int N;

public:

    SegmentTree(const vector<int> &arr){
        N = arr.size();
        tree.resize(4 * N);
        build(1, 0, N - 1, arr);
    }

    void update(int pos, int key){
        update(1, 0, N - 1, pos, key);
    }

    int query(int x, int y) const{
        return query(1, 0, N - 1, x, y);
    }
};

const int MAX_N = 100000 + 1;

vector<int> G[MAX_N];
int father[MAX_N], depth[MAX_N], sizeTree[MAX_N], key[MAX_N];

vector<SegmentTree> ST;
int path[MAX_N], lengthPath[MAX_N], posInPath[MAX_N];
int startNode[MAX_N];

int N, Q;
int numberOfPaths;

void dfs(int node){
    sizeTree[node] = 1;

    int heavySon = 0;

    for (int son : G[node]){
        if (!father[son]){
            father[son] = node;
            depth[son] = depth[node] + 1;

            dfs(son);
            sizeTree[node] += sizeTree[son];

            if (sizeTree[son] > sizeTree[heavySon])
                heavySon = son;
        }
    }

    if (heavySon == 0)
        path[node] = numberOfPaths++;
    else
        path[node] = path[heavySon];

    posInPath[node] = lengthPath[path[node]]++;
}

void build_heavy_path(){
    father[1] = 1;
    depth[1] = 1;
    dfs(1);

    for (int i = 1; i <= N; i++){
        posInPath[i] = lengthPath[path[i]] - posInPath[i] - 1;

        if (posInPath[i] == 0)
            startNode[path[i]] = i;
    }
}

void build_segment_trees(){
    vector<vector<int>> values(numberOfPaths);

    for (int i = 0; i < numberOfPaths; i++)
        values[i].resize(lengthPath[i]);

    for (int i = 1; i <= N; i++)
        values[path[i]][posInPath[i]] = key[i];

    for (int i = 0; i < numberOfPaths; i++)
        ST.push_back(SegmentTree(values[i]));
}

void updateNode(int node, int newKey){
    ST[path[node]].update(posInPath[node], newKey);
}

int query(int x, int y){
    if (depth[startNode[path[x]]] < depth[startNode[path[y]]])
        swap(x, y);

    if (path[x] == path[y])
        return ST[path[x]].query(min(posInPath[x], posInPath[y]), max(posInPath[x], posInPath[y]));
    else
        return max(ST[path[x]].query(0, posInPath[x]), query(father[startNode[path[x]]], y));
}

int main()
{
    ifstream in("heavypath.in");
    ofstream out("heavypath.out");

    in >> N >> Q;

    for (int i = 1; i <= N; i++)
        in >> key[i];

    for (int i = 0; i < N - 1; i++){
        int x, y;
        in >> x >> y;

        G[x].push_back(y);
        G[y].push_back(x);
    }

    build_heavy_path();
    build_segment_trees();

    while (Q--){
        int t;
        in >> t;

        if (t == 0){
            int n, k;
            in >> n >> k;
            updateNode(n, k);
        }
        else{
            int x, y;
            in >> x >> y;
            out << query(x, y) << "\n";
        }
    }


    return 0;
}
