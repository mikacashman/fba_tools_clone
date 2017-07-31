//============================================================================
// Name        : GA_logic.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : GA logic for KBase SDK App
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

struct chrom
{
        int genes[6];//6 integer array for genes
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
float MUTATIONRATE=.1;//.03
int SIZE=16;//32
chrom population[8];//16
int NUMGENES=6;
fitness Fit; 
chrom initialPop[16];//32
bool FAILED=false;
int MAXRUNS=2;//increasing number doesn't change pop
float randNum;
string iname = "/kb/module/work/fbafiles/GA/log.txt";
ifstream inFile;
float ov;
string replacement;
string replace_call;
string call;
string temp;
int crossPT=3;

//function globals
long long numMutated=0;//Mutation
long long numTotal=0;//Mutation
chrom tempC, tempC2;//Crossover

//Values to vary
int minFlux_range[2] =  {0,1};
int maxC_range[10] = {0,1,5,10,20,30,40,50,100,1000};
int maxN_range[10] = {0,1,5,10,20,30,40,50,100,1000};
int maxP_range[10] = {0,1,5,10,20,30,40,50,100,1000};
int maxS_range[10] = {0,1,5,10,20,30,40,50,100,1000};
int maxO_range[10] = {0,1,5,10,20,30,40,50,100,1000};

//functions
void GenInitialPop();
void getFitness(chrom[]);
void PrintChrom(chrom);
void Sort(chrom[]);
void GeneticAlgorithm();
float getOV();
void CrossOver2();
void Mutation();

int main(int argc, char* argv[])
{
	//parse any inputs

	//set random seed
	srand (time(NULL));

	//generate initial population//////////////
        GenInitialPop();
        getFitness(initialPop);

	//test print pop
	cout << "-- Initial Population --" << endl;
	for (int i=0; i<SIZE; i++)
		PrintChrom(initialPop[i]);	
	cout << endl << endl;	

        Sort(initialPop);
        SIZE=8;//16
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
	return 0;
}

void GenInitialPop()
{
        //for all chroms
        for (int i=0; i<SIZE; i++)
        {
                //first bool gene
		initialPop[i].genes[0]=minFlux_range[rand()%2];
                //next 5 that range from 0-10
                initialPop[i].genes[1]=maxC_range[rand()%10];
                initialPop[i].genes[2]=maxN_range[rand()%10];
                initialPop[i].genes[3]=maxP_range[rand()%10];
                initialPop[i].genes[4]=maxS_range[rand()%10];
                initialPop[i].genes[5]=maxO_range[rand()%10];
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
	cout << setw(5) << "minF" << setw(5) << "C" << setw(5) << "N" << setw(5) << "P" << setw(5) << "S" << setw(5) << "O" << setw(5) << "fitness" << endl;
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
		//Alter the files
		replacement = "C:" + to_string(pop[i].genes[1]) +
				",N:" + to_string(pop[i].genes[2]) +
				",O:" + to_string(pop[i].genes[3]) + 
				",P:" + to_string(pop[i].genes[4]) + 
				",S:" + to_string(pop[i].genes[5]);
		//cout << replacement << endl;
		replace_call = "sed -i 's/uptake limits|.*|Specialized parameters/uptake limits|" + replacement + "|Specialized parameters/g' '/kb/module/work/fbafiles/GA/SpecializedParameters.txt'";
		system(replace_call.c_str());

		replacement = to_string(pop[i].genes[0]);
		//cout << replacement << endl;  
		replace_call = "sed -i 's/flux minimization|.*|Specialized parameters/flux minimization|" + replacement + "|Specialized parameters/g' '/kb/module/work/fbafiles/GA/SpecializedParameters.txt'";
		system(replace_call.c_str());

		//Call the MFAToolkit (aka FBA)
		call = "bash /kb/module/work/fbafiles/GA/runMFAToolkitGA.sh";
		system(call.c_str());
		pop[i].fitness = getOV(); 
	}
}

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
        //        crossPT=(rand()%((MAX+1)-MIN))+MIN;

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
                        if(randNum<(MUTATIONRATE*100))
                        {//mutate
                                numMutated++;
				if (j==0)
					population[i].genes[j]=minFlux_range[rand()%2]; //mutate bool
				else
					population[i].genes[j]=maxC_range[rand()%10];//mutate between 0-10
                        }
                }
        }
}

