/*
 * Copyright(C) 2016 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * This file is part of GMPR.
 * 
 * GMPR is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * GMPR is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
 * License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GMPR. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Copyright(C) 2004-2014 Michael Dipperstein <mdipper@alumni.cs.ucsb.edu>
 * 
 * This file is part of BitFile.
 * 
 * BitFile is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * BitFile is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with BitFile. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Copyright(C) 2007, 2014 Michael Dipperstein <mdipper@alumni.engr.ucsb.edu>
 *
 * This file is part of the OptList library.
 *
 * OptList is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * OptList is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
 * License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OptList. If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <string.h>
#include <stdio.h>
#include "optlist.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <iterator>
#include <vector>
#include <stdlib.h>
#include <dirent.h>
#include <sys/time.h>
#include <thread>
#include <algorithm>    // std::sort

#include "opencv2/opencv.hpp" //for image processing later on
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/ml/ml.hpp"


using namespace std;
using namespace cv;

/***************************************************************************
*              Sample Program Using Arithmetic Encoding Library
*
*   File    : sample.c
*   Purpose : Demonstrate usage of arithmetic encoding library
*   Author  : Michael Dipperstein
*   Date    : March 10, 2004
*
****************************************************************************
*
* SAMPLE: Sample usage of the arcode Arithmetic Encoding Library
* Copyright (C) 2004, 2007, 2014 by
* Michael Dipperstein (mdipper@alumni.engr.ucsb.edu)
*
* This file is part of the arcode library.
*
* The arcode library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License as
* published by the Free Software Foundation; either version 3 of the
* License, or (at your option) any later version.
*
* The arcode library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
* General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
***************************************************************************/

/***************************************************************************
*                             INCLUDED FILES
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "optlist.h"
#include "arcode.h"

/***************************************************************************
*                               PROTOTYPES
***************************************************************************/

/***************************************************************************
*                                CONSTANTS
***************************************************************************/
#ifndef FALSE
#define FALSE   0
#endif

#ifndef TRUE
#define TRUE    1
#endif

/***************************************************************************
*                                FUNCTIONS DCT and IDCT
***************************************************************************/
#include <math.h>
#include <stdlib.h>

void dct(float **DCTMatrix, float **Matrix, int N, int M);
void write_mat(FILE *fp, float **testRes, int N, int M);
void idct(float **Matrix, float **DCTMatrix, int N, int M);
float **calloc_mat(int dimX, int dimY);
void free_mat(float **p);

float **calloc_mat(int dimX, int dimY)
{
    float **m = (float**)calloc(dimX, sizeof(float*));
    float *p  = (float*)calloc(dimX*dimY, sizeof(float));
    int i;
    for(i=0; i <dimX;i++){
    m[i] = &p[i*dimY];

    }
   return m;
}

void free_mat(float **m){
  free(m[0]);
  free(m);
}

void write_mat(FILE *fp, float **m, int N, int M){

   int i, j;
   for(i =0; i< N; i++){
    fprintf(fp, "%f", m[i][0]);
    for(j = 1; j < M; j++){
       fprintf(fp, "\t%f", m[i][j]);
        }
    fprintf(fp, "\n");
   }
   fprintf(fp, "\n");
}




//***********************************************************************************************************
//new gmpr code developed by Marcos Rodrigues [October 2016] starts here....

string vec2string( vector<int> vec)
{
    //convert input vector to a string stream
    std::stringstream ss;
    std::copy(vec.begin(), vec.end(), std::ostream_iterator<int>(ss," "));
    string s = ss.str();
    //replace the trailing space by EOF
    s.replace(s.end()-1,s.end(),1,(char)EOF_CHAR);
    return s;
}

vector<int> string2vec( string sMsg )
{
    vector<int> vOut;
    istringstream is( sMsg );
    int n;
    while( is >> n )
    {
    	vOut.push_back( n );
    }
    return vOut;
}


