/**
* @author   Irene Pisani 
* @note     University of Pisa, Computer Science department. 
*           M.Sc. Computer Science, Artificial Intelligence 
*           Parallel and Distributed Systems: Paradigms and models (23/24).
*
* @brief    Project track 3: Locality Sensitive Hashing based Similarity Join (LSHSJ)
* @details  Parallel implementation of LSHSJ for Distributed Memory Systems with MPI and OpenMP.   
*
*/

// Streams
#include <iostream>
#include <fstream>
#include <sstream>
#include <ostream>

// Memory mapping utilities
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

// Others utilities 
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <limits>
#include <random>
#include <utility>
#include <algorithm>
#include <unordered_map>

// Message Passing Interface (MPI) lib
#include <mpi.h>

// Custom headers for geometric operations
#include "hash.hpp"
#include "geometry_basics.hpp"
#include "frechet_distance.hpp"

using namespace std;

#define LSH_FAMILY_SIZE 8     // Number of LSH functions
#define LSH_SEED 234          // Seed for LSH function
#define LSH_RESOLUTION 80     // Resolution for LSH function

// Similarity threasholds
const static double SIM_THRESHOLD = 10;
const static double SIM_THRESHOLD_SQR = sqr(SIM_THRESHOLD);

size_t foundSimilar = 0; 
size_t foundSimilarTot; 
vector<long> simPairs; 
vector<long> simPairsTot; 

struct item {
    size_t id;      // Unique identifier 
    int dataset;    // Dataset identifier
    curve content;  // Curve representing the trajectory
};

struct element_t {
    
    long LSH;           // LSH values           
    int dataSet;        // Dataset identifier
    curve trajectory;   // Curve representing the trajectory
    array<long, LSH_FAMILY_SIZE> relativeLSHs; // LSH values for the LSH function 
    long id;            // Unique identifier 
    
    // Constructor
    element_t() = default;
    element_t(long hash, int dataset, curve trajectory, decltype(relativeLSHs) relativeLSHs, long id)
        : LSH(hash), dataSet(dataset), trajectory(trajectory), relativeLSHs(relativeLSHs), id(id) {}
    
};

