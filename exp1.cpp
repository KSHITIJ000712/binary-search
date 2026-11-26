EXPERIMENT 1 — RECURSION TREE METHOD
============================================================
#include <iostream>
#include <cmath>
using namespace std;

int main() {
    int n, a, b, k;
    cout << "Enter n, a, b and k: ";
    cin >> n >> a >> b >> k;

    if (n <= 0 || a <= 0 || b <= 1 || k < 0) {
        cout << "Invalid input";
        return 0;
    }

    double nodes = 1, size = n, total = 0;
    int level = 0;

    cout << "\nLevel\tNodes\tSize\tCost\n";
    while (size >= 1) {
        double cost = nodes * pow(size, k);
        cout << level << "\t" << nodes << "\t" << size
             << "\t" << cost << "\n";
        total += cost;
        nodes *= a;
        size /= b;
        level++;
    }

    double p = log(a) / log(b);
    cout << "\nTotal cost = " << total << "\n";
    cout << "Time complexity: ";
    if (k < p) cout << "Theta(n^" << p << ")";
    else if (fabs(k - p) < 0.000001) cout << "Theta(n^" << p << " log n)";
    else cout << "Theta(n^" << k << ")";
    cout << "\n";
    return 0;
}



EXPERIMENT 2 — MASTER'S THEOREM

#include <iostream>
#include <cmath>
using namespace std;

int main() {
    double a, b, k;
    cout << "For T(n) = aT(n/b) + n^k\n";
    cout << "Enter a, b and k: ";
    cin >> a >> b >> k;

    if (a <= 0 || b <= 1) {
        cout << "Invalid input";
        return 0;
    }

    double p = log(a) / log(b);
    cout << "log_b(a) = " << p << "\n";

    if (k < p)
        cout << "Case 1: T(n) = Theta(n^" << p << ")\n";
    else if (fabs(k - p) < 0.000001)
        cout << "Case 2: T(n) = Theta(n^" << p << " log n)\n";
    else
        cout << "Case 3: T(n) = Theta(n^" << k << ")\n";

    return 0;
}


============================================================
EXPERIMENT 3A — B-TREE (ORDER 4)
============================================================
#include <iostream>
#include <vector>
#include <queue>
using namespace std;

class BTree {
    struct Node {
        vector<int> key;
        vector<Node*> child;
        bool leaf;
        Node(bool l) : leaf(l) {}
    };

    Node* root = new Node(true);
    const int T = 2; // Minimum degree; maximum 3 keys

    void splitChild(Node* parent, int i) {
        Node* y = parent->child[i];
        Node* z = new Node(y->leaf);
        int middle = y->key[1];
        z->key.push_back(y->key[2]);
        y->key.resize(1);

        if (!y->leaf) {
            z->child.push_back(y->child[2]);
            z->child.push_back(y->child[3]);
            y->child.resize(2);
        }

        parent->child.insert(parent->child.begin() + i + 1, z);
        parent->key.insert(parent->key.begin() + i, middle);
    }

    void insertNonFull(Node* node, int value) {
        int i = (int)node->key.size() - 1;

        if (node->leaf) {
            node->key.push_back(value);
            while (i >= 0 && node->key[i] > value) {
                node->key[i + 1] = node->key[i];
                i--;
            }
            node->key[i + 1] = value;
        } else {
            while (i >= 0 && value < node->key[i]) i--;
            i++;
            if (node->child[i]->key.size() == 3) {
                splitChild(node, i);
                if (value > node->key[i]) i++;
            }
            insertNonFull(node->child[i], value);
        }
    }

public:
    void insert(int value) {
        if (search(value)) return;

        if (root->key.size() == 3) {
            Node* newRoot = new Node(false);
            newRoot->child.push_back(root);
            splitChild(newRoot, 0);
            root = newRoot;
        }
        insertNonFull(root, value);
    }

    bool search(int value) {
        Node* cur = root;
        while (cur) {
            int i = 0;
            while (i < cur->key.size() && value > cur->key[i]) i++;
            if (i < cur->key.size() && cur->key[i] == value) return true;
            if (cur->leaf) return false;
            cur = cur->child[i];
        }
        return false;
    }