//Some global variables to facilitate thread functions
std::vector<int>  K(3);              //make key int for lossless text processing. Remove this limitation later on.
std::vector<int>  nData;             //data to compress
std::vector<int>  nLimited;          //list of unique ASCII/UTF characters in the file
std::vector<int>  nCoded;            //coded vector by the triple key
std::vector<vector<int> > nDecoded;  //2D decoded vector by multiple threads
int nThreads;          //how many threads were ever spawn
int nThreadCount;      //how many threads are there at execution time
int nThreadRange;      //range of each thread (how many numbers each thread decodes)

int max_in_vector(std::vector<int> vnData)
{
    int nMaximum = -1; //data is ASCII/UTF, so it has only positive values
    for (int i=0; i<vnData.size(); i++)
    {
        if (vnData[i]>nMaximum) nMaximum = vnData[i];
	 }
    return(nMaximum);
}

bool is_in(int n, std::vector<int> vnData)
{
    bool bResult = false;
    for (int i=0; i<vnData.size(); i++)
    {
        if (n == vnData[i]) bResult = true;
    }
    return bResult;
}

std::vector<int> sort_limited_by_counts( std::vector<int> vnLimited, std::vector<int> vnData)
{
    std::vector<int> counts( vnLimited.size(), 0);

    for (int i=0; i<vnLimited.size(); i++)
    {
        for (int j=0; j<vnData.size(); j++)
        {
            if (vnLimited[i] == vnData[j])
            {
                counts[i]++;
            }
        }
    }
    std::vector<int> counts_sorted = counts;
    std::sort( counts_sorted.rbegin(), counts_sorted.rend() ); //sort descending
    std::vector<int> limited_sorted( vnLimited.size() );

    int k=0;                                        //find the indices of sorted in vnLimited
    for (int i=0; i<counts_sorted.size(); i++)
    {
        for (int j=0; j<counts.size(); j++)
        {
            if (counts_sorted[i] == counts[j])
            {
                limited_sorted[k] = vnLimited[j];   //j is the index I am looking for
                counts[j] = -1;                     //make j invalid as there is only one occurrence
                k++;
            }
        }
    }

    return limited_sorted;
}

std::vector<int> generate_limited_data(std::vector<int> vnData)
{
    std::vector<int>tmp(256); //max number of possible different characters
    tmp[0]=vnData[0]; int k=1;
    for (int i=1; i<vnData.size(); i++)
    {
        if (!is_in(vnData[i], tmp))
        {
            tmp[k]=vnData[i];
            k++;
        }
    }
    std::vector<int>vnLimited(k);
    for (int i=0; i<k; i++) vnLimited[i]=tmp[i];
    return vnLimited;
}

void generate_key(std::vector<int> vnData, int nStartValue=1, int nFactor=2)
{
    int fMaxValue = 2*max_in_vector(vnData);
    K[0]=nStartValue;
    K[1]=((nStartValue*fMaxValue)+1);
    K[2]=((K[1]*fMaxValue)+1)* nFactor;
}

void generate_random_key(std::vector<int> vnData, int nSeed1=10, int nSeed2=1000)
{
    srand (time(NULL));
    int nStartValue = rand()%nSeed1+1;
    int nFactor = rand()%nSeed1+1;
    int nMaxValue = 2*max_in_vector(vnData);
    K[0]=rand()%nSeed2+1;
    K[1]=((nStartValue*nMaxValue)+1);
    K[2]=((K[1]*nMaxValue)+1) * nFactor;
}

std::vector<int> generate_coded_vector(std::vector<int> vnData)
{
    //this function codes every 3 numbers into one, so need to check length as multiple of 3
    int rem = vnData.size()%3;
    int ext = vnData.size()%3>0?1:0;
    std::vector<int>vnResult(vnData.size()/3+ext);

    int k=0;
    if (rem==0)//lucky me
    {
        for (int i=0; i<vnData.size(); i+=3) //code each set of 3 entries
        {
            vnResult[k] = K[0]*vnData[i] + K[1]*vnData[i+1] + K[2]*vnData[i+2];
            k++;
        }
    }
    else if (rem==1)
    {
       for (int i=0; i<vnData.size(); i+=3)
       {
           if (i+1==vnData.size())
           {
               vnResult[k] = K[0]*vnData[i]; //i is last entry, the set of 3 has only one value
               k++;
           }
            else //compute as normal, we have not reached the last set of 3 entries
            {
                vnResult[k] = K[0]*vnData[i] + K[1]*vnData[i+1] + K[2]*vnData[i+2];
                k++;
            }
       }
    }
    else if (rem==2)
    {
       for (int i=0; i<vnData.size(); i+=3)
       {
           if (i+2==vnData.size())
           {
               vnResult[k] = K[0]*vnData[i] + K[1]*vnData[i+1]; //last entry, set of 3 has 2 values
               k++;
           }
            else //compute as normal, not reached the last set yet
            {
                vnResult[k] = K[0]*vnData[i] + K[1]*vnData[i+1] + K[2]*vnData[i+2];
                k++;
            }
       }
    }
    return vnResult;
}

