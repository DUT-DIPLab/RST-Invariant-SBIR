#include "Config.h"
#include "Preprocesser.h"

int main(int argc, char* argv[])
{
    Config* config = Config::instance();
    config->read("default.config");
    Preprocesser preprocesser;
    preprocesser.generateSketch("1.jpg", "1_1.jpg");
    preprocesser.cutOutSketch("1_1.jpg", "1_2.jpg");
    preprocesser.sketchThinning("1_2.jpg", "1_3.jpg");
	return 0;
}

