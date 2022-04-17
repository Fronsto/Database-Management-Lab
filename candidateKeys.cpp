
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace std;
/////////////////////////////////////////////////

#define ENT "\n"

#define FOR(i,a,b)  for (long long i = a; i < (b); i++)
#define F0R(i, n)   for(long long i=0; i<(n); i++)
#define     sz(x)   ((long long)(x).size())

#define trav(a,x) for (auto& a : x)

void fast();

///////////////////////////////////////////////////

void print_deps(int num_func_dep, vector<pair<vector<int>,vector<int>>> func_dep){
    cout<<"##############################"<<ENT;
    F0R(i,num_func_dep) {
        cout<<"\nFIRST LINE\n";
        F0R(j, sz(func_dep[i].first)) {
            cout<<func_dep[i].first[j]<<" ";
        }
        cout<<"\nSECOND LINE\n";
        F0R(j, sz(func_dep[i].second)) {
            cout<<func_dep[i].second[j]<<" ";
        }
    }
}

vector<int> create_vec(string s){
    vector<int>arr;
    istringstream is(s);
    int num;
    while(is>>num){
        arr.push_back(num);
    }
    return arr;
}
int get_int_from_line(string s){
    istringstream is(s);
    int num;
    is>>num;
    return num;
}

int get_num_bits (int n) {
    return __builtin_popcount(n);
}

bool cand_compare(int s1, int s2) {
    if(get_num_bits(s1) != get_num_bits(s2)) {
        return get_num_bits(s1) < get_num_bits(s2) ;
    }
    else return s1 < s2;
}
void print_as_vector(int cand_key, int num_att) {

    F0R(i, num_att){

        if(cand_key & (1<<i)) cout<<(i+1)<<" ";
    }
}

int main() {  fast();

    // temp variables
    string lhs, rhs;
    string temp;

    // taking input first 2 lines
    int num_att = 0;
    getline(cin, temp);
    num_att = get_int_from_line(temp); 

    int num_func_dep =0;
    getline(cin, temp);
    num_func_dep = get_int_from_line(temp); 

    // taking func deps input 
    vector<pair<vector<int>, vector<int>>> func_dep;
    F0R(i, num_func_dep) {
       getline(cin, lhs);
       getline(cin, rhs);
       func_dep.push_back(make_pair(create_vec(lhs), create_vec(rhs)));
    }

    
    // we use binary rep of ints to denote sets and subsets
    
    // maintain the ans vector
    vector<int> cand_keys;
    
    
    int num_of_sets = (1<<num_att);

    for(int my_set =0 ; my_set < num_of_sets ; my_set++) {
        // for each set first check if its a superset of an existing cand key 
        bool is_superkey = false;
        for(int i=0; i<sz(cand_keys); i++){
            if((my_set & cand_keys[i]) == cand_keys[i]){
                is_superkey = true;
                break;
            }
        }
        if(is_superkey) continue;

        // then now check if its a valid candidate key
        
        int prev =0;
        int next =my_set;

        while(prev != next) {
            prev = next;
            for(int i =0; i<num_func_dep; i++){
                bool satisfy_func_deps = true;
                
                for(int j=0; j< sz(func_dep[i].first); j++){
                    if(! (next & (1<<(func_dep[i].first[j]-1) ) )){
                        satisfy_func_deps = false;
                        break;
                    }
                }
                if(satisfy_func_deps){
                    for(int j=0; j< sz(func_dep[i].second); j++){
                        next |= (1<< (func_dep[i].second[j] -1 ));
                    }

                }
                
            }

        }
        if( next == num_of_sets -1 ) {
            cand_keys.push_back(my_set);
        }

        
    }
    // sort cand_keys accordingly

    sort(cand_keys.begin(), cand_keys.end(), cand_compare);
    cout<<sz(cand_keys)<<ENT;
    F0R(i, sz(cand_keys)){
        print_as_vector(cand_keys[i], num_att);
        cout<<ENT;
    }

}


void fast(){
    ios_base::sync_with_stdio(false);
	cin.tie(NULL);
	cout.tie(NULL);
}

