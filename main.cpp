// #define DEBUG_FLAG
#include <bits/stdc++.h>
#include "Mmap.h"
#include <time.h>

#include "common/common.h"
#include "common/sketch.h"
#include "elastickll/ElasticKLL.cpp"
#include "req/ReqSketch.cpp"
#include "tdigest/Tdigest.cpp"
#include "kll/KLL.cpp"
#include "dd/DD.cpp"
#include "gk/GK.cpp"


#define QUERYTESTNUMBER 5
#define TEST_NUMBER 5

using namespace std;

class Benchmark{
public:
    unordered_map<elem_t, uint32_t> insert_distribution;
    vector<elem_t> insert_element_list;
    vector<double> aqe, are, throughput, aqe_time, are_time;
    vector<vector<double> > aqe_distribution, are_distribution;
    vector<string> sketch_name_list;
    vector<int> current_test_number;
    uint32_t current_sketch_memory;

    uint32_t total_elem;
    elem_t min_e, max_e;

    Benchmark() {
        insert_distribution.clear();
        insert_element_list.clear();
        current_test_number.clear();
        total_elem = 0;
        min_e = 0;
        max_e = 1e9;
    }
    virtual ~Benchmark() {}

    void insert_element(vector<elem_t> &target_vector, elem_t e){
        target_vector.push_back(e);
    }

    double calc_average_rank_error(vector<elem_t> &all_element, vector<double> &predict_quantile, vector<double> &truth_w, vector<double> &save_result){
        double err = 0;
        assert(predict_quantile.size() == truth_w.size());
        assert(save_result.size() == truth_w.size());
        for (int i = 0; i < predict_quantile.size(); i++){
            int id1 = int(floor(predict_quantile[i] * (all_element.size() - 1)));
            int id2 = int(floor(truth_w[i] * (all_element.size() - 1)));
            if (all_element[id1] == all_element[id2]) err += 0;
            // else err += abs(predict_quantile[i] - truth_w[i]);
            else {
                int rank_L = getValueLessThan(all_element, all_element[id2]);
                int rank_R = getValueGreaterThan(all_element, all_element[id2]);
                if (id1 > id2){
                    err += (double)abs(rank_R - id1) / all_element.size();
                    save_result[i] += (double)abs(rank_R - id1) / all_element.size();
                } else {
                    err += (double)abs(rank_L - id1) / all_element.size();
                    save_result[i] += (double)abs(rank_L - id1) / all_element.size();
                }
            }
        }
        return err / predict_quantile.size();
    }

    int getValueLessThan(vector<elem_t> &all_element, elem_t e){
        int L = 0, R = all_element.size() - 1;
        while (L < R) {
            int mid = (L + R) >> 1;
            if (e > all_element[mid]) L = mid + 1;
            else R = mid;
        }
        return L;
    }

    int getValueGreaterThan(vector<elem_t> &all_element, elem_t e){
        int L = 0, R = all_element.size() - 1;
        while (L < R) {
            int mid = (L + R + 1) >> 1;
            if (e < all_element[mid]) R = mid - 1;
            else L = mid;
        }
        return L;
    }

    double calc_average_quantile_error(vector<elem_t> &all_element, vector<elem_t> &predict_element, vector<double> &truth_w, vector<double> &save_result){
        double err = 0, bias = 0;
        assert(predict_element.size() == truth_w.size());
        assert(save_result.size() == truth_w.size());
        double total_error[101];
        for (int i = 0; i < predict_element.size(); i++){
            int true_id = int(floor(truth_w[i] * (all_element.size() - 1)));
            int rank_L = getValueLessThan(all_element, predict_element[i]);
            int rank_R = getValueGreaterThan(all_element, predict_element[i]);
            if (predict_element[i] < all_element[true_id]){
                err += (double)abs(rank_R - true_id) / all_element.size();
                bias += ((double)rank_R - true_id) / all_element.size();
                save_result[i] += (double)abs(rank_R - true_id) / all_element.size();
            } else if (predict_element[i] > all_element[true_id]){
                err += (double)abs(rank_L - true_id) / all_element.size();
                bias += ((double)rank_L - true_id) / all_element.size();
                save_result[i] += (double)abs(rank_L - true_id) / all_element.size();
            } else {
                err += 0;
                save_result[i] += 0;
            }
            total_error[int(floor(truth_w[i] * 100))] = err;
        }
        total_error[100] = err;
    #ifdef DEBUG_FLAG
        for (int i = 0; i < 100; i++) cout << total_error[i + 1] - total_error[i] << ",";
        cout << endl;
        cout << bias << " - " << err << endl;
    #endif
        return err / predict_element.size();
    }
    
