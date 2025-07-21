#ifndef _INC_BVRPLT
#define _INC_BVRPLT

#define DIMSTRING "pt"

#include "dlist.h"
#include "ikawdefs.h"
#include <math.h>
#include <iostream>

double twoth(int exponent);

class doubledata{

public:
	
	doubledata(){
		data=0.0;
	}

	doubledata(const double& newdata){
		data=newdata;
	}

	doubledata(const double& newdata, const double& newnormfactor){
		data=newdata;
		normfactor=newnormfactor;
	}
	
	doubledata(const double& newdata, const double& newnormfactor, const double& newnegnormfactor){
		data=newdata;
		normfactor=newnormfactor;
		negnormfactor=newnegnormfactor;
	}

	inline const doubledata& operator=(const double& arg){
		data=arg;
		return *this;
	}

	inline const doubledata& operator=(const int& arg){
		data=(double)arg;
		return *this;
	}

	inline const doubledata& operator+=(const doubledata& arg){
		data+=arg.data;
		return *this;
	}

	inline const doubledata& operator+=(const double& arg){
		data+=arg;
		return *this;
	}

	inline const doubledata& operator+=(const int& arg){
		data+=(double)arg;
		return *this;
	}

	inline const bool operator>(const doubledata& arg) const {
		if (data>arg.data) return true;
		return false;
	}

	inline doubledata& operator/(const doubledata& arg) const {
		doubledata temp;
		temp.data=data/arg.data;
		return temp;
	}

	inline double getData() const {return data;}

	void plot(const double& newsize, const double& offset);
	void plot();
	void histplot(doubledata widthfactor=1.0);
	
	void setsize(double newsize){size=newsize;}
	void setheight(double newheight){vsize=newheight;}
	void setwidth(double newwidth){hsize=newwidth;}
	void setoffset(double newoffset){offset=newoffset;}

	void setnormfactor(double newnormfactor){normfactor=newnormfactor;}
	void setnegnormfactor(double newnegnormfactor){negnormfactor=newnegnormfactor;}

	double getVspace();
	double getVspace(double factor);
	double getVspace(doubledata factor);

private:

	double data;
	static double size;
	static double hsize;
	static double vsize;
	static double offset;
	static double normfactor;
	static double negnormfactor;
};

std::ostream& operator << (std::ostream& os, const doubledata& d);

template <class T> class bivariate{

public:

	inline void zeroinit();
	bivariate();
	bivariate(int newwidth, int newheight);
	~bivariate();
	
	void plotpreamble(double dim);
	void plotgraph(double dim);
	void plotclose(double dim);
	void plot(double dim);
	void plot(double xdim, double ydim);
	void dimension(int newwidth, int newheight);
	void null();
	void populate(int X, int Y, T data);
	void add(int X, int Y, T data);
	void setXidx(int X, T data);
	void setYidx(int Y, T data);
	void setnormfactor(double newnormfactor){T temp; temp.setnormfactor(newnormfactor);}


private:

	void clean();
	void empty();

	bool protectdata;
	T** grid;
	int width;
	int height;

};

template <class T> inline void bivariate<T>::zeroinit(){
	grid=0;
	width=0;
	height=0;
	protectdata=false;
}

template <class T> bivariate<T>::bivariate<T>(int newwidth, int newheight){
	zeroinit();
	try{
		dimension(newwidth, newheight);
	}
	//or should I let the error percolate up?  hmmm....
	catch(int error){
		if(error==IKAW_OUT_OF_MEMORY) throw IKAW_INIT_FAIL;
	}
}

template <class T> bivariate<T>::~bivariate<T>(){
	if(grid){
		for(int i=0; i<width+1; ++i){
			delete grid[i];
		}
		delete grid;
	}
}

template <class T> void bivariate<T>::empty(){
	if(!grid) return;
	for(int i=0; i<width; ++i){
		delete grid[i];
	}
	delete grid;
}

