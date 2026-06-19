#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <unordered_set>
using namespace std;

class Task {
public:
    string id;
    int burst;
    int remaining;
    vector<string> memory;
    int memIndex;

    Task() {
        memIndex = 0;
    }
};

vector<Task> readTasks(string filename) {

    ifstream fin(filename);

    vector<Task> tasks;
    string line;

    while (getline(fin, line)) {

        stringstream ss(line);

        string temp;
        Task t;

        ss >> temp;      // TASK
        ss >> t.id;
        ss >> temp;      // BURST
        ss >> t.burst;

        t.remaining = t.burst;

        ss >> temp;      // MEM

        while (ss >> temp) {
            t.memory.push_back(temp);
        }

        tasks.push_back(t);
    }

    return tasks;
}

class CacheLevel {

public:

    int capacity;
    int latency;

    queue<string> q;
    unordered_set<string> present;

    CacheLevel(int c = 0, int l = 0) {
        capacity = c;
        latency = l;
    }

    bool contains(string block) {
        return present.count(block);
    }

    void insert(string block) {

        if (contains(block))
            return;

        if (q.size() == capacity) {

            string oldest = q.front();

            q.pop();

            present.erase(oldest);
        }

        q.push(block);

        present.insert(block);
    }

    void print(string name) {

        cout << name << ": [";

        queue<string> temp = q;

        bool first = true;

        while (!temp.empty()) {

            if (!first)
                cout << ", ";

            cout << temp.front();

            temp.pop();

            first = false;
        }

        cout << "]\n";
    }
};

class CacheHierarchy {

public:

    CacheLevel L1, L2, L3;

    int ramAccesses;

    CacheHierarchy() : L1(32,4), L2(128,12), L3(512,40) {

        ramAccesses = 0;
    }

    int requestBlock(string block) {

        if (L1.contains(block)) {

            cout << "L1 -> HIT\n";

            return 4;
        }

        cout << "L1 -> MISS\n";

        if (L2.contains(block)) {

            cout << "L2 -> HIT\n";

            L1.insert(block);

            return 12;
        }

        cout << "L2 -> MISS\n";

        if (L3.contains(block)) {

            cout << "L3 -> HIT\n";

            L1.insert(block);

            return 40;
        }

        cout << "L3 -> MISS\n";

        cout << "Fetching from RAM\n";

        ramAccesses++;

        L1.insert(block);

        return 200;
    }

    void printState() {

        L1.print("L1");
        L2.print("L2");
        L3.print("L3");
    }
};

int main() {

    vector<Task> tasks = readTasks("input.txt");

    cout << "Tasks loaded = " << tasks.size() << "\n\n";

    CacheHierarchy cache;

    queue<int> readyQueue;

    for (int i = 0; i < tasks.size(); i++) {
        readyQueue.push(i);
    }

    int quantum = 3;

    int cycle = 1;

    int completed = 0;

    while (!readyQueue.empty()) {

        int idx = readyQueue.front();

        readyQueue.pop();

        Task &curr = tasks[idx];

        int runTime = min(curr.remaining, quantum);

        for (int i = 0; i < runTime; i++) {

            string block = curr.memory[curr.memIndex];

            curr.memIndex =
                    (curr.memIndex + 1)
                    % curr.memory.size();

            cout << "\n=========================\n";

            cout << "Cycle " << cycle << "\n";

            cout << "Running : "
                 << curr.id << "\n";

            cout << "Requesting : "
                 << block << "\n";

            int latency = cache.requestBlock(block);

            cout << "Latency = "
                 << latency
                 << " cycles\n";

            cache.printState();

            curr.remaining--;

            cycle++;
        }

        if (curr.remaining > 0)
            readyQueue.push(idx);
        else
            completed++;
    }

    cout << "\n==============================\n";

    cout << "FINAL RESULTS\n";

    cout << "Tasks completed : "
         << completed << "\n";

    cout << "Scheduler : Round Robin\n";

    cout << "Quantum : 3\n";

    cout << "Total CPU cycles : "
         << cycle - 1 << "\n";

    cout << "RAM accesses : "
         << cache.ramAccesses << "\n";

    cout << "==============================\n";

    return 0;
}