    void prepare_truth_element(vector<elem_t> &truth_element, const vector<double> &truth_w, vector<elem_t> &correct_detector){
        truth_element.clear();
        for (int i = 0; i < truth_w.size(); i++){
            int j = int(floor(truth_w[i] * (correct_detector.size() - 1)));
            truth_element.push_back(correct_detector[j]);
        }
    }

    void update_distribution(elem_t e){
        total_elem += 1;
        // if (insert_distribution[e] == 0)
        //     insert_distribution[e] = 1;
        // else insert_distribution[e] += 1;
        auto result = insert_distribution.find(e);
        if (result != insert_distribution.end()) {
            result->second += 1;
        } else {
            insert_distribution[e] = 1;
        }
    }

    void dump_distribution(){
        int group_number = 200;
        vector<pair<elem_t, uint32_t> > all_insertion;
        vector<uint32_t> group_frequency;
        vector<elem_t> group_index;
        all_insertion.clear();
        group_frequency.clear();
        group_index.clear();
        for (auto it: insert_distribution){
            all_insertion.push_back(make_pair(it.first, it.second));
        }
        sort(all_insertion.begin(), all_insertion.end(),
            [](const std::pair<elem_t, uint32_t>& a, const std::pair<elem_t, uint32_t>& b) {
                return a.first < b.first;
            });
        elem_t min_e = all_insertion[0].first, max_e = all_insertion[all_insertion.size() - 1].first;
        int current_elem = 0;
        for (int i = 1; i < group_number; i++){
            if (i == 1) group_index.push_back(min_e);
            elem_t next_index = (max_e - min_e) * ((double)i / (group_number - 1)) + min_e;
            group_index.push_back(next_index);
            uint32_t group_counter = 0;
            while (current_elem < all_insertion.size() 
                && all_insertion[current_elem].first <= next_index){
                    group_counter += all_insertion[current_elem].second;
                    ++current_elem;
            }
            group_frequency.push_back(group_counter);
        }
        std::ofstream file("distribution.txt", std::ios::out);
        for (int i = 0; i < group_index.size(); i++)
            file << group_index[i] << " ";
        file << endl;
        for (int i = 0; i < group_frequency.size(); i++)
            file << group_frequency[i] << " ";
        file << endl;
        file.close();
        cout << "Values : " << total_elem << endl;
        cout << "Unique values : " << insert_distribution.size() << endl;
        uint32_t max_freq = 0;
        elem_t max_freq_value = 0;
        for (auto it : insert_distribution){
            if (it.second > max_freq){
                max_freq_value = it.first;
                max_freq = it.second;
            }
        }
        cout << "Most Frequent value : " << max_freq_value << " occur " << max_freq << " times" << endl;
    }

    string doubleToString(double value) {
        std::ostringstream out;
        out << std::fixed << std::setprecision(2) << value;
        return out.str();
    }

    void reserve_length(uint32_t n){
        if (aqe.size() != n){
            aqe.clear();
            are.clear();
            throughput.clear();
            aqe_time.clear();
            are_time.clear();
            aqe_distribution.clear();
            are_distribution.clear();
            current_test_number.clear();
            for (int i = 0; i < n; i++){
                aqe.push_back(0);
                are.push_back(0);
                throughput.push_back(0);
                aqe_time.push_back(0);
                are_time.push_back(0);
                auto aqe_arr = new vector<double>;
                auto are_arr = new vector<double>;
                aqe_arr->clear();
                aqe_arr->clear();
                aqe_distribution.push_back(*aqe_arr);
                are_distribution.push_back(*are_arr);
                for (int j = 1; j < 10000; j++){
                    aqe_distribution[i].push_back(0);
                    are_distribution[i].push_back(0);
                }
                current_test_number.push_back(0);
            }
        }
    }