item parseLine(string& line) {

    // Parse a line of the input dataset as an item 
    istringstream ss(line);
    item output;
    
    ss >> output.id;        // Extract id 
    ss >> output.dataset;   // Extract dataset
    
    // extract trajectory 
    string tmp;
    ss >> tmp;
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

bool similarity_test(const curve& c1, const curve& c2) {

    // Similarity test heuristsic 

    if (euclideanSqr(c1[0], c2[0]) > SIM_THRESHOLD_SQR || euclideanSqr(c1.back(), c2.back()) > SIM_THRESHOLD_SQR) 
        return false;
    if (equalTime(c1, c2, SIM_THRESHOLD_SQR) || get_frechet_distance_upper_bound(c1, c2) <= SIM_THRESHOLD)
        return true;
    if (negfilter(c1, c2, SIM_THRESHOLD))
        return false;
    if (is_frechet_distance_at_most(c1, c2, SIM_THRESHOLD))
        return true;
    return false;
}

inline void checkHelper(const long lsh, const element_t& a, const element_t& b) {

    for (size_t ii = 0; ii < a.relativeLSHs.size(); ii++) {
        if (a.relativeLSHs[ii] == b.relativeLSHs[ii]) {
            if (lsh == a.relativeLSHs[ii]) {
                if (similarity_test(a.trajectory, b.trajectory)) {
                    #pragma omp critical 
                    {
                        simPairs.push_back(a.id);
                        simPairs.push_back(b.id); 
                        ++foundSimilar;
                    }
                    
                }
            }
            return;
        }
    }
}


void init_infile_batch (
    MPI_Comm comm, int size, int rank, 
    const char* inFileMapped, const size_t& inFileBytes, 
    vector<int>& charsPerLines, vector<int>& lines_counts, vector<int>& chars_counts, int& reps ){

    // Use only root-process 
    if (!rank) {
        
        // Get num of chars per lines in input file
        int count_chars = 0;
        for (size_t i = 0; i < inFileBytes; ++i) {
            ++count_chars;
            if (inFileMapped[i] == '\n') {
                charsPerLines.push_back(count_chars);
                count_chars = 0;
            }
        }
        if (count_chars > 0) {
            charsPerLines.push_back(count_chars);
        }

        // Get chars and lines displacemnts/count for each input filepartions 
        // Input file must be partitioned in portion of at most 2GB each 
        int c_lines = 0, c_bytes = 0; 
        size_t check; 
        for (size_t i = 0; i < charsPerLines.size(); ++i) {

            check = c_bytes + charsPerLines[i]; 
            if ( check > numeric_limits<int>::max()) {
                chars_counts.push_back(c_bytes); 
                lines_counts.push_back(c_lines); 
                c_lines = 0;
                c_bytes = 0; 
            }
            ++c_lines;  
            c_bytes += charsPerLines[i];
        }
        if (c_bytes > 0) {
            chars_counts.push_back(c_bytes); 
            lines_counts.push_back(c_lines); 
        }

        // Final number of input file partition required
        reps = chars_counts.size(); 
    }

    // Broadcast to other process the computed informations
    MPI_Barrier(comm); 
    MPI_Bcast(&reps, 1, MPI_INT, 0, comm); 
    lines_counts.resize(reps); 
    chars_counts.resize(reps); 
    MPI_Bcast(lines_counts.data(), reps, MPI_INT, 0, comm); 
    MPI_Bcast(chars_counts.data(), reps, MPI_INT, 0, comm);
    
    return; 
}

stringstream distributeChunks(MPI_Comm comm, int size, int rank, size_t start_line, size_t start_byte, vector<int>& charsPerLines, int& numLines,  const char* inFileMapped ) {

    stringstream chunk; 

    // Count and displacements for scatter ops
    vector<int> counts_send(size, 0); 
    vector<int> displs_send(size, 0);
    int counts_recv; // Size of receive buffer 

    // Compute how many lines each process will receive
    int linesPerProcess = numLines / size;
    int linesRemainer = numLines % size;

    if (!rank){

        // Distribute lines across processes without breaking them
        int linesAssigned = 0;
        for (int i = 0; i < size; ++i) {
            int linesToAssign = linesPerProcess + (i < linesRemainer ? 1 : 0);
            
            for (int j = start_line; j < start_line + linesToAssign; ++j) {
                counts_send[i] += charsPerLines[j + linesAssigned]; 
            }
            displs_send[i] = (i == 0) ? 0 : displs_send[i - 1] + counts_send[i - 1];
            linesAssigned += linesToAssign;
        }
    }

    // Scatter to each process the num of chars they will receive
    MPI_Scatter(
        counts_send.data(), 1, MPI_INT,
        &counts_recv, 1, MPI_INT, 
        0, comm
    );
    
    // Get InFileMapped displacement (for the current input file partition) 
    const char* inFileMapped_displ = &inFileMapped[start_byte];
        
    // Create receive buffer
    vector<char> chunkBuffer (counts_recv); 
    
    // Scatter chunk of input file (chars) to other process 
    MPI_Scatterv(
        inFileMapped_displ, counts_send.data(), displs_send.data(), MPI_CHAR,
        chunkBuffer.data(), counts_recv, MPI_CHAR, 
        0, comm
    );

    // write to stringstream 
    chunk.write(chunkBuffer.data(), counts_recv); 

    return chunk; 
}

vector<vector<element_t>> mapPhase (int size, int rank, stringstream& chunk, int& numLines){

    // Build LSH function family
    FrechetLSH lsh_family[LSH_FAMILY_SIZE];
    for (size_t i = 0; i < LSH_FAMILY_SIZE; i++){
        lsh_family[i].init(LSH_RESOLUTION, LSH_SEED * i, i);
    }
    
    // Final vector of elements aggregated by destination rank
    vector<vector<element_t>> elements(size);

    vector<string> lines;
    string line;
    while (getline(chunk, line)) {
        lines.push_back(line);
    }

    // Parallel processing of lines
    #pragma omp parallel for
    for (size_t i = 0; i < lines.size(); i++) {
        // Parse a line
        item it = parseLine(lines[i]);

        // Compute LSH values for each LSH function
        array<long, LSH_FAMILY_SIZE> relative_lshs;
        for (size_t j = 0; j < LSH_FAMILY_SIZE; j++) {
            relative_lshs[j] = lsh_family[j].hash(it.content);
        }

        // Use a critical section to avoid race conditions when updating the shared elements vector
        for (long h : relative_lshs) {
            int out_rank = h % size;
            #pragma omp critical
            {
                elements[out_rank].emplace_back(h, it.dataset, it.content, relative_lshs, it.id);
            }
        }
    }

    // Free memory of chunk
    chunk.str(""); 
    chunk.clear(); 

    return elements; 
}

void shufflePhase(MPI_Comm comm, int size, int rank, vector<vector<element_t>>& elements, unordered_map<long, vector<element_t>>& umap) {
    
    int byte_limit = numeric_limits<int>::max();
    int byte_limit_per_rank = byte_limit / size;
    int num_elem_per_rank = byte_limit_per_rank / static_cast<int>(sizeof(element_t));

    while (true) {
        int local_rep = 0, global_rep;

        // Check if another iteration is required
        for (size_t i = 0; i < size; ++i) {
            if (!elements[i].empty()) {
                local_rep = 1;
                break;
            }
        }
        MPI_Allreduce(&local_rep, &global_rep, 1, MPI_INT, MPI_LOR, comm);

        // Stop if no more data to process
        if (!global_rep) break;

        vector<char> sendBuffer;
        sendBuffer.reserve(byte_limit);

        vector<int> sendCounts(size, 0), sendDispls(size, 0);
        vector<int> recvCounts(size, 0), recvDispls(size, 0);

        // Pack data into sendBuffer
        for (size_t i = 0; i < size; ++i) {
            int prev_size = sendBuffer.size();
            size_t num_elements_to_send = min(elements[i].size(), static_cast<size_t>(num_elem_per_rank));

            for (size_t j = 0; j < num_elements_to_send; ++j) {
                element_t& elem = elements[i][j];
                sendBuffer.resize(sendBuffer.size() + sizeof(element_t));
                memcpy(sendBuffer.data() + sendBuffer.size() - sizeof(element_t), &elem, sizeof(element_t));
            }
            
            sendCounts[i] = sendBuffer.size() - prev_size;
            sendDispls[i] = prev_size;

            // Clear and free memory for the sent elements
            elements[i].erase(elements[i].begin(), elements[i].begin() + num_elements_to_send);
            elements[i].shrink_to_fit();
        }
 
        // Exchange counts
        MPI_Alltoall(sendCounts.data(), 1, MPI_INT, recvCounts.data(), 1, MPI_INT, comm);

        // Calculate recv displacements and total receive size
        int recv_size = 0;
        for (size_t i = 0; i < size; ++i) {
            recvDispls[i] = (i == 0) ? 0 : recvDispls[i - 1] + recvCounts[i - 1];
            recv_size += recvCounts[i];
        }

        // Exchange data
        vector<char> recvBuffer(recv_size);
        MPI_Alltoallv(
            sendBuffer.data(), sendCounts.data(), sendDispls.data(), MPI_CHAR,
            recvBuffer.data(), recvCounts.data(), recvDispls.data(), MPI_CHAR,
            comm
        );

        // Unpack received data into umap
        int num_elements = recv_size / static_cast<int>(sizeof(element_t));
        for (int j = 0; j < num_elements; ++j) {
            element_t elem;
            memcpy(&elem, recvBuffer.data() + j * sizeof(element_t), sizeof(element_t));
            umap[elem.LSH].push_back(elem);
        }
    }

    // Free memory
    for (auto& innerVec : elements) {
        innerVec.clear();
        innerVec.shrink_to_fit();
    }
    elements.clear();
    elements.shrink_to_fit();
}

unordered_map<long, vector<element_t>> process_in_batch(MPI_Comm comm, int size, int rank, const char* inFileMapped, const size_t& inFileBytes, double& time_distr){

    // Final map of elements for each process 
    unordered_map<long, vector<element_t>> umap; 

    MPI_Barrier(comm); 
    double start_time_batch = MPI_Wtime(); 

    vector<int> charsPerLines;  // Num of chars for each line in input file
    vector<int> lines_counts;   // Lines counts for each input file partition 
    vector<int> chars_counts;   // Chars displacements for each input file partition 
    
    // Num or reps and batch index
    int reps;
    size_t start_byte = 0;
    int start_line = 0;   
    
    init_infile_batch(
        comm, size, rank, 
        inFileMapped, inFileBytes, 
        charsPerLines, lines_counts, chars_counts, reps
    ); 

    MPI_Barrier(comm); 
    double end_time_batch = MPI_Wtime(); 
    double time_batch = end_time_batch - start_time_batch; 
    time_distr += time_batch; 

    for (size_t r=0; r < reps; ++r){

        // Distribute chunks of a file partition 
        MPI_Barrier(comm); 
        double start_time_distr_r = MPI_Wtime(); 
        int numLines = lines_counts[r]; 
        stringstream chunk = distributeChunks(comm, size, rank, start_line, start_byte, charsPerLines, numLines, inFileMapped);
        MPI_Barrier(comm); 
        double end_time_distr_r = MPI_Wtime(); 
        double time_distr_r = end_time_distr_r - start_time_distr_r; 

        // Map phase: compute LSH values 
        vector<vector<element_t>> elements = mapPhase(size, rank, chunk, numLines); 

        // Shuffle phase: 
        shufflePhase(comm, size, rank, elements, umap); 

        // Update index of input file to process next batch
        start_byte += static_cast<size_t>(chars_counts[r]);
        start_line += numLines; 
        time_distr += time_distr_r; 
    }
    
    return umap; 

}

void reducePhase( MPI_Comm comm, int size, int rank, unordered_map<long, vector<element_t>>& elementsReceived){

    // Store in a vector all keys 
    vector<long> keys;
    keys.reserve(elementsReceived.size());
    for (auto& pair : elementsReceived) {
        keys.push_back(pair.first);
    }

    // Compute similarity join
    #pragma omp parallel for reduction(+:foundSimilar)
    for (size_t k = 0; k < keys.size(); ++k) {
        long lsh = keys[k];
        auto& elements_v = elementsReceived[lsh];
        for (size_t i = 0; i < elements_v.size(); i++) {
            for (size_t j = i + 1; j < elements_v.size(); j++) {
                if (elements_v[i].dataSet != elements_v[j].dataSet) {
                    checkHelper(lsh, elements_v[i], elements_v[j]);
                }
            }
        }
    }

    /*
    for (auto& [lsh, elements_v] : elementsReceived) {

        // #pragma omp parallel for reduction(+:foundSimilar) collapse(2)
        for (size_t i = 0; i < elements_v.size(); i++) {
            for (size_t j = i + 1; j < elements_v.size(); j++) {
                if (elements_v[i].dataSet != elements_v[j].dataSet) {
                    checkHelper(lsh, elements_v[i], elements_v[j]);
                }
            }
        }
    }
    */
    // Aggregate total number of founded pairs in root process 
    MPI_Reduce(&foundSimilar, &foundSimilarTot, 1, MPI_UNSIGNED, MPI_SUM, 0, comm);    
    
    return; 
}

void outputPairs(MPI_Comm comm, int size, int rank, ostream* resultsStream){
    
    // Prepare counts and displacements 
    vector<int> recv_counts(size);
    vector<int> displs(size, 0);
    int count = static_cast<int>(simPairs.size()); 

    // Aggregate counts in root process 
    MPI_Gather(
        &count, 1, MPI_INT, 
        recv_counts.data(), 1, MPI_INT, 
        0, comm
    ); 
   
    // Compute displacements 
    size_t tot_recvSize = 0; 
    for (int i = 0; i < size; ++i) {
        displs[i] = (i==0) ? 0 : displs[i - 1] + recv_counts[i - 1];
        tot_recvSize += recv_counts[i]; 
    }
    
    // Perform MPI_Gatherv to gather data on the root process (rank 0)
    simPairsTot.resize(tot_recvSize); 
    MPI_Gatherv(
        simPairs.data(), count, MPI_LONG,
        simPairsTot.data(), recv_counts.data(), displs.data(), MPI_LONG,
        0, comm
    );
 
    // Output pairs in outstream with root process
    if (!rank) {
        for (size_t i = 0; i < simPairsTot.size(); i += 2) {
            *resultsStream << simPairsTot[i] << "\t" << simPairsTot[i+1] << endl;
        }
    }
    return; 
}

int main(int argc, char* argv[]) {

    // Lambda function for usage description message 
    auto usage_and_exit = [argv]() {
        printf("   use: %s inputFile [outputFile]\n", argv[0]);
        printf("   inputFile -> path to input file (required) \n");
        printf("   outputFile -> path to ouput file (optional) \n\n");
        exit(-1);
    };

    // Argument checking
    if (argc < 2) {
        usage_and_exit();
    }

    // MPI environment initialization
    int provided, flag, claimed; 
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
    
    // Check correct initialization 
    MPI_Is_thread_main(&flag);
    if (!flag) {
        printf("This thread called MPI_Init_thread but it is not the main thread\n");
        MPI_Abort(MPI_COMM_WORLD, -1);
    }
    MPI_Query_thread(&claimed);
    if (claimed != provided) {
        printf("MPI_THREAD_FUNNELED not provided\n");
        MPI_Abort(MPI_COMM_WORLD, -1);
    }

    // Get process ID and total num. of process 
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Get processors/nodes names
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    // Get number  of nodes used 
    vector<char> all_names(size * MPI_MAX_PROCESSOR_NAME);
    MPI_Gather(
        processor_name, MPI_MAX_PROCESSOR_NAME, MPI_CHAR,
        all_names.data(), MPI_MAX_PROCESSOR_NAME, MPI_CHAR,
        0, MPI_COMM_WORLD
    );
    set<string> unique_nodes;
    for (int i = 0; i < size; ++i) {
        string node(&all_names[i * MPI_MAX_PROCESSOR_NAME]);
        unique_nodes.insert(node);
    }
    
    // Variable to handle time measurements
    double start_time, end_time;
    double start_time_read, end_time_read; 
    double start_time_out, end_time_out; 
    double time_distr; 

    // Start measuring total elapsed time, and time for chunks distributions
    MPI_Barrier(MPI_COMM_WORLD); 
    start_time = MPI_Wtime();
    
    // Set output stream
    ostream* resultsStream = &cout;
    ofstream filestream;
    if (argc > 2) {
        filestream = ofstream(argv[2]);
        if (filestream.is_open())
            resultsStream = &filestream;
    }

    MPI_Barrier(MPI_COMM_WORLD); 
    double start_time_mapfile = MPI_Wtime(); 

    // Input file array and its dimension (bytes)
    char* inFileMapped = nullptr;
    size_t inFileBytes = 0;

    // Opening input file (only root process)
    if (!rank){

        // Open input file in read-only mode and store file descriptor
        const string inFilename = argv[1];
        int inFileDesc = open(inFilename.c_str(), O_RDONLY);
        
        // Get input file information using fstat 
        struct stat inFileStat;
        fstat (inFileDesc, &inFileStat);     
        inFileBytes = inFileStat.st_size; 
        
        // Pointer to memory-mapped file data 
        inFileMapped = reinterpret_cast<char*>(
            mmap(0, inFileBytes, PROT_READ, MAP_PRIVATE, inFileDesc, 0)
        );

        // Close input file descriptor
        close(inFileDesc);      
    }

    MPI_Barrier(MPI_COMM_WORLD); 
    double end_time_mapfile = MPI_Wtime(); 
    
    // Perform chunk distribution, map phase and shuffle-communication phase in batch 
    unordered_map<long, vector<element_t>> elementsReceived = process_in_batch (MPI_COMM_WORLD, size, rank, inFileMapped, inFileBytes, time_distr);
    
    // Unmap input file in root process 
    
    MPI_Barrier(MPI_COMM_WORLD);
    double start_time_unmapfile = MPI_Wtime(); 
    if (!rank) {
        munmap(inFileMapped, inFileBytes);
    }
    MPI_Barrier(MPI_COMM_WORLD); 
    double end_time_unmapfile = MPI_Wtime(); 

    // Perform reduce-phase (similarity join computation)
    reducePhase(MPI_COMM_WORLD, size, rank, elementsReceived); 

    // Write to outuput file similar pairs 
    MPI_Barrier(MPI_COMM_WORLD); 
    start_time_out = MPI_Wtime(); 

    // Write to similar pairs to output file
    outputPairs(MPI_COMM_WORLD, size, rank, resultsStream);
    
    // Stop mesuring total elapsed time
    MPI_Barrier(MPI_COMM_WORLD);
    end_time_out = MPI_Wtime(); 
    end_time = MPI_Wtime(); 

    // Compute elapsed time
    double time_mapfile = end_time_mapfile - start_time_mapfile; 
    double time_unmapfile = end_time_unmapfile - start_time_unmapfile; 
    double elapsed_time = end_time - start_time;                   
    double elapsed_time_distr = time_distr + time_mapfile + time_unmapfile;
    double elapsed_time_out = end_time_out - start_time_out;

    // Results for metrics computation
    if (!rank){

        cout << 
            argv[0] << "\t" <<              // executables name 
            argv[1] << "\t" <<              // dataset name
            unique_nodes.size() << "\t" <<  // num of nodes
            size << "\t" <<                 // total num of process 
            foundSimilarTot << "\t" <<      // num of similar pair
            elapsed_time_distr << "\t" <<   // time for chunck distribution
            elapsed_time_out << "\t" <<     // time for outputting pairs
            elapsed_time << "\t" <<         // total elapsed time 
        endl; 

    }

    // MPI environment finalization
    MPI_Finalize(); 
    
    // Sucess status 
    return 0; 

}