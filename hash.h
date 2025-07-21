#include "ikawdefs.h"

#ifndef _INC_IKAW_HASH
#define _INC_IKAW_HASH
//#define __IKAW_EDGEDIST


class cHash_abstract{

public:

	virtual cHash_abstract* logbin()=0;

};

#ifndef IKAW_CHASH
/*_____________________________________________________________.
                                                        cHash */
// parent hash class
template <class T> class cHash : public cHash_abstract{

friend class cNet;

public:
	void null();
	cHash<T>();
	cHash<T>(int i);
	cHash<T>(cHash<T>* prev, int i);
	~cHash<T>();

	cHash<T>& operator += (const cHash<T>& arg){
		const cHash<T>* temp=&arg;
		while(temp!=0){
			addData(temp->getIndex(), temp->getData());
			temp=temp->next;
		}
	}

	cHash<T>* logbin();
	void display();
	cHash<T>* seek(int i);
	cHash<T>* seekOrLower(int i);

	cHash<T>(cHash<T>* prev, int i, T input){
		next=prev->next;
		prev->next=this;
		index=i;
		data=input;
		};

	inline int getMaxIdx() const {return maxIdx;}

	int setData(int i, T newdata){
		cHash<T>* temp=this;
		if(i>maxIdx) maxIdx=i;
		while(0!=temp){
			if(temp->index==i) break;
			if((temp->next==0)||(temp->next->index>i)){
				new cHash<T>(temp, i, newdata);
				hashSpan++;
				return 0;
				}
			temp=temp->next;
			}
		temp->data=newdata;
		return 0;
		};

	int addData(int i, T newdata){
		cHash<T>* temp=this;
		if(i>maxIdx) maxIdx=i;
		while(0!=temp){
			if(temp->index==i) break;
			if((temp->next==0)||(temp->next->index>i)){
				new cHash<T>(temp, i, newdata);
				hashSpan++;
				return 0;
				}
			temp=temp->next;
			}
		temp->data+=newdata;
		return 0;
		};

	T getData(int i){
		cHash* temp=this;
		while(0!=temp){
			if(temp->index==i) break;
			if(temp->index>i) return 0;
			if(temp->next==0) return 0;
			temp=temp->next;
			}
		return temp->data;
		};

	T getData() const{
		return data;
	}

	T getData(int I, bool enth){
		cHash<T>* temp=this;

		if(!enth) return getData(I);
		for(int i=0; i<I; i++){
			if(temp->next==0) return data;
			temp=temp->next;
		}

		return temp->data;
	}

	int getHashSpan(){
		return hashSpan;
	}

	void normalize();

	void scale(T scalefactor);

	void linscale();

private:

	static cHash* head;
	static cHash* logbinptr;

	int getIndex() const {
		return index;
	}

	int index;
	int maxIdx;
	T data;
	cHash* next;
	cHash* prev;
	int hashSpan;

};

/*template <> class cHash<cHash<double>*>{
public:
	int cHash<cHash<double>* >::addData(int i, int j, double newdata);
};*/

template <class T> void cHash<T>::scale(T scalefactor){
	cHash* temp=this;
	while(temp){
		temp->data*=scalefactor;
		temp=temp->next;
	}
}

template <class T> void cHash<T>::linscale(){
	cHash* temp=this;
	while(temp){
		temp->data*=(T)temp->index;
		temp=temp->next;
	}
}

typedef cHash<double> cDoubleHash;
typedef cHash<int> cIntHash;

#define IKAW_CHASH
#endif

int twoabove(int n);
int twoaboveidx(int n);
/**/
#endif