    void Run(uint32_t sketch_memory = 256 * 1024, bool print_first_line_flag = true){
        current_sketch_memory = sketch_memory;
        uint32_t running_length = min(insert_element_list.size(), (size_t)TOTAL_ELEMENTS);

        vector<quantile_sketch*> sketch_list;
        sketch_list.clear();
        sketch_name_list.clear();
        // for (double hratio = 0.02; hratio <= 0.201; hratio += 0.02){
        //     sketch_list.push_back(new ElasticKLL(sketch_memory, hratio));
        //     sketch_name_list.push_back(doubleToString(hratio));
        // }
        // for (int lambda = 1; lambda <= 32; lambda++){
        //     sketch_list.push_back(new ElasticKLL(sketch_memory, 0.1, lambda));
        //     sketch_name_list.push_back(to_string(lambda));
        // }
        sketch_list.push_back(new ElasticKLL(sketch_memory));
        sketch_name_list.push_back("Cooled-KLL");
        sketch_list.push_back(new KLL(sketch_memory));
        sketch_name_list.push_back("KLL");
        sketch_list.push_back(new ReqSketch(sketch_memory));
        sketch_name_list.push_back("req");
        sketch_list.push_back(new GK(sketch_memory));
        sketch_name_list.push_back("GK");
        sketch_list.push_back(new DD(sketch_memory, min_e, max_e));
        sketch_name_list.push_back("ddsketch");

        /*
        // get epsilon
        double l_eps = 1.0 / (1 << 15), r_eps = 1.0 / (1 << 8);
        while ((r_eps - l_eps) > (1.0 / (1 << 20))){
            double mid_eps = (l_eps + r_eps) / 2;
            auto gks = new GK(mid_eps);
            for (int i = 0; i < running_length; ++i) 
                gks->insert(insert_element_list[i]);
            if (gks->memory() > sketch_memory) l_eps = mid_eps;
            else r_eps = mid_eps;
        }
        cout << std::fixed<<std::setprecision(7) << sketch_memory / 1024 << "KB : " << l_eps << endl;
        return;
        // end get epsilon
        */

        bool first_round_flag = false;
        if (aqe.size() == 0) first_round_flag = true;
        reserve_length(sketch_list.size());
        if (print_first_line_flag){
            FILE *fp1_aqe = fopen("aqe.csv", "w");
            FILE *fp2_are = fopen("are.csv", "w");
            FILE *fp3_throughput = fopen("insert_speed.csv", "w");
            FILE *fp4_aqe_time = fopen("query_aqe_time.csv", "w");
            FILE *fp5_are_time = fopen("query_are_time.csv", "w");
            FILE *fp6_aqe_distribution = fopen("aqe_distribution.csv", "w");
            FILE *fp7_are_distribution = fopen("are_distribution.csv", "w");
            string first_line = "Memory,";
            for (int i = 0; i < sketch_name_list.size(); i++)
                first_line += sketch_name_list[i] + ",";
            first_line += "\n";
            fprintf(fp1_aqe, first_line.c_str()); fflush(fp1_aqe);
            fprintf(fp2_are, first_line.c_str()); fflush(fp2_are);
            fprintf(fp3_throughput, first_line.c_str()); fflush(fp3_throughput);
            fprintf(fp4_aqe_time, first_line.c_str()); fflush(fp4_aqe_time);
            fprintf(fp5_are_time, first_line.c_str()); fflush(fp5_are_time);
            first_line = "Sketch,";
            // aggregate 10000 to 100
            // for (int i = 1; i < 10000; i++)
            //     first_line += to_string(i / 100) + "." + to_string(i % 100) + ",";
            for (int i = 1; i < 100; i++)
                first_line += to_string(i) + ",";
            first_line += "\n";
            fprintf(fp6_aqe_distribution, first_line.c_str()); fflush(fp6_aqe_distribution);
            fprintf(fp7_are_distribution, first_line.c_str()); fflush(fp7_are_distribution);
        }

        for (int sketch_id = 0; sketch_id < sketch_list.size(); sketch_id++){
            if (sketch_name_list[sketch_id] == "GK"){
                if (!first_round_flag) continue;
                // slow and not random, no need to run TEST_NUMBER times
            }
            current_test_number[sketch_id]++;
            std::vector<elem_t> correct_detector; 
            std::map<uint64_t, uint32_t> id_map;
            std::map<uint64_t, uint64_t> last_time;
            std::set<uint64_t> id_set;

            clock_t begin,finish;
            clock_t total=0, aqe_query_total=0, are_query_total=0;
            double totaltime;

            quantile_sketch* current_sketch = sketch_list[sketch_id];
            cout << sketch_name_list[sketch_id] << " result:\n";

            for (int i = 0; i < running_length; ++i) 
            {
                if (sketch_id == 0 && print_first_line_flag) update_distribution(insert_element_list[i]);
                insert_element(correct_detector, insert_element_list[i]);
                
                begin=clock();
                current_sketch->insert(insert_element_list[i]);
                finish=clock();
                total += finish-begin;
            }
            
            totaltime=(double)(total)/CLOCKS_PER_SEC;
            std::cout <<"time taken: "<<totaltime<<" seconds"<< std::endl;
            double tp = double(running_length) / totaltime / 1000000; // Mops
            std::cout <<"throughput: "<<std::fixed<<std::setprecision(4)<< tp <<" item/s"<< std::endl;
            throughput[sketch_id] += tp;

            double query_w = 0.0001;
            // init truth_w and truth_element
            vector<double> truth_w;
            truth_w.clear();
            for (int i = 1; (double)i * query_w < 1; i++)
                truth_w.push_back((double)i * query_w);
            
            vector<elem_t> truth_element;
            sort(correct_detector.begin(), correct_detector.end());
            prepare_truth_element(truth_element, truth_w, correct_detector);

            // calculate average rank error
            int testnum = QUERYTESTNUMBER;
            double average_quantile_error = 0;
            double aaqe = 0;

            for (int t = 0; t < testnum; t++){
                vector<elem_t> predict_element;
                predict_element.clear();
                begin=clock();
                current_sketch->multi_query_find_e_by_w(predict_element, truth_w);
                finish=clock();
                aqe_query_total += finish-begin;
    #ifdef DEBUG_FLAG
                current_sketch->debug_aee(correct_detector);
    #endif
                // end debug
                average_quantile_error = calc_average_quantile_error(correct_detector, predict_element, truth_w, aqe_distribution[sketch_id]);
                aaqe += average_quantile_error;
                double delta_w = query_w / testnum / 5;
                for (int i = 0; i < truth_w.size(); i++)
                    if (truth_w[i] + delta_w < 1) truth_w[i] += delta_w;
            }
            aaqe /= testnum;
            std::cout <<"aqe: "<< setprecision(7) << aaqe << std::endl;
            aqe[sketch_id] += aaqe;
            
            totaltime=(double)(aqe_query_total)/CLOCKS_PER_SEC;
            std::cout <<"aqe query time taken: "<<totaltime<<" seconds"<< std::endl;
            double aqe_query_time = totaltime / testnum;
            std::cout <<"average: "<<std::fixed<<std::setprecision(7)<< aqe_query_time <<" s"<< std::endl;
            aqe_time[sketch_id] += aqe_query_time;


            // calculate average quantile error
            double average_rank_error = 0;
            double aare = 0;

            for (int t = 0; t < testnum; t++){
                vector<double> predict_quantile;
                predict_quantile.clear();
                begin=clock();
                current_sketch->multi_query_find_w_by_e(predict_quantile, truth_element);
                finish=clock();
                are_query_total += finish-begin;
                average_rank_error = calc_average_rank_error(correct_detector, predict_quantile, truth_w, are_distribution[sketch_id]);
                aare += average_rank_error;
                double delta_w = query_w / testnum / 5;
                for (int i = 0; i < truth_w.size(); i++)
                    if (truth_w[i] + delta_w < 1) truth_w[i] += delta_w;
            }
            aare /= testnum;
            std::cout <<"are: "<< setprecision(7) << aare << std::endl;
            are[sketch_id] += aare;
            
            totaltime=(double)(are_query_total)/CLOCKS_PER_SEC;
            std::cout <<"are query time taken: "<<totaltime<<" seconds"<< std::endl;
            double are_query_time = totaltime / testnum;
            std::cout <<"average: "<<std::fixed<<std::setprecision(7)<< are_query_time <<" s"<< std::endl;
            are_time[sketch_id] += are_query_time;


            std::cout << "Estimate: " << correct_detector.size() << "\n";
            // std::cout << "query_w: " << query_w<< "\n";
            std::cout << "memory: " << 1.0 * current_sketch->memory() / 1024<< "KB\n";
            std::cout << "\n";
            if (sketch_id == 0 && print_first_line_flag) dump_distribution();
        }
        for (int i = 0; i < sketch_list.size(); i++)
            delete sketch_list[i];
    }