template <class T> inline void bivariate<T>::clean(){
	empty();
	width=0;
	height=0;
	protectdata=false;
}

template <class T> void bivariate<T>::dimension(int newwidth, int newheight){
	//if dimensioned decide whether to overwrite
	if(grid){
		if(protectdata) throw IKAW_OVERWRITE;
		empty();
	}
	
	//set array size markers
	width=newwidth;
	height=newheight;
	
	//dimension primary array
	grid=new T* [width+1];
	if(!grid) throw IKAW_OUT_OF_MEMORY;
	
	//dimension secondary arrays
	for(int i=0; i<width+1; ++i){
		grid[i]=new T [height+1];
		if(!grid[i]){
			clean();
			throw IKAW_OUT_OF_MEMORY;
		}
	}
	protectdata=true;
	return;
}

template <class T> void bivariate<T>::null(){
	for(int i=0; i<width; i++){
		for(int j=0; j< width; j++){
			populate(i, j, 0);
		}
	}
}

template <class T> void bivariate<T>::populate(int X, int Y, T data){
	grid[X+1][Y+1]=data;
}

template <class T> void bivariate<T>::add(int X, int Y, T data){
	grid[X+1][Y+1]+=data;
}

template <class T> void bivariate<T>::setXidx(int X, T data){
	grid[X+1][0]=data;
}

template <class T> void bivariate<T>::setYidx(int Y, T data){
	grid[0][Y+1]=data;
}


template <class T> void bivariate<T>::plotpreamble(double dim){
	//turn off line spacing
	std::cout << "{\n\\offinterlineskip\n";
	//turn off tab skipping
	std::cout << "\\tabskip=0" << DIMSTRING << std::endl;
	//create template
	std::cout << "\\halign{\n";
	std::cout << "\\hfill # \\hfill & ";
	for(int i=0; i<=width; ++i)
		std::cout << "\\hfill # \\hfill & ";
	std::cout << "\n\\cr\n";
	}

template <class T> void bivariate<T>::plotclose(double dim){
	//close linespaceless region
	std::cout << "}\n}\n";
	}

template <class T> void bivariate<T>::plotgraph(double dim){
	double ptsize=dim/width;
	double minVspace=-1;
	
	if(grid){
	
		grid[0][0].setsize(ptsize);
		grid[0][0].setoffset(0);
		
		//first line:  scale
		for(int X=0; X<=width; ++X){
//			std::cout << "& ";
//			if( (X%5==0) || (X==width-1) ) std::cout << " $" << grid[X][0] << "$ ";
			if(X>0) std::cout << " $" << grid[X][0] << "$ ";
			std::cout << "&";
		}
		
		for(int Y=1; Y<=height; ++Y){
		
			minVspace=grid[1][Y].getVspace();
			for(int X=2; X<=width; ++X){
				minVspace=(minVspace<(grid[X][Y].getVspace()))?minVspace:(grid[X][Y].getVspace());
			}
			std::cout << "\\vspace{" << minVspace << "}\n\\cr\n";
			

			//first column:  scale
//			std::cout << "&";
//			if( (Y%5==0) || (Y==height-1) ) std::cout << " $" << grid[0][Y] << "$ ";
			std::cout << " $" << grid[0][Y] << "$ ";
			std::cout << "\n&";

			for(int X=1; X<=width; ++X){
//				std::cout << "&";
				grid[X][Y].plot();
				std::cout << "&";
			}
			std::cout << "\\vspace{" << minVspace << "}\n";

			if(Y==height) std::cout << "\\cr";
			

		}

	}
	return;
}

template <class T> void bivariate<T>::plot(double dim){
	if(grid){
	
		plotpreamble(dim);
		plotgraph(dim);
		plotclose(dim);

	}
	return;
}

