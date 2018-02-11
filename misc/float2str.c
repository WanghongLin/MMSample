
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char *float2str(float f)
{
	int integer = (int) f;
	float fract = f - integer;

	char *ret = (char *)malloc(64);
	memset(ret, 0, 64);

	int i = 0;
	for (i = 0; integer > 0; integer /= 10, i++) {
		char c = (char) (integer % 10 + '0');
		*(ret+i) = c;
	}

	int N = strlen(ret) - 1;
	int j = 0;
	for (j = 0; j <= N / 2; j++) {
		char tmp = ret[j];
		ret[j] = ret[N-j];
		ret[N-j] = tmp;
	}

	ret[N+1] = '.';
	for (i = 0; fract - (int) fract > 0; i++) {
		fract *= 10;
		char c = (char) (((int) fract) % 10 + '0');
		ret[N+2+i] = c;
	}

	return ret;
}

int main(int argc, char *argv[])
{
	printf("%s\n", float2str(8.7777777f));
	return 0;
}
