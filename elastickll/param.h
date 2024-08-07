#ifndef _PARAM_H_
#define _PARAM_H_

#include <x86intrin.h>
#include <string.h>
#include <stdint.h>
#include <random>
#include <string>
#include <memory>
#include <iostream>
#include <cmath>
#include <math.h>

#define COUNTER_PER_BUCKET 8
#define MAX_VALID_COUNTER 7

#define ALIGNMENT 64

#define COUNTER_PER_WORD 8
#define BIT_TO_DETERMINE_COUNTER 3
#define K_HASH_WORD 1


#define KEY_LENGTH_4 4
#define KEY_LENGTH_13 13

#define CONSTANT_NUMBER 2654435761u
#define CalculateBucketPos(fp) (((*((uint64_t*)&fp)) * CONSTANT_NUMBER) >> 15)

#define GetCounterVal(val) ((uint32_t)((val) & 0x7FFFFFFF))

#define JUDGE_IF_SWAP(min_val, guard_val) ((guard_val) > ((min_val) * elastic_lambda))

#define UPDATE_GUARD_VAL(guard_val) ((guard_val) + 1)

#define SWAP_MIN_VAL_THRESHOLD 5

#define HIGHEST_BIT_IS_1(val) ((val) & 0x80000000)


// typedef uint64_t elem_t;

struct Bucket
{
	elem_t key[COUNTER_PER_BUCKET];
	uint32_t val[COUNTER_PER_BUCKET];
};

struct WeightedElement
{
	elem_t e;
	int w;
	bool operator < (const WeightedElement &y){
		return e < y.e;
	}
};

#define MAX_POSSIBLE_LEVEL_NUMBER 50

#define MIN_LEVEL_LENGTH 7

#define log_base 1.5

struct ConfigType
{
	int top_level_length, level_number;
	double c;
	ConfigType(){
		top_level_length = 1000;
		level_number = 10;
		c = 1.4142;
	}
}config;

#endif
