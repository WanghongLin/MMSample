
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int str2int(const char* str)
{
	int N = strlen(str);
	int base = 1;
	int i, j;

	int result = 0;
	for (i = 0, j = N-1; i < N; i++, j--) {
		result += (int) (str[j] - '0') * base;
		base *= 10;
	}
	return result;
}

void usage(const char* progname)
{
	fprintf(stderr, "%s string\n", progname);
	exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		usage(argv[0]);
	}

	printf("%d\n", str2int(argv[1]));
	return EXIT_SUCCESS;
}
