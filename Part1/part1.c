#include <sys/sysinfo.h>

int main()
{
	struct sysinfo info;

	for (int i = 0; i < 4; ++i)
	{
		sysinfo(&info);
	}
}