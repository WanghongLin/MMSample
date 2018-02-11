
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char *int2str(int from)
{
	char *ret = (char *) malloc(64);
	memset(ret, 0, 64);

	if (from == 0) {
		ret[0] = '0';
		return ret;
	}

	int i;
	for (i = 0; from > 0; from /= 10, i++) {
		char c = (char) (from % 10 + '0');
		ret[i] = c;
	}

	int j = 0;
	int N = strlen(ret) - 1;
	for (j = 0; j <= N / 2; j++) {
		char tmp = ret[j];
		ret[j] = ret[N-j];
		ret[N-j] = tmp;
	}

	return ret;
}

int main(int argc, char *argv[])
{

	printf("%s\n", int2str(8192));
	return 0;
}
