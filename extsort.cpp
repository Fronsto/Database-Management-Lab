#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <vector>
#include <array>
using namespace std;

#define ENT "\n"
#define     sz(x)   ((int)(x).size())
#define trav(a,x) for (auto& a : x)

#define dout(x) if(debug) cout<<x<<ENT
bool debug = false;

void merge(vector<vector<int>>& runs, vector<int>& temp){
    vector<int> pointer;
    int num = 0, min, k, i, j;
    // Initialize the pointer with the first location for each run
    for (i = 0; i < runs.size(); i++){
        pointer.push_back(0);
        num += runs[i].size();
    }
    // For each element in the pages, find the smallest record and add into output page
    for (i = 0; i < num; i++){
        min = (int)1e9;
        k = 0;
        for (j = 0; j < pointer.size(); j++){
            if (pointer[j] < runs[j].size() && runs[j][pointer[j]] < min){
                min = runs[j][pointer[j]];
                k = j;
            }
        }
        // Add the smallest record into the output page
        temp.push_back(min);
        // Increment the pointer corresponding to the smallest record
        pointer[k]++;
    }
}

void external_sort(vector<vector<int>>& runs, int B){
    // Intially sorting each page
    for (int i = 0; i < runs.size(); i++){
        sort(runs[i].begin(), runs[i].end());
    }
    // Recusively merge the pages taking (buffer - 1) pages at a time
    while (runs.size() > 1){
        vector<vector<int>> sorted_runs, temp;
        int k = 0, i = 0;
        vector<int> temp1;
        // Group (buffer - 1) pages and merge them
        while (i < runs.size()){
            temp.clear();
            for (k = 0; k < B - 1 && i + k < runs.size(); k++){
                temp.push_back(runs[i + k]);
            }
            temp1.clear();
            // Merging
            merge(temp, temp1);
            // Add the merged pages into the next round.
            sorted_runs.push_back(temp1);
            i += k;
        }
        runs.clear();
        runs = sorted_runs;
    }
}

int main() { 
    int page_size;  // number of records in a data file
    int num_pages;  // number of records per page;
    int B; // number of avaiable buffer pages
    int total_records; // total number of records in the database

    cin >> B >> page_size >> total_records;

    num_pages = (total_records + page_size - 1 ) / page_size; // num_runs = ceil(total_records / page_size)

    // now initially run size is page_size and num_runs is num_pages;
    int num_runs = num_pages;
    int run_size = page_size;

    vector<vector<int>> runs(num_pages, vector<int>(page_size));
    int page_no =0;
    int j=0;
    int i=0;
    for( ; i< total_records; i++, j++){
        if(j == page_size){ // go to next page;
            j=0;
            page_no++;
        }
        cin >> runs[page_no][j] ;
    }
    while(i<num_pages*page_size){
        i++;
        runs[page_no].erase(runs[page_no].end()-1);
    }


    // Sorting the records
    external_sort(runs, B);
    
    // Printing the sorted records
    for(auto page:runs){
        for(auto record:page){
            cout << record << "\n";
        }
    }
}