    void dump_result(){
        FILE *fp1_aqe = fopen("aqe.csv", "a");
        FILE *fp2_are = fopen("are.csv", "a");
        FILE *fp3_throughput = fopen("insert_speed.csv", "a");
        FILE *fp4_aqe_time = fopen("query_aqe_time.csv", "a");
        FILE *fp5_are_time = fopen("query_are_time.csv", "a");
        FILE *fp6_aqe_distribution = fopen("aqe_distribution.csv", "a");
        FILE *fp7_are_distribution = fopen("are_distribution.csv", "a");

        fprintf(fp1_aqe, "%d,", int(current_sketch_memory / 1024));
        fprintf(fp2_are, "%d,", int(current_sketch_memory / 1024));
        fprintf(fp3_throughput, "%d,", int(current_sketch_memory / 1024));
        fprintf(fp4_aqe_time, "%d,", int(current_sketch_memory / 1024));
        fprintf(fp5_are_time, "%d,", int(current_sketch_memory / 1024));
        fprintf(fp6_aqe_distribution, "(%d)", int(current_sketch_memory / 1024));
        fprintf(fp7_are_distribution, "(%d)", int(current_sketch_memory / 1024));

        cout << aqe.size() << " : ";
        for (int i = 0; i < aqe.size(); i++) cout << aqe[i] << ", ";
        cout << endl;

        for (int i = 0; i < aqe.size(); i++)
            fprintf(fp1_aqe, "%.12lf,", aqe[i] / current_test_number[i]);
        for (int i = 0; i < are.size(); i++)
            fprintf(fp2_are, "%.7lf,", are[i] / current_test_number[i]);
        for (int i = 0; i < throughput.size(); i++)
            fprintf(fp3_throughput, "%.4lf,", throughput[i] / current_test_number[i]);
        for (int i = 0; i < aqe_time.size(); i++)
            fprintf(fp4_aqe_time, "%.7lf,", aqe_time[i] / current_test_number[i]);
        for (int i = 0; i < are_time.size(); i++)
            fprintf(fp5_are_time, "%.7lf,", are_time[i] / current_test_number[i]);
        for (int i = 0; i < aqe_distribution.size(); i++){
            fprintf(fp6_aqe_distribution, (sketch_name_list[i] + ",").c_str());
            // aggregate 100 to 1
            double sum = 0;
            for (int j = 0; j < 9999; j++){
                sum += aqe_distribution[i][j] / current_test_number[i] / QUERYTESTNUMBER;
                if (j % 100 == 99){
                    sum /= 100;
                    fprintf(fp6_aqe_distribution, "%.12lf,", sum);
                    sum = 0;
                }
            }
            fprintf(fp6_aqe_distribution, "\n");
        }
        for (int i = 0; i < are_distribution.size(); i++){
            fprintf(fp7_are_distribution, (sketch_name_list[i] + ",").c_str());
            // aggregate 100 to 1
            double sum = 0;
            for (int j = 0; j < 9999; j++){
                sum += are_distribution[i][j] / current_test_number[i] / QUERYTESTNUMBER;
                if (j % 100 == 99){
                    sum /= 100;
                    fprintf(fp7_are_distribution, "%.7lf,", sum);
                    sum = 0;
                }
            }
            fprintf(fp7_are_distribution, "\n");
        }
        
        fprintf(fp1_aqe, "\n"); fflush(fp1_aqe);
        fprintf(fp2_are, "\n"); fflush(fp2_are);
        fprintf(fp3_throughput, "\n"); fflush(fp3_throughput);
        fprintf(fp4_aqe_time, "\n"); fflush(fp4_aqe_time);
        fprintf(fp5_are_time, "\n"); fflush(fp5_are_time);
        fprintf(fp6_aqe_distribution, "\n"); fflush(fp6_aqe_distribution);
        fprintf(fp7_are_distribution, "\n"); fflush(fp7_are_distribution);
        aqe.clear();
        are.clear();
        throughput.clear();
        aqe_time.clear();
        are_time.clear();
        aqe_distribution.clear();
        are_distribution.clear();
        current_test_number.clear();
    }
};