template <class T> class histogram{

public:

	inline void zeroinit();
	histogram();
	histogram(int newsize);
	~histogram();
	
	void plotpreamble(double pltwidth, double pltheight);
	void plotgraph(double pltwidth, double pltheight);
	void plotclose(double pltwidth, double pltheight);
	void plot(double dim);
	void plot(double pltwidth, double pltheight);
	void dimension(int newsize);
	void populate(int X, T width, T height);
	void setnormfactor(double newnormfactor){T temp; temp.setnormfactor(newnormfactor);}


private:

	void clean();
	void empty();

	bool protectdata;
	T* heights;
	T* widths;
	int size;

};

template <class T> inline void histogram<T>::zeroinit(){
	heights=0;
	widths=0;
	size=0;
	protectdata=false;
}

template <class T> histogram<T>::histogram<T>(int newsize){
	zeroinit();
	try{
		dimension(newsize);
	}
	//or should I let the error percolate up?  hmmm....
	catch(int error){
		if(error==IKAW_OUT_OF_MEMORY) throw IKAW_INIT_FAIL;
	}
}

template <class T> histogram<T>::~histogram<T>(){
	if(heights) delete heights;
	if(widths) delete widths;
}

template <class T> void histogram<T>::empty(){
	if(heights) delete heights;
	if(widths) delete widths;
}

template <class T> inline void histogram<T>::clean(){
	empty();
	size=0;
	protectdata=false;
}

template <class T> void histogram<T>::dimension(int newsize){
	//if dimensioned decide whether to overwrite
	if(widths||heights){
		if(protectdata) throw IKAW_OVERWRITE;
		empty();
	}
	
	//set array size markers
	size=newsize;

	//dimension arrays
	widths=new T [size+1];
	if(!widths) throw IKAW_OUT_OF_MEMORY;

	heights=new T [size+1];
	if(!heights){
		clean();
		throw IKAW_OUT_OF_MEMORY;
	}

	protectdata=true;
	return;
}

template <class T> void histogram<T>::populate(int X, const T width, const T height){
	widths[X+1]=width;
	widths[0]+=width;
	heights[X+1]=height;
	if(height>heights[0]) heights[0]=height;
}


template <class T> void histogram<T>::plotpreamble(double pltwidth, double pltheight){
	//turn off line spacing
	std::cout << "{\n\\offinterlineskip\n";
	//turn off tab skipping
	std::cout << "\\tabskip=0" << DIMSTRING << std::endl;
	//create template
	std::cout << "\\halign{\n";
	std::cout << "\\hfill # \\hfill & ";
	for(int i=0; i<=size; ++i)
		std::cout << "\\hfill # \\hfill & ";
	std::cout << "\n\\cr\n";
	}


template <class T> void histogram<T>::plotgraph(double pltwidth, double pltheight){
	double eltwidth=pltwidth/widths[0].getData();
	double minVspace=-1;
	
	if(heights){

		heights[0].setwidth(eltwidth);
		heights[0].setheight(pltheight);
		
		minVspace=heights[1].getVspace(2);
		for(int X=2; X<=size; ++X){
			minVspace=(minVspace<(heights[X].getVspace(2)))?minVspace:(heights[X].getVspace(2));
		}
		std::cout << "\\vspace{" << minVspace << "}\n\\cr\n";
		

		//first column:  scale
//		std::cout << "&";
//		if( (Y%5==0) || (Y==height-1) ) std::cout << " $" << grid[0][Y] << "$ ";
		std::cout << " $" << heights[0] << "$ ";
		std::cout << "\n&";

		for(int X=1; X<=size; ++X){
//			std::cout << "&";
			heights[X].histplot(widths[X]);
			std::cout << "&";
		}

	}
	return;
}

template <class T> void histogram<T>::plotclose(double pltwidth, double pltheight){
	//close linespaceless region
	std::cout << "}\n}\n";
}

template <class T> void histogram<T>::plot(double pltwidth, double pltheight){
	plotpreamble(pltwidth, pltheight);
	plotgraph(pltwidth, pltheight);
	plotclose(pltwidth, pltheight);
	return;
}

#endif
