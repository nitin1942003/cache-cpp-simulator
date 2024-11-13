#include <iostream>
#include <vector>
#include <bitset>
#include <random>
#include <chrono>
#include <iomanip>

using namespace std;

// Using unsigned long long for larger counters
unsigned long long hit = 0, miss = 0;

// 1 bit for valid(1) /invalid(0) and 64 sets and 8 ways in each.
vector<vector<pair<bitset<1>, bitset<28>>>> cache(
    64, vector<pair<bitset<1>, bitset<28>>>(
            8, {bitset<1>("0"), bitset<28>(0)}));

//for read write selection
double generateRandomFloorValue() {
    static random_device rd;
    static mt19937 gen(rd());
    static uniform_real_distribution<> dist(0.0, 1.0);
    
    return dist(gen);
}

// for read read request 
void read_req(bitset<28> tag, bitset<6> set_no) {
    int arr_ind = static_cast<int>(set_no.to_ulong());
    
    for (auto &way : cache[arr_ind]) {
        // if tag matches and it is valid it is hit.
        if (way.first == bitset<1>("1") && way.second == tag) {
            hit++;
            return;
        }
    }
    
    // if we do not find the required tag (with valid state) we will run below code.
    int i = 0; // will store the first invalid way.
    for (i = 0; i < 8; i++) {
        if (cache[arr_ind][i].first == bitset<1>("0"))
            break;
    }
    
    //if we have invalid way in given set we will pull data there
    if (i < 8) {
        cache[arr_ind][i].second = tag;
        cache[arr_ind][i].first = bitset<1>("1");
    }

    // else we select any random way between 0 to 7 and evict that and bring the requested data.
    else {
        int way = rand()%8;
        cache[arr_ind][way].second = tag;
        cache[arr_ind][way].first = bitset<1>("1");
    }
    miss++;
}

void write_req(bitset<28> tag, bitset<6> set_no){
    int arr_ind = static_cast<int>(set_no.to_ulong());
    for (auto &way : cache[arr_ind]) {
        if (way.first == bitset<1>("1") && way.second == tag) {
            hit++; // write through, we first write in cache and send the mem request at same time
            return;
        }
    }
    miss++; //write no allocate, send mem request
}

uint64_t generateNormalDistributionAddress(double mean, double stddev) {
    random_device rd;
    mt19937_64 gen(rd());
    normal_distribution<double> dist(mean, stddev);

    double address = dist(gen);
    
    address = max(0.0, min(address, static_cast<double>((1ULL << 40) - 1)));

    return static_cast<uint64_t>(address);
}


int main() {
    //we can setup the duration according to our convinance
    auto duration = chrono::seconds(13);
    cout<<"running for 13 sec, since moodle limit is 14 sec, u may change it(chang in line 87)."<<endl;
    auto start = chrono::steady_clock::now();
    auto last = start;

    //according to this to generateNormalDistributionAddress() gives the random value
    double mean = (1ULL << 10); //mean value 
    double std_dev = (1ULL << 22); // standard deviation

    cout << "Time(sec)\tAccesses\tHit Rate(%)" << endl;
    
    while (chrono::steady_clock::now() - start < duration) {
        uint64_t randomAddress = generateNormalDistributionAddress(mean,std_dev);

        uint64_t tag = (randomAddress >> 12) & ((1ULL << 28) - 1); // our ttag is 28 bit long => 40 - 6(set_no) -6(block-offset)
        uint64_t set_no = (randomAddress >> 6) & ((1ULL << 6) - 1); // 6 bit long set no. since we have 64 sets in cache
        
        if(generateRandomFloorValue()<0.5){
            read_req(bitset<28>(tag), bitset<6>(set_no)); 
        }
        else{
            write_req(bitset<28>(tag), bitset<6>(set_no));
        }
        
        // every second we will print the hit rate.
        auto now = chrono::steady_clock::now();
        if (now - last >= chrono::seconds(1)) {
            auto elapsed_seconds = chrono::duration_cast<chrono::seconds>(now - start).count();
            unsigned long long total_accesses = hit + miss;
            double hit_rate = (total_accesses > 0) ? (100.0 * hit / total_accesses) : 0.0;
            
            cout << elapsed_seconds << "\t\t\t"
                 << total_accesses << "\t\t"
                 << fixed << setprecision(3) << hit_rate << endl;
                 
            last = now;
        }
    }
    
    unsigned long long total_accesses = hit + miss;
    double final_hit_rate = (total_accesses > 0) ? (100.0 * hit / total_accesses) : 0.0;
    
    cout << "\nFinal Status:" << endl;
    cout << "Total Accesses: " << total_accesses << endl;
    cout << "Hit Rate: " << fixed << setprecision(2) << final_hit_rate << "%" << endl;
    
    return 0;
}