struct CAIDA_Tuple {
    uint64_t timestamp;
    uint64_t id;
};

class CAIDABenchmark : public Benchmark
{
public:
    LoadResult load_result;
    CAIDA_Tuple* dataset;
    uint64_t length;

    CAIDABenchmark(std::string PATH) 
    {
        std::cout<<"dataset = "<<PATH<<std::endl;
        load_result = Load(PATH.c_str());
        dataset = (CAIDA_Tuple*)load_result.start;
        length = load_result.length / sizeof(CAIDA_Tuple);
        
        std::map<uint64_t, uint32_t> id_map;
        std::map<uint64_t, uint64_t> last_time;
        std::set<uint64_t> id_set;

        bool first_e_flag = true;
        for (int i = 0; i < length; ++i) 
        {
            if (id_set.find(dataset[i].id) == id_set.end()) 
            {
                id_set.insert(dataset[i].id);
                id_map[dataset[i].id] = 0;
            }
            id_map[dataset[i].id]++;

            if (id_map[dataset[i].id] > 1 && dataset[i].timestamp > last_time[dataset[i].id]) 
            {
                if (dataset[i].timestamp - last_time[dataset[i].id] < 1e9){
                    elem_t new_e = dataset[i].timestamp - last_time[dataset[i].id];
                    insert_element_list.push_back(new_e);
                    if (first_e_flag){
                        min_e = max_e = new_e;
                        first_e_flag = false;
                    }
                    else {
                        min_e = min(min_e, new_e);
                        max_e = max(max_e, new_e);
                    }
                }
            }
            last_time[dataset[i].id] = dataset[i].timestamp;
        }
    }
    ~CAIDABenchmark() override {}
};


