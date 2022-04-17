#include <iostream>
#include <vector>
using namespace std;


/////////////// 
//   PAGE    //
///////////////
class Page
{
private:
    int page_size;
public:
    vector<pair<int,int>>data;
    int count_records;
    int available_space;
    Page* next_page;
    Page* prev_page;

    Page(int page_size_init){
        count_records=0;
        page_size = page_size_init;
        available_space = page_size_init - 16;
        next_page = nullptr;
        prev_page = nullptr;
    }

    bool insert_element(int record_size, int key){
        pair<int,int> record;
        record.first = record_size;
        record.second = key;
        if(available_space >= record_size + 4){
            data.push_back(record);
            count_records++;
            available_space = available_space - (record_size + 4);
            return true;
        }
        else{
            return false;
        }
    }

    int search_element(int primary_key){
        for(int i=0;i<data.size();i++){
            if(data[i].second == primary_key){
                return i;
            }
        }
        return -1;
    }
};

/////////////////// 
//   HEAP FILE   //
///////////////////
class HeapFile
{
public:
    Page* firstptr;
    int num_pages;
    int page_size;

    // constructor
    HeapFile(int page_size_init){
        num_pages = 0;
        firstptr = nullptr;
        page_size = page_size_init;
    }

    // insert method
    bool insert_element(int record_size,int key){
        Page* curr = firstptr;
        Page* prevcurr = nullptr;
        while(curr != nullptr){
            bool yes = curr->insert_element(record_size,key);
            if(yes == true){
                break;
            }
            else{
                prevcurr = curr;
                curr = curr->next_page;
            }
        }

        // didn't find any page with sufficient space
        // so create a new page
        if(curr == nullptr){
            Page* new_page = new Page(page_size);
            new_page->insert_element(record_size,key);

            new_page->next_page = nullptr;
            if(prevcurr != nullptr){
                new_page->prev_page = prevcurr;
                prevcurr->next_page = new_page;
            }

            num_pages++;
            if(num_pages == 1){
                firstptr = new_page;
            }
        }
        return true;
    }

    pair<int,int> search_element(int key){
        int v = num_pages;
        Page* curr = firstptr;

        // count starts from 0.
        int page_slot=0;

        while(curr != nullptr){
            int yes = curr->search_element(key);

            if(yes != -1){
                return make_pair(page_slot,yes);
            }
            else{
                curr = curr->next_page;
            }
             page_slot++;
        }

        // if not found then return -1
        return make_pair(-1,-1);
    }

    void status(){
        cout << num_pages ;

        Page* curr = firstptr;

        while(curr != nullptr){
            int num_records = curr->count_records;
            cout << " " << num_records ;
            curr = curr->next_page;
        }
        cout << endl;
    }
};


/////////////// 
//   MAIN    //
///////////////
int main()
{
    int page_size;
    cin >> page_size;

    HeapFile fantastic(page_size);
    int command;
    while(1){
        cin >> command;

        if(command == 1){
            int record_size, key;
            cin >> record_size >> key;
            fantastic.insert_element(record_size,key);
        }
        else if(command == 2){
            fantastic.status();
        }
        else if(command == 3){
            int key;
            cin >> key;
            pair<int,int> ans;
            ans = fantastic.search_element(key);
            cout << ans.first << " " << ans.second << endl;
        }
        else if(command == 4){
            break;
        }
    }
}