    void display() {
        queue<Node*> q;
        q.push(root);
        while (!q.empty()) {
            int count = q.size();
            while (count--) {
                Node* p = q.front(); q.pop();
                cout << "[ ";
                for (int x : p->key) cout << x << " ";
                cout << "] ";
                for (Node* c : p->child) q.push(c);
            }
            cout << "\n";
        }
    }
};

int main() {
    BTree tree;
    int choice, x;
    do {
        cout << "\n1.Insert 2.Search 3.Display 4.Exit\nChoice: ";
        cin >> choice;
        if (choice == 1) {
            cout << "Enter value: "; cin >> x;
            tree.insert(x);
        } else if (choice == 2) {
            cout << "Enter value: "; cin >> x;
            cout << (tree.search(x) ? "Found\n" : "Not found\n");
        } else if (choice == 3) tree.display();
    } while (choice != 4);
    return 0;
}


============================================================
EXPERIMENT 3B — B+ TREE (SIMPLE LEAF-LEVEL DEMONSTRATION)
============================================================
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

class BPlusTree {
    struct Leaf {
        vector<int> keys;
        Leaf* next = nullptr;
    };

    vector<Leaf*> leaves;
    const int MAX = 3;

public:
    BPlusTree() { leaves.push_back(new Leaf()); }

    void insert(int value) {
        int i = 0;
        while (i < leaves.size() && !leaves[i]->keys.empty()
               && value > leaves[i]->keys.back()) i++;

        if (i == leaves.size()) i--;
        Leaf* p = leaves[i];

        if (find(p->keys.begin(), p->keys.end(), value) != p->keys.end())
            return;

        p->keys.insert(lower_bound(p->keys.begin(), p->keys.end(), value), value);

        if (p->keys.size() > MAX) {
            Leaf* q = new Leaf();
            q->keys.assign(p->keys.begin() + 2, p->keys.end());
            p->keys.resize(2);
            q->next = p->next;
            p->next = q;
            leaves.insert(leaves.begin() + i + 1, q);
        }
    }

    bool search(int value) {
        for (Leaf* p : leaves)
            if (find(p->keys.begin(), p->keys.end(), value) != p->keys.end())
                return true;
        return false;
    }

    void remove(int value) {
        for (Leaf* p : leaves) {
            auto it = find(p->keys.begin(), p->keys.end(), value);
            if (it != p->keys.end()) {
                p->keys.erase(it);
                return;
            }
        }
    }

    void display() {
        cout << "Leaf nodes: ";
        for (Leaf* p : leaves) {
            cout << "[ ";
            for (int x : p->keys) cout << x << " ";
            cout << "] -> ";
        }
        cout << "NULL\n";
    }
};

int main() {
    BPlusTree tree;
    int ch, x;
    do {
        cout << "\n1.Insert 2.Search 3.Delete 4.Display 5.Exit\nChoice: ";
        cin >> ch;
        if (ch == 1) { cout << "Value: "; cin >> x; tree.insert(x); }
        else if (ch == 2) {
            cout << "Value: "; cin >> x;
            cout << (tree.search(x) ? "Found\n" : "Not found\n");
        }
        else if (ch == 3) { cout << "Value: "; cin >> x; tree.remove(x); }
        else if (ch == 4) tree.display();
    } while (ch != 5);
    return 0;
}


============================================================
EXPERIMENT 4 — RED-BLACK TREE
============================================================
#include <iostream>
#include <set>
using namespace std;

