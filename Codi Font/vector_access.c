#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

// Timing code (get_time) based on http://stackoverflow.com/a/2349941

#ifdef _MSC_VER

#include <windows.h>

/**
* Get the current time through gettimeofday().
* Returns: A timestamp, in seconds, of the current time of the day.
*/
double get_time()
{
	LARGE_INTEGER t, f;
	QueryPerformanceCounter(&t);
	QueryPerformanceFrequency(&f);
	return (double)t.QuadPart / (double)f.QuadPart;
}

#else

#include <sys/time.h>
#include <sys/resource.h>

/**
* Get the current time through gettimeofday().
* Returns: A timestamp, in seconds, of the current time of the day.
*/
static double get_time()
{
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);
	return tv.tv_sec + tv.tv_usec / 1000000.0;
}

#endif

/**
* Generate a cyclic random permutation of the given vector.
* See http://www.sciencedirect.com/science/article/pii/0020019086900736
* Params:
*     vector: The vector to permute.
*     vector_size: The number of elements in the vector.
*/
static void random_permute(size_t *vector, size_t vector_size)
{
	// Generate a cyclic random permutation
	// See http://www.sciencedirect.com/science/article/pii/0020019086900736
	for (size_t i = vector_size - 1; i >= 1; i--)
	{
		size_t k = (size_t)(((rand() << 15) | (rand() & 0x7FFF)) % i);

		size_t tmp = vector[i];
		vector[i] = vector[k];
		vector[k] = tmp;
	}
}

/**
* Executes a test checking the speed of linear or random access to a vector.
* Params:
*     num_iterations: Number of vector accesses to run the benchmark for.
*     vector_size: Number of elements of the vector (in size_t's).
*     randomize: false to test linear accesses, true to test random accesses.
* Returns: The time elapsed to run the test (in seconds), or NAN on error.
*/
static double run_test(size_t num_iterations, size_t vector_size, bool randomize)
{
	// Allocate memory for the vector
	size_t *vector = malloc(vector_size * sizeof(size_t));
	if (vector == NULL)
	{
		printf("Can't allocate the vector.\n");
		return NAN;
	}

	// Fill the vector with sequential indices, for linear iteration
	for (size_t i = 0; i < vector_size - 1; i++)
		vector[i] = i + 1;
	vector[vector_size - 1] = 0; // (cycle at the end)

	if (randomize)
	{
		random_permute(vector, vector_size);
	}

	// Run the benchmark
	double start_time = get_time();

	// Mark start of loop in output assembly
#ifndef _MSC_VER
	asm("/* Start of loop */");
#endif

	size_t idx = 0;
	for (size_t i = 0; i < num_iterations; i += 32)
	{
		idx = vector[idx];
		idx = vector[idx];
		idx = vector[idx];
		idx = vector[idx];
		idx = vector[idx];
		idx = vector[idx];
		idx = vector[idx];
		idx = vector[idx];
		idx = vector[idx];
		idx = vector[idx];
		idx = vector[idx];
		idx = vector[idx];
		idx = vector[idx];
		idx = vector[idx];
		idx = vector[idx];
		idx = vector[idx];
		idx = vector[idx];
		idx = vector[idx];
		idx = vector[idx];
		idx = vector[idx];
		idx = vector[idx];
		idx = vector[idx];
		idx = vector[idx];
		idx = vector[idx];
		idx = vector[idx];
		idx = vector[idx];
		idx = vector[idx];
		idx = vector[idx];
		idx = vector[idx];
		idx = vector[idx];
		idx = vector[idx];
		idx = vector[idx];
	}

	// Mark end of loop in output assembly
#ifndef _MSC_VER
	asm("/* End of loop */");
#endif

	// This will prevent idx from being optimized away, even at -Ofast
	volatile size_t useme = idx;

	double end_time = get_time();

	// Release memory used for the vector
	free(vector);

	return end_time - start_time;
}

/**
* Runs a test with the specified parameters and then prints the test results in CSV format.
* Params:
*     num_iterations: Number of vector accesses to run the benchmark for.
*     vector_size: Number of elements of the vector (in size_t's).
* Returns: true on success, false on failure.
*/
static bool print_test(size_t num_iterations, size_t vector_size)
{
	double time_linear = run_test(num_iterations, vector_size, false);
	double time_random = run_test(num_iterations, vector_size, true);

	printf("%zu,%zu", vector_size, num_iterations);
	if (!isnan(time_linear))
		printf(",%f", time_linear);
	else
		printf(",ERROR");

	if (!isnan(time_random))
		printf(",%f", time_random);
	else
		printf(",ERROR");
	printf("\n");

	return !isnan(time_linear) && !isnan(time_random);
}

int main(void)
{
	const size_t NUM_ITERATIONS = 1073741824;
	const size_t VECTOR_SIZE_SMALL = 8;
	const size_t VECTOR_SIZE_BIG = 536870912;

	// Seed random number generator
	srand((unsigned int)time(NULL));

	// Print CSV header row
	printf("Vector size,Num iterations,Time (linear),Time (random)\n");

	// Run test set
	bool success = true;
	for (size_t size = VECTOR_SIZE_SMALL; size <= VECTOR_SIZE_BIG; size *= 2)
	{
		success &= print_test(NUM_ITERATIONS, size);
	}

	return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
