////////////////////////////////////////////////////////////////////////////////
//	Driver to loop the MFAtoolkit script
//	For running GA_MA experiments
//	Authors: Mikaela Cashman & Myra Cohen
//	Date: Summer 2017
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <map>
#include <iostream>
#include <sys/time.h>
#include <string.h>
#include <chrono>
#include <vector>
using namespace std;

float getOV();
void deleteFiles();

string oname = "/kb/module/work/fbafiles/GA_MA/OV.txt";
ofstream OVfile;
string idname = "/kb/module/work/fbafiles/GA_MA/ID.txt";
ofstream IDfile;
string tname = "/kb/module/work/fbafiles/GA_MA/time.txt";
ofstream Tfile;
string iname = "/kb/module/work/fbafiles/GA_MA/MFAOutput/RawData/RawSolutions.txt";
ifstream inFile;
string iname2 = "/kb/module/work/fbafiles/GA_MA/media.tbl";
ifstream inFile2;
string temp;
string temp2;
float ov;
int pos;
string test;
string tempSplit[12];
vector<string> vec;
char c;

string R1 = "100|100|100|0|100|100|0|0|0|100|100|100|.*	-100|-100|-100|-100|-100|-100|-100|-100|-100|-100|-100|-100|-100|-100|-100|-100	e|e|e|e|e|e|e|e|e|e|e|e|c|c|c|c	1|1|1|1|1|1|1|1|0.001|1|1|1|0.001|0.001|0.001|0.001";
string R2 = "100|100|100|0|100|100|0|0|0|100|100|100|";
string R3 = "	-100|-100|-100|-100|-100|-100|-100|-100|-100|-100|-100|-100|-100|-100|-100|-100	e|e|e|e|e|e|e|e|e|e|e|e|c|c|c|c	1|1|1|1|1|1|1|1|0.001|1|1|1|0.001|0.001|0.001|0.001";


int main ( int argc, char **argv) {

	struct timeval time;
    	srand(std::time(NULL));

	OVfile.open(oname);
	IDfile.open(idname);
	Tfile.open(tname);

	//=================================================================
        //============== Change Range of Values here ======================
        //=================================================================
	int cpd9[10] = {0,1,5,10,20,30,40,50,100,1000};
        int cpd29[10] = {0,1,5,10,20,30,40,50,100,1000};
        int cpd84[10] = {0,1,5,10,20,30,40,50,100,1000};
        int cpd11[10] = {0,1,5,10,20,30,40,50,100,1000};
	
	int c9;
	int c29;
	int c84;
	int c11;
	//create varaibles
	string replacement;
	string replace_call;		
	string call;
    	clock_t clockTicksTaken;
	string ID;
	float run_time;
	int NUM_TESTS = 23;
	int count=0;
	string test[4];
	for (int t=0; t<NUM_TESTS; t++)
	{
		count++;
		cout << "Run: " << count << endl;
        auto startTime =std::chrono::high_resolution_clock::now();
		
		c9=cpd9[rand()%10];
                c29=cpd29[rand()%10];
                c84=cpd84[rand()%10];
                c11=cpd11[rand()%10];

		//startTime=clock();
		replacement = "cpd00009_c0[c]:-100:" + to_string(c9) + 
                                ";cpd00029_c0[c]:-100:" + to_string(c29) + 
                                ";cpd00084_c0[c]:-100:" + to_string(c84) + 
                                ";cpd00011_c0[c]:-100:" + to_string(c11) + ";";
                //cout << replacement << endl;
                replace_call = "sed -i 's/exchange species|.*|Specialized parameters/exchange species|" + replacement + 
                                "cpd11416_c0[c]:-10000:0|Specialized parameters/g' '/kb/module/work/fbafiles/GA_MA/SpecializedParameters.txt'";
		system(replace_call.c_str());
		//replacement = cpd9[t].str() + "|" + cpd11[t].str() + "|" + cpd29[t].str() + "|" + cpd84[t].str();
		replacement = to_string(c9) + "|" + to_string(c11) + "|" + to_string(c29) + "|" + to_string(c84);
		
		replace_call = "sed -i 's/" + R1 + "/" + R2 + replacement + R3 + "/g' '/kb/module/work/fbafiles/GA_MA/media.tbl'";
		//cout << endl << endl << replace_call << endl << endl;
		system(replace_call.c_str());		

		auto midTime=std::chrono::high_resolution_clock::now();
	      auto secsBeforeFBA = std::chrono::duration_cast<std::chrono::milliseconds>(midTime- startTime);

		//Call the MFAToolkit (aka FBA)
		call = "bash /kb/module/work/fbafiles/GA_MA/runMFAToolkitGA_MA.sh";
		system(call.c_str());
        auto endTime=std::chrono::high_resolution_clock::now();
		//endTime=clock();
	
		//Save ID and time
		ID = replacement; 
		IDfile << ID << endl;
		cout << "  ID=" << ID << endl;
	//	clockTicksTaken = endTime - startTime;
        
        
        auto totalSeconds = std::chrono::duration_cast<std::chrono::seconds>(endTime- startTime);
//		totalSeconds = (double) clockTicksTaken / (double) CLOCKS_PER_SEC;
      //  totalSeconds=difftime(endTime,startTime);
	Tfile << "millisecs before fba:  " << secsBeforeFBA.count() << endl;
	        Tfile << totalSeconds.count() << endl;
		
		//Check for and
		//Save objective value
		
		OVfile << getOV() << endl; 	

	};		
	OVfile.close();
	IDfile.close();
	Tfile.close();
	return 0;
};

void deleteFiles()
{
	remove("/kb/module/work/fbafiles/GA_MA/MFAOutput/CombinationKO.txt");
	remove("/kb/module/work/fbafiles/GA_MA/MFAOutput/SolutionCompoundData.txt");
	remove("/kb/module/work/fbafiles/GA_MA/MFAOutput/TightBoundsCompoundData.txt");
	remove("/kb/module/work/fbafiles/GA_MA/MFAOutput/TightBoundsReactionData.txt");
	remove("/kb/module/work/fbafiles/GA_MA/MFAOutput/RawData/RawSolutions.txt");
	remove("/kb/module/work/fbafiles/GA_MA/MFAOutput/RawTightBounds0.txt");
}

float getOV()
{
	inFile.open(iname);
	if (!inFile)
	{
		//cout << "Output File not found" << endl;
		ov = 0;
	}
	else
	{
		getline(inFile,temp);
		getline(inFile,temp);
		pos = temp.rfind(";");
		ov = stof(temp.substr(pos+1,temp.length()));
		deleteFiles();
	}
	cout << "  OV=" << ov << endl;
	inFile.close();
	return ov;
};