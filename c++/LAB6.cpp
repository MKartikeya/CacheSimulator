#include<fstream>
#include<iostream>
#include<vector>
#include<string>
#include<cmath>
#include<ctime>
using namespace std;
class Cache {
public:
    int size;
    int blockSize;
    int associativity;
    string replacementPolicy;
    string writebackPolicy;
    int numSets;
    int numBlocks;
    int numBlocksPerSet;
    int numBitsBlockOffset;
    int numBitsSetIndex;
    int numBitsTag;
    int totalAccesses;
    vector<vector<long long>> cache;
    vector<vector<bool>> dirty;
    vector<vector<int>> validBits;
    int hitCount;
    int missCount;
    Cache(int size, int blockSize, int associativity, string replacementPolicy, string writebackPolicy) {
        this->size=size;
        this->blockSize=blockSize;
        if(associativity==0) associativity=size/blockSize;
        this->associativity=associativity;
        this->replacementPolicy=replacementPolicy;
        this->writebackPolicy=writebackPolicy;
        this->numBlocks=size/blockSize;
        this->numSets=numBlocks/associativity;
        this->numBlocksPerSet=numBlocks/numSets;
        this->numBitsBlockOffset=log2(blockSize);
        this->numBitsSetIndex=log2(numSets);
        this->numBitsTag=32-numBitsBlockOffset-numBitsSetIndex;
        this->cache=vector<vector<long long>>(numSets, vector<long long>(numBlocksPerSet, 0));
        this->dirty = vector<vector<bool>>(numSets, vector<bool>(numBlocksPerSet, false));
        this->validBits = vector<vector<int>>(numSets, vector<int>(numBlocksPerSet, 0));
        this->hitCount=0;
        this->missCount=0;
        this->totalAccesses=0;
        //initializingg tags to -1
        for(int i=0;i<numSets;i++){
            for(int j=0;j<numBlocksPerSet;j++){
                cache[i][j]=-1;
                validBits[i][j]=0;
            }
        }
    }      

    void access(char mode, long long address) {
        totalAccesses++;
        long long tag=address>>(numBitsBlockOffset+numBitsSetIndex);
        int setIndex=(address>>numBitsBlockOffset)&((1<<numBitsSetIndex)-1);
        int blockOffset=address&((1<<numBitsBlockOffset)-1);
        // cout<<tag<<" "<<setIndex<<" "<<blockOffset<<endl;
        for(int i=0;i<numBlocksPerSet;i++){
            if(cache[setIndex][i]==tag){
                //printing tag,address,setIndex in hexadecimal
            cout<<hex<<"Address: 0x"<<address<<" Set: 0x"<<setIndex<<" Hit "<<"Tag: 0x"<<tag;
                cout<<""<<endl;
                hitCount++;
                update(setIndex,tag,address,1);
                return;
            }
        }
        missCount++;
        //comparig writeback policy
        if(writebackPolicy=="WB"){
            cout<<hex<<"Address: 0x"<<address<<" Set: 0x"<<setIndex<<" Miss "<<"Tag: 0x"<<tag;
            cout<<""<<endl;
            update(setIndex,tag,address,0);
        }
        else if(writebackPolicy=="WT" ){
            cout<<hex<<"Address: 0x"<<address<<" Set: 0x"<<setIndex<<" Miss "<<"Tag: 0x"<<tag;
            cout<<""<<endl;
            if(mode=='R'){
                update(setIndex,tag,address,0);
            }
        }
    }

    void update(int setIndex, long long tag, int address,int hit) {
        int blkIndex=-1;
        if(replacementPolicy=="LRU"){
                for(int i=0;i<numBlocksPerSet;i++){
                    if(cache[setIndex][i]==tag){
                        blkIndex=i;
                        // cout<<"Hit"<<endl;
                        for(int j=i;j>0;j--){
                            cache[setIndex][j]=cache[setIndex][j-1];
                        }
                        cache[setIndex][0]=tag;
                        return;
                    }
                }
                for(int i=numBlocksPerSet-1;i>0;i--){
                    cache[setIndex][i]=cache[setIndex][i-1];
                    if(cache[setIndex][i]!=-1){
                        validBits[setIndex][i]=1;
                    }
                }
                cache[setIndex][0]=tag;
            
            }
            else if(replacementPolicy=="FIFO"){
                for(int i=0;i<numBlocksPerSet;i++){
                    if(cache[setIndex][i]==tag){
                        blkIndex=i;
                        return;
                    }
                }
                for(int i=0;i<numBlocksPerSet;i++){
                    if(cache[setIndex][i]==-1){
                        cache[setIndex][i]=tag;
                        validBits[setIndex][i]=1;
                        return;
                    }
                }
                for(int i=0;i<numBlocksPerSet-1;i++){
                    cache[setIndex][i]=cache[setIndex][i+1];
                }
                cache[setIndex][numBlocksPerSet-1]=tag;
            }
            else if(replacementPolicy=="Random" || replacementPolicy=="RANDOM"){
                for(int i=0;i<numBlocksPerSet;i++){
                    if(cache[setIndex][i]==tag){
                        blkIndex=i;
                        return;
                    }
                }
                for(int i=0;i<numBlocksPerSet;i++){
                    if(cache[setIndex][i]==-1){
                        cache[setIndex][i]=tag;
                        validBits[setIndex][i]=1;
                        return;
                    }
                }
                int i=rand()%numBlocksPerSet;
                cache[setIndex][i]=tag;
                return;
            }
            if(hit==1) dirty[setIndex][blkIndex] = true;   
    }
};

vector<string> readConfig(const string& filename) {
    ifstream configFile("cache.config");
    if (!configFile.is_open()) {
        cout<<"Error opening the configuration file."<<endl;
        return vector<string>();
    }
    string size,block_size,associativity,replacement_policy,writeback_policy;
    configFile>>size;
    configFile>>block_size;
    configFile>>associativity;
    configFile>>replacement_policy;
    configFile>>writeback_policy;
    vector<string> config={size,block_size,associativity,replacement_policy,writeback_policy};
    return config;

}

vector<pair<char, long long>> readAccess(const string& filename) {
    ifstream accessFile("cache.access");
    if (!accessFile.is_open()) {
        cout<<"Error opening the configuration file."<<endl;
        return vector<pair<char, long long>>();
    }
    string line;
    vector<pair<char, long long>> accessSequence;
    while (getline(accessFile, line)){
        if(line.find("R")==0){
            accessSequence.push_back(make_pair('R', stoll(line.substr(3), nullptr, 16)));
        }
        else if(line.find("W")==0){
            accessSequence.push_back(make_pair('W', stoll(line.substr(3), nullptr, 16)));
        }
    }
    return accessSequence;
}

void simulateCache(const string& configFilename, const string& accessFilename) {
    vector<string> config=readConfig(configFilename);
    if(config.size()==0){return;}
    Cache cache(stoi(config[0]), stoi(config[1]), stoi(config[2]), config[3], config[4]);
    vector<pair<char, long long>> accessSequence=readAccess(accessFilename);
    for (const auto& access : accessSequence) cache.access(access.first, access.second);
    cout<<"Hit Count: "<<dec<<cache.hitCount<<endl;
    cout<<"Miss Count: "<<dec<<cache.missCount<<endl;
    cout<<"Total Accesses: "<<dec<<cache.totalAccesses<<endl;
    // cache.update();
}

int main() {
        srand(static_cast<unsigned>(time(0)));
    simulateCache("cache.config", "cache.access");
    return 0;
}              