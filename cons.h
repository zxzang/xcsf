/*
 * Copyright (C) 2015 Richard Preen <rpreen@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * ************
 * Description:
 * ************
 *
 * XCSF global constants; read from cons.txt
 */

#define max( a, b ) ( ((a) > (b)) ? (a) : (b) )
#define min( a, b ) ( ((a) < (b)) ? (a) : (b) )

void constants_init(int argc, char **argv);

// experiment parameters
_Bool POP_INIT; // population initially empty or filled with random conditions
double THETA_MNA; // minimum number of classifiers in a match set
int MAX_TRIALS; // number of problem instances to run in one experiment
int NUM_EXPERIMENTS; // number of experiments to run
int PERF_AVG_TRIALS; // number of problem instances to average performance output
int POP_SIZE; // maximum number of macro-classifiers in the population
// classifier parameters
double ALPHA; // linear coefficient used in calculating classifier accuracy
double BETA; // learning rate for updating error, fitness, and set size
double DELTA; // fit used in prob of deletion if fit less than this frac of avg pop fit 
double EPS_0; // classifier target error, under which the fitness is set to 1
double ERR_REDUC; // amount to reduce an offspring's error
double FIT_REDUC; // amount to reduce an offspring's fitness
double INIT_ERROR; // initial classifier error value
double INIT_FITNESS; // initial classifier fitness value
double NU; // exponent used in calculating classifier accuracy
double THETA_DEL; // min experience before fitness used in probability of deletion
// genetic algorithm parameters
double P_CROSSOVER; // probability of applying crossover (for hyperrectangles)
double P_MUTATION; // probability of mutation occuring per allele
double THETA_GA; // average match set time between GA invocations
int THETA_OFFSPRING; // number of offspring to create each GA invocation
// self-adaptive mutation parameters
double muEPS_0; // minimum value of a self-adaptive mutation rate
int NUM_MU; // number of self-adaptive mutation rates
// classifier condition parameters
double MAX_CON; // maximum value of a hyperrectangle interval
double MIN_CON; // minimum value of a hyperrectangle interval
double S_MUTATION; // maximum amount to mutate an allele
int NUM_HIDDEN_NEURONS; // number of hidden neurons to perform matching condition
int DGP_NUM_NODES; // number of nodes in a DGP graph
// prediction parameters
double XCSF_ETA; // learning rate for updating the computed prediction
double XCSF_X0; // prediction weight vector offset value
// subsumption parameters
_Bool GA_SUBSUMPTION; // whether to try and subsume offspring classifiers
_Bool SET_SUBSUMPTION; // whether to perform match set subsumption
double THETA_SUB; // minimum experience of a classifier to become a subsumer
// set by environment
int state_length; // number of input variables
