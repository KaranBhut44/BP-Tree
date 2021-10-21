#include "InternalNode.hpp"

//creates internal node pointed to by tree_ptr or creates a new one
InternalNode::InternalNode(const TreePtr &tree_ptr) : TreeNode(INTERNAL, tree_ptr) {
    this->keys.clear();
    this->tree_pointers.clear();
    if(!is_null(tree_ptr))
        this->load();
}

//max element from tree rooted at this node
Key InternalNode::max() {
    Key max_key = DELETE_MARKER;
    TreeNode* last_tree_node = TreeNode::tree_node_factory(this->tree_pointers[this->size - 1]);
    max_key = last_tree_node->max();
    delete last_tree_node;
    return max_key;
}

//if internal node contains a single child, it is returned
TreePtr InternalNode::single_child_ptr() {
    if(this->size == 1)
        return this->tree_pointers[0];
    return NULL_PTR;
}

//inserts <key, record_ptr> into subtree rooted at this node.
//returns pointer to split node if exists
//TODO: InternalNode::insert_key to be implemented
TreePtr InternalNode::insert_key(const Key &key, const RecordPtr &record_ptr) {
    
    TreePtr new_tree_ptr = NULL_PTR;
    cout << "InternalNode::insert_key not implemented" << endl;

    int tKey_pos = lower_bound((this->keys).begin(), (this->keys).end(), key) - (this->keys).begin();
    
    TreePtr leafPtr = (this->tree_pointers)[tKey_pos];
    TreeNode * leaf = TreeNode::tree_node_factory(leafPtr);
    
    TreePtr potential_split_node_ptr = leaf->insert_key(key, record_ptr);

    if(!is_null(potential_split_node_ptr)){

        Key Kx = leaf->max();
        TreePtr Kptr = potential_split_node_ptr;

        this->keys.insert(this->keys.begin() + tKey_pos, Kx);
        this->tree_pointers.insert(this->tree_pointers.begin() + tKey_pos + 1, Kptr);

        this->size++;
    }    

    if(this->overflows()){

        InternalNode * R = new InternalNode(NULL_PTR);
        
        int lCount = (int)ceil( (float)FANOUT / 2.0 );

        for(int count=lCount; count<this->keys.size(); count++){
            R->keys.push_back(this->keys[count]);
        }

        for(int count=lCount; count<this->tree_pointers.size(); count++){
            R->tree_pointers.push_back(this->tree_pointers[count]);
            R->size++;
        }

        for(int count=lCount; count<this->keys.size(); count++){
            this->keys.pop_back();
        }

        for(int count=lCount; count<this->tree_pointers.size(); count++){
            this->tree_pointers.pop_back();
            this->size--;
        }

        this->size--;

        new_tree_ptr = R->tree_ptr;
        R->dump();
    }

    this->dump();
    return new_tree_ptr;
}

