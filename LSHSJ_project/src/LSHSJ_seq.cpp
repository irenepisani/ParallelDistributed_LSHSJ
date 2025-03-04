/**
* @author   Irene Pisani 
* @note     University of Pisa, Computer Science department. 
*           M.Sc. Computer Science, Artificial Intelligence 
*           Parallel and Distributed Systems: Paradigms and models (23/24).
* 
* @brief    Project track 3: Locality Sensitive Hashing based Similarity Join (LSHSJ)
* @details  Sequantial implementation of LSHSJ.  
*/

#include <iostream>
#include <ostream>
#include <sstream>
#include <map>
#include <vector>
#include <random>

#include <ff/utils.hpp>

#include "hash.hpp"
#include "geometry_basics.hpp"
#include "frechet_distance.hpp"

using namespace std; 
using namespace ff; 

#define LSH_FAMILY_SIZE 8      // Number of LSH functions
#define LSH_SEED 234           // Seed for LSH function
#define LSH_RESOLUTION 80      // Resolution for LSH function

// Similarity threasholds
const static double SIM_THRESHOLD = 10;
const static double SIM_THRESHOLD_SQR = sqr(SIM_THRESHOLD);

size_t foundSimilar = 0;          // Counter for similar trajectories
ostream* resultsStream = &cout;   // Output streams

/** 
 * @brief Structure to represent an item in the dataset.
 */
struct item {
    size_t id;         // Unique identifier 
    int dataset;       // Dataset identifier
    curve content;     // Curve representing the trajectory
};

/**
 * @brief Structure to represent an element after LSH values computations.
 */
struct element_t {
    
    long LSH;
    int dataSet;
    curve trajectory;
    array<long, LSH_FAMILY_SIZE> relativeLSHs;
    long id;

    /** 
     * @brief Constructor to initialize the element.
     * @param hash LSH hash value
     * @param dataset Dataset identifier
     * @param trajectory Trajectory of the element
     * @param relativeLSHs LSH values for the element
     * @param id Unique identifier for the element
     */
    
    element_t(long hash, int dataset, curve trajectory, decltype(relativeLSHs) relativeLSHs, long id)
        : LSH(hash), dataSet(dataset), trajectory(trajectory), relativeLSHs(relativeLSHs), id(id) {}
};

/**
 * @brief Parses a line of data into an item object.
 * 
 * @param line The input line as a string
 * @return item The parsed item containing id, dataset, and trajectory
 */
item parseLine(string& line) {
    istringstream ss(line);
    item output;
    ss >> output.id;
    ss >> output.dataset;
    string tmp;
    ss >> tmp;

    // Parse trajectory
    if (!(tmp.find_first_of("[") == string::npos)) {
        tmp.replace(0, 1, "");
        tmp.replace(tmp.length() - 1, tmp.length(), "");
        bool ext = false;
        while (!ext) {
            string extrait = tmp.substr(tmp.find("["), tmp.find("]") + 1);
            string extrait1 = extrait.substr(1, extrait.find(",") - 1);
            string extrait2 = extrait.substr(extrait.find(",") + 1);
            extrait2 = extrait2.substr(0, extrait2.length() - 1);
            double e1 = stod(extrait1);
            double e2 = stod(extrait2);
            output.content.push_back(move(point(e1, e2)));
            ext = (tmp.length() == extrait.length());
            if (!ext)
                tmp = tmp.substr(tmp.find_first_of("]") + 2, tmp.length());
        }
    }
    return output;
}

/**
 * @brief Checks if two curves (trajectories) are similar based on various distance metrics.
 * 
 * @param c1 The first trajectory
 * @param c2 The second trajectory
 * @return bool True if the curves are similar, false otherwise
 */
bool similarity_test(const curve& c1, const curve& c2) {

    // Check euclidean distance
    if (euclideanSqr(c1[0], c2[0]) > SIM_THRESHOLD_SQR || euclideanSqr(c1.back(), c2.back()) > SIM_THRESHOLD_SQR) 
        return false;

    // Check equal time
    if (equalTime(c1, c2, SIM_THRESHOLD_SQR) || get_frechet_distance_upper_bound(c1, c2) <= SIM_THRESHOLD)
        return true;

    // Check using negative filter
    if (negfilter(c1, c2, SIM_THRESHOLD))
        return false;

    // Full check using Frechet distance
    if (is_frechet_distance_at_most(c1, c2, SIM_THRESHOLD))
        return true;
    
    return false;
}

/**
* @brief Helper function to check if two elements are similar based on LSH and trajectory comparison.
* @param lsh The hash value to compare
* @param a The first element to compare
* @param b The second element to compare
*/
void checkHelper(const long lsh, const element_t& a, const element_t& b) {

    for (size_t ii = 0; ii < a.relativeLSHs.size(); ii++) {
        if (a.relativeLSHs[ii] == b.relativeLSHs[ii]) {
            if (lsh == a.relativeLSHs[ii]) {
                if (similarity_test(a.trajectory, b.trajectory)) {
                    ++foundSimilar;
                    *resultsStream << a.id << "\t" << b.id << endl;
                }
            }
            return;
        }
    }
}

/**
* @brief Main function that initializes the LSH family, reads input data, and performs similarity checks.
* @param argc Argument count
* @param argv Argument values (input dataset and output file)
* @return int Exit status
*/
int main(int argc, char** argv) {
    
    // Check for proper command-line arguments
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " inputDataset <outputFile>";
        return EXIT_FAILURE;
    }

    ffTime(START_TIME);

    ofstream filestream;
    // If output file is provided, open it for writing results
    if (argc > 2) {
        filestream = ofstream(argv[2]);
        if (filestream.is_open())
            resultsStream = &filestream;
    }

    // Build LSH function family
    FrechetLSH lsh_family[LSH_FAMILY_SIZE];
    for (size_t i = 0; i < LSH_FAMILY_SIZE; i++)
        lsh_family[i].init(LSH_RESOLUTION, LSH_SEED * i, i);
    
    // Open input file
    string line;
    ifstream file(argv[1]); 
    if (!file.is_open()) {
        cerr << "Error opening dataset file!" << endl;
        return -1;
    }

    unordered_map<long, vector<element_t>> elements;

    // Process each line in the input file
    while (getline(file, line)) {
        item it = parseLine(line);

        // Compute LSH values for each LSH function
        array<long, LSH_FAMILY_SIZE> relative_lshs;
        for (size_t i = 0; i < LSH_FAMILY_SIZE; i++)
            relative_lshs[i] = lsh_family[i].hash(it.content);

        // Store the elements with their corresponding LSH hash values
        for (long h : relative_lshs)
            elements[h].emplace_back(h, it.dataset, it.content, relative_lshs, it.id);
    }

    // Close input file after processing
    file.close();
    // cout << elements.size() << endl; 

    // Compare all possible pairs of elements
    for (auto& [lsh, elements_v] : elements)
        for (size_t i = 0; i < elements_v.size(); i++)
            for (size_t j = i + 1; j < elements_v.size(); j++)
                
                // Ensure they are from different datasets 
                if (elements_v[i].dataSet != elements_v[j].dataSet) 

                    // Perform similarity chenk 
                    checkHelper(lsh, elements_v[i], elements_v[j]);
    
    // Stop timer and get execution time
    ffTime(STOP_TIME);

    // Collect outputs (similar pairs) and results (execution time and)
    cout << 
        argv[1] << "\t" << 
        foundSimilar << "\t" << 
        ffTime(GET_TIME) / 1000 << 
    endl;
    
    return 0;
}
