#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <map>

using namespace std;

#define dout(x) if(debug) std::cout << x << std::endl;
bool debug = false;

#define sz(v) ((int)(v).size())

// the behaviour of data nodes and index nodes could be modelled with 
// either inheritance or composition.

// I'm choosing inheritance here, 
// since a datanode _is_ a node, and doesn't _have_ a node

class Node 
{
protected:
    int* arr;
    int size;
    int occ;
    char type;
public:
    Node(int cap) {
        arr = new int[cap];
        size = cap;
        occ = 0;
    }
    ~Node() {
        delete[] arr;
    }
    char getType() {
        return type;
    }
    void setType(char t) {
        type = t;
    }
    bool isFull() {
        return occ == size;
    }
    bool insert(int val){
        // insert in sorted order
        if(isFull()) return false;
        int i = getPlaceToFitIn(val);
        for(int j = occ; j > i; j--) arr[j] = arr[j-1];
        arr[i] = val;
        occ++;
        return true;
    }
    void print() {
        for(int i = 0; i < occ; i++) {
            cout << arr[i] << " ";
        }
        cout << endl;
    }
    int getPlaceToFitIn(int val) {
        int i = 0;
        while(i < occ && arr[i] < val) i++;
        return i;
    }
    int popLowest(){
        int ret = arr[0];
        for(int i = 0; i < occ; i++) arr[i] = arr[i+1];
        occ--;
        return ret;
    }
    int getLowest() {
        return arr[0];
    }
    int popHighest(){
        int ret = arr[occ-1];
        occ--;
        return ret;
    }
    int getHighest() {
        return arr[occ-1];
    }
    void change(int a, int b){
        int i=0;
        while(arr[i]!=a) i++;
        arr[i] = b;
    }
};

class IndexNode : public Node
{
private:
public:
    Node** children;
    IndexNode(int cap) : Node(cap) {
        children = new Node*[(cap+1)];
    }
    ~IndexNode() {
        delete[] children;
    }
    void insertChild(int i, Node* child){

		for (int j = Node::occ; j > i + 1; j--) {
			this->children[j] = this->children[j - 1];
		}
		this->children[i + 1] = child;
    }

    int shiftStuff(int val, IndexNode* newcur, Node* child){

        int t = Node::size / 2;

        // define temp arrays to hold everything together
		int* virtualKey = new int[2*t+2];
		Node** virtualPtr = new Node*[2*t+3];
		for (int i = 0; i < 2*t + 1; i++) {
			virtualKey[i] = Node::arr[i];
		}
		for (int i = 0; i < 2*t + 2; i++) {
			virtualPtr[i] = this->children[i];
		}
		int i = 0, j;
		while (val > virtualKey[i] && i < 2*t+1) i++;
		for (int j = 2*t+1; j > i; j--) {
			virtualKey[j] = virtualKey[j - 1];
		}
		virtualKey[i] = val;
		for (int j = 2*t+2; j > i + 1; j--) {
			virtualPtr[j] = virtualPtr[j - 1];
		}
		virtualPtr[i + 1] = child;
        // copying completed

        // divide virtualKey 2t+2 into 0-(t-1) and t+1-(2t+1), 
        for(int i = 0; i < t; i++) {
            this->arr[i] = virtualKey[i];
        }
		for ( j = t+1 ; j<2*t+2; j++) {
			newcur->insert(virtualKey[j]);
		}
        for(int i = 0; i < t+1; i++) {
            this->children[i] = virtualPtr[i];
        }
		for ( j = t+1; j< 2*t+3; j++) {
			newcur->children[i] = virtualPtr[j];
		}
        this->occ = t;

        int val_sent_up = virtualKey[t];
        delete[] virtualKey;
        delete[] virtualPtr;
        return val_sent_up;
    }
};

class DataNode : public Node
{
private:
public:
    IndexNode* parent;
    DataNode* left;
    DataNode* right;
    DataNode(int cap) : Node(cap) {
        this->left = NULL;
        this->right = NULL;
        this->parent = NULL;
    }
    ~DataNode() {}
    void updateParent(int low){
        parent->change(low, getLowest());
    }
    void shiftStuff(int val, DataNode* p){
        // first determine whether val will be in prev or new node
        if(val>Node::arr[(Node::occ / 2) - 1]){
            // val will be in new node
            p->insert(val);
            for(int i=0; i<(Node::size / 2 ); i++){
                p->insert(this->popHighest());
            }
        }
        else{
            // val will be in prev node
            for(int i=0; i < ( (Node::size / 2 ) + 1 ); i++){
                p->insert(this->popHighest());
            }
            this->insert(val);
        }
    }
};

//==========//
//  B+Tree  //
//==========//

class BplusTree
{
private:
    int d; int t;
    vector<IndexNode*> index_nodes;
    vector<DataNode*> data_nodes;
    Node* root;
    void insertInternal(int, IndexNode*, Node*);
    IndexNode* findParent( Node*);
public:
    BplusTree(int d, int t);
    ~BplusTree();
    bool insert(int val);
    void print();
};