//deletes key from subtree rooted at this if exists
//TODO: InternalNode::delete_key to be implemented
void InternalNode::delete_key(const Key &key) {
    TreePtr new_tree_ptr = NULL_PTR;
    cout << "InternalNode::delete_key not implemented" << endl;

    // cout<<"\n\n\n *********** THIS \n"<<this<<endl;

    int tKey_pos = lower_bound((this->keys).begin(), (this->keys).end(), key) - (this->keys).begin();
    
    TreePtr leafPtr = (this->tree_pointers)[tKey_pos];
    TreeNode * leaf = TreeNode::tree_node_factory(leafPtr);

    leaf->delete_key(key);
    this->keys[tKey_pos] = leaf->max();

    if(leaf->underflows()){

        if(leaf->node_type == LEAF){

            LeafNode * L = NULL;
            LeafNode * curr = NULL;
            LeafNode * R = NULL;

            if(tKey_pos - 1 >= 0)
                L = (LeafNode *)TreeNode::tree_node_factory((this->tree_pointers)[tKey_pos-1]);

            curr = (LeafNode *)TreeNode::tree_node_factory((this->tree_pointers)[tKey_pos]);

            if(tKey_pos + 1 < this->size)
                R = (LeafNode *)TreeNode::tree_node_factory((this->tree_pointers)[tKey_pos+1]);


            // cout<<"\n\n\n *********** THIS \n"<<this<<endl;

            if(L){
                L->dump();
                // cout<<"\n\n\n *********** L \n"<<L<<endl;    
            }

            if(curr){
                curr->dump();
                // cout<<"\n\n\n *********** Curr \n"<<curr<<endl;    
            }

            if(R){
                R->dump();
                // cout<<"\n\n\n *********** R \n"<<R<<endl;    
            }

            if(L != NULL){
                
                // CASE 1: RDISTRIBUTION LEFT SIDE
                // CASE 2: Merge LEFT SIDE

                if(L->size + curr->size >= 2 * MIN_OCCUPANCY){

                    Key leftMaxKey = L->max();                
                    RecordPtr ptr = L->data_pointers[leftMaxKey];

                    curr->data_pointers.insert({leftMaxKey, ptr});
                    curr->size++;

                    L->data_pointers.erase(leftMaxKey);
                    L->size--;

                    this->keys[tKey_pos-1] = L->max(); 

                }else{

                    for(auto KV : curr->data_pointers){
                        L->data_pointers.insert(KV);
                        L->size++;
                    }

                    L->next_leaf_ptr = curr->next_leaf_ptr;
                    this->keys[tKey_pos] = L->max();

                    this->keys.erase(this->keys.begin() + tKey_pos - 1);
                    this->tree_pointers.erase(this->tree_pointers.begin() + tKey_pos);

                    this->size--;
                }

            }else{

                // CASE 1: RDISTRIBUTION RIGHT SIDE
                // CASE 2: Merge RIGHT SIDE

                if(R->size + curr->size >= 2 * MIN_OCCUPANCY){

                    auto KV = *(R->data_pointers.begin());

                    curr->data_pointers.insert(KV);
                    curr->size++;

                    R->data_pointers.erase(R->data_pointers.begin());
                    R->size--;

                    this->keys[tKey_pos] = curr->max(); 

                }else{
                                        
                    for(auto KV : R->data_pointers){
                        curr->data_pointers.insert(KV);
                        curr->size++;
                    }

                    curr->next_leaf_ptr = R->next_leaf_ptr;
                    this->keys[tKey_pos+1] = curr->max(); 

                    this->keys.erase(this->keys.begin() + tKey_pos);
                    this->tree_pointers.erase(this->tree_pointers.begin() + tKey_pos+1);

                    this->size--;
                }
            }


            // cout<<"\n\n\n *********** THIS \n"<<this<<endl;    

            if(L){
                L->dump();
                // cout<<"\n\n\n *********** L \n"<<L<<endl;    
            }

            if(curr){
                curr->dump();
                // cout<<"\n\n\n *********** Curr \n"<<curr<<endl;    
            }

            if(R){
                R->dump();
                // cout<<"\n\n\n *********** R \n"<<R<<endl;    
            }

        }else{

            InternalNode * L = NULL;
            InternalNode * curr = NULL;
            InternalNode * R = NULL;

            if(tKey_pos - 1 >= 0)
                L = (InternalNode *)TreeNode::tree_node_factory((this->tree_pointers)[tKey_pos-1]);

            curr = (InternalNode *)TreeNode::tree_node_factory((this->tree_pointers)[tKey_pos]);

            if(tKey_pos + 1 < this->size)
                R = (InternalNode *)TreeNode::tree_node_factory((this->tree_pointers)[tKey_pos+1]);


            // cout<<"\n\n\n *********** THIS \n"<<this<<endl;

            if(L){
                L->dump();
                // cout<<"\n\n\n *********** L \n"<<L<<endl;    
            }

            if(curr){
                curr->dump();
                // cout<<"\n\n\n *********** Curr \n"<<curr<<endl;    
            }

            if(R){
                R->dump();
                // cout<<"\n\n\n *********** R \n"<<R<<endl;    
            }

            if(L != NULL){
                
                // CASE 1: RDISTRIBUTION LEFT SIDE
                // CASE 2: Merge LEFT SIDE

                if(L->size + curr->size >= 2 * MIN_OCCUPANCY){

                    Key lKey = L->keys[L->keys.size() - 1];
                    TreePtr lRecordPtr = L->tree_pointers[L->tree_pointers.size() - 1];

                    L->keys.pop_back();
                    L->tree_pointers.pop_back();

                    L->size--;

                    curr->keys.insert(curr->keys.begin(), this->keys[tKey_pos - 1]);
                    curr->tree_pointers.insert(curr->tree_pointers.begin(), lRecordPtr);

                    curr->size++;

                    this->keys[tKey_pos - 1] = lKey;

                }else{

                    L->keys.push_back(this->keys[tKey_pos - 1]);

                    for(Key k : curr->keys){
                        L->keys.push_back(k);
                    }

                    for(TreePtr ptr : curr->tree_pointers){
                        L->tree_pointers.push_back(ptr);
                        L->size++;
                    }

                    this->keys.erase(this->keys.begin() + tKey_pos -1 );
                    this->tree_pointers.erase(this->tree_pointers.begin() + tKey_pos);

                    this->size--;
                }

            }else{

                // CASE 1: RDISTRIBUTION RIGHT SIDE
                // CASE 2: Merge RIGHT SIDE

                if(R->size + curr->size >= 2 * MIN_OCCUPANCY){
                    
                    Key rKey = R->keys[0];
                    TreePtr rRecordPtr = R->tree_pointers[0];

                    R->keys.erase(R->keys.begin());
                    R->tree_pointers.erase(R->tree_pointers.begin());

                    R->size--;

                    curr->keys.push_back(this->keys[tKey_pos]);
                    curr->tree_pointers.push_back(rRecordPtr);

                    curr->size++;

                    this->keys[tKey_pos] = rKey;
                    
                }else{

                    curr->keys.push_back(this->keys[tKey_pos]);

                    for(Key k : R->keys){
                        curr->keys.push_back(k);
                    }

                    for(TreePtr ptr : R->tree_pointers){
                        curr->tree_pointers.push_back(ptr);
                        curr->size++;
                    }

                    this->keys.erase(this->keys.begin() + tKey_pos);
                    this->tree_pointers.erase(this->tree_pointers.begin() + tKey_pos+1);

                    this->size--;
                    
                }
            }

            // cout<<"\n\n\n *********** THIS \n"<<this<<endl;

            if(L){
                L->dump();
                // cout<<"\n\n\n *********** L \n"<<L<<endl;    
            }

            if(curr){
                curr->dump();
                // cout<<"\n\n\n *********** Curr \n"<<curr<<endl;    
            }

            if(R){
                R->dump();
                // cout<<"\n\n\n *********** R \n"<<R<<endl;    
            }
        }
    }

    this->dump();

    // cout<<"\n\n\n -------------------------------------------------------- DONE \n\n";
}

