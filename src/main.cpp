#include <string.h>

extern "C" char run_parser(const char *input);

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		return run_parser(nullptr);
	} else {
		return 0;
	}
	return 0;
}
