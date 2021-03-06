//Copyright Notice
/*
    eXintegrator integrated expression analysis system
    Copyright (C) 2004  Martin Jakt & Okada Mitsuhiro
  
    This file is part of the eXintegrator integrated expression analysis system. 
    eXintegrator is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version. 

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
  
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    PS. If you can think of a better name, please let me know...
*/
//End Copyright Notice

// simply the declaration of some functions that 
// can be used with vectors and others to return some 
// statistical functions


#ifndef STAT_H
#define STAT_H

#include <vector>

using namespace std;
// I always wonder about that bloody namespace std thingy..
// as I read somewhere that it was bad, but I can't quite 
// remember where..
float absolute(float v);
float mean(vector<float>& v);
float std_dev(vector<float>& v);
float min(vector<float>& v);
float max(vector<float>& v);
float med(vector<float> v);   // return the median.. use slow sorting function.. -- don't use a reference as we'll sort the vector.. 

float euclidean(vector<float>& v1, vector<float>& v2);
float sqEuclidean(vector<float>& v1, vector<float>& v2);   // return the square of the euclidean, useful for somethings.. 
vector<float> z_score(vector<float>& v);
void zScore(vector<float>& v);                /// MODIFIES VECTOR V !!!!!
void zThreshold(vector<vector<float> >& v);            // complicated story.. 
float maxMeanDeviation(vector<float>& v);        // returns the maximum deviation from the mean value calculated as abs((value-mean)/mean)

vector<float> devsFromMean(vector<vector<float> > v);   // see code for details.. 

//float median(vector<float> v);
//float mad(vector<float>& v);
//vector<float> m_score(vector<float>& v);

vector<float> d_series(vector<float>& v);  // see stat.cpp for description of this vague concept

vector<int> f_distribution(vector<float>& v, float minV, float maxV, int divs);
vector<int> l_distribution(vector<float>& v, float minV, float maxV, int divs);
//vector<float> norm_median(vector<float> v);
vector<float> norm_mean(vector<float> v);   // these operate on and change a local copy, hence not passed by reference
//vector<float> norm_min_median(vector<float> v); // value-min / median-min
vector<float> norm_min_mean(vector<float> v);   // value-min / mean-min.

float modelNormalise(vector< vector<float> >& v);  // normalise assuming an extension of the Li and Wong model..
                                                   // essentially set the std / mean to the same for all of the things by 
                                                   // changing the mean.. -- set to the median value of the coordinates..
                                                   // then directly calculate sensitivity parameters based on the actual std_deviations
                                                   // or maybe the means.. -- I'm not sure at the moment, just trying things.. first.
float model2Normalise(vector< vector<float> >& v); // a bit different.. 
//float model2Normalise(vector< vector<float> >& v); -- why repeated ??? 

float binomialProb(int N, int s, float p);

//////////// some string or char* functions..
char* randomBytes(int length);             // uses new to allocate memory, so remember to delete after use !!.. 
int mutateBytes(char* bytes, int length, float freq); // mutate to random the bytes in  (freq should be between 0 and 1, but it isn't checked).
int softMutateBytes(char* bytes, int length, int maxDistance, float freq);   // mutate with a set maxDistance.. 
void expandPositions(char* oldBytes, char* newBytes, int oldLength, int newLength); // simulate nucleotide expansion.. 

#endif