struct kaggle_Tuple_Voltage {
    float e;

    friend std::istream& operator>>(std::istream& is, kaggle_Tuple_Voltage& tuple) {
        std::string line;
        if (std::getline(is, line)) {
            std::vector<std::string> tokens;
            std::stringstream ss(line);
            std::string token;
            
            while (std::getline(ss, token, ',')) {
                tokens.push_back(token);
            }

            if (tokens.size() == 11) {
                try {
                    tuple.e = std::stof(tokens[5]);
                } catch (const std::exception& ex) {
                    std::cerr << "Error parsing float: " << ex.what() << std::endl;
                    is.setstate(std::ios::failbit);
                }
            } else {
                std::cerr << "Error: Expected 11 comma-separated values, but got " << tokens.size() << std::endl;
                is.setstate(std::ios::failbit);
            }
        }
        return is;
    }
};

class kaggleDatasetVoltage : public Benchmark
{
public:

    kaggleDatasetVoltage(std::string PATH) 
    {
        std::cout<<"dataset = "<<PATH<<std::endl;

        std::ifstream file(PATH, std::ios::in);
        std::string first_line;
        std::getline(file, first_line);
        //discard first line
        kaggle_Tuple_Voltage kt;
        bool first_e_flag = true;
        while (file >> kt){
            insert_element_list.push_back(elem_t(kt.e));
            if (first_e_flag){
                min_e = max_e = elem_t(kt.e);
                first_e_flag = false;
            }
            else {
                min_e = min(min_e, elem_t(kt.e));
                max_e = max(max_e, elem_t(kt.e));
            }
        }
    }
    ~kaggleDatasetVoltage() {}
};


struct kaggle_Tuple_Price {
    float e;

