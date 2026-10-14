#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

struct Job {
    int id, deadline, profit;
};

bool compareProfit(const Job &a, const Job &b) {
    return a.profit > b.profit;
}

int findSlot(int deadline, vector<int>& small){
    if (small[deadline] != deadline){
        small[deadline] = findSlot(small[deadline], small);
    }
    return small[deadline];
}

void schedule(vector<Job>& jobs, int d) {
    sort(jobs.begin(), jobs.end(), compareProfit);

    int total_profit = 0;
    vector<int> small(d + 1);
    for (int i = 0; i <= d; i++) {
        small[i] = i;
    }
    vector<int> job_sequence(d, -1);

    for (Job job : jobs) {
        if (job.deadline <= 0 || job.profit <= 0) continue;
        int free_slot = findSlot(min(job.deadline, d), small);
        if(free_slot > 0){
            small[free_slot] = findSlot(free_slot - 1, small);
            total_profit += job.profit;
            job_sequence[free_slot - 1] = job.id;
        }
    }

    cout << "Sequence of jobs: ";
    for (int i = 0; i < d; i++) {
        if (job_sequence[i] != -1) {
            cout << job_sequence[i] << " ";
        }
    }
    cout << "\nTotal profit: " << total_profit << "\n";
}

int main() {
    cout << "\n\'0\' for default" << endl;
    int n;
    cout << "Enter the number of jobs: ";
    cin >> n;
    vector<Job> jobs;
    if (n <= 0) {
        n = 7;
        jobs = {
            {1, 2, 40},
            {2, 4, 15},
            {3, 3, 60},
            {4, 2, 20},
            {5, 3, 10},
            {6, 1, 45},
            {7, 1, 55}
        };
    }
    else {
        jobs.resize(n);
        cout << "Enter deadline and profit for each job:\n";
        for (int i = 0; i < n; i++) {
            cin >> jobs[i].deadline >> jobs[i].profit;
            jobs[i].id = i + 1;
        }
    }

    int max_deadline = 0;
    for (const Job& job : jobs) {
        if (job.deadline > max_deadline) {
            max_deadline = job.deadline;
        }
    }

    schedule(jobs, min(max_deadline, n));

    return 0;
}
