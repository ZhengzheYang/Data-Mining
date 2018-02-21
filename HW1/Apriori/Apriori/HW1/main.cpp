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

vector<vector<int>> candidate_itemset;  //self-joined from the last frequent itemset
vector<vector<int>> frequent_itemset;   //current frequent itemset found and will be used to generate candidates

//the result vector
vector<vector<int>> output;
vector<vector<int>> first_subset;   //1-element subset after reading in the dataset

/* This 3-d vector is used to store the subset for each entry in the dataset. After counting all the occurences, 
 we will generate new subsets using current subsets by joining them with the first_subset above. */
vector<vector<vector<int>>> subset;

/* This part of the code is from Boost library tutorial. Boost can be used to hash the vector because it provides 
 relatively better hash function */
template <typename Container>
struct container_hash {
    size_t operator()(Container const& c) const {
        return boost::hash_range(c.begin(), c.end());
    }
};

/* debug function to print out a 2-d vector */
void print_vector(vector<vector<int>> a) {
    for(auto i : a) {
        for(auto j : i) {
            cout << j << " ";
        }
        cout << "\n";
    }
}

unordered_map<vector<int>, int, container_hash<vector<int>>> hash_map;


/*join two qualified set. For example, abc + abd = abcd */
vector<int> self_joining(vector<int> a, vector<int> b) {
    a.push_back(b[b.size() - 1]);
    return a;
}



/*First, make sure every element is the same except the last, and the last in acsending order */
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


/*Then we prune out the ones that do contains infrequent subsets */
void prune_candidate_set() {
    for(size_t i = 0; i < candidate_itemset.size(); i++) {
        for(size_t j = 0; j < candidate_itemset[i].size(); j++) {
            vector<int> temp = candidate_itemset[i];
            temp.erase(temp.begin() + j);
            if(hash_map[temp] < threshold) {
                candidate_itemset.erase(candidate_itemset.begin() + i);
            }
        }
    }
}

/*update the candidate set by joining every element in the last frequent_itemset */
void update_candidate_set() {
    vector<vector<int>> candidate_set;
    if(frequent_itemset.size() == 0) {
        candidate_itemset = candidate_set;
        return;
    }
    
    for(size_t i = 0; i < frequent_itemset.size() - 1; i++) {
        for(size_t j = i + 1; j < frequent_itemset.size(); j++) {
            if(qualified(frequent_itemset[i], frequent_itemset[j])) {
                candidate_set.push_back(self_joining(frequent_itemset[i], frequent_itemset[j]));
            }
        }
    }

    candidate_itemset = candidate_set;
}

/* Recursive helper method to generate subset with k-element */
void generate_helper(vector<int>& nums, int start, vector<int>& sub, vector<vector<int>>& subs, int k) {
    if(sub.size() == k) {
        subs.push_back(sub);
        return;
    }
    if(sub.size() > 1 && hash_map[sub] < threshold) {
        return;
    }
    for (int i = start; i < nums.size(); i++) {
        sub.push_back(nums[i]);
        generate_helper(nums, i + 1, sub, subs, k);
        sub.pop_back();
    }
}

/* Using backtracking to generate subsets. However, we only use this for the 2-element subsets.
 Later, when it becomes more complicated and we need to avoid adding unnecesssary subsets, we use the method
 below. */
void generate_and_count_subset(size_t index, int k) {
    vector<vector<int>> subs;
    vector<int> sub;
    generate_helper(first_subset[index], 0, sub, subs, k);
    subset.push_back(subs);
    
    for(auto& it : subs) {
        if(hash_map.find(it) == hash_map.end()) {
            hash_map[it] = 1;
        } else {
            hash_map[it] = hash_map[it] + 1;
        }
    }
}

/* Use this method to generate subset after 2-element subsets */
void generate_and_count_subset_later(size_t index, int k) {
    vector<vector<int>> res;
    for(auto& j : subset[index]) {
        for(auto& num : first_subset[index]) {
            if(j.size() > 0 && j[j.size() - 1] >= num) continue;
            if(k > 1 && hash_map[j] < threshold) {
                break;
            }
            vector<int> vec = j;
            vec.push_back(num);
            res.push_back(vec);
            if(hash_map.find(vec) == hash_map.end()) {
                hash_map[vec] = 1;
            } else {
                hash_map[vec] = hash_map[vec] + 1;
            }
        }

    }
    
    subset[index] = res;
}

//update the frequent set by counting all the subsets in the data
void update_frequent_set(int k) {
    vector<vector<int>> temp_freq;
    
    if(k == 2) {
        for(size_t index = 0; index < first_subset.size(); index++) {
            generate_and_count_subset(index, k);
        }
    } else {
        for(size_t index = 0; index < first_subset.size(); index++) {
            generate_and_count_subset_later(index, k);
        }
    }

    for(auto& it : candidate_itemset) {
        if(hash_map[it] >= threshold) {
            temp_freq.push_back(it);
            output.push_back(it);
        }
    }
    frequent_itemset = temp_freq;
}

//find k-element itemset
void find_frequent_itemset() {
    for(int k = 1; k < freq_num; k++) {
        cout<<"Currently working on " << k << endl;
        update_candidate_set();
        if(k > 1) {
            prune_candidate_set();
        }
        if(candidate_itemset.size() == 0) {
            break;
        }
        update_frequent_set(k + 1);
    }
}

/* Build the dataset line by line. 
 Find the 1-element frequent item set.
 Use this to prune the unwanted elements.*/
int buildDataSet(string filename) {
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
            if(it.size() > 0) {
                vector<int> single;
                int cast = stoi(it);
                single.push_back(cast);
                int_row.push_back(cast);
                if(hash_map.find(single) == hash_map.end()) {
                    hash_map[single] = 1;
                } else {
                    hash_map[single] = hash_map[single] + 1;
                }
            }
        }
        first_subset.push_back(int_row);
    }

    vector<vector<int>>::iterator it = first_subset.begin();
    for(; it != first_subset.end(); it++) {
        vector<int>::iterator j = it->begin();
        while(j != it->end()) {
            vector<int> temp;
            temp.push_back(*j);
            if(hash_map[temp] < threshold) {
                it->erase(j);
            } else {
                j++;
            }
        }
    }
    
    for(auto& pair : hash_map) {
        if(pair.second >= threshold) {
            frequent_itemset.push_back(pair.first);
            output.push_back(pair.first);
            freq_num++;
        }
    }
    
    sort(frequent_itemset.begin(), frequent_itemset.end());
    cout<<"Done building the dataset" << endl;
    return 1;
}

int main(int argc, const char * argv[]) {
    clock_t start_time = clock();
    if(argc != 4) {
        cout << "Wrong number of arguments" << endl;
        return -1;
    }
    //Get file name
    string filename = argv[1];
    threshold = stoi(argv[2]);
    string output_filename = argv[3];
    
    //Build the dataset
    if(buildDataSet(filename) == -1) {
        return -1;
    }
    
    //Find the frequent itemset
    find_frequent_itemset();

    //sort the result to be written into the file
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

    //write to the file
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

    clock_t end_time = clock();
    cout << "time: " << (end_time-start_time)/double(CLOCKS_PER_SEC) << endl;
    return 0;
}