    friend std::istream& operator>>(std::istream& is, kaggle_Tuple_Price& tuple) {
        std::string line;
        if (std::getline(is, line)) {
            std::vector<std::string> tokens;
            std::stringstream ss(line);
            std::string token;
            
            while (std::getline(ss, token, ',')) {
                tokens.push_back(token);
            }

            // if (tokens.size() == 9) {
            if (tokens.size() > 7) {
                try {
                    tuple.e = std::stof(tokens[6]);
                } catch (const std::exception& ex) {
                    std::cerr << "Error parsing float: " << ex.what() << std::endl;
                    is.setstate(std::ios::failbit);
                }
            } else {
                std::cerr << "Error: Expected 9 comma-separated values, but got " << tokens.size() << std::endl;
                is.setstate(std::ios::failbit);
            }
        }
        return is;
    }
};

class kaggleDatasetPrice : public Benchmark
{
public:

    kaggleDatasetPrice(std::string PATH) 
    {
        std::cout<<"dataset = "<<PATH<<std::endl;

        std::ifstream file(PATH, std::ios::in);
        std::string first_line;
        std::getline(file, first_line);
        //discard first line
        kaggle_Tuple_Price kt;
        bool first_e_flag = true;
        while (file >> kt){
            insert_element_list.push_back(elem_t(kt.e));
            if (first_e_flag){
                min_e = max_e = elem_t(kt.e);
                first_e_flag = false;
            }
            else {
                min_e = min(min_e, elem_t(kt.e));
                max_e = max(max_e, elem_t(kt.e));
            }
        }
    }
    ~kaggleDatasetPrice() {}
};


struct kaggle_Tuple_Custom {
    uint32_t e;

    friend std::istream& operator>>(std::istream& is, kaggle_Tuple_Custom& tuple) {
        std::string line;
        if (std::getline(is, line)) {
            std::vector<std::string> tokens;
            std::stringstream ss(line);
            std::string token;
            
            while (std::getline(ss, token, ',')) {
                tokens.push_back(token);
            }

            if (tokens.size() == 8) {
                try {
                    tuple.e = std::stof(tokens[7]);
                } catch (const std::exception& ex) {
                    std::cerr << "Error parsing int: " << ex.what() << std::endl;
                    is.setstate(std::ios::failbit);
                }
            } else {
                std::cerr << "Error: Expected 8 comma-separated values, but got " << tokens.size() << std::endl;
                is.setstate(std::ios::failbit);
            }
        }
        return is;
    }
};

class kaggleDatasetCustom : public Benchmark
{
public:

    kaggleDatasetCustom(std::string PATH) 
    {
        std::cout<<"dataset = "<<PATH<<std::endl;

        std::ifstream file(PATH, std::ios::in);
        std::string first_line;
        std::getline(file, first_line);
        //discard first line
        kaggle_Tuple_Custom kt;
        bool first_e_flag = true;
        while (file >> kt){
            insert_element_list.push_back(elem_t(kt.e));
            if (first_e_flag){
                min_e = max_e = elem_t(kt.e);
                first_e_flag = false;
            }
            else {
                min_e = min(min_e, elem_t(kt.e));
                max_e = max(max_e, elem_t(kt.e));
            }
        }
    }
    ~kaggleDatasetCustom() {}
};


struct kaggle_Tuple_Delay {
    float e;

    friend std::istream& operator>>(std::istream& is, kaggle_Tuple_Delay& tuple) {
        std::string line;
        while (std::getline(is, line)) {
            std::vector<std::string> tokens;
            std::stringstream ss(line);
            std::string token;
            
            while (std::getline(ss, token, ',')) {
                tokens.push_back(token);
            }

            if (tokens.size() == 20) {
                try {
                    tuple.e = std::stof(tokens[6]);
                    break;
                } catch (const std::exception& ex) {
                    // std::cerr << "Error parsing float: " << ex.what() << std::endl;
                    continue;
                    is.setstate(std::ios::failbit);
                }
            } else {
                std::cerr << "Error: Expected 20 comma-separated values, but got " << tokens.size() << std::endl;
                is.setstate(std::ios::failbit);
            }
        }
        return is;
    }
};

class kaggleDatasetDelay : public Benchmark
{
public:

