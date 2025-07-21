#ifndef _INC_DLIST
#define _INC_DLIST

//linked list template

//______________________________________________________________
// delement

template <class T> class delement{

	public:
	
	delement<T>(T* targetObj);
	~delement<T>();
	void isolate();
	void insert(delement* insertand);

	private:

	void delink();
	
	delement<T>* prev;
	delement<T>* next;
	T* storedObj;
	
};

//PUBLIC

template <class T> delement<T>::delement<T>(T* targetObj){
	storedObj=targetObj;
	prev=0;
	next=0;
}

template <class T> delement<T>::~delement<T>(){
	if(prev) prev->next=next;
	if(next) next->prev=prev;
}
	
template <class T> void delement<T>::isolate(){
	prev=0;
	next=0;
}
	
template <class T> void delement<T>::insert(delement* insertand){
	insertand->delink();
	insertand->next=next;
	next->prev=insertand;
	next=insertand;
	insertand->prev=this;
}

//PRIVATE

template <class T> void delement<T>::delink(){
	if(prev) prev->next=next;
	if(next) next->prev=prev;
}

//______________________________________________________________
// dlist

template <class T> class dlist{

	public:

	dlist(){
		head=new delement;
		tail=head;
		sortTail=head;
		listLen=0;
//		sortListLen=0;
	}
	
	~dlist(){
		delete head;
	}
	
	void append(delement<T>* appendum){
		appendum=tail->next;
		appendum->prev=tail;
		tail=appendum;
		listLen++;
		sorted=false;
	}
	
	void append(T* appendum){
		appendElt=new delement<T>(appendum);
		append(appendElt);
	}
	
	void join(dlist* source){
		append(source->head->next);
		source->head->next=0;
		listLen += source.listLen;
		tail=source->tail;
		delete source;
		source=this;
	}
	
	void sort();

	void merge(dlist* source){
		delement<T>* temp0=head;
		
		//make the lists ordered
		sort();
		source->sort();
		
		while(temp0->next&&source->head->next){
			switch temp0->next->ptr->cmp(source->head->next->ptr){
				
				case 1://temp0->next greater
					//merge in source->head->next to temp0
					temp0->insert(source->head->next);
					break;
				
				case -1://source->head->next greater
					//advance temp0
					temp0=temp0->next;
					break;
				
				case 0://two clusters with the same pointer!?
					std::cerr << "unlinked clusters with same element\n";
					//fall through to error
				
				default://some kind of error.
					std::cerr << "matching error\n";
			}
		}
	}
	
	void cleanse();
	
	private:

	//merge assumes two lists are ordered.

	bool sorted;
	int listLen;
//	int sortListLen;
	delement<T>* head;
	delement<T>* tail;
//	delement* sortTail;

};

template <class T> int tCmp(const void* elem1, const void* elem2){
	return ( (T*) elem1 )->cmp( (T*) elem2 );
}

template <class T> void dlist<T>::sort(){

	if(sorted) return;
	//array of pointers to T objects
	int I=listLen-sortListLen;
	delement** sortand=new (delement*)[I];
	delement* temp=sortTail->next;

	//transfer unsorted linked-list objects to the array
	for(int i=0; i<listLen-sortListLen; ++i){
		sortand[i]=temp;
		temp=temp->next;
		sortand[i]->isolate();
	}
	
	//quicksort the objects using the sort function
	std::qsort( (void*)sortand, listLen-sortListLen,
			 sizeof(delement*), tCmp<T> );
	temp=head;

	//merge the array with the list again.
	for(int i=0; i<I; ++i){
		int disparity=1;
		//wait till sortand[i]'s Ptr eq/exceeds temp's
		while(
			temp->next &&
			( ( disparity=cmpPtr( (sortand[i]), (temp->next) ) ) > 0 )
			){
			temp=temp->next;
		}

		if(0==disparity){
			//linked pointers.	make to sublist.
			for(int j=0; j<(sortand[i])->ptrs.size(); ++j){
				temp->next->ptrs.push_back((sortand[i])->ptrs[j]);
			}
			delete sortand[i];
			sortand[i]=0;
			continue;
		}
		else{
			temp->link(sortand[i]);
		}
	}
	delete sortand;
	sortListLen=listLen;
	sorted=true;
}

template <class T> void dlist<T>::cleanse(){
	delement* temp=head->next;
	if(!temp->next) return;

	while(temp->next){
		while(!temp->cmp(temp->next)){
			delete temp->next;
			if(!temp->next) return;
		}
		temp=temp->next;
	}
}

#endif