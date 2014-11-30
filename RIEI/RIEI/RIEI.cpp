#include "Sketch.h"

int main(int argc, char* argv[])
{
    Sketch<> sketch("test.jpg");
    sketch.write("test_o.jpg");
	return 0;
}

