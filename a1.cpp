
#include <iostream>
#include <fstream>
#include <vector>

#include "geom.h"
#include "graphics.h"
#include "postscript.h"
#include "xpm.h"

#define WIDTH 500
#define HEIGHT 500

const Color WHITE = { 255, 255, 255 };
const Color BLACK = { 0, 0, 0 };

void usage(){
	std::cout << "Usage: a1 <input> [<output>]" << std::endl;
}

int main(int argc, char** argv){
	if(argc < 2 || argc > 3){
		usage();
		return 1;
	}
	std::ifstream input(argv[1]);
	std::vector<Line2D<int> >* lines = new std::vector<Line2D<int> >(parsePostScript<int>(input));
	input.close();
	Image img(WIDTH, HEIGHT);
	img.fill(WHITE);
	auto end = lines->end();
	for(auto itr=lines->begin(); itr!=end; itr++){
		Line2D<int>& line = *itr;
		auto end2 = line.end();
		for(auto itr2=line.begin(); itr2!=end2; itr2++){
			Point2D<int>& pt = *itr2;
			pt.setY(HEIGHT - 1 - pt.getY());
		}
		img.drawLine(line, BLACK);
	}
	std::ostream* out = argc == 3 ? new std::ofstream(argv[2]) : &std::cout;
	writeXPM(*out, img);
	return 0;
}
