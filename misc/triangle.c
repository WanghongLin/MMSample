
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
	int n = atoi(argv[1]);
	int i = 0;

	for (i = 0; i < n; i++) {
		int j = 0;
		for (j = 0; j < (n - i); j++) {
			printf("%c", ' ');
		}

		int k = 0;
		for (k = 0; k < (2*i + 1); k++) {
			printf("%c", '*');
		}
		printf("%c", '\n');
	}
	return 0;
}
