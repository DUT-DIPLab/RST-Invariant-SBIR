#include "Config.h"
#include "Worker.h"

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
    Hashing hashing;
    Sketch sketch("m_parts/anklet_0_0.jpg");
    auto bits = hashing.hashing(sketch);
    //worker.work("PI100.task", threadNum);
    //worker.work("PI100_Small_1.task", threadNum);
    //worker.work("PI100_Small_2.task", threadNum);
    //worker.work("test.task", threadNum);
	return 0;
}