// C++ set is implemented as a balanced Red-Black Tree.
int main() {
    set<int> tree;
    int choice, id;

    do {
        cout << "\n1.Insert Student ID\n2.Delete Student ID\n";
        cout << "3.Search Student ID\n4.Display IDs\n5.Exit\nChoice: ";
        cin >> choice;

        if (choice == 1) {
            cout << "Enter ID: "; cin >> id;
            tree.insert(id);
            cout << "ID inserted\n";
        } else if (choice == 2) {
            cout << "Enter ID: "; cin >> id;
            if (tree.erase(id)) cout << "ID deleted\n";
            else cout << "ID not found\n";
        } else if (choice == 3) {
            cout << "Enter ID: "; cin >> id;
            cout << (tree.count(id) ? "Found\n" : "Not found\n");
        } else if (choice == 4) {
            for (int x : tree) cout << x << " ";
            cout << "\n";
        }
    } while (choice != 5);

    return 0;
}


============================================================
EXPERIMENT 5 — BINARY SEARCH
============================================================
#include <iostream>
#include <algorithm>
using namespace std;

int binarySearch(int a[], int n, int key) {
    int low = 0, high = n - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;

        if (a[mid] == key) return mid;
        else if (key < a[mid]) high = mid - 1;
        else low = mid + 1;
    }
    return -1;
}

int main() {
    int n, key;
    cout << "Enter number of elements: ";
    cin >> n;

    int a[100];
    cout << "Enter elements: ";
    for (int i = 0; i < n; i++) cin >> a[i];

    sort(a, a + n);
    cout << "Sorted array: ";
    for (int i = 0; i < n; i++) cout << a[i] << " ";

    cout << "\nEnter element to search: ";
    cin >> key;

    int result = binarySearch(a, n, key);
    if (result == -1) cout << "Element not found\n";
    else cout << "Element found at index " << result << "\n";

    return 0;
}


============================================================
EXPERIMENT 6 — MERGE SORT AND QUICK SORT
============================================================
#include <iostream>
#include <vector>
using namespace std;

void merge(vector<int>& a, int l, int m, int r) {
    vector<int> left(a.begin() + l, a.begin() + m + 1);
    vector<int> right(a.begin() + m + 1, a.begin() + r + 1);

    int i = 0, j = 0, k = l;
    while (i < left.size() && j < right.size()) {
        if (left[i] <= right[j]) a[k++] = left[i++];
        else a[k++] = right[j++];
    }
    while (i < left.size()) a[k++] = left[i++];
    while (j < right.size()) a[k++] = right[j++];
}

void mergeSort(vector<int>& a, int l, int r) {
    if (l >= r) return;
    int m = l + (r - l) / 2;
    mergeSort(a, l, m);
    mergeSort(a, m + 1, r);
    merge(a, l, m, r);
}

int partitionArray(vector<int>& a, int low, int high) {
    int pivot = a[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (a[j] <= pivot) {
            i++;
            swap(a[i], a[j]);
        }
    }
    swap(a[i + 1], a[high]);
    return i + 1;
}

void quickSort(vector<int>& a, int low, int high) {
    if (low < high) {
        int p = partitionArray(a, low, high);
        quickSort(a, low, p - 1);
        quickSort(a, p + 1, high);
    }
}

void display(const vector<int>& a) {
    for (int x : a) cout << x << " ";
    cout << "\n";
}

int main() {
    int n;
    cout << "Enter number of elements: ";
    cin >> n;

    vector<int> a(n), b;
    cout << "Enter elements: ";
    for (int i = 0; i < n; i++) cin >> a[i];

    b = a;
    mergeSort(a, 0, n - 1);
    cout << "Merge Sort: ";
    display(a);

    quickSort(b, 0, n - 1);
    cout << "Quick Sort: ";
    display(b);

    return 0;
}


============================================================
EXPERIMENT 7 — FRACTIONAL KNAPSACK
============================================================
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

struct Item {
    int weight, profit;
};

bool compare(Item a, Item b) {
    return (double)a.profit / a.weight >
           (double)b.profit / b.weight;
}