std::vector<int> open_raw_file(FILE *fp) //file pointer is already open when this function is called!
{
    long lSize; //number of characters in the file
    char *cbuffer;

    //fopen ( fp ); //the file is already open
    if( !fp ) perror("Error file does not exist."),exit(1);

    fseek( fp , 0L , SEEK_END);
    lSize = ftell( fp );
    rewind( fp );

    cbuffer = (char*)calloc( 1, lSize+1 ); // allocate memory for entire content
    if( !cbuffer ) fclose(fp),fputs("Memory alloc fails.",stderr),exit(1);

    if( 1!=fread( cbuffer , lSize, 1 , fp) ) // copy the file into the buffer
      fclose(fp),free(cbuffer),fputs("Entire fread function fails. Something wrong with the file.",stderr),exit(1);

    fclose(fp);

    // 'conversion' to ASCII/UTF happens here, also eliminate negative numbers
    std::vector<int>tmp(lSize);
    int k=0;
    for (int i = 0; i < lSize; i++)
    {
        if (cbuffer[i]>0)
        {
            tmp[k] = cbuffer[i];
            k++;
        }
    }

    std::vector<int>data(k); //k is the correct size of the input vector
    data = tmp; //tmp is longer than data, so tmp is cut off to the size of data
    return data;
}

std::vector<int> open_coded_file(FILE *fp) //fp is already open
{
    long lSize; //number of characters in the file
    char *cbuffer;

    //fopen ( fp ); //the file is already open
    if( !fp ) perror("Error file does not exist."),exit(1);

    fseek( fp , 0L , SEEK_END);
    lSize = ftell( fp );
    rewind( fp );

    cbuffer = (char*)calloc( 1, lSize+1 ); // allocate memory for entire content
    if( !cbuffer ) fclose(fp),fputs("Memory alloc fails.",stderr),exit(1);

    if( 1!=fread( cbuffer , lSize, 1 , fp) ) // copy the file into the buffer
      fclose(fp),free(cbuffer),fputs("Entire fread fails.",stderr),exit(1);

    rewind( fp );

    std::vector<int>tmp(lSize);
    int k=0;
    while (!feof (fp))
    {
      fscanf (fp, "%d", &tmp[k]);
      k++;
    }
    fclose (fp);

    std::vector<int>data(k); //k is the correct size of the input vector
    data = tmp;
    return data;
}


void save_coded_file(FILE *fp, std::vector<int> K, std::vector<int> vnLimited, std::vector<int> vnCoded)
{
    //save keys, size of limited, size of coded, limited, coded:
    fprintf(fp,"%i\n",K[0]);
    fprintf(fp,"%i\n",K[1]);
    fprintf(fp,"%i\n",K[2]);
    fprintf(fp, "%u\n", (int)vnLimited.size());
    fprintf(fp, "%u\n", (int)vnCoded.size());
    for (int i=0; i<vnLimited.size(); i++) fprintf(fp,"%d ",vnLimited[i]);
    fprintf(fp,"\n");
    for (int i=0; i<vnCoded.size(); i++) fprintf(fp,"%u ",vnCoded[i]);
    fclose(fp);
}

