//
//  main.cpp
//  Apriori
//
//  Name: Zhengzhe Yang
//
//

#include <iostream>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <boost/functional/hash.hpp>
#include <boost/algorithm/string.hpp>
#include <ctime>
using namespace std;

int i = 0;
int threshold;
int freq_num;

//.dat file taken in
vector<vector<int>> data;

vector<vector<int>> candidate_itemset;
vector<vector<int>> frequent_itemset;

//the result vector
//vector<vector<vector<int>>> total_frequent_itemset;
vector<vector<vector<int>>> subset;

template <typename Container>
struct container_hash {
    size_t operator()(Container const& c) const {
        return boost::hash_range(c.begin(), c.end());
    }
};
void print_vector(vector<vector<int>> a) {
    for(auto i : a) {
        for(auto j : i) {
            cout << j << " ";
        }
        cout << "\n";
    }
}

unordered_map<vector<int>, int, container_hash<vector<int>>> hash_map;

void prune_subset() {
    for(size_t i = 0; i < subset.size(); i++) {
        for(size_t j = 0; j < subset[i].size(); j++) {
            if(hash_map[subset[i][j]] < threshold) {
                subset[i].erase(subset[i].begin() + j);
                j--;
            }
        }
    }
}


//find the 1- and 2-element frequent set
void find_frequent_items(){
    vector<vector<int>> temp_list;
    for(int i = 0; i < data.size(); i++) {
        vector<vector<int>> vec;
        for(int j = 0; j < data[i].size(); j++) {
            vector<int> key;
            key.push_back(data[i][j]);
            vec.push_back(key);
            if(hash_map.find(key) == hash_map.end()) {
                hash_map[key] = 1;
            } else {
                hash_map[key] = hash_map[key] + 1;
            }
        }
        subset.push_back(vec);
    }
    prune_subset();

    for(auto& pair : hash_map) {
        if(pair.second >= threshold) {
            temp_list.push_back(pair.first);
            freq_num++;
        }
    }
    
    sort(temp_list.begin(), temp_list.end());
    frequent_itemset = temp_list;
}


//join two qualified set. For example, abc + abd = abcd
vector<int> self_joining(vector<int> a, vector<int> b) {
//    vector<int> test1 = {1, 3};
//    vector<int> test2 = {1, 5};
    a.push_back(b[b.size() - 1]);
//    for(auto& it : a) {
//        cout<<it<<" ";
//    }
//    cout<<"\n";
//    for(auto& it : b) {
//        cout<<it<<" ";
//    }
//    cout<<"\n";
//    if(a == test1 && b == test2) cout<<"jack"<< endl;
    
    return a;
}



//First, make sure every element is the same except the last, and the last in acsending order
bool qualified(vector<int> a, vector<int> b) {
    size_t len = a.size();
    for(size_t i = 0; i < len - 1; i++) {
        if(a[i] != b[i]) {
            return false;
        }
    }
    
    if(a[len - 1] > b[len - 1]) {
        return false;
    }
  
    return true;
}


//Then we prune out the ones that do contains infrequent subsets
vector<vector<int>> prune_candidate_set(vector<vector<int>> candidate_set) {
//    set<vector<int>> frequent_set;
    vector<vector<int>> pruned_candidate_set;
//
//    for(auto &it : frequent_itemset) {
//        frequent_set.insert(it);
//    }
    
    for(auto vec : candidate_set) {
        bool not_found = false;
//        vector<int> test = {1, 3, 5};
//        if(vec == test) {
//            cout<<"jack"<<endl;
//        }
        for(size_t i = 0; i < vec.size(); i++) {
            vector<int> temp = vec;
//            if(temp == test) {
//                cout<<"135jack"<<endl;
//            }
            temp.erase(temp.begin() + i);
//            for(auto& it : temp) {
//                cout<<it<<" ";
//            }
//            cout<<endl;
//            if(find(frequent_itemset.begin(), frequent_itemset.end(), temp) == frequent_itemset.end()) {
////                vector<int> temp2 = {3, 5};
////                if(temp2 == temp) cout<<"jack"<<endl;
//                not_found = true;
//                break;
//            }
            if(hash_map[temp] < threshold) {
                not_found = true;
                break;
            }
        }
        if(!not_found) {
            pruned_candidate_set.push_back(vec);
//            for(auto& it : vec) {
//                cout<<it<<" ";
//            }
//            cout<<endl;
        }
    }
    return pruned_candidate_set;
}

