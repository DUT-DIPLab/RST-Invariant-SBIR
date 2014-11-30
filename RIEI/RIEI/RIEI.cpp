#include "Config.h"
#include "Worker.h"

int main(int argc, char* argv[])
{
    Config* config = Config::instance();
    config->read("default.config");
    Worker worker;
    worker.work("PI100_Small_2.task");
	return 0;
}