int main() {
    int n, capacity;
    cout << "Enter number of items: ";
    cin >> n;

    vector<Item> items(n);
    for (int i = 0; i < n; i++) {
        cout << "Enter weight and profit of item " << i + 1 << ": ";
        cin >> items[i].weight >> items[i].profit;
    }

    cout << "Enter knapsack capacity: ";
    cin >> capacity;

    sort(items.begin(), items.end(), compare);

    double totalProfit = 0;
    for (int i = 0; i < n && capacity > 0; i++) {
        if (items[i].weight <= capacity) {
            capacity -= items[i].weight;
            totalProfit += items[i].profit;
        } else {
            double fraction = (double)capacity / items[i].weight;
            totalProfit += items[i].profit * fraction;
            capacity = 0;
        }
    }

    cout << "Maximum profit = " << totalProfit << "\n";
    return 0;
}


============================================================
EXPERIMENT 8 — JOB SEQUENCING WITH DEADLINES
============================================================
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

struct Job {
    char id;
    int deadline, profit;
};

bool compare(Job a, Job b) {
    return a.profit > b.profit;
}

int main() {
    int n;
    cout << "Enter number of jobs: ";
    cin >> n;

    vector<Job> jobs(n);
    int maxDeadline = 0;

    for (int i = 0; i < n; i++) {
        cout << "Enter job ID, deadline and profit: ";
        cin >> jobs[i].id >> jobs[i].deadline >> jobs[i].profit;
        maxDeadline = max(maxDeadline, jobs[i].deadline);
    }

    sort(jobs.begin(), jobs.end(), compare);
    vector<char> slot(maxDeadline + 1, '-');
    int totalProfit = 0;

    for (int i = 0; i < n; i++) {
        for (int t = min(maxDeadline, jobs[i].deadline); t >= 1; t--) {
            if (slot[t] == '-') {
                slot[t] = jobs[i].id;
                totalProfit += jobs[i].profit;
                break;
            }
        }
    }

    cout << "Scheduled jobs: ";
    for (int t = 1; t <= maxDeadline; t++)
        if (slot[t] != '-') cout << slot[t] << " ";

    cout << "\nTotal profit = " << totalProfit << "\n";
    return 0;
}


============================================================
EXPERIMENT 9 — 0/1 KNAPSACK
============================================================
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main() {
    int n, capacity;
    cout << "Enter number of items: ";
    cin >> n;

    vector<int> weight(n), value(n);
    for (int i = 0; i < n; i++) {
        cout << "Enter weight and value of item " << i + 1 << ": ";
        cin >> weight[i] >> value[i];
    }

    cout << "Enter capacity: ";
    cin >> capacity;

    vector<vector<int>> dp(n + 1, vector<int>(capacity + 1, 0));

    for (int i = 1; i <= n; i++) {
        for (int w = 0; w <= capacity; w++) {
            dp[i][w] = dp[i - 1][w];
            if (weight[i - 1] <= w) {
                dp[i][w] = max(dp[i][w],
                    value[i - 1] + dp[i - 1][w - weight[i - 1]]);
            }
        }
    }

    cout << "Maximum value = " << dp[n][capacity] << "\n";
    cout << "Selected items: ";
    int w = capacity;
    for (int i = n; i > 0; i--) {
        if (dp[i][w] != dp[i - 1][w]) {
            cout << i << " ";
            w -= weight[i - 1];
        }
    }
    cout << "\n";
    return 0;
}


============================================================
EXPERIMENT 10 — LONGEST COMMON SUBSEQUENCE (LCS)
============================================================
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

int main() {
    string a, b;
    cout << "Enter first string: ";
    cin >> a;
    cout << "Enter second string: ";
    cin >> b;

    int m = a.length(), n = b.length();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));

    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (a[i - 1] == b[j - 1])
                dp[i][j] = 1 + dp[i - 1][j - 1];
            else
                dp[i][j] = max(dp[i - 1][j], dp[i][j - 1]);
        }
    }

    string lcs = "";
    int i = m, j = n;
    while (i > 0 && j > 0) {
        if (a[i - 1] == b[j - 1]) {
            lcs += a[i - 1];
            i--;
            j--;
        } else if (dp[i - 1][j] > dp[i][j - 1]) {
            i--;
        } else {
            j--;
        }
    }

    reverse(lcs.begin(), lcs.end());
    cout << "LCS = " << lcs << "\n";
    cout << "Length = " << dp[m][n] << "\n";
    return 0;
}