void *decode_vector_segment(void* argument)
{
    int nThisThread = *((int*) argument);
    int nLastIndex  = nCoded.size();
    int nLow        = nThisThread * nThreadRange;
    int nHigh       = (nLow+nThreadRange)>nLastIndex?nLastIndex:(nLow+nThreadRange);

    for (int r=nLow; r<nHigh; r++) //decode nRange numbers
    {
        for (int i=0; i<nLimited.size(); i++)
        {
            for (int j=0; j<nLimited.size(); j++)
            {
                for (int k=0; k<nLimited.size(); k++)
                {
                    if (r != nLastIndex-1)
                    {
                        if (nCoded[r] == K[0]*nLimited[i] + K[1]*nLimited[j] + K[2]*nLimited[k] ) //found it
                        {
                            nDecoded[nThisThread][3*(r-nLow)  ]  = nLimited[i];
                            nDecoded[nThisThread][3*(r-nLow)+1]  = nLimited[j];
                            nDecoded[nThisThread][3*(r-nLow)+2]  = nLimited[k];
                            break;
                        }
                    }
                    else    // There are three possibilities:
                    {
                        if (nCoded[r] == K[0]*nLimited[i] + K[1]*nLimited[j] + K[2]*nLimited[k] ) //3 entries
                        {
                            nDecoded[nThisThread][3*(r-nLow)+1]  = nLimited[j];
                            nDecoded[nThisThread][3*(r-nLow)+2]  = nLimited[k];
                            nDecoded[nThisThread][3*(r-nLow)  ]  = nLimited[i];
                            break;
                        }
                         else if (nCoded[r] == K[0]*nLimited[i] + K[1]*nLimited[j]) //this is the last number coded into 2.
                        {
                            nDecoded[nThisThread][3*(r-nLow)  ]  = nLimited[i];
                            nDecoded[nThisThread][3*(r-nLow)+1]  = nLimited[j];
                            //nDecoded[nThisThread].resize( nDecoded[nThisThread].size()-1); //delete last entry
                            break;
                        }
                        else if  (nCoded[r] == K[0]*nLimited[i]) //this is the last number coded into 1.
                        {
                            nDecoded[nThisThread][3*(r-nLow)  ]  = nLimited[i];
                            //nDecoded[nThisThread].resize( nDecoded[nThisThread].size()-2); //delete 2 last entries
                            break;
                        }
                    }
                }
            }
        }
    }
    __sync_fetch_and_sub(&nThreadCount,1); //counter is decremented as each thread exits
    return 0L;
}


void decode_vector(std::vector<int> cdata, int nRange=100)
{
    nThreadRange    = nRange; //update global variable
    nThreadCount    = cdata.size()%nThreadRange==0?cdata.size()/nThreadRange:cdata.size()/nThreadRange+1;//how many threads
    int nLastEntry  = cdata.size()%nThreadRange>0?cdata.size()%nThreadRange:nThreadRange;
    nThreads        = nThreadCount; //how many threads will be created
    nDecoded.resize(nThreads);
    for (int i=0; i<nThreads; i++)
    {
        if ( i!= nThreads-1 )
            nDecoded[i].resize(3*nThreadRange);
        else
            nDecoded[i].resize(3*nLastEntry);
    }

    pthread_t *tr[nThreads]; //array of thread ids
    for (int i=0; i<nThreads; i++)
    {
        pthread_create((pthread_t *)&tr[i], NULL, &decode_vector_segment, (void*)&i);
        waitKey(1);
    }
    do
    {
        __sync_synchronize();
    } while (nThreadCount);
    return;
}