    kaggleDatasetDelay(std::string PATH) 
    {
        std::cout<<"dataset = "<<PATH<<std::endl;

        std::ifstream file(PATH, std::ios::in);
        std::string first_line;
        std::getline(file, first_line);
        //discard first line
        kaggle_Tuple_Delay kt;
        bool first_e_flag = true;
        while (file >> kt){
            insert_element_list.push_back(elem_t(kt.e));
            if (first_e_flag){
                min_e = max_e = elem_t(kt.e);
                first_e_flag = false;
            }
            else {
                min_e = min(min_e, elem_t(kt.e));
                max_e = max(max_e, elem_t(kt.e));
            }
        }
    }
    ~kaggleDatasetDelay() {}
};
struct kaggle_Tuple_Ecom {
    float e;

    friend std::istream& operator>>(std::istream& is, kaggle_Tuple_Ecom& tuple) {
        std::string line;
        while (std::getline(is, line)) {
            std::vector<std::string> tokens;
            std::stringstream ss(line);
            std::string token;
            
            while (std::getline(ss, token, ',')) {
                tokens.push_back(token);
            }

            // if (tokens.size() == 8) {
            if (tokens.size() > 6) {
                try {
                    tuple.e = std::stof(tokens[6]);
                    break;
                } catch (const std::exception& ex) {
                    std::cerr << "Error parsing float: " << ex.what() << std::endl;
                    continue;
                    is.setstate(std::ios::failbit);
                }
            } else {
                // std::cerr << "Error: Expected 8 comma-separated values, but got " << tokens.size() << std::endl;
                continue;
                is.setstate(std::ios::failbit);
            }
        }
        return is;
    }
};

class kaggleDatasetEcom : public Benchmark
{
public:

    kaggleDatasetEcom(std::string PATH) 
    {
        std::cout<<"dataset = "<<PATH<<std::endl;

        std::ifstream file(PATH, std::ios::in);
        std::string first_line;
        std::getline(file, first_line);
        //discard first line
        kaggle_Tuple_Ecom kt;
        bool first_e_flag = true;
        while (file >> kt){
            insert_element_list.push_back(elem_t(kt.e));
            if (first_e_flag){
                min_e = max_e = elem_t(kt.e);
                first_e_flag = false;
            }
            else {
                min_e = min(min_e, elem_t(kt.e));
                max_e = max(max_e, elem_t(kt.e));
            }
        }
    }
    ~kaggleDatasetEcom() {}
};

int main()
{    
#ifdef USE_CAIDA
    std::string path_caida = "./data/caida.dat";  //"/share/datasets/CAIDA2016/CAIDA.dat";
    CAIDABenchmark benchmark(path_caida);
    // benchmark.Run();
#endif
#ifdef USE_KAGGLE_VOLTAGE
    std::string path_kaggle = "./data/Time_series_2000rpm.csv";
    kaggleDatasetVoltage benchmark(path_kaggle);
    // benchmark.Run();
#endif
#ifdef USE_KAGGLE_PRICE
    std::string path_kaggle = "./data/2019-Oct.csv";
    kaggleDatasetPrice benchmark(path_kaggle);
#endif
#ifdef USE_KAGGLE_CUSTOM
    std::string path_kaggle = "./data/custom_1988_2020.csv";
    kaggleDatasetCustom benchmark(path_kaggle);
    // benchmark.Run();
#endif
#ifdef USE_KAGGLE_DELAY
    std::string path_kaggle = "./data/2019.csv";
    kaggleDatasetDelay benchmark(path_kaggle);
    // benchmark.Run();
#endif
#ifdef USE_KAGGLE_ECOM
    std::string path_kaggle = "./data/kz.csv";
    kaggleDatasetEcom benchmark(path_kaggle);
    // benchmark.Run();
#endif
    uint32_t start_mem = 16 * 1024;
    uint32_t end_mem = 512 * 1024;
    bool print_first_line = true;
    for (uint32_t mem = start_mem; mem <= end_mem; mem *= 2){
        for (int i = 0; i < TEST_NUMBER; i++){
            benchmark.Run(mem, print_first_line);
            print_first_line = false;
        }
        benchmark.dump_result();
    }
}