/*
 * Copyright (C) 2016 Richard Preen <rpreen@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include "cons.h"
#include "random.h"
#include "dgp.h"

void node_add(GRAPH *dgp, GNODE *new);
void node_free(GNODE *node);
void node_rand_conn(GNODE *node, int n);
_Bool node_mutate(GNODE *node, double rate, int n);
void node_print(GNODE *node);
char node_symbol(int func);
void node_init(GNODE *node, int n);
void node_copy(GNODE *to, GNODE *from);
void node_update(double *state, int func, double input);

double graph_output(GRAPH *dgp, int i)
{
	return dgp->nodes[i].state;
}

void graph_reset(GRAPH *dgp)
{
	for(int i = 0; i < dgp->n; i++)
		dgp->nodes[i].state = dgp->nodes[i].initial_state;
}

void graph_rand(GRAPH *dgp)
{
	for(int i = 0; i < dgp->n; i++)
		node_init(&dgp->nodes[i], dgp->n);
}

void graph_update(GRAPH *dgp, double *inputs)
{
	graph_reset(dgp);
	for(int t = 0; t < dgp->t; t++) {
		// synchronous update
		for(int i = 0; i < dgp->n; i++) {
			GNODE *a = &dgp->nodes[i];
			for(int k = 0; k < MAX_K; k++) {
				// external input
				if(a->conn[k] < 0)
					node_update(&a->state, a->func, inputs[abs(a->conn[k])-1]);
				// internal input
				else if(a->conn[k] > 0)
					node_update(&a->state, a->func, dgp->nodes[a->conn[k]-1].state);
			}
		}
	}
}

void graph_init(GRAPH *dgp, int n)
{
	dgp->t = irand(0,MAX_T)+1;
	dgp->n = n;
	dgp->nodes = malloc(sizeof(GNODE)*dgp->n);
	graph_rand(dgp);
}

void graph_copy(GRAPH *to, GRAPH *from)
{
	to->t = from->t;
	to->n = from->n;
	free(to->nodes);
	to->nodes = malloc(sizeof(GNODE)*from->n);
	memcpy(to->nodes, from->nodes, sizeof(GNODE)*from->n);
}

void node_copy(GNODE *to, GNODE *from)
{
	to->k = from->k;
	to->state = from->state;
	to->initial_state = from->initial_state;
	to->func = from->func;
	memcpy(to->conn, from->conn, sizeof(int)*MAX_K);
}

void graph_print(GRAPH *dgp)
{
	printf("Graph: N=%d; T=%d\n", dgp->n, dgp->t);
	for(int i = 0; i < dgp->n; i++) {
		printf("(%d) ", i+1);
		node_print(&dgp->nodes[i]);
	}
}
 
void node_init(GNODE *node, int n)
{
	node->initial_state = (2.0*drand())-1.0; 
	node->state = node->initial_state;
	node->func = irand(0,NUM_FUNC);
	node_rand_conn(node, n);
}

void node_rand_conn(GNODE *node, int n)
{
	node->k = 0;
	for(int i = 0; i < MAX_K; i++) {
		if(drand() < 0.1) {
			node->conn[i] = 0; // inert
		}
		else if(drand() < 0.2) {
			node->conn[i] = -irand(1,state_length+1);
			(node->k)++;
		}
		else {
			node->conn[i] = irand(1,n+1);
			(node->k)++;
		}
	}
}

void graph_free(GRAPH *dgp)
{
	free(dgp->nodes);
}

_Bool graph_mutate(GRAPH *dgp, double rate)
{
	_Bool mod = false;
	for(int i = 0; i < dgp->n; i++) {
		if(node_mutate(&dgp->nodes[i], rate, dgp->n))
			mod = true;
	}
	
	// mutate T
	if(drand() < rate) {
		int t = dgp->t;
		if(drand() < 0.5) {
			if(dgp->t > 1)
				(dgp->t)--;
		}
		else {
			if(dgp->t < MAX_T)
				(dgp->t)++;
		}
		if(t != dgp->t)
			mod = true;
	}

	return mod;
}

_Bool node_mutate(GNODE *node, double rate, int n)
{  
	_Bool mod = false;
	// mutate function
	if(drand() < rate) {
		int old = node->func;
		node->func = irand(0,NUM_FUNC);
		if(old != node->func)
			mod = true;
	}                    
	// mutate connectivity map
	for(int i = 0; i < MAX_K; i++) {
		if(drand() < rate) {
			int old = node->conn[i];
			if(drand() < 0.1)
				node->conn[i] = 0;
			else if(drand() < 0.2)
				node->conn[i] = -irand(1,state_length+1);
			else
				node->conn[i] = irand(1,n+1);
			if(old != node->conn[i])
				mod = true;
		}
	}
	// refresh k
	if(mod) {
		node->k = 0;
		for(int i = 0; i < MAX_K; i++) {
			if(node->conn[i] != 0)
				(node->k)++;
		}
	}
	return mod;
}

double graph_avg_k(GRAPH *dgp)
{
	int k = 0;
	for(int i = 0; i < dgp->n; i++) {
		if(dgp->nodes[i].func > 3) { // sin/cos
			if(dgp->nodes[i].k > 0)
				k++;
		}
		else
			k += dgp->nodes[i].k;
	}
	return k/(double)dgp->n;
}

void node_print(GNODE *node)
{
	printf("Node: (%c) c: ", node_symbol(node->func));
	for(int i = 0; i < MAX_K; i++)
		printf("%d,", node->conn[i]);
	printf(" s: %f\n", node->state);
}

char node_symbol(int func)
{
	switch(func) {
		case 0: return '+';
		case 1: return '-';
		case 2: return '*';
		case 3: return '/';
		case 4: return 'S';
		case 5: return 'C';
		case 6: return 'T';
		default: return ' ';
	}
}

void node_update(double *state, int func, double input)
{
	switch(func) {
		case 0: *state += input; break;
		case 1: *state -= input; break;
		case 2: *state *= input; break;
		case 3: if(input != 0.0) *state /= input; break;
		case 4: *state = sin(input); break;
		case 5: *state = cos(input); break;
		case 6: *state = tanh(input); break;
		default: break;
	}
	if(*state > 1.0)
		*state = 1.0;
	else if(*state < -1.0)
		*state = -1.0;
}