int batch_encode(char* dirin, char* dirout, const model_t model)
{
    char filein[256];
    char fileout[256];
    int  counter = 0;

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir ( dirin )) != NULL)
    {
        while ((ent = readdir (dir)) != NULL) //read all file names, open, compress, and save compressed
        {
            char* name= ent->d_name;
            if (name[0]==46) //igore filenames starting with . (46='.')
            {
                counter+=0; //do nothing
            }
            else
            {
                memset(filein, 0, sizeof filein);//clear array
                strcpy(filein,dirin);            // copy string one into the result.
                strcat(filein,"/");              // append string two to the result.
                strcat(filein,name);
                FILE *fpin;
                fpin = fopen(filein, "rb");
                nData    = open_raw_file( fpin ); //this file is closed within open_file function
                nLimited = generate_limited_data(nData);
                nLimited = sort_limited_by_counts( nLimited, nData);
                generate_random_key(nData);
                nCoded   = generate_coded_vector(nData);

                std::string s = name;
                int Len = s.length();
                name[Len-1]='c'; //add c to file extention
                name[Len]='.';
                memset(fileout, 0, sizeof fileout); //clear array
                strcpy(fileout,dirout);             // copy string one into the result.
                strcat(fileout,"/");                // append string two to the result.
                strcat(fileout,name);
                FILE *fpout;
                fpout = fopen(fileout, "w");
                //save_coded_file(fpout, K, nLimited, nCoded);//this file is closed within save_coded function

                vector<int> coded_vector = K;
                coded_vector.push_back( nLimited.size());
                coded_vector.push_back( nCoded.size());
                coded_vector.insert(coded_vector.end(), nLimited.begin(), nLimited.end());
                coded_vector.insert(coded_vector.end(), nCoded.begin(), nCoded.end());
                string sMsg = vec2string( coded_vector );
                //printf("%s", sMsg.c_str());
                myArEncodeString(sMsg, fpout, model);
                fclose(fpout);
                counter++;
            }
        }
        closedir (dir);
        //cout<<"\nEncoded "<<counter<<" files."<<"\n";
        //cout<<"source     : "<<dirin<<"\n";
        //cout<<"destination: "<<dirout<<"\n";
        return 0;
    }
    else
    {
        return -1; //failure
    }
}

int batch_decode(char* dirin, char* dirout, const model_t model)
{
    char filein[256];
    char fileout[256];
    int  counter = 0;

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir ( dirin )) != NULL)
    {
        while ((ent = readdir (dir)) != NULL) //read all file names, open, compress, and save compressed
        {
            char* name= ent->d_name;
            if (name[0]==46) //igore filenames starting with . (46='.')
            {
                //do nothing
            }
            else
            {
                memset(filein, 0, sizeof filein);//clear array
                strcpy(filein,dirin);            // copy string one into the result.
                strcat(filein,"/");              // append string two to the result.
                strcat(filein,name);
                FILE *fpin;
                fpin = fopen(filein, "rb");
                string sMsg = myArDecodeFile(fpin, model);
                fclose(fpin);
                std::vector<int> data = string2vec( sMsg );
                //std::vector<int>data = open_coded_file( fpin );    //this file is closed within open_file function
                K[0]     = data[0];
                K[1]     = data[1];
                K[2]     = data[2];
                int nL   = data[3];
                int nC   = data[4];
                //cout<<"\nK[0],K[1],K[2]: "<<K[0]<<" "<<K[1]<<" "<<K[2]<<" nL="<<nL<<" nC="<<nC<<"\n";
                int iFirst = 5;    //first index to copy to limited data
                int iLast  = 5+nL; //last index + 1
                int iLen   = iLast - iFirst;

                nLimited.resize(iLen); //pre-allocate the space needed to write the data directly
                memcpy(&nLimited[0], &data[iFirst], iLen*sizeof(int)); //write directly to destination buffer from source buffer
                //cout<<"\nnlimite[0]: "<<nLimited[0];
                //cout<<"\nnLimited[last]: "<<nLimited[nLimited.size()-1];

                iFirst = 5+nL;
                iLast  = 5+nL+nC;
                iLen   = iLast - iFirst;
                nCoded.resize(iLen);
                //cod.resize(iLen); //pre-allocate the space needed to write the data directly
                memcpy(&nCoded[0], &data[iFirst], iLen*sizeof(int)); //write directly to destination buffer from source buffer
                //for (int i=0; i<cdata.size(); i++) printf("%d ",cdata[i]);
                decode_vector( nCoded );

                std::string s = name;
                int Len = s.length();
                name[Len-2]='d'; //add d to file extention
                memset(fileout, 0, sizeof fileout); //clear array
                strcpy(fileout,dirout);             // copy string one into the result.
                strcat(fileout,"/");                // append string two to the result.
                strcat(fileout,name);
                //cout<<"\nfile:"<<name<<" nThreads:"<<nThreads;

                int N=nThreads;
                //cout<<"\nfileout: "<<fileout;
                FILE *fpout;
                fpout = fopen(fileout, "w");
                for (int i=0; i<N; i++)
                {
                    for (int j=0; j<nDecoded[i].size(); j++)
                    {
                        fprintf(fpout,"%c", nDecoded[i][j]);
                        //printf("%d", nDecoded[i][j]);
                    }
                }
                fclose(fpout);

                counter++;
            }
        }
        closedir (dir);
        //cout<<"\nDecoded "<<counter<<" files."<<"\n";
        //cout<<"source     : "<<dirin<<"\n";
        //cout<<"destination: "<<dirout<<"\n";
        return 0;
    }
    else
    {
        return -1; //failure
    }
}