BplusTree::BplusTree(int d, int t)
{
    this->d = d;
    this->t = t;
    root = new DataNode(2*d);
    DataNode* dn = (DataNode*)root;
    dn->setType('d');
    dn->right = dn;
    dn->left = dn;
    data_nodes.push_back(dn);
}

BplusTree::~BplusTree()
{
    for(int i=0; i<sz(index_nodes); i++) {
        delete index_nodes[i];
    }
    for(int i=0; i<sz(data_nodes); i++) {
        delete data_nodes[i];
    }
}

bool BplusTree::insert(int val)
{
    Node* cur = root;
    while(cur->getType() == 't'){
        // iterate over indices of cur and move cur to the appropriate child
        int pos = cur->getPlaceToFitIn(val);
        IndexNode* in = (IndexNode*)cur;
        cur = in->children[pos];
    }
    // then insert into that node
    if( ! cur->isFull()){
        cur->insert(val);
        return true;
    } 
    else{
        DataNode* dn = (DataNode*)cur;

        // // can't insert normally, either
        // // shift to left sibling, 
        // if( dn->left != nullptr){
        //     if(  ! dn->left->isFull()){
        //         int low = dn->popLowest();
        //         if(val<low){
        //             swap(val, low);
        //         }
        //         dn->left->insert(low);
        //         dn->insert(val);
        //         dn->updateParent(low);
        //         // TODO this update Parent is not enough, you gotta update all ancestors 
        //         return true;
        //     }
        // }
        // // shift to right sibling, 
        // if(dn->right != nullptr){
        //     if( ! dn->right->isFull()){
        //         int high = dn->popHighest();
        //         if(val>high){
        //             swap(val, high);
        //         }
        //         int low = dn->right->getLowest();
        //         dn->right->insert(high);
        //         dn->insert(val);
        //         dn->right->updateParent(low);
        //         return true;
        //     }
        // }
        // or split and insert into the new node
        DataNode* new_node = new DataNode(2*d);
        new_node->setType('d');
        data_nodes.push_back(new_node);

        // first maintain the linked list
        DataNode* righty = dn->right;
        dn->right = new_node;
        if(righty != NULL) righty->left = new_node;
        new_node->left = dn;
        new_node->right = righty;

        // then shift keys to the new node
        dn->shiftStuff(val, new_node);

        // add an element to the parent node
        // if the parent is null, create a parent
        if( dn == root){
            IndexNode* in = new IndexNode(2*t +1);
            in->setType('t');
            index_nodes.push_back(in);

            in->children[0] = dn;
            in->children[1] = new_node;
            in->insert(new_node->getLowest());
            root = in;
        }
        // else normal insert
        else{
			insertInternal(new_node->getLowest(), findParent(dn), new_node);
        }

    }
    return true;
}

// Function to implement insert in the BTree part of B+Tree
void BplusTree::insertInternal( int x, IndexNode* cur, Node* child){

	// If we don't have overflow
	if (! cur->isFull()) {
		int i = cur->getPlaceToFitIn(x);
        cur->insert(x);
        cur->insertChild(i, child);
	}

	// For overflow, break the node
	else {
        IndexNode* newcur = new IndexNode(2*t+1);
        newcur->setType('t');
        index_nodes.push_back(newcur);
        int val_sent_up = cur->shiftStuff(x, newcur, child);

		// If cursor is the root node
		if (cur == root) {

            // TODO can root have less than t keys?
            // CORMER SAYS YES
            
			// Create a new root node
			IndexNode* newRoot = new IndexNode(2*t+1);
            newRoot->setType('t');
            index_nodes.push_back(newRoot);
			// Update key value
            newRoot->insert(val_sent_up);
            newRoot->children[0] = cur;
            newRoot->children[0] = newcur;
			root = newRoot;
		}

		else {
			// Recursive Call to insert the data
			insertInternal(val_sent_up, findParent(cur), newcur);
		}
	}
}

// Function to find the parent node
IndexNode* BplusTree::findParent(Node* child)
{
    // use the values in child to trav linearly rather than starting a DFS
    IndexNode* parent;
    Node* cur = root;
    while(cur != child && cur->getType() == 't'){
        int pos = cur->getPlaceToFitIn(child->getHighest());
        parent = (IndexNode*)cur;
        cur = parent->children[pos];
    }
    if(cur == child) return parent;
    else return NULL;
}

void BplusTree::print()
{
    cout<<sz(index_nodes)<<" "<<sz(data_nodes)<<" ";
    root->print();
    // for(int i=0; i<sz(data_nodes); i++){
    //     cout<<"data node "<<i<<"\n";
    //     data_nodes[i]->print();
    // }
}

//==========//
//   MAIN   //
//==========//
int main(){
    int data_node = 0; // cap is 2d
    int index_node = 0; // cap is 2t+1, and atleast t keys
    cin>>data_node>>index_node;

    int command = 0;
    BplusTree fantastic(data_node, index_node);
    while(true){
        cin>>command;
        if(command==1){
            // insert
            int val; cin>>val;
            if(fantastic.insert(val)){
                dout("Inserted "<<val);
            }else{
                dout("not inserted" << val);
            }
        } else if(command==2){
            fantastic.print(); // print 
        } else if(command==3){
            break; // exit
        }

    }
}

