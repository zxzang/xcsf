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
 **************
 * Description: 
 **************
 * The main XCSF module.  
 *
 * 1) Initialises the environment: constants, random number generator, problem
 * function, and performance output writing. If neural conditions are enabled
 * then a single neural network is initialised in memory for the classifiers to
 * copy their weights into and compute the matching function.
 *
 * 2) Executes the experiments: iteratively retrieving a problem instance,
 * generating a match set, calculating a system prediction, providing
 * reinforcement and running the genetic algorithm.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <errno.h>
#include "cons.h"
#include "random.h"
#include "cl.h"
#include "cl_set.h"
#include "ga.h"
#include "function.h"
#include "perf.h"

void trial(int trial, _Bool train, double *err);

int main(int argc, char *argv[0])
{    
	if(argc < 2 || argc > 4) {
		printf("Usage: xcsf inputfile [MaxTrials] [NumExp]\n");
		exit(EXIT_FAILURE);
	} 

	// initialise environment
	constants_init(argc, argv);
	random_init();
	func_init(argv[1]);
	gen_outfname(argv[1]);

	// run experiments
	double err[PERF_AVG_TRIALS];
	double terr[PERF_AVG_TRIALS];
	for(int e = 1; e < NUM_EXPERIMENTS+1; e++) {
		printf("\nExperiment: %d\n", e);
		pop_init();
		outfile_init(e);
		// each trial in an experiment
		for(int cnt = 0; cnt < MAX_TRIALS; cnt++) {
			trial(cnt, true, err); // train
			trial(cnt, false, terr);// test
			// display performance
			if(cnt%PERF_AVG_TRIALS == 0 && cnt > 0)
				disp_perf(err, terr, cnt, pop_num);
		}
		// clean up
		set_kill(&pset);
		outfile_close();
	}
	func_free();
	return EXIT_SUCCESS;
}

void trial(int cnt, _Bool train, double *err)
{
	// get problem function state and solution
	double *state = func_state(train);
	double answer = func_answer(train);
	// create match set
	NODE *mset = NULL, *kset = NULL;
	int msize = 0, mnum = 0;
	set_match(&mset, &msize, &mnum, state, cnt, &kset);
	// calculate system prediction and track performance
	double pre = set_pred(&mset, msize, state);
	err[cnt%PERF_AVG_TRIALS] = fabs(answer - pre);
	if(train) {
		// provide reinforcement to the set
		set_update(&mset, &msize, &mnum, answer, &kset, state);
		// run the genetic algorithm
		ga(&mset, msize, mnum, cnt, &kset);
	}
	// clean up
	set_kill(&kset); // kills deleted classifiers
	set_free(&mset); // frees the match set list
}