//update the candidate set by joining every element in the last frequent_itemset
void update_candidate_set() {
    vector<vector<int>> candidate_set;
    if(frequent_itemset.size() == 0) {
        candidate_itemset = candidate_set;
        return;
    }
    for(size_t i = 0; i < frequent_itemset.size() - 1; i++) {
//        cout<<i<<endl;
        for(size_t j = i + 1; j < frequent_itemset.size(); j++) {
            if(qualified(frequent_itemset[i], frequent_itemset[j])) {
//                if(frequent_itemset[i] == test1 && frequent_itemset[j] == test2) cout<<"jack"<< endl;
//                if() cout<<"jack"<< endl;
                candidate_set.push_back(self_joining(frequent_itemset[i], frequent_itemset[j]));
            }
        }
//        cout<<"frequent size: "<<frequent_itemset.size()<<endl;
//        cout<<"currently i: " << i<<endl;
    }
//    print_vector(candidate_set);
    if(candidate_set.size() != 0) {
//        cout<<"jack"<<endl;
//        cout<<candidate_set.size()<<endl;
//        cout<<"Before prune: ";
//        print_vector(candidate_set);
        candidate_itemset = prune_candidate_set(candidate_set);
//        cout<<"After prune: ";
//        print_vector(candidate_itemset);
    } else {
        candidate_itemset = candidate_set;
    }
}

vector<vector<int>> generate_and_count_subset(size_t index, int i) {
    vector<vector<int>> res;
//    for(auto& num : row) {
//        vector<vector<int>> temp;
//        for(auto& j : subset) {
//            vector<int> vec = j;
//            vec.push_back(num);
//            temp.push_back(vec);
//        }
//        res.insert(res.end(), temp.begin(), temp.end());
//    }
//    vector<int> test = {1, 2};
    for(auto& j : subset[index]) {
//        vector<vector<int>> temp;
//        cout<<subset[index].size()<<endl;
        
        for(auto& num : data[index]) {
//            cout<<num<<endl;
            if(j.size() > 0 && j[j.size() - 1] >= num) continue;
            if(hash_map[j] < threshold) {
                break;
            }
            vector<int> vec = j;
            
//            if(vec == test) cout<<"jack"<<endl;
            vec.push_back(num);
//            temp.push_back(vec);
            res.push_back(vec);
        }
//        cout<<j.size()<<endl;
//        print_vector(frequent_itemset);
//        if(update)
//            res.insert(res.end(), temp.begin(), temp.end());
//        if(j == test) cout<<"jack"<<endl;
    }
    
    for(auto& j : res) {
//        if(j == test) cout<<"jack"<<endl;
        if(hash_map.find(j) == hash_map.end()) {
            hash_map[j] = 1;
        } else {
            hash_map[j] = hash_map[j] + 1;
        }
    }
    subset[index] = res;

    return res;
}

//update the frequent set by counting all the subsets in the data
void update_frequent_set(int i) {
    vector<vector<int>> temp_freq;
    for(size_t index = 0; index < data.size(); index++) {
        generate_and_count_subset(index, i);
    }
//    prune_subset();
//    cout<<"Pruned subset"<<endl;
    for(auto& it : candidate_itemset) {
//        print_vector(candidate_itemset);
        if(hash_map[it] >= threshold) {
            temp_freq.push_back(it);
        }
    }
    frequent_itemset = temp_freq;
}



//find k-element itemset
void find_frequent_itemset() {
    for(int k = 1; k < freq_num; k++) {
        cout<<"Currently working on " << k << endl;
        update_candidate_set();
        if(candidate_itemset.size() == 0) {
            break;
        }
        update_frequent_set(k + 1);
//        print_vector(frequent_itemset);
//        total_frequent_itemset.push_back(frequent_itemset);
        
    }
}

