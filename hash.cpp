/*_____________________________________________________________.
                                                               |
                                               H A S H . C P P |
_(c)_2006_Andrew_Wyld_________________________________________*/

#include "hash.h"
#include <iostream>

/*_____________________________________________________________.
                                                        cHash */

/*template <class T> void cHash<T>::null(){
  //nulls the data element.  for overriding.
  data=0;
}*/

template <class T> inline void cHash<T>::null(){
	data=0;
}

template <class T> cHash<T>::cHash(){
	null();
	next=0;
	index=0;
	maxIdx=0;
	hashSpan=0;
}														

template <class T> cHash<T>::cHash(int i){
	null();
	next=0;
	index=i;
}														

//destructor
template <class T> cHash<T>::~cHash(){
	if(0!=next){
		cHash<T>* temp0=next;
		cHash<T>* temp1=temp0;
		while(temp0){
			temp1=temp0->next;
			temp0->next=0;
			delete temp0;
			temp0=temp1;
			}
		}
	}

template <class T> cHash<T>* cHash<T>::seek(int i){
	cHash<T>* temp=this;
	while(temp){
		if(temp->index==i) return temp;
		if(temp->index>i) return 0;
		temp=temp->next;
	}
	return 0;
}

template <class T> cHash<T>* cHash<T>::seekOrLower(int i){
	cHash<T>* temp=this;
	while(temp){
		if((temp->index==i)||(temp->index>i)||(temp->next==0)){
			return temp;
		}
	}
	return 0;
}

template <class T> cHash<T>::cHash(cHash<T> *prev, int i){
	null();
	next=prev->next;
	prev->next=this;
	index=i;
}

template <class T> cHash<T>* cHash<T>::logbin(){
	//not defined for arbitary T
	}

template <class T> void cHash<T>::display(){
	//not defined for arbitary T
	}

cHash<double>* cHash<double>::logbin(){
	cHash<double>* ret=0;
	int logDex;
	cHash<double>* temp=this;

	if(0==(ret=new cHash<double>)) return 0;

	while(temp){
		logDex=twoabove(temp->index);
		if(0!=(ret->addData(logDex, temp->data/logDex))){
			delete ret;
			return 0;
		}
		temp=temp->next;
	}
	return ret;
}

template <class T> void cHash<T>::normalize(){
	cHash* temp=this;
	T total=0;
	while(temp){
		total+=temp->data;
		temp=temp->next;
	}
	temp=this;
	while(temp){
		temp->data=temp->data/total;
		temp=temp->next;
	}
	return;
}

void cHash<double>::display(){
	//
}

void cHash<cHash<double> >::normalize(){
	cHash<cHash<double> >* temp=this;

	while(temp){
		temp->data.normalize();
		temp=temp->next;
	}
	return;
}

/* */

cHash<double>* cHash< cHash<double>* >::getData(int i){
	cHash< cHash<double>* >* temp=this;
	while((0!=temp)){
		if(temp->index==i) return temp->data;
		if((temp->next==0)||(temp->next->index>i)){
			if (0==new cHash< cHash<double>* >(temp, i)) return 0;
			if (0==(temp->next->data=new cHash<double>)){
				delete temp->next;
				return 0;
			}
			return temp->next->data;
		}
		temp=temp->next;
	}
	return 0;
}

int cHash< cHash<double>* >::addData(int i, cHash<double>* newdata){
	cHash< cHash<double>* >* ddtemp=this;
	cHash<double>* dtemp=newdata;
	while(0!=ddtemp){
		if(ddtemp->index==i) break;
		if((ddtemp->next==0)||(ddtemp->next->index>i)){
			new cHash<cHash<double>*>(ddtemp, i, newdata);
			return 0;
			}
		ddtemp=ddtemp->next;
		}
	*(ddtemp->data)+=*newdata;
	return 0;
	}


cHash<cHash<double>* >* cHash<cHash<double>* >::logbin(){
	cHash<cHash<double>* >* ret=0;
	int label=1;
	cHash<cHash<double>* >* supertemp=this;
	cHash<double> * temp=this->data;

	if(0==(ret=new cHash<cHash<double>* >)) return 0;
	if(0==(ret->data=new cHash<double> )){
		delete ret;
		return 0;
	}

	while(supertemp){
		if( 0==( ret->data=supertemp->data->logbin() ) ){
			delete ret;
			return 0;
		}
		supertemp=supertemp->next;
	}
	return ret;
}

/*int cHash<cHash<double>* >::addData(int i, int j, double newdata){
	cHash<cHash<double>*>* supertemp=this;
    while(0!=temp){
	if(temp->index==i) break;
		if((temp->next==0)||(temp->next->index>i)){
			temp=new cHash<cHash<double>*>(temp, i);
			break;
			}
		temp=temp->next;
		}
	temp->data->addData(j, newdata);
	return 0;
	}*/


/* */
int twoabove(int n){
	int i=1;
	while(i<n){
		i*=2;
		if(i<=0) return -1;
	}
	return i;
}

int twoaboveidx(int n){
	int i=1;
	int j=0;
	while(i<n){
		i*=2;
		++j;
		if(i<=0) return -1;
	}
	return j;
}

