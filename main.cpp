#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;

#define maxSIZE 16384 // max block numbers
#define maxWay 4

int main(int argc, char const *argv[])
{
	string inputPath;
	string outputPath;
	cout << "Place type the path of input file. ex:.././benchmark/trace1.txt" << endl;
	cin >> inputPath;
	cout << "Place type the path of output file. ex:.././out/trace1.out" << endl;
	cin >> outputPath;

	//creat file for output
	ofstream outPut(outputPath, ios::out);

	string inputStr;
	int cacheSize, blockSize, associativity, replace;
	int myTag[maxWay][maxSIZE];
	int myRef[maxWay][maxSIZE] = {0};
	int myState[maxWay][maxSIZE]= {0};
	vector<int> hitV;
	vector<int> missV;
	for(int i=0; i<maxWay; i++){
		for(int j=0; j<maxSIZE; j++){
			myTag[i][j] = -1;
		}
	}
	// read trace file
	ifstream input(inputPath);
	// ifstream input("./F740XXXXX/SRC/trace1.txt");
	// set cache condition
	for(int i = 0; i<4; i++){
		getline(input, inputStr);
		if(i == 0){
			cacheSize = atoi(inputStr.c_str());
		}
		else if(i == 1){
			blockSize = atoi(inputStr.c_str());
		}
		else if (i == 2){
			associativity = atoi(inputStr.c_str());
		}
		else{
			replace = atoi(inputStr.c_str());
		}
	}

	unsigned int addr;
	int instructionNum = 0;
	int state = 0;
	int refff = 0;
	while(getline(input, inputStr)){
        instructionNum++;
        addr = stoul(inputStr.assign(inputStr, 2, 8), nullptr, 16);
        //del offset
        int blockAddr = addr / blockSize;
        if(associativity == 0){
        	int indexNum = cacheSize * 1024 / blockSize;
        	int index = blockAddr % indexNum;
        	int tag = blockAddr / indexNum;
        	if(myTag[0][index] == tag){
        		hitV.push_back(instructionNum);
        	}
        	else{
        		myTag[0][index] = tag;
        		missV.push_back(instructionNum);
        	}
        }
        else if(associativity == 1){
        	int indexNum = cacheSize * 1024 / (blockSize * 4);
        	int index = blockAddr % indexNum;
        	int tag = blockAddr / indexNum;
        	bool flagHit = false;

        	// hit
        	for(int i=0; i<maxWay; ++i){
        		if(myTag[i][index] == tag){
        			hitV.push_back(instructionNum);
        			myRef[i][index] = ++refff;
        			flagHit = true;
        			break;
        		}
        	}
        	// miss
        	if(flagHit == false){
        		bool empty = false;
        		for(int i=0; i < maxWay; i++){
        			if(myTag[i][index] == -1){
        				myTag[i][index] = tag;
        				missV.push_back(instructionNum);
        				myState[i][index] = ++state;
        				myRef[i][index] = ++refff;
        				empty = true;
        				break;
        			}
        		}
        		if(empty == false){
        			if(replace == 1){
        				int wayMin = 0;
        				int minRef = 99999999;
        				for(int i = 0; i<maxWay; ++i){
        					if(minRef > myRef[i][index]){
        						minRef = myRef[i][index];
        						wayMin = i;
        					}
        				}
        				myTag[wayMin][index] = tag;
        				myRef[wayMin][index] = ++refff;
	        			missV.push_back(instructionNum);
        			}
        			else{
        				int wayMin = 0;
        				int minState = 99999999;
        				for(int i = 0; i<maxWay; ++i){
        					if(minState > myState[i][index]){
        						minState = myState[i][index];
        						wayMin = i;
        					}
        				}
        				myTag[wayMin][index] = tag;
        				myState[wayMin][index] = ++state;
        				missV.push_back(instructionNum);
        			}
        		}
        	}

        }
        else{
        	int blokcNum = cacheSize * 1024 / blockSize;
        	int tag = blockAddr;
        	bool flagHit = false;
        	// hit
        	for(int i=0; i < blokcNum; i++){
        		if(myTag[0][i] == tag){
        			hitV.push_back(instructionNum);
        			myRef[0][i] = ++refff;
        			flagHit = true;
        			break;
        		}
        	}
        	// miss
        	if(flagHit == false){
        		bool empty = false;
        		for(int i=0; i < blokcNum; i++){
        			if(myTag[0][i] == -1){
        				myTag[0][i] = tag;
        				missV.push_back(instructionNum);
        				myState[0][i] = ++state;
        				myRef[0][i] = ++refff;
        				empty = true;
        				break;
        			}
        		}
        		if(empty == false){
	        		if(replace == 1){
	        			int minTag = 0;
	        			int minRef = 99999999;
	        			for(int i=0; i < blokcNum; i++){
	        				if(minRef > myRef[0][i]){
	        					minRef = myRef[0][i];
	        					minTag = i;
	        				}
	        			}
	        			myTag[0][minTag] = tag;
	        			myRef[0][minTag] = ++refff;
	        			missV.push_back(instructionNum);
	        		}

	        		else{
	        			int minTag = 0;
	        			int minState = 99999999;
	        			for(int i=0; i < blokcNum; i++){
	        				if(minState > myState[0][i]){
	        					minState = myState[0][i];
	        					minTag = i;
	        				}
	        			}
	        			myTag[0][minTag] = tag;
	        			myState[0][minTag] = ++state;
	        			missV.push_back(instructionNum);
	        		}
	        	}
        	}
        }


    }
    outPut <<"Hits instructions: ";

    outPut << hitV[0];
    for(int i =1; i < hitV.size(); ++i){
    	outPut << ',' << hitV[i];
    }
    outPut << endl;

    outPut <<"Misses instructions: ";
    outPut << missV[0];

    for(int i =1; i < missV.size(); ++i){
    	outPut << ',' << missV[i];
    }
    outPut << endl;
    outPut <<"Miss rate: ";
    float rate = (float)missV.size() / ((float)missV.size() + (float)hitV.size());
    outPut << rate << endl;

	return 0;
}





