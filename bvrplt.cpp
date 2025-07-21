#include "bvrplt.h"

double doubledata::size;
double doubledata::hsize;
double doubledata::vsize;
double doubledata::offset;
double doubledata::normfactor;
double doubledata::negnormfactor;

double twoth(int exponent){
	double twain=1.0;
	for(int i=0; i<exponent; ++i) twain *= 2.0;
	return twain;
}

std::ostream& operator << (std::ostream& os, const doubledata& d){
	os << d.getData();
	return os;
}

double doubledata::getVspace(){
	return size*(1-sqrt(data/normfactor))/2.0;
}

double doubledata::getVspace(double factor){
	return factor*size*(1-sqrt(data/normfactor))/2.0;
}

double doubledata::getVspace(doubledata factor){
	return factor.data*size*(1-sqrt(data/normfactor))/2.0;
}

void doubledata::plot(){
	double width = size*sqrt(data/normfactor);
	double height=width/2.0+offset;
	double depth=width/2.0-offset;
	double hspace=(size-width)/2.0;
	std::cerr << width << ", " << data << std::endl;
	std::cout << std::fixed << "\\hspace{" << hspace << DIMSTRING << "}"
		<< "\\vrule height " << height << DIMSTRING
		<<			" width " << width << DIMSTRING
		<<			" depth " << depth << DIMSTRING
		<< " \\hspace{" << hspace << DIMSTRING << "}" << std::endl;
}

void doubledata::histplot(doubledata widthfactor){
	double width = hsize*widthfactor.getData();
	double height = vsize*data/normfactor + offset;
	std::cerr << height << ", " << data << std::endl;
	std::cout << std::fixed << "\\vrule height " << height << DIMSTRING
			<<	" width " << width << DIMSTRING
			<<	" depth " << -offset << DIMSTRING
			<< std::endl;
}

void doubledata::plot(const double& newsize, const double& newoffset){
	setsize(newsize);
	setoffset(newoffset);
	plot();
}

