
#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <set>

#include "geom.h"

template<class T>
T signum(T val){
	if(val == 0){
		return 0;
	}
	if(val < 0){
		return -1;
	}
	return 1;
}

enum Ord
{
	LT, EQ, GT
};

struct Color
{
	int _red, _green, _blue;
};

Ord compare(const Color& first, const Color& second){
	if(&first == &second){
		return EQ;
	}
	int as[] = {first._red, first._green, first._blue};
	int bs[] = {second._red, second._green, second._blue};
	for(int i=0; i<3; i++){
		int a=as[i];
		int b=bs[i];
		if(a > b){
			return GT;
		}
		if(a < b){
			return LT;
		}
	}
	return EQ;
}

bool operator==(const Color& first, const Color& second){
	return compare(first, second) == EQ;
}

bool operator!=(const Color& first, const Color& second){
	return compare(first, second) != EQ;
}

bool operator>(const Color& first, const Color& second){
	return compare(first, second) == GT;
}

bool operator<(const Color& first, const Color& second){
	return compare(first, second) == LT;
}

bool operator>=(const Color& first, const Color& second){
	Ord cmp = compare(first, second);
	return cmp == EQ || cmp == GT;
}

bool operator<=(const Color& first, const Color& second){
	Ord cmp = compare(first, second);
	return cmp == EQ || cmp == LT;
}

#define disp(x) #x << " = " << (x)
#define assert(msg, x) do{if(!(x)){throw std::runtime_error(std::string("Assertion error: ")+msg);}}while(0)
#define assertEq(a,b) assert(toString(a)+" should equal "+toString(b), (a) == (b))

template<class T>
std::string toString(T val){
	std::stringstream strm;
	strm << val;
	return strm.str();
}

class Image{
private:
	const int _width, _height;
	Color **_raster;

	void scan(Point2D<int>& start, Point2D<int>& end, Color col){
		int dx = end.getX() - start.getX();
		int dy = end.getY() - start.getY();
		if(dx == 0){
			for(int y=start.getY(); y!=end.getY(); y+=signum(dy)){
				setPixel(y, start.getX(), col);
			}
			return;
		}
		int a=2*dy, b=-2*dx;
		int scanner = 0;
		int choice = 1;
		int incr;
		int dir;
		int c=2*(dx*start.getY() - dy*start.getX());
		// Integer division in C truncates towards 0
		if(dx/dy==0){ // -1<=m<1 : (x+1, y+-1/2)
			incr = 1;
			dir = signum(dy*dx);
			c+=2*dy-dir*dx;
		}else{ // m >= 1 : (x+1/2, y+1)
			std::swap(scanner, choice);
			incr = signum(dy*dx);
			dir = 1;
			c+=dy-2*incr*dx;
		}
		if(a < 0){
			a = -a;
			b = -b;
			c = -c;
		}
		std::cout << "Drawing from " << start << " to " << end << std::endl;
		std::cout << disp(dx) << ", " << disp(dy) << std::endl;
		std::cout << disp(a) << ", " << disp(b) << ", " << disp(c) << std::endl;
		while(start != end){
			setPixel(start, col);
			std::cout << "Drew " << start << std:: endl;
			if(a*start.getX() + b*start.getY() + c < 0){
				start.get(choice,0) += dir;
			}
			start.get(scanner, 0) += incr;
		}
		setPixel(end, col);
	}
public:
	Image(int width, int height) : _width(width), _height(height){
		_raster = new Color*[height];
		for(int i=0; i<height; i++){
			_raster[i] = new Color[width];
		}
	}
	Image(const Image& copy) : _width(copy._width), _height(copy._height){
		_raster = new Color*[_height];
		for(int i=0; i<_height; i++){
			_raster[i] = new Color[_width];
			for(int j=0; j<_width; j++){
				_raster[i][j] = copy._raster[i][j];
			}
		}
	}
	Image(Image&& move) : _width(move._width), _height(move._height){
		_raster = move._raster;
		move._raster = nullptr;
	}
	~Image(){
		for(int i=0;i<_height; i++){
			delete[] _raster[i];
		}
		delete[] _raster;
	}
	Image& operator=(Image other){
		if(other._width != _width || other._height != _height){
			throw std::runtime_error("Images must be the same size for assignment");
		}
		std::swap(_raster, other._raster);
		return *this;
	}
	bool inBounds(Point2D<int> pt){
		return inBounds(pt.getY(), pt.getX());
	}
	bool inBounds(int row, int col){
		return row >= 0 && col >= 0 && row < _height && col < _width;
	}
	void fill(Color col){
		for(int i=0; i<_height; i++){
			for(int j=0; j<_width; j++){
				_raster[i][j] = col;
			}
		}
	}
	Color getPixel(int row, int col) const{
		return _raster[row][col];
	}
	void setPixel(Point2D<int> pt, Color color){
		if(!inBounds(pt)){
			std::stringstream strm;
			strm << "Pixel " << pt << " is out of bounds";
			throw std::runtime_error(strm.str());
		}
		setPixel(pt.getY(), pt.getX(), color);
	}
	void setPixel(int row, int col, Color color){
		_raster[row][col] = color;
	}
	int getWidth() const{
		return _width;
	}
	int getHeight() const{
		return _height;
	}
	std::set<Color> getColors() const{
		std::set<Color> res;
		for(int i=0; i<_height; i++){
			for(int j=0; j<_width; j++){
				res.insert(_raster[i][j]);
			}
		}
		return res;
	}
	void drawLine(Point2D<int> start, Point2D<int> end, Color col){
		scan(start, end, col);
	}
	void drawLine(Line2D<int> line, Color col){
		auto end = line.end();
		auto prev = line.begin();
		auto curr = line.begin();
		curr++;
		while(curr!=end){
			drawLine(*prev, *curr, col);
			prev++;
			curr++;
		}
	}
};

#endif // __GRAPHICS_H__