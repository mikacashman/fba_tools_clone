////////////////////////////////////////////////////////////////////////////////
//	Driver to loop the MFAtoolkit script
//	For running GA experiments
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
using namespace std;

float getOV();

string oname = "/kb/module/work/fbafiles/GA/OV.txt";
ofstream OVfile;
string idname = "/kb/module/work/fbafiles/GA/ID.txt";
ofstream IDfile;
string tname = "/kb/module/work/fbafiles/GA/time.txt";
ofstream Tfile;
string iname = "/kb/module/work/fbafiles/GA/log.txt";
ifstream inFile;
string temp;
float ov;

int main ( int argc, char **argv) {

	struct timeval time;
    	srand(std::time(NULL));

	OVfile.open(oname);
	IDfile.open(idname);
	Tfile.open(tname);

	//=================================================================
        //============== Change Range of Values here ======================
        //=================================================================
        int minFlux_range[2] =  {0,1};
        int maxC_range[10] = {0,1,5,10,20,30,40,50,100,1000};
        int maxN_range[10] = {0,1,5,10,20,30,40,50,100,1000};
        int maxP_range[10] = {0,1,5,10,20,30,40,50,100,1000};
        int maxS_range[10] = {0,1,5,10,20,30,40,50,100,1000};
        int maxO_range[10] = {0,1,5,10,20,30,40,50,100,1000};

	//create varaibles
	int minimize_flux;
	int max_c_uptake;
	int max_n_uptake;
	int max_p_uptake;
	int max_s_uptake;
	int max_o_uptake;
	string replacement;
	string replace_call;		
	string call;
    	clock_t clockTicksTaken;
	string ID;
	float run_time;
	int NUM_TESTS = 5;
	int count=0;
	for (int t=0; t<NUM_TESTS; t++)
	{
		count++;
		cout << count << endl;
        auto startTime =std::chrono::high_resolution_clock::now();
		//startTime=clock();
		//Set the params
		minimize_flux=minFlux_range[rand()%2];
		max_c_uptake=maxC_range[rand()%10];
		max_n_uptake=maxN_range[rand()%10];
		max_p_uptake=maxP_range[rand()%10];
		max_s_uptake=maxS_range[rand()%10];
		max_o_uptake=maxO_range[rand()%10];
	
		//Alter the files
		replacement = "C:" + to_string(max_c_uptake) + 
				",N:" + to_string(max_n_uptake) +
				",O:" + to_string(max_o_uptake) + 
				",P:" + to_string(max_p_uptake) + 
				",S:" + to_string(max_s_uptake);
		//cout << replacement << endl;
		replace_call = "sed -i 's/uptake limits|.*|Specialized parameters/uptake limits|" + replacement + "|Specialized parameters/g' '/kb/module/work/fbafiles/GA/SpecializedParameters.txt'";
		system(replace_call.c_str());	

		replacement = to_string(minimize_flux);
		//cout << replacement << endl;	
		replace_call = "sed -i 's/flux minimization|.*|Specialized parameters/flux minimization|" + replacement + "|Specialized parameters/g' '/kb/module/work/fbafiles/GA/SpecializedParameters.txt'";
		system(replace_call.c_str());
        auto midTime=std::chrono::high_resolution_clock::now();
	      auto secsBeforeFBA = std::chrono::duration_cast<std::chrono::milliseconds>(midTime- startTime);

		//Call the MFAToolkit (aka FBA)
		call = "bash /kb/module/work/fbafiles/GA/runMFAToolkitGA.sh";
		system(call.c_str());
        auto endTime=std::chrono::high_resolution_clock::now();
		//endTime=clock();
	
		//Save ID and time
		ID = to_string(max_c_uptake) + 
			"," + to_string(max_n_uptake) +
			"," + to_string(max_o_uptake) + 
			"," + to_string(max_p_uptake) + 
			"," + to_string(max_s_uptake) +
			"," + to_string(minimize_flux);
		IDfile << ID << endl;
	//	clockTicksTaken = endTime - startTime;
        
        
        auto totalSeconds = std::chrono::duration_cast<std::chrono::seconds>(endTime- startTime);
//		totalSeconds = (double) clockTicksTaken / (double) CLOCKS_PER_SEC;
      //  totalSeconds=difftime(endTime,startTime);
	Tfile << "millisecs before fba:  " << secsBeforeFBA.count() << endl;
	        Tfile << totalSeconds.count() << endl;
		//Save objective value
		OVfile << getOV() << endl; 	

	};		
	OVfile.close();
	IDfile.close();
	Tfile.close();
	return 0;
};

float getOV()
{
	inFile.open(iname);
	inFile >> temp;
	while (temp!="value:")
		inFile >> temp;		
	inFile >> ov;
	inFile.close();
	return ov;
};
