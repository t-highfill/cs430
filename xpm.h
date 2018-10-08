
#ifndef __XPM_H__
#define __XPM_H__

#include <iomanip>
#include <ostream>
#include <string>
#include <set>
#include <map>

#include "graphics.h"

template<class T>
std::ostream& hex2(std::ostream& out, T val){
	out << std::hex << std::setfill('0') << std::setw(2) << val << std::dec;
	return out;
}

std::ostream& writeXPM(std::ostream& out, const Image& img, std::string name="img"){
	out << "static char* " << name << "[] = {" << std::endl;
	std::set<Color> colors = img.getColors();
	out << '"' << img.getWidth() << ' ' << img.getHeight() << ' ' << colors.size() << " 1\",\n";
	std::map<Color, char> colorMap;
	auto colorsEnd = colors.end();
	char ch = '#';
	for(auto itr = colors.begin(); itr!=colorsEnd; itr++){
		Color col = *itr;
		colorMap[col] = ch;
		out << '"' << ch << " c #";
		hex2(out, col._red);
		hex2(out, col._green);
		hex2(out, col._blue) << "\",\n";
		ch++;
	}
	for(int i=0; i<img.getHeight(); i++){
		out << '"';
		for(int j=0; j<img.getWidth(); j++){
			out << colorMap[img.getPixel(i,j)];
		}
		out << '"';
		if(i < img.getHeight()-1){
			out << ',';
		}
		out << std::endl;
	}
	out << "};" << std::endl;
	return out;
}

#endif