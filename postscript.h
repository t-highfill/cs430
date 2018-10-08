
#ifndef __POSTSCRIPT_H__
#define __POSTSCRIPT_H__

#include <vector>
#include <iostream>
#include <string>
#include <stdexcept>

#define PS_BEGIN "%%%BEGIN"
#define PS_END "%%%END"

template<class T>
std::vector<Line2D<T> > parsePostScript(std::istream& input){
	std::string lineStr;
	while(lineStr != PS_BEGIN){
		std::getline(input, lineStr);
	}
	std::vector<Line2D<T> > res;
	while(!input.eof() && input.peek()!=((int) '%')){
		Line2D<T> line;
		int x1, x2, y1, y2;
		input >> x1 >> y1 >> x2 >> y2 >> lineStr;
		Point2D<T> pt1(x1, y1), pt2(x2, y2);
		if(input.peek() == 10){
			char c;
			input >> c;
		}
		if(lineStr != "Line"){
			throw std::runtime_error("Line must end with \"Line\"");
		}
		line.add(pt1);
		line.add(pt2);
		// std::cout << "Added " << pt1 << ", " << pt2 << std::endl;
		// std::cout << "Peek: " << input.peek() << std::endl;
		res.push_back(line);
	}
	if(input.eof()){
		throw std::runtime_error("Unexpected end-of-file");
	}
	// std::getline(input, lineStr);
	// if(lineStr != PS_END){
	// 	throw std::runtime_error("File must end with \""+std::string(PS_END)+"\" found: "+lineStr);
	// }
	return res;
}

#endif // __POSTSCRIPT_H__