//*******************************************************************************
int main(int argc, char *argv[])
{
    printf("Hello world!\n");
    //run from the command line:
    // ./ac -c -i infile -o outfile
    // ./ac -d -i infile -o outfile
    // ./ac -? for help

    option_t *optList, *thisOpt;
    FILE *inFile, *outFile; /* input & output files */
    char encode;            /* encode/decode */
    model_t model;          /* static/adaptive model*/

    /* initialize data */
    inFile = NULL;
    outFile = NULL;
    encode = TRUE;
    model = MODEL_STATIC;

//#define BATCH //run this as a batch processing, comment this out for command line
#if !defined(BATCH) //check command line
    /* parse command line */
    optList = GetOptList(argc, argv, (char*)"acdi:o:h?");
    thisOpt = optList;

    while (thisOpt != NULL)
    {
        switch(thisOpt->option)
        {
            case 'a':       /* adaptive model vs. static */
                model = MODEL_ADAPTIVE;
                break;

            case 'c':       /* compression mode */
                encode = TRUE;
                break;

            case 'd':       /* decompression mode */
                encode = FALSE;
                break;

            case 'i':       /* input file name */
                if (inFile != NULL)
                {
                    fprintf(stderr, "Multiple input files not allowed.\n");
                    fclose(inFile);

                    if (outFile != NULL)
                    {
                        fclose(outFile);
                    }

                    FreeOptList(optList);
                    exit(EXIT_FAILURE);
                }
                else if ((inFile = fopen(thisOpt->argument, "rb")) == NULL)
                {
                    perror("Opening Input File");

                    if (outFile != NULL)
                    {
                        fclose(outFile);
                    }

                    FreeOptList(optList);
                    exit(EXIT_FAILURE);
                }

                break;

            case 'o':       /* output file name */
                if (outFile != NULL)
                {
                    fprintf(stderr, "Multiple output files not allowed.\n");
                    fclose(outFile);

                    if (inFile != NULL)
                    {
                        fclose(inFile);
                    }

                    FreeOptList(optList);
                    exit(EXIT_FAILURE);
                }
                else if ((outFile = fopen(thisOpt->argument, "wb")) == NULL)
                {
                    perror("Opening Output File");

                    if (inFile != NULL)
                    {
                        fclose(inFile);
                    }

                    FreeOptList(optList);
                    exit(EXIT_FAILURE);
                }

                break;

            case 'h':
            case '?':
                printf("Usage: %s <options>\n\n", FindFileName(argv[0]));
                printf("options:\n");
                printf("  -c : Encode input file to output file.\n");
                printf("  -d : Decode input file to output file.\n");
                printf("  -i <filename> : Name of input file.\n");
                printf("  -o <filename> : Name of output file.\n");
                printf("  -a : Use adaptive model instead of static.\n");
                printf("  -h | ?  : Print out command line options.\n\n");
                printf("Default: %s -c\n", FindFileName(argv[0]));

                FreeOptList(optList);
                return(EXIT_SUCCESS);
        }

        optList = thisOpt->next;
        free(thisOpt);
        thisOpt = optList;
    }

    /* validate command line */
    if (NULL == inFile)
    {
        fprintf(stderr, "Input file must be provided\n");
        fprintf(stderr, "Enter \"%s -?\" for help.\n", FindFileName(argv[0]));

        if (outFile != NULL)
        {
            fclose(outFile);
        }

        exit (EXIT_FAILURE);
    }
    else if (NULL == outFile)
    {
        fprintf(stderr, "Output file must be provided\n");
        fprintf(stderr, "Enter \"%s -?\" for help.\n", FindFileName(argv[0]));

        if (inFile != NULL)
        {
            fclose(inFile);
        }

        exit (EXIT_FAILURE);
    }

    if (encode) //we want to encode a file, single threaded always
    {
        nData = open_raw_file(inFile);
        nLimited = generate_limited_data(nData);
        nLimited = sort_limited_by_counts( nLimited, nData);
        generate_random_key(nData);
        nCoded=generate_coded_vector(nData);
        //save_coded_file(outFile, K, nLimited, nCoded); // this will save the big numbers, not needed
        vector<int> coded_vector = K;
        coded_vector.push_back( nLimited.size());
        coded_vector.push_back( nCoded.size());
        coded_vector.insert(coded_vector.end(), nLimited.begin(), nLimited.end());
        coded_vector.insert(coded_vector.end(), nCoded.begin(), nCoded.end());
        string sMsg = vec2string( coded_vector );
        myArEncodeString(sMsg, outFile, model); //arithmetic code the encoded vector, save to file
        fclose(outFile);
    }
    else //decode using multiple threads
    {
        struct timeval tic, toc; gettimeofday(&tic,NULL);
        string sMsg = myArDecodeFile(inFile, model);
        fclose(inFile);
        std::vector<int> data = string2vec( sMsg );
        //std::vector<int>data = open_coded_file( inFile );
        K[0]     = data[0];
        K[1]     = data[1];
        K[2]     = data[2];
        int nL   = data[3];
        int nC   = data[4];
        int iFirst = 5;    //first index to copy to limited data
        int iLast  = 5+nL; //last index + 1
        int iLen   = iLast - iFirst;

        nLimited.resize(iLen); //pre-allocate the space needed to write the data directly
        memcpy(&nLimited[0], &data[iFirst], iLen*sizeof(int)); //write directly to destination buffer from source buffer

        iFirst = 5+nL;
        iLast  = 5+nL+nC;
        iLen   = iLast - iFirst;
        nCoded.resize(iLen); //pre-allocate the space needed to write the data directly
        memcpy(&nCoded[0], &data[iFirst], iLen*sizeof(int)); //write directly to destination buffer from source buffer
        //for (int i=0;i<nCoded.size(); i++) printf("%d ",nCoded[i]);

        decode_vector( nCoded, 100 ); //multiple threads here each thread decodes 100 numbers
        for (int i=0; i<nThreads; i++)
        {
            for (int j=0; j<nDecoded[i].size(); j++)
            {
                fprintf(outFile,"%c",nDecoded[i][j]); //save decoded file
            }
        }
        fclose(outFile);

        gettimeofday(&toc,NULL);
        printf ("Total time = %f seconds\n",
            (double) (toc.tv_usec - tic.tv_usec) / 1000000 +
            (double) (toc.tv_sec - tic.tv_sec));
    }

    return 0;
#endif //if !defined(BATCH)

    struct timeval tic, toc;
    gettimeofday(&tic,NULL);

    //if we are here, then we will be  coding and decoding in batch, all files from directory dirin
    char *dirin  = (char*)"/Users/mar/Caco/Projects/Compress/ce/test";
    char *dirout = (char*)"/Users/mar/Caco/Projects/Compress/ce/testc";
    batch_encode( dirin, dirout, model);
    dirin  = (char*)"/Users/mar/Caco/Projects/Compress/ce/testc";
    dirout = (char*)"/Users/mar/Caco/Projects/Compress/ce/testd";
    batch_decode( dirin, dirout, model );

    gettimeofday(&toc,NULL);
    printf ("Total time = %f seconds\n", (double) (toc.tv_usec - tic.tv_usec) / 1000000 +
            (double) (toc.tv_sec - tic.tv_sec));
    return 0;


}
