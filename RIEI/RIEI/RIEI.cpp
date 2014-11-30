#include "Config.h"
#include "Preprocesser.h"
#include "Decomposer.h"

int main(int argc, char* argv[])
{
    Config* config = Config::instance();
    config->read("default.config");
    Decomposer decomposer;
    Sketch<> sketch("1_2.jpg");
    auto parts = decomposer.decompose(sketch);
    parts[0].write("1_4_0.jpg");
    parts[1].write("1_4_1.jpg");
    parts[2].write("1_4_2.jpg");
    parts[3].write("1_4_3.jpg");
    parts[4].write("1_4_4.jpg");
    parts[5].write("1_4_5.jpg");
    parts[6].write("1_4_6.jpg");
    parts[7].write("1_4_7.jpg");
	return 0;
}

