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
	};

	doubledata(const double& newdata){
		data=newdata;
	};

	doubledata(const double& newdata, const double& newnormfactor){
		data=newdata;
		normfactor=newnormfactor;
	};
	
	doubledata(const double& newdata, const double& newnormfactor, const double& newnegnormfactor){
		data=newdata;
		normfactor=newnormfactor;
		negnormfactor=newnegnormfactor;
	};

	inline const doubledata& operator=(const double& arg){
		data=arg;
		return *this;
	};

	inline const doubledata& operator=(const int& arg){
		data=(double)arg;
		return *this;
	};

	inline double getData() const {return data;}

	void plot(const double& newsize, const double& offset);
	void plot();
	
	void setsize(double newsize){size=newsize;}
	void setoffset(double newoffset){offset=newoffset;}

	void setnormfactor(double newnormfactor){normfactor=newnormfactor;}
	void setnegnormfactor(double newnegnormfactor){negnormfactor=newnegnormfactor;}

	double getVspace();

private:

	double data;
	static double size;
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
	
	void plot(double dim);
	void plot(double xdim, double ydim);
	void dimension(int newwidth, int newheight);
	void populate(int X, int Y, T data);
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

template <class T> void bivariate<T>::populate(int X, int Y, T data){
	grid[X+1][Y+1]=data;
}

template <class T> void bivariate<T>::setXidx(int X, T data){
	grid[X+1][0]=data;
}

template <class T> void bivariate<T>::setYidx(int Y, T data){
	grid[0][Y+1]=data;
}

template <class T> void bivariate<T>::plot(double dim){
	int end;
	double ptsize=dim/width;
	double minVspace=-1;
	
	if(grid){
	
		grid[0][0].setsize(ptsize);
		grid[0][0].setoffset(0);
		
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

		//first line:  scale
		for(int X=0; X<width; ++X){
//			std::cout << "& ";
//			if( (X%5==0) || (X==width-1) ) std::cout << " $" << grid[X][0] << "$ ";
			if(X>0) std::cout << " $" << grid[X][0] << "$ ";
			std::cout << "&";
		}
		
		for(int Y=1; Y<height; ++Y){
		
			minVspace=grid[1][Y].getVspace();
			for(int X=2; X<width; ++X){
				minVspace=(minVspace<(grid[X][Y].getVspace()))?minVspace:(grid[X][Y].getVspace());
			}
			std::cout << "\\vspace{" << minVspace << "}\n\\cr\n";
			

			//first column:  scale
//			std::cout << "&";
//			if( (Y%5==0) || (Y==height-1) ) std::cout << " $" << grid[0][Y] << "$ ";
			std::cout << " $" << grid[0][Y] << "$ ";
			std::cout << "\n&";

			for(int X=1; X<width; ++X){
//				std::cout << "&";
				grid[X][Y].plot();
				std::cout << "&";
			}
			std::cout << "\\vspace{" << minVspace << "}\n";

			if(Y==height) std::cout << "\\cr";
			

		}

		//close linespaceless region
		std::cout << "}\n}\n";
		
	}
	return;
}

#endif
