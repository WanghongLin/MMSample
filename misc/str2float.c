
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

float str2float(const char *str)
{
	float ret = 0;

	float fbase = 0.1;
	float ibase = 1;

	int i = 0;
	int N = strlen(str) - 1;
	int point = -1;
	for (i = 0; i <= N; i++) {
		if (str[i] == '.') {
			point = i;
		}

		if (point != -1 && str[i] != '.') {
			ret += (float) ((str[i] - '0') * fbase);
			fbase *= 0.1;
		}
	}

	for (i = point-1; i >= 0; i--) {
		ret += (float) ((str[i] - '0') * ibase);
		ibase *= 10;
	}

	return ret;
}

int main(int argc, char *argv[])
{
	printf("%f\n", str2float("3.1415926"));
	return 0;
}