int main(int argc, const char * argv[]) {
    // insert code here...
    //    std::cout << "Hello, World!\n";
    clock_t start_time = clock();
    if(argc != 4) {
        cout << "Wrong number of arguments" << endl;
        return -1;
    }
    string filename = argv[1];
    
    string output_filename = argv[3];
    
    ifstream input_file;
    input_file.open(filename);
    
    if(!input_file) {
        cout<<"Couldn't open file. Exit." <<endl;
        return -1;
    }
    
    string sentence;
    vector<string> row;
    while(getline(input_file, sentence)) {
        vector<int> int_row;
        boost::split(row, sentence, boost::is_any_of(" "));
        for(auto& it : row) {
            if(it.size() > 0)
                int_row.push_back(stoi(it));
        }
        data.push_back(int_row);
    }
    threshold = stoi(argv[2]);
    cout<<"Done building the dataset" << endl;
//    print_vector(data);
    
//    vector<vector<int>> a;
//    for(int i = 0; i < 5; i++) {
//        vector<int> b;
//        b.push_back(i);
////        if(i == 2 || i == 3) {
////            a.push_back(b);
////        }
//        a.push_back(b);
//    }
//    data = {
//        {1, 2, 5},
//        {2, 4},
//        {2, 3},
//        {1, 2, 4},
//        {1, 3},
//        {2, 3},
//        {1, 3},
//        {1, 2, 3, 5},
//        {1, 2, 3}
//    };
//    data = {
//        {1, 2, 5},
//        {1, 2, 3, 5},
//        {1, 2, 3, 5},
//        {1, 2, 3}
//    };
//    find_frequent_items();
//    cout<< freq_num << endl;
//    print_vector(frequent_itemset);
//    print_vector(total_frequent_itemset[0]);
//    print_vector(generate_candidate_set());
//    print_vector(data);
//    vector<int> init1;
//    init2.push_back(init1);
    
//    for(auto& it : subset) {
//        print_vector(it);
//    }
    
    
//    print_vector(data);
//    vector<vector<int>> temp;
//    vector<int> temp1;
//    temp.push_back(temp1);
//    temp = {{0, 1}, {1, 3}, {2, 3}, {3, 4}};
//    print_vector(generate_and_count_subset(a, temp, 1));
    
//
//    for(auto& it : total_frequent_itemset) {
//        print_vector(it);
//    }
    subset.reserve(data.size());
    find_frequent_items();
    cout<<"Done finding the first frequent itemset" << endl;
    clock_t find_start = clock();
    find_frequent_itemset();
    vector<vector<int>> output;
    clock_t find_end = clock();
    cout << "find time: " << (find_end-find_start)/double(CLOCKS_PER_SEC) << endl;
//    for(auto& it : total_frequent_itemset) {
//        for(auto& ele : it) {
//            output.push_back(ele);
//        }
//    }
    
    vector<int> test = {1,2};
    for(auto& pair : hash_map) {
        if(pair.second >= threshold) {
            output.push_back(pair.first);
        }
    }
//
    sort(output.begin(), output.end(), [](const vector<int>& a, const vector<int>& b) {
        size_t len = min(a.size(), b.size());
        for(size_t i = 0; i < len; i++) {
            if(a[i] == b[i]) {
                continue;
            }
            return a[i] < b[i];
        }
        return a.size() < b.size();
    });
//    sort(total_frequent_itemset.begin(), total_frequent_itemset.end(), [](const vector<vector<int>>& a, const vector<vector<int>>& b) {
//        return a[0][0] < b[0][0];
//    });
    
//    for(auto& it : total_frequent_itemset) {
//        print_vector(it);
//    }

    ofstream output_file(output_filename);
    if (output_file.is_open()) {
        for(auto& it : output) {
            for(auto& col : it) {
                output_file << to_string(col) << " ";
            }
            output_file << "(" << hash_map[it] << ")";
            output_file << "\n";
            
        }
        output_file.close();
    }
//
    clock_t end_time = clock();
    cout << "time: " << (end_time-start_time)/double(CLOCKS_PER_SEC) << endl;
    return 0;
}



