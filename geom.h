
#ifndef __GEOM_H__
#define __GEOM_H__

#include <ostream>
#include <vector>
#include "linear.h"

template<class T>
class Point2D : public Matrix<T, 2, 1>
{
public:
	Point2D() {
	}
	Point2D(const T x, const T y){
		setX(x);
		setY(y);
	}
	Point2D(const Point2D<T>& copy) : Point2D(){
		setX(copy.getX());
		setY(copy.getY());
	}
	Point2D<T>& operator=(Point2D<T> other){
		setX(other.getX());
		setY(other.getY());
		return *this;
	}
	T getX() const{
		return this->get(0,0);
	}
	void setX(const T val) {
		this->get(0,0) = val;
	}
	T getY() const{
		return this->get(1,0);
	}
	void setY(const T val) {
		this->get(1,0) = val;
	}
};

template<class T>
std::ostream& operator<<(std::ostream& out, Point2D<T> pt){
	out << '(' << pt.getX() << ',' << pt.getY() << ')';
	return out;
}

template<class T>
class Line2D {
private:
	std::vector<Point2D<T> > _pts;
public:
	Line2D(){}
	Line2D(const Line2D<T>& copy){
		_pts = copy._pts;
	}
	void add(Point2D<T> point){
		_pts.push_back(point);
	}
	auto begin(){
		return _pts.begin();
	}
	auto end(){
		return _pts.end();
	}
};

#endif // __GEOM_H__