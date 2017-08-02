//============================================================================
// Name        : GA_MA_logic.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : GA_MA logic for KBase SDK App
// Date	       : Summer 2017
//============================================================================

#include <iomanip>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <tgmath.h>
#include <cstdlib>
#include <array>
#include <cmath>
#include <string.h>
#include <chrono>
#include <sys/time.h>
using namespace std;

const int NUMGENES=4;

struct chrom
{
        int genes[NUMGENES];//6 integer array for genes
		     //This will need to be modified for mixed type genes
        float fitness;  //fitness = objective value
};

struct fitness
{
        bool solved=false;
        int location;
        chrom answer;
        int numGen=0;
};

//Globals
int SEED=100;
//mutation rate seems high
float MUTATIONRATE=1.2;//.03
const int SIZE=8;//32
chrom population[SIZE];//16

fitness Fit; 
chrom initialPop[SIZE];//32
bool FAILED=false;
int MAXRUNS=2;//increasing number doesn't change pop
float randNum;
//string iname = "/kb/module/work/fbafiles/GA_MA/log.txt";
string iname = "/kb/module/work/fbafiles/GA_MA/MFAOutput/RawData/RawSolutions.txt";
ifstream inFile;
float ov;
string replacement;
string replace_call;
string call;
string temp;
int crossPT=2;
int pos;
string R1 = "100|100|100|0|100|100|0|0|0|100|100|100|.* -100|-100|-100|-100|-100|-100|-100|-100|-100|-100|-100|-100|-100|-100|-100|-100 e|e|e|e|e|e|e|e|e|e|e|e|c|c|c|c 1|1|1|1|1|1|1|1|0.001|1|1|1|0.001|0.001|0.001|0.00    1";
string R2 = "100|100|100|0|100|100|0|0|0|100|100|100|";
string R3 = "   -100|-100|-100|-100|-100|-100|-100|-100|-100|-100|-100|-100|-100|-100|-100|-100 e|e|e|e|e|e|e|e|e|e|e|e|c|c|c|c 1|1|1|1|1|1|1|1|0.001|1|1|1|0.001|0.001|0.001|0.001";


//function globals
long long numMutated=0;//Mutation
long long numTotal=0;//Mutation
chrom tempC, tempC2;//Crossover

//Values to vary
int minFlux_range[2] = {0,1};
int *cpd9; 
int *cpd29; 
int *cpd84; 
int *cpd11; 


//functions
void GenInitialPop();
void getFitness(chrom[]);
void PrintChrom(chrom);
void Sort(chrom[]);
void GeneticAlgorithm();
float getOV();
void CrossOver2();
void Mutation();
void deleteFiles();

int main(int argc, char* argv[])
{
	//parse any inputs

	//set random seed
	srand (time(NULL));

	//intialize globals (need to remove these later)
	cpd9=new int[102]; 
	cpd29=new int[102]; 
	cpd84=new int[102]; 
	cpd11=new int[102]; 


	for (int i=0; i< 101; i++){
	   cpd9[i]=i;
	   cpd29[i]=i;
	   cpd84[i]=i;
	   cpd11[i]=i;
	}


	  cpd9[101]=1000;
	  cpd29[101]=1000;
	  cpd84[101]=1000;
	  cpd11[101]=1000;



	//generate initial population//////////////
        GenInitialPop();
        getFitness(initialPop);

	//test print pop
	cout << "-- Initial Population --" << endl;
	for (int i=0; i<SIZE; i++)
		PrintChrom(initialPop[i]);	
	cout << endl << endl;	

        Sort(initialPop);
        for(int i=0; i<SIZE ;i++)
                population[i]=initialPop[i];

        GeneticAlgorithm();

        //Print out results////////////////////////
        if (!FAILED)
        {
                cout << "Result: " << endl;
                cout << "    chrom: ";
                PrintChrom(Fit.answer);
                cout << endl;
                cout << "    number of generations: " << Fit.numGen << endl;
                cout << endl;
        }
        else
        {
                cout << "No solution found within " << MAXRUNS << " iterations" << endl;
                cout << "Closest solution: ";
                PrintChrom(Fit.answer);
                cout << "with a fitness of: " << Fit.answer.fitness << endl;
                cout << endl;
	}

	cout << "Done" << endl;
        delete cpd9;
        delete cpd29;
        delete cpd84;
        delete cpd11;

	return 0;
}

void GenInitialPop()
{
        //for all chroms
        for (int i=0; i<SIZE; i++)
        {
                initialPop[i].genes[0]=cpd9[rand()%102];
                initialPop[i].genes[1]=cpd29[rand()%102];
                initialPop[i].genes[2]=cpd84[rand()%102];
                initialPop[i].genes[3]=cpd11[rand()%102];
        }
        return;
}

void PrintChrom(chrom kitten)
{
        for(int i=0; i<NUMGENES ;i++)
                cout << setw(5) << (int)kitten.genes[i];
        cout << "  -  " << (float)kitten.fitness;
	cout << endl;
}

void GeneticAlgorithm()
{
        //check for termination condition
        if (Fit.numGen>MAXRUNS)
        {
                FAILED=true;
                Fit.answer=population[0];
                return;
        }

        //increase number of generations
        Fit.numGen++;

        //Run algorithm
        CrossOver2();
	getFitness(population);
        Sort(population);
	
	//test print with new children
 	cout << "--- New Population ---" << endl;
	cout << setw(5) << "cpd9" << setw(5) << "cpd29" << setw(5) << "cpd84" << setw(5) << "cpd11" << setw(5) << "fitness" << endl;
	for (int i=0; i<SIZE; i++)
        	PrintChrom(population[i]);
 	cout << endl << endl;

	Mutation();
        getFitness(population);
        Sort(population);

        //check if we have found both solutions
        //if (!Fit.solved)
        GeneticAlgorithm();

}

void getFitness(chrom pop[])
{

	for (int i=0; i<SIZE; i++)
	{
		//Alter SpecializedParmeter.txt
		replacement = "cpd00009_c0[c]:-100:" + to_string(pop[i].genes[0]) + 
				";cpd00029_c0[c]:-100:" + to_string(pop[i].genes[1]) + 
				";cpd00084_c0[c]:-100:" + to_string(pop[i].genes[2]) + 
				";cpd00011_c0[c]:-100:" + to_string(pop[i].genes[3]) + ";";
		//cout << replacement << endl;
		replace_call = "sed -i 's/exchange species|.*|Specialized parameters/exchange species|" + replacement + 
				"cpd11416_c0[c]:-10000:0|Specialized parameters/g' '/kb/module/work/fbafiles/GA_MA/SpecializedParameters.txt'";
		system(replace_call.c_str());

		//Alter media.tbl
		replacement = to_string(pop[i].genes[0]) + "|" + to_string(pop[i].genes[1]) + "|" + to_string(pop[i].genes[2]) + "|" + to_string(pop[i].genes[3]);
		//cout << replacement << endl;  
		replace_call = "sed -i 's/" + R1 + "/" + R2 + replacement + R3 + "/g' '/kb/module/work/fbafiles/GA_MA/media.tbl'";
		system(replace_call.c_str());

		//Call the MFAToolkit (aka FBA)
		call = "bash /kb/module/work/fbafiles/GA_MA/runMFAToolkitGA_MA.sh";
		system(call.c_str());
		pop[i].fitness = getOV(); 
	}
}

/*
float getOV()
{
        inFile.open(iname);
        inFile >> temp;
        while (temp!="value:")
                inFile >> temp;
        inFile >> ov;
        inFile.close();
	//cout << ov << endl;
        return ov;
};
*/
void Sort(chrom pop[])
//sorts chromosomes from best(lowest) to worse(highest) fitness using bubble sort
//yeah this is terrible I know
{
        int numSwapped=0;
        chrom temp;
	//for all chorms
        for(int i=0; i<(SIZE-1) ;i++)
        {
                if(pop[i].fitness<pop[i+1].fitness)
                {
                        //swap rules i and i+1
                        temp=pop[i];
                        pop[i]=pop[i+1];
                        pop[i+1]=temp;
                        numSwapped++;
                }
        }
        if(numSwapped==0)
                return;
        else
                Sort(pop);
}

void CrossOver2()
{
	//TODO
	//This needs to be made more fexible

        //if (CROSSOVERRANGE)
  // need to change the six to a constant
  //need to fix this later - myra
  crossPT=((rand()%3)+2); //choosed one of a few spots
      
        for (int j=0; j<crossPT ;j++)
        {
                tempC.genes[j]=population[0].genes[j];
                tempC2.genes[j]=population[2].genes[j];
        }
        for (int j=crossPT; j<NUMGENES ;j++)
        {
                tempC2.genes[j]=population[0].genes[j];
                tempC.genes[j]=population[2].genes[j];
        }

        //add children to population
        population[4]=tempC;
        population[5]=tempC2;

        for (int j=0; j<crossPT ;j++)
        {
                tempC.genes[j]=population[1].genes[j];
                tempC2.genes[j]=population[3].genes[j];
        }
        for (int j=crossPT; j<NUMGENES ;j++)
        {
                tempC2.genes[j]=population[1].genes[j];
                tempC.genes[j]=population[3].genes[j];
        }

        //add children to population
        population[6]=tempC;
        population[7]=tempC2;}

void Mutation()
{
        numTotal=0;
        numMutated=0;
        //for all chromosomes
        for(int i=0; i<SIZE ;i++)
        {
                //for all genes
                for(int j=0; j<NUMGENES ;j++)
                {
                        numTotal++;
                        randNum=(rand() % 100)+1;
                        if(randNum<=(MUTATIONRATE*100))
                        {//mutate
                                numMutated++;
				population[i].genes[j]=cpd9[rand()%102];//mutate between 0-10
                        }
                }
        }

}

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
