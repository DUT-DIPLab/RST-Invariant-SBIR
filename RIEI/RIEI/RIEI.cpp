#include "Config.h"
#include "Worker.h"

int main(int argc, char* argv[])
{
    Config* config = Config::instance();
    config->read("default.config");
    Worker worker;
    worker.work("PI100.task", 8);
    //worker.work("PI100_Small_2.task", 8);
    //worker.work("test.task", 8);
	return 0;
}

