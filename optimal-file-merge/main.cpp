#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <functional>
using namespace std;

int merge(vector<int>& files) {
    priority_queue<int, vector<int>, greater<int>> queue(files.begin(), files.end());
    int total_cost = 0;

    while (queue.size() > 1) {
        int first = queue.top();
        queue.pop();
        int second = queue.top();
        queue.pop();

        int merged_file = first + second;
        total_cost += merged_file;

        queue.push(merged_file);

        cout << first << " + " << second
             << " -> " << merged_file << endl;
    }

    cout << "Total cost: ";
    return total_cost;
}

int main() {
    cout << "\n\'0\' for default" << endl;
    int n;
    cout << "Enter the number of files: ";
    cin >> n;
    vector<int> files;
    if (n <= 0) {
        n = 4;
        files = {16, 8, 2, 4};
        cout << "Input files: ";
        for(int file : files)
            cout << file << " ";
        cout << "\n" << merge(files) << endl << endl;
        files = {6, 9, 3, 18};
        cout << "Input files: ";
        for(int file : files)
            cout << file << " ";
        cout << "\n" << merge(files) << endl;
    }
    else {
        files.resize(n);
        cout << "Enter file details (2 4 8 12):\n";
        for (int i = 0; i < n; i++) {
            cin >> files[i];
        }
        cout << merge(files) << endl;
    }

    return 0;
}
