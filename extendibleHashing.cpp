#include <iostream>
#include <algorithm>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include <array>

using namespace std;
/////////////////////////////////////////////////
#define ENT "\n"
#define sz(x)   ((long long)(x).size())
#define dout(x) if(debug) cout<<x<<"\n";
bool debug = false;


////////////////////////////////////////
///////////// BUCKET obj ///////////////
////////////////////////////////////////
class Bucket
{
private:
    int* arr;
    int size;
    int occupancy;
    int local_depth;
public:
    Bucket(int initial_depth, int init_size ){
        size = init_size;
        local_depth = initial_depth;
        arr = new int[size];
        occupancy =0;
    };
    ~Bucket(){
        delete[] arr;
    };
    int get_local_depth(){return local_depth;}
    bool set_local_depth(int new_depth){
        if(new_depth >= local_depth){
            local_depth = new_depth;
            return true;
        }
        return false;
    }
    int get_occupancy(){return occupancy;}
    bool is_full(){
        return (occupancy == size);
    };
    bool insert(int val){
        if(occupancy == size){
            return false;
        }
        arr[occupancy] = val;
        occupancy++;
        return true;
    };
    bool delete_val(int val){
        for(int i=0; i<occupancy; i++){
            if(arr[i] == val){
                arr[i] = arr[occupancy-1];
                occupancy--;
                return true;
            }
        }
        return false;
    };
    bool search(int val){
        for(int i=0; i<occupancy; i++){
            if(arr[i] == val){
                return true;
            }
        }
        return false;
    };
    bool reorder_with(Bucket* new_bucky){
        int* temp = new int[size];
        int j=0;
        for(int i=0; i<occupancy; i++){
            if((arr[i])&(1<<(local_depth-1))){
                new_bucky->insert(arr[i]);
            } else {
                temp[j] = arr[i];
                j++;
            }
        }
        delete[] arr;
        arr = temp;
        occupancy = j;
        return true;
    }
    void print_contents(){
        for (int i = 0; i < occupancy; i++){
            cout<<arr[i]<<" ";
        }
        cout<<ENT;
    }
};

////////////////////////////////////////
///////////// HASH TABLE ///////////////
////////////////////////////////////////
class HashTable
{
private:
    int global_depth;
    int bucket_size;
    vector<Bucket*> table;
    vector<Bucket*> buckets;
public:
    HashTable(int, int);
    ~HashTable();

    int get_global_depth(){return global_depth;}

    bool insert(int);
    bool search(int);
    bool delete_val(int);
    void display();
};

HashTable::HashTable(int initial_depth, int init_bucket_size)
{
    global_depth = initial_depth;
    bucket_size = init_bucket_size;
    for(int i =0; i<(1<<initial_depth); i++){
        Bucket* bucky = new Bucket( initial_depth, bucket_size);
        table.push_back( bucky );
        buckets.push_back(bucky);
    }
}

HashTable::~HashTable()
{
    for(auto& x : buckets){
        delete x;
    }
}
bool HashTable::insert(int val){
    int hash_val = val % (1<<global_depth);
    int bucket_index = hash_val % (1<<global_depth);
    Bucket* bucky = table[bucket_index];
    if( !bucky->is_full() ){
        return bucky->insert(val);
    }
    else{
        if(bucky->get_local_depth() == global_depth){
            // double the directory
            if(global_depth == 20){
                return false;
            }
            for (int i = 0 ; i < 1<<(global_depth ); i++)
            {
                Bucket* bucky = table[i];
                table.push_back(bucky);
            }
            global_depth++;
        }
        // doesn't matter if doubled the directory or not, now global_depth > local_depth

        bucky->set_local_depth(bucky->get_local_depth()+1) ;
        int current_local_depth = bucky->get_local_depth();

        Bucket* new_bucky = new Bucket(bucky->get_local_depth(), bucket_size);
        buckets.push_back(new_bucky);

        table[((1<<(current_local_depth-1))|bucket_index)] = new_bucky;

        bucky->reorder_with(new_bucky);
        insert(val);
        return true;
    }
}
bool HashTable::delete_val(int val){
    int hash_val = val % (1<<global_depth);
    int bucket_index = hash_val % (1<<global_depth);
    Bucket* bucky = table[bucket_index];
    return bucky->delete_val(val);
}

bool HashTable::search(int val){
    int hash_val = val % (1<<global_depth);
    int bucket_index = hash_val % (1<<global_depth);
    Bucket* bucky = table[bucket_index];
    return bucky->search(val);
}

void HashTable::display(){
    cout<<global_depth<<"\n";
    cout<<sz(buckets)<<"\n";
    for(int i=0; i<sz(buckets); i++){
        cout<<buckets[i]->get_occupancy()<<" "<<buckets[i]->get_local_depth()<<"\n";
    }
}

///////////////////////////////////////////////////
/////////////////   MAIN FUNCTION   ///////////////
///////////////////////////////////////////////////

int main() { 

    // taking input
    int global_depth = 1; int bucket_size = 1;
    string inp;
    getline(cin, inp);
    istringstream s1(inp);
    s1>>global_depth;
    getline(cin, inp);
    istringstream s2(inp);
    s2>>bucket_size;

    // initializing hash table
    HashTable fantastic(global_depth, bucket_size);

    // starting the control
    while(getline(cin,inp )){

        if(inp == "")  break;

        // extracting the command
        istringstream ss(inp);
        int command;
        ss>>command;

        if(command == 2){
            // insert
            int val;
            ss>>val;
            if(fantastic.insert(val)){
                dout("Inserted "<<val);
            }
            else{
                dout("Failed to insert "<<val);
            }
        }
        else if(command == 3){
            // search
            int val;
            ss>>val;
            if(fantastic.search(val)){
                dout("Found "<<val);
            }
            else{
                dout("Not found "<<val);
            }
        }
        else if(command == 4){
            //delete
            int val;
            ss>>val;
            if(fantastic.delete_val(val)){
                dout("Deleted "<<val);
            }
            else{
                dout("Failed to delete "<<val);
            }
        }
        else if (command == 5){
            // display
            fantastic.display();
        }
        else if(command == 6){
            // exit
            break;
        }
        else {
            dout("Invalid command");
        }
    }

}