//runs range query on subtree rooted at this node
void InternalNode::range(ostream &os, const Key &min_key, const Key &max_key) const {
    BLOCK_ACCESSES++;
    for(int i = 0; i < this->size - 1; i++){
        if(min_key <= this->keys[i]){
            auto* child_node = TreeNode::tree_node_factory(this->tree_pointers[i]);
            child_node->range(os, min_key, max_key);
            delete child_node;
            return;
        }
    }
    auto* child_node = TreeNode::tree_node_factory(this->tree_pointers[this->size - 1]);
    child_node->range(os, min_key, max_key);
    delete child_node;
}

//exports node - used for grading
void InternalNode::export_node(ostream &os) {
    TreeNode::export_node(os);
    for(int i = 0; i < this->size - 1; i++)
        os << this->keys[i] << " ";
    os << endl;
    for(int i = 0; i < this->size; i++){
        auto child_node = TreeNode::tree_node_factory(this->tree_pointers[i]);
        child_node->export_node(os);
        delete child_node;
    }
}

//writes subtree rooted at this node as a mermaid chart
void InternalNode::chart(ostream &os) {
    string chart_node = this->tree_ptr + "[" + this->tree_ptr + BREAK;
    chart_node += "size: " + to_string(this->size) + BREAK;
    chart_node += "]";
    os << chart_node << endl;

    for(int i = 0; i < this->size; i++) {
        auto tree_node = TreeNode::tree_node_factory(this->tree_pointers[i]);
        tree_node->chart(os);
        delete tree_node;
        string link = this->tree_ptr + "-->|";

        if(i == 0)
            link += "x <= " + to_string(this->keys[i]);
        else if (i == this->size - 1) {
            link += to_string(this->keys[i-1]) + " < x";
        } else {
            link += to_string(this->keys[i-1]) + " < x <= " + to_string(this->keys[i]);
        }
        link += "|" + this->tree_pointers[i];
        os << link << endl;
    }
}

ostream& InternalNode::write(ostream &os) const {
    TreeNode::write(os);
    for(int i = 0; i < this->size - 1; i++){
        if(&os == &cout)
            os << "\nP" << i+1 << ": ";
        os << this->tree_pointers[i] << " ";
        if(&os == &cout)
            os << "\nK" << i+1 << ": ";
        os << this->keys[i] << " ";
    }
    if(&os == &cout)
        os << "\nP" << this->size << ": ";
    os << this->tree_pointers[this->size - 1];
    return os;
}

istream& InternalNode::read(istream& is){
    TreeNode::read(is);
    this->keys.assign(this->size - 1, DELETE_MARKER);
    this->tree_pointers.assign(this->size, NULL_PTR);
    for(int i = 0; i < this->size - 1; i++){
        if(&is == &cin)
            cout << "P" << i+1 << ": ";
        is >> this->tree_pointers[i];
        if(&is == &cin)
            cout << "K" << i+1 << ": ";
        is >> this->keys[i];
    }
    if(&is == &cin)
        cout << "P" << this->size;
    is >> this->tree_pointers[this->size - 1];
    return is;
}
