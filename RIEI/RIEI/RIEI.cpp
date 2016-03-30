#include "Config.h"
#include "Worker.h"

#include <cstdio>
#include <cstdlib>
#include "Hashing.h"

const int TASK_IDS[] = {0, 1, 2, 4, 6, 7, 8, 9, 10, 11, 12, 13, 14, 20, 21, 23, 24, 25, 26, 27, 28, 30, 33, 34, 38, 42, 43, 45, 46, 47, 48};
const int TASK_NUM = sizeof(TASK_IDS) / sizeof(int);

int main(int argc, char* argv[])
{
    Config* config = Config::instance();
    config->read("default.config");
#ifdef _DEBUG
    int threadNum = 1;
#else
    int threadNum = 1;
#endif
    /*config->flipCost = 10000.0;
    config->rotateCost = 10000.0;
	for (int i = 0; i < TASK_NUM; ++i) {
		Worker worker;
		char path[128];
		sprintf(path, "task_%d.task", TASK_IDS[i]);
		worker.work(path, threadNum);
	}*/
    Worker worker;
    worker.work("PI100.task", threadNum);
    //worker.work("PI100_Small_1.task", threadNum);
    //worker.work("PI100_Small_2.task", threadNum);
    //worker.work("test.task", threadNum);
	return 0;
}

