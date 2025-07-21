#ifndef _INC_NET
#define _INC_NET

#include "node.h"
#include "bvrplt.h"
#include <string.h>

/*______________________________________________________________
                                                 D E F I N E S |
______________________________________________________________*/

#define IKAW_NETSIZE 4096
#define IKAW_NETP 0.5
#define IKAW_NETM 4
#define IKAW_NETS 2




/*_____________________________________________________________.
                                                         cNet */
class cNet {

public:

	cNet();
	~cNet();

	void displayMode(){std::cerr << "mode " << vMode << std::endl;}


	int reset();

	void unflag();

	int init(int argc, char * argv[]);
	int parse(int argc, char * argv[]);

	int vertexArray();

	int cumlStore();
	int cumlNetDump();

	int dump(int iteration);

	int run();
	
	int choose();

	bool mode(int comp);

	int distDistFn();
	int degDistFn();
	int distDistDisplay();
	int degDistDisplay();
	int clustCoeff();


	//TIME!
	double time;
	
	//number of vertices
	int size;
	double _1_N2;

	//probability as double
	double fp;

	//probability range as double
	double fminp;
	double fmaxp;

	//probability as int (based on rand() values)
	int ip;

	//probability range as int
	int iminp;
	int imaxp;

	int psteps;
	
	//unit of sqrtable goodness
	//double unit;

	// THINK ABOUT:
	// double versions of int variables for use in double arith

	//total edge strength
	//int edgeTotalStrength;
	//int edgeTotalWeakness;

	//array of vertices
	cVertex * vertices;

	//pointers for general use
	cVertex * vertex0;
	cVertex * vertex1;
	cEdge * edge0;
	cEdge * edge1;

	//output data hash
	cDoubleHash* hash;
	cHash<cDoubleHash*>* ddfnhash;
	cHash<cDoubleHash*>* degdhash;
	cHash<cDoubleHash*>* pchash;
	cDoubleHash* ccfthash;
	cDoubleHash* clustsuc;
	cDoubleHash* currentHash;
	cDoubleHash* idxHash;
	int hashIndex;

	int numNod;
	int spare;
	int verbose;
	int runtime;
	int dumpInterval;
	int dumpWait;
	int vMode;

//vector depth
	int m;
	double fm;

//vector depth range	
	int minm;
	int maxm;
	double fminm;
	double fmaxm;

//vector distribution
	int s;
	double fs;

//vector distribution range
	int mins;
	int maxs;
	double fmins;
	double fmaxs;

//difference (for speed)
	int m_s;
	double fm_s;
	double f1_m_s;

//repetitions for aggregate data
	int repeat;

//output print size
	double printsize;


	//some kind of counter
	int score;
	int disruptRedundancy;
	int disruptEdgesKilled;
	int disruptClusterSize;

	FILE* outStream;
	
private:

	void zeroinit();
	int runas_bivar_pc();
	int runas_bivar_ps();
	int runas_bivar_ms();
	int runas_progression();
	int runas_normal();

};

#endif