#include "Config.h"
#include "Worker.h"

#include <cstdio>
#include <cstdlib>
#include "Hashing.h"

int main(int argc, char* argv[])
{
    Config* config = Config::instance();
    config->read("default.config");
#ifdef _DEBUG
    int threadNum = 1;
#else
    int threadNum = 6;
#endif
    Worker worker;
    config->flipCost = 10000.0;
    config->rotateCost = 10000.0;
    worker.work("PI100.task", threadNum);
    //worker.work("PI100_Small_1.task", threadNum);
    //worker.work("PI100_Small_2.task", threadNum);
    //worker.work("test.task", threadNum);
	return 0;
}

