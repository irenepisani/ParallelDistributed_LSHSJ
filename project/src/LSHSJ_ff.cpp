/**
* @author   Irene Pisani 
* @note     University of Pisa, Computer Science department. 
*           M.Sc. Computer Science, Artificial Intelligence 
*           Parallel and Distributed Systems: Paradigms and models (23/24).
*
* @brief    Project track 3: Locality Sensitive Hashing based Similarity Join (LSHSJ)
* @details  Parallel implementation of LSHSJ for Shared Memory Systems with FF.   
*
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <ostream>
#include <random>
#include <vector>
#include <map>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <cstdlib>
#include <utility>

// FastFlow (FF) header libs
#include <ff/ff.hpp>
#include <ff/all2all.hpp>
#include <ff/pipeline.hpp>

// Custom header 
#include "hash.hpp"             // Hashing functions
#include "geometry_basics.hpp"  // Basic geometric operations
#include "frechet_distance.hpp" // Frechet distance computations

// LSH function parameters: family size, seed, resolution
#define LSH_FAMILY_SIZE 8       
#define LSH_SEED 234            
#define LSH_RESOLUTION 80

// Similarity threashold 
const static double SIM_THRESHOLD = 10;
const static double SIM_THRESHOLD_SQR = sqr(SIM_THRESHOLD);

using namespace ff; 
using namespace std; 

// Globals 
size_t similar{0};
ostream* resultsStream = &cout;

struct membuf : public streambuf {
    membuf(char* start, size_t size) {
        this->setg(start, start, start + size);
    }

    pos_type seekoff(off_type off, ios_base::seekdir dir,  ios_base::openmode which =  ios_base::in) {
        if (dir == ios_base::cur)
            gbump(off);
        else if (dir == ios_base::end)
            setg(eback(), egptr() + off, egptr());
        else if (dir == ios_base::beg)
            setg(eback(), eback() + off, egptr());
        return gptr() - eback();
    }
};

std::istream& GotoLine(std::istream& file, unsigned int num){
    file.clear(); file.seekg(0, file.beg);
    for(int i=0; i < num - 1; ++i)
        file.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    return file;
}

struct item {
    size_t id;
    int dataset;
    curve content;
};


struct element_t {
    long LSH;
    int dataSet;
    curve trajectory;
    array<long, LSH_FAMILY_SIZE> relativeLSHs;
    long id;

    element_t() = default; 
    element_t(long hash, int dataset, curve trajectory, decltype(relativeLSHs) relativeLSHs, long id) : LSH(hash), dataSet(dataset), trajectory(trajectory), relativeLSHs(relativeLSHs), id(id) {}
};

item parseLine(string& line){
    istringstream ss(line);
    item output;
    ss >> output.id;
    ss >> output.dataset;
    string tmp;
    ss >> tmp;
    if (!(tmp.find_first_of("[")==string::npos)) {
        tmp.replace(0, 1, "");
        tmp.replace(tmp.length() - 1, tmp.length(), "");
        bool ext = false;
        while (!ext) {
            ::string extrait = tmp.substr(tmp.find("["), tmp.find("]") + 1);
            ::string extrait1 = extrait.substr(1, extrait.find(",") - 1);
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

bool similarity_test(const curve& c1, const curve& c2){
    // check euclidean distance
    if (euclideanSqr(c1[0], c2[0]) > SIM_THRESHOLD_SQR || euclideanSqr(c1.back(), c2.back()) > SIM_THRESHOLD_SQR) 
        return false;
    // check equal time
    if (equalTime(c1, c2, SIM_THRESHOLD_SQR) || get_frechet_distance_upper_bound(c1, c2) <= SIM_THRESHOLD)
        return true;
    if (negfilter(c1, c2, SIM_THRESHOLD))
        return false;
    // full check 
    if (is_frechet_distance_at_most(c1, c2, SIM_THRESHOLD))
        return true;
    return false;
}

struct Mapper: ff_monode_t<element_t, element_t>{
    
    Mapper(membuf membuf_, size_t startChunk, size_t endChunk, FrechetLSH* familyLSH) 
    : membuff(membuf_), in(&membuff), startChunk(startChunk), endChunk(endChunk), familyLSH(familyLSH) {}

    int svc_init(){

        // Get num reducers and go starting line of the chunk
        num_outChannel = get_num_outchannels();
        GotoLine(in, startChunk+1);

        return 0;
    }

    element_t* svc(element_t* ){
        
        // Read lines of the chunk
        string line;
        for(int i = 0; i < endChunk; i++){
            getline(in, line);
            if (in.eof()) break;
            
            // Parse a line as item e prepare out pointer
            item it = parseLine(line);

            // Apply LSH function over item
            array<long, LSH_FAMILY_SIZE> rel_LSHs; 
            for(int index = 0; index < LSH_FAMILY_SIZE; index++)
                rel_LSHs[index] = familyLSH[index].hash(it.content);
            
            // Iterate over LSH values to create out element
            element_t* out;
            for (long h : rel_LSHs){
                
                out = new element_t();
                out->LSH = h;
			    out->dataSet = it.dataset;
                out->trajectory = it.content; 
                out->id = it.id;
                out->relativeLSHs = rel_LSHs;
                
                // send to selected reducers (by key)
                // size_t outChannel = hash<long>()(h) % num_outChannel; 
                size_t outChannel = h % num_outChannel; 
                ff_send_out_to(out, outChannel);
            }
        }
        // End-of-stream special message 
        return EOS; 
    }
    
    membuf membuff;
    istream in;
    size_t startChunk;
    size_t endChunk;
    FrechetLSH* familyLSH; 
    size_t num_outChannel;

};


struct Reducer: ff_minode_t<element_t, element_t>{
    
    
    element_t* svc(element_t* in){
        // Group elements by key 
        auto& input = *in;
        elements[input.LSH].push_back(move(input));
        
        delete in; 
        return GO_ON; 
    }

    inline void similarity(const long lsh, const element_t& a, const element_t& b){
        for(size_t ii = 0; ii < a.relativeLSHs.size(); ii++)
            if (a.relativeLSHs[ii] == b.relativeLSHs[ii]){
                if (lsh == a.relativeLSHs[ii]){
                    if (similarity_test(a.trajectory, b.trajectory)){
                        ++foundSimilar;
                        similarPair.emplace_back(a.id, b.id); 
                        //*resultsStream << a.id << "\t" << b.id << endl;
                    }
                }
                return;
            }
        }

    void svc_end(){
        
        // Similarity Join procedure
        for (auto& [lsh, elements_v] : elements){
            
            // compute all possilbe combination of elements in elements_v vector
            for(size_t i = 0; i < elements_v.size(); i++){
                for(size_t j = i+1; j < elements_v.size(); j++){
                    if (elements_v[i].dataSet != elements_v[j].dataSet)
                        similarity(lsh, elements_v[i], elements_v[j]);
                        

                }
            }
        }
        similar += foundSimilar;
    }

    vector<pair<long, long>> similarPair;
    unordered_map<long, vector<element_t>> elements; // Local (key-values) elements 
    size_t foundSimilar = 0;                         // Local counter of similar pairs

};


int main (int argc, char *argv[]) {

    // Usage description 
    auto usage_and_exit = [argv]() {
        printf("   use: %s inputFile Lworkers Rworkers [outputFile]\n", argv[0]);
        printf("   inputFile   -> input file path \n");
        printf("   Lworker     -> number of left-workers (mappers) \n");
        printf("   Rworker     -> number of right-workers (reducer) \n");
        printf("   policy      -> 0 (roundrobin) - 1 (on demand) \n");
        printf("   outputFile  -> output file path \n\n");
        exit(-1);
    };

    // Get datasets and num. threads from parameter
    const string inFilename = argv[1];
    const string outFilename = argv[5];
    const size_t num_mappers = stol(argv[2]);
    const size_t num_reducers = stol(argv[3]);
    const size_t policy = stol(argv[4]); 

    // Start timer
    ffTime(ff::START_TIME); 

    // Set output stream 
    ofstream filestream;
    filestream = ofstream(outFilename);
    if (filestream.is_open())
        resultsStream = &filestream;

    // Open input file and map it to memory 
    int inFileDesc = open(inFilename.c_str(), O_RDONLY);
    struct stat inFileStat;
    fstat (inFileDesc, &inFileStat);
    size_t inFileBytes = inFileStat.st_size;
    char* inFileMapped = reinterpret_cast<char*>(mmap(0, inFileBytes, PROT_READ, MAP_PRIVATE, inFileDesc, 0));
    
    // Count lines for chunk assignments
    size_t numLines = 0;
    size_t linesPerMapper; 
    for (size_t i = 0; i < inFileBytes; ++i) {
        if (inFileMapped[i] == '\n') {
            ++numLines;
        }
    }
    ++numLines; 
    close(inFileDesc);
    linesPerMapper = numLines / num_mappers; 

    // LSH family functions initilization 
    FrechetLSH lshFamily[LSH_FAMILY_SIZE];
    for (size_t i = 0; i < LSH_FAMILY_SIZE; i++) {
        lshFamily[i].init(LSH_RESOLUTION, LSH_SEED * i, i);
    }
    
    // Vectors of workers  
    vector<ff_node*> mapperSet;
    vector<ff_node*> reducerSet;

    // Popolute vector of workers 
    for (size_t i=0; i < num_mappers; i++){
        mapperSet.push_back(new Mapper(
            move(membuf(inFileMapped, inFileBytes)), 
            i*linesPerMapper, 
            (i == num_mappers-1 ? numLines-i*linesPerMapper : linesPerMapper),
            lshFamily)
        ); 
    }
    for (size_t i=0; i < num_reducers; i++){
        reducerSet.push_back(new Reducer()); 
    }

    // Build all-to-all network 
    ff_a2a a2a;
    if (!policy){
        a2a.add_firstset(mapperSet, 0); // Round robin
    } else {
        a2a.add_firstset(mapperSet, 1); // On-demand
    }
    a2a.add_secondset(reducerSet);

    // Run all-to-all network
    if (a2a.run_and_wait_end()<0) {
		error("running a2a\n");
		return -1;
    }

    // Free memory 
    munmap(inFileMapped, inFileBytes);

    // Print results to output file
    for (size_t i=0; i <num_reducers; i++){
        Reducer* r = reinterpret_cast<Reducer*>(reducerSet[i]);
        for (auto& p : r->similarPair){
            *resultsStream << p.first << " " << p.second <<endl; 
        }
    }
    

    // Stop timer
    ffTime(STOP_TIME);

    // Collect results
    cout << inFilename << "\t"
        << num_mappers << "\t" 
        << num_reducers << "\t" 
        << policy << "\t"
        << similar << "\t" 
        << ff::ffTime(ff::GET_TIME) / 1000
        << "\t" << a2a.ffTime()  / 1000 
        << endl;
 
    return 0;
}