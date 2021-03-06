#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define SIZE 1600
#define BS 64

double A[SIZE * SIZE], B[SIZE * SIZE], C[SIZE * SIZE];

void print_matrix(double *a);

void dgemm_default(double *a, double *b, double *c)
{
	int i, j, k;

	for (i = 0; i < SIZE; i++)
	{
		for (j = 0; j < SIZE; j++)
		{
			for (k = 0; k < SIZE; k++)
			{
				*(c + i * SIZE + j) += *(a + i * SIZE + k) * *(b + k * SIZE + j);
			}
		}
	}
}

void dgemm_transpose(double *a, double *b, double *c)
{
	int i, j, k;
	for (i = 0; i < SIZE; i++)
	{
		for (k = 0; k < SIZE; k++)
		{
			for (j = 0; j < SIZE; j++)
			{
				*(c + i * SIZE + j) += *(a + i * SIZE + k) * *(b + k * SIZE + j);
			}
		}
	}
}

void dgemm_block(double *a, double *b, double *c)
{
	int i0, j0, k0;
	int c0[SIZE], a0[SIZE], b0[SIZE];
	for (int i = 0; i < SIZE; i += BS)
	{
		for (int j = 0; j < SIZE; j += BS)
		{
			for (int k = 0; k < SIZE; k += BS)
			{
				for (i0 = 0, *c0 = *(c + i * SIZE + j), *a0 = *(a + i * SIZE + k); i0 < BS; ++i0, *c0 += SIZE, *a0 += SIZE)
				{
					for (k0 = 0, *b0 = *(b + k * SIZE + j); k0 < BS; ++k0, *b0 += SIZE)
					{
						for (j0 = 0; j0 < BS; ++j0)
						{
							c0[j0] += a0[k0] * b0[j0];
						}
					}
				}
			}
		}
	}
}

void init_matrix(double *a, double *b, double *c)
{
	int i, j, k;

	for (i = 0; i < SIZE; i++)
	{
		for (j = 0; j < SIZE; j++)
		{
			for (k = 0; k < SIZE; k++)
			{
				*(a + i * SIZE + j) = rand() % 100;
				*(b + i * SIZE + j) = rand() % 100;
				*(c + i * SIZE + j) = 0;
			}
		}
	}
}

void init_matrix_C(double *c)
{
	int i, j, k;
	for (i = 0; i < SIZE; i++)
	{
		for (j = 0; j < SIZE; j++)
		{
			for (k = 0; k < SIZE; k++)
			{
				*(c + i * SIZE + j) = 0;
			}
		}
	}
}

void print_matrix(double *a)
{
	int i, j;

	printf("Matrix:\n");
	for (i = 0; i < SIZE; i++)
	{
		for (j = 0; j < SIZE; j++)
		{
			printf("%.2f  ", *(a + i * SIZE + j));
		}
		printf("\n");
	}
}

/*long long GetCacheAlignment()
{
	FILE *fcpu;
	if ((fcpu = fopen("/proc/cpuinfo", "r")) == NULL)
	{
		printf("Error: can't open /proc/cpuinfo \n");
		return -1;
	}
	size_t m = 0;
	char *line = NULL, *temp = (char *)malloc(50);
	while (getline(&line, &m, fcpu) > 0)
	{
		if (strstr(line, "cache_alignment"))
		{
			strcpy(temp, &line[18]);
			break;
		}
	}

	for (int i = 0; i < 50; i++)
	{
		if (temp[i] == ' ' || temp[i] == '\n')
		{
			temp[i] = '\0';
			//strncpy(temp, temp, i);
			//break;
		}
	}
	//printf("temp=%s|\n", temp);
	long long val = atoll(temp);
	if (val == 0)
	{
		printf("Error in GetCacheAlignment(): can't atoll \n");
		return -1;
	}

	fclose(fcpu);
	free(temp);
	return val;
}*/

int main()
{
	srand(time(0));
	struct timespec mt1, mt2;
	long int tt = 0;
	//long long cash = GetCacheAlignment();

	init_matrix(A, B, C);

	clock_gettime(CLOCK_REALTIME, &mt1);
	dgemm_default(A, B, C);
	clock_gettime(CLOCK_REALTIME, &mt2);
	tt = pow(10, 9) * (mt2.tv_sec - mt1.tv_sec) + (mt2.tv_nsec - mt1.tv_nsec);
	printf("time default = %f\n", (double)tt / pow(10, 9));

	//printf("cashm1 = %lld \n", cash);

	init_matrix_C(C);

	clock_gettime(CLOCK_REALTIME, &mt1);
	dgemm_transpose(A, B, C);
	clock_gettime(CLOCK_REALTIME, &mt2);
	tt = pow(10, 9) * (mt2.tv_sec - mt1.tv_sec) + (mt2.tv_nsec - mt1.tv_nsec);
	printf("time transpose= %f\n", (double)tt / pow(10, 9));

	//printf("cashm2 = %lld \n", cash);

	init_matrix_C(C);

	clock_gettime(CLOCK_REALTIME, &mt1);
	dgemm_block(A, B, C);
	clock_gettime(CLOCK_REALTIME, &mt2);
	tt = pow(10, 9) * (mt2.tv_sec - mt1.tv_sec) + (mt2.tv_nsec - mt1.tv_nsec);
	printf("time block= %f\n", (double)tt / pow(10, 9));

	//printf("cashm3 = %lld \n", cash);

	return 0;
}
