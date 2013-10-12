#include <stdio.h>
#include <string.h>

#include <stdlib.h>
#include <time.h>

typedef struct{
	int x;
	int y;
} point_t;

typedef struct{
	point_t pt;
	int rsteps;
} node_t;


//1. UTILITY FUNCTIONS - START
void cutline(char cutstr[])
{
	int linebreakpos = 0;
	int terminate = 0;

	int lenstr = strlen(cutstr);

	while(!terminate)
	{
		if (cutstr[linebreakpos] == '\n')
			terminate = 1; // END OF LINE
		if (cutstr[linebreakpos] == '\0')
			terminate = 2; // END OF STRING

		if (linebreakpos > lenstr)
			printf("CUTLINE ERROR");

		linebreakpos++;
	}

	if (terminate == 2)
	{
		cutstr[0] = '\0';
		return;
	}

	int i=0;
	for (i=linebreakpos; i<lenstr+1; i++)
	{
		cutstr[i-linebreakpos] = cutstr[i];
	}

}


int ptsequal(point_t pt1, point_t pt2)
{
	if ((pt1.x == pt2.x) && (pt1.y == pt2.y))
		return 1;
	return 0;
}


//1. UTILITY FUNCTIONS - END





//2. FILE FUNCTIONS START
int stringvarseek(char filetext[], char varname[])
{
	int i=0;

	int curchar = 0;
	int charsverified = 0;
	for (i=0; i<strlen(filetext); i++)
	{
		if (filetext[i] == varname[charsverified])
			charsverified++;
		else
		{
			charsverified = 0;
			if (filetext[i] == varname[charsverified])
				charsverified++;
		}
		
		if (charsverified == strlen(varname))
		{
			curchar = i; // extract current character
			i = strlen(filetext); // break loop
		}
	}

	//         * current char
	// "variable = 56\n"

	i=0;
	if ((filetext[curchar+1] == ' ') && // space equals space number
		(filetext[curchar+2] == '=') &&
		(filetext[curchar+3] == ' '))
	       //((filetext[curchar+4] >= '0') && (filetext[curchar+4] <= '9')))
			return curchar+4; // return position of number
	else
		return -1; // invalid
}


int intvarread(char filetext[], char varname[], int *readint)
{
	int curchar = 0;
	curchar = stringvarseek(filetext, varname);
	
	if (curchar == -1) // ERROR
	{
		printf("ERROR READING VARIABLE %s\n", varname);
		*readint = 0;
		return 1; // error
	}

	char numstr[21] = ""; // Max 20 characters for number

	int i=0;
	while((filetext[curchar+i] != '\n') && (filetext[curchar+i] != '\n') && (i<20)) 
	{
		numstr[i] = filetext[curchar+i];
		i++;
	}
	numstr[i] = '\0';

	*readint = atoi(numstr);
	return 0; // no problem
}


int floatvarread(char filetext[], char varname[], float *readfloat)
{
	int curchar = 0;
	curchar = stringvarseek(filetext, varname);
	
	if (curchar == -1) // ERROR
	{
		printf("ERROR READING VARIABLE %s\n", varname);
		*readfloat = 0;
		return 1; // error
	}

	char numstr[21] = ""; // Max 20 characters for number

	int i=0;
	while((filetext[curchar+i] != '\n') && (filetext[curchar+i] != '\n') && (i<20)) 
	{
		numstr[i] = filetext[curchar+i];
		i++;
	}
	numstr[i] = '\0';

	*readfloat = atof(numstr);
	return 0; // no problem
}

int stringvarread(char filetext[], char varname[], char readstring[])
{
	int curchar = 0;
	curchar = stringvarseek(filetext, varname);
	
	if (curchar == -1) // ERROR
	{
		printf("ERROR READING VARIABLE %s\n", varname);
		readstring = "maze"; // default value
		return 1; // error
	}

	int i=0;
	while((filetext[curchar+i] != '\n') && (filetext[curchar+i] != '\n') && (i<20)) 
	{
		readstring[i] = filetext[curchar+i];
		i++;
	}
	readstring[i] = '\0';

	return 0; // no problem
}


void readfile(char filetext[], char filename[])
{
	FILE *filep;

	filep = fopen(filename, "r");

	fseek( filep, 0, SEEK_END);
	long fsize = ftell( filep);
	rewind( filep);

	//printf("%d\n", fsize);
	fread( filetext, 1, fsize, filep);
	filetext[fsize] = '\0';
}


void loadgridfromfile(char filename[], char sbse[],
		point_t *startpoint, point_t *endpoint, char grid[256][256],
		int *gridwidth, int *gridlines)
{
	// Map variables
	char filetext[65536];

	readfile(filetext, filename);

	*gridwidth = 0;
	
	while(*gridwidth<256)
	{
		int cont = 0;
		int i=0;

		for (i=0; i<4; i++)
			if (filetext[*gridwidth] == sbse[i]) cont = 1;

		if (!cont) break;
	
		*gridwidth += 1;
	}

	// lines
	//
	int i=0;

	*gridlines = 0;

	{
		char filetextread[65536];
		for (i=0; i<strlen(filetext); i++)
			filetextread[i] = filetext[i];

		
		int i2=0;
		while (i2 != -1)
		{
			for (i=0; i<*gridwidth; i++)
			{
				if (filetextread[i] == sbse[0]) grid[i2][i] = 0;
				else if (filetextread[i] == sbse[1]) grid[i2][i] = 1;
				else if (filetextread[i] == sbse[2])
				{	
					grid[i2][i] = 0;
					startpoint->x = i;
					startpoint->y = i2;
				}
				else if (filetextread[i] == sbse[3])
				{
					grid[i2][i] = 0;
					endpoint->x = i;
					endpoint->y = i2;
				}
	
			}

			cutline(filetextread);
			i2++;
			if (filetextread[0] == '\0')
			{
				*gridlines = i2;
				i2 = -1;
			}
		}
	}
}


void writefile(char filetext[], char filename[])
{
	FILE *filep;

	char filename2[100];
	int i=0;
	for (i=0; i<=strlen(filename); i++)
		filename2[i] = filename[i];

	filep = fopen(strcat(filename2,".txt"), "w");

	if (filep == NULL)
		printf("File open error\n");

	//printf("%s\n", filetext);

	fprintf(filep, "%s", filetext);
	
	fclose(filep);
}


void writegridtofile(char sbse[4], point_t startpoint, point_t endpoint, char grid[256][256],
		int gridwidth, int gridlines, char filename[])
{
	char filetext[65536] = "";

	int ix=0; int iy =0;

	for (iy=0; iy<gridlines; iy++)
	{
		for (ix=0; ix<gridwidth; ix++)
		{
			char charconcat[2] = " ";

			if ((startpoint.x == ix) && (startpoint.y == iy))
			{
				charconcat[0] = sbse[2];
			}
			else if ((endpoint.x == ix) && (endpoint.y == iy))
			{
				charconcat[0] = sbse[3];
			}
			else if (grid[iy][ix] == 1) // Block
			{
				charconcat[0] = sbse[1];
			}
			else // (grid[iy][ix] == 0) // Space
			{
				charconcat[0] = sbse[0];
			}
			
			strcat(filetext,charconcat);
		}

		if (iy < gridlines-1)
			strcat(filetext,"\n");
	}

	writefile(filetext, filename);
}



void loadparameters(char configfilename[], 
	int* gridwidth,
	int* gridlines,
	
	float* blockratio,
	int* minsteps,
	float* minjump,

	int* trapallowed,

	int* maxloops,
	int* maxretries,
	int* filestogenerate,
	char filename[])
{
	/*
	int gridwidth = 20;
	int gridlines = 15;
	
	float blockratio = 0.15;
	int minsteps = 25;
	float minjump = 30;

	int trapallowed = 0;

	int maxloops = 100;
	int maxretries = 100000;
	int filestogenerate = 10;
	char filename[30] = "maze";*/


	char filetext[65536];
	readfile(filetext, configfilename);

	int errors = 0;

	errors += intvarread(filetext, "gridwidth", gridwidth);
	errors += intvarread(filetext, "gridlines", gridlines);
	
	errors += floatvarread(filetext, "blockratio", blockratio);
	errors += intvarread(filetext, "minsteps", minsteps);
	errors += floatvarread(filetext, "minjump", minjump);

	errors += intvarread(filetext, "trapallowed", trapallowed);

	errors += intvarread(filetext, "maxloops", maxloops);
	errors += intvarread(filetext, "maxretries", maxretries);
	errors += intvarread(filetext, "filestogenerate", filestogenerate);
	
	errors += stringvarread(filetext, "filename", filename);

	printf("CONFIG FILE ERROR COUNT: %d\n", errors);

	printf("gridwidth = %d\n", *gridwidth);
	printf("gridlines = %d\n", *gridlines);
	printf("blockratio = %f\n", *blockratio);
	printf("minsteps = %d\n", *minsteps);
	printf("minjump = %f\n", *minjump);
	printf("trapallowed = %d\n", *trapallowed);
	printf("maxloops = %d\n", *maxloops);
	printf("maxretries = %d\n", *maxretries);
	printf("filestogenerate = %d\n", *filestogenerate);
	printf("filename = %s\n", filename);

	if (errors == 10)
	{
		printf("UNABLE TO READ CONFIG FILE. GENERATING GENCONFIG.TXT");
		
		writefile("gridwidth = 20\ngridlines = 15\n\nblockratio = 0.15\nminsteps = 25\nminjump = 22.5\n\ntrapallowed = 0\n\nmaxloops = 100\nmaxretries = 100000\n\nfilestogenerate = 5\nfilename = maze", "genconfig");

	}
}


//2. FILE FUNCTIONS - END







//3. MAZE ANALYSIS FUNCTIONS - START

int getnodenum(point_t nodepos, node_t node[], int numnodes)
{
	int nodenum = -1;
	
	int i=0;
	for (i=0; i<numnodes; i++)
		if ((nodenum == -1) && (ptsequal(nodepos, node[i].pt)))
			nodenum = i;

	return nodenum;
}




void printnodemap(node_t node[], int numnodes, char grid[256][256], int gridwidth, int gridlines)
{

	int iy=0;
	int ix=0;

	char rstepprint[36] = {'0','1','2','3','4','5','6','7','8','9',
				'A','B','C','D','E','F','G','H','I','J',
				'K','L','M','N','O','P','Q','R','S','T',
				'U','V','W','X','Y','Z'};

	printf("\n");

	for (iy=0; iy<gridlines; iy++)
	{
		for (ix=0; ix<gridwidth; ix++)
		{
			point_t xy;
			xy.x = ix;
			xy.y = iy;

			int i=0;

			int nodenum = getnodenum(xy, node, numnodes);
			
			/*int nodenum = -1;
	
			for (i=0; i<numnodes; i++)
				if (ptsequal(xy, node[i].pt))
					nodenum = i;*/

			if (nodenum == -1)
			{
				if (grid[iy][ix] == 0)
					printf(" ");
				else
					printf("#");
			}
			else
			{
				if (node[nodenum].rsteps == -1)
					printf("!");
				else
					printf("%c", rstepprint[node[nodenum].rsteps]);
			}
		}
		printf("\n");
	}
}










point_t dirmove(point_t curpos, point_t endpoint, int dir, char grid[256][256], int gridwidth, int gridlines)
{
	//dir: udlr

	point_t finalpos = curpos;

	if (dir == 0) // UP
	{
		while( (finalpos.y > 0) && (grid[finalpos.y-1][finalpos.x] == 0) )
		{
			finalpos.y--;

			if (ptsequal(finalpos,endpoint))break; // Stop if pass by finish point
		}
	}
	else if (dir == 1) // DOWN
	{
		while( (finalpos.y < gridlines-1) && (grid[finalpos.y+1][finalpos.x] == 0) )
		{
			finalpos.y++;

			if (ptsequal(finalpos,endpoint))break;
		}
	}
	else if (dir == 2) // LEFT
	{
		while( (finalpos.x > 0) && (grid[finalpos.y][finalpos.x-1] == 0) )
		{
			finalpos.x--;

			if (ptsequal(finalpos,endpoint))break;
		}
	}
	else if (dir == 3) // RIGHT
	{
		while( (finalpos.x < gridwidth-1) && (grid[finalpos.y][finalpos.x+1] == 0) )
		{
			finalpos.x++;

			if (ptsequal(finalpos,endpoint))break;
		}
	}


	if (ptsequal(finalpos, curpos))
	{
		finalpos.x = -1;
		finalpos.y = -1; // Unnecessary, but for OCD's sake...
		return finalpos;
	}
	else
		return finalpos;

}



int fixnode(int nodenum, node_t node[], int *numnodes,
		char grid[256][256], int gridwidth, int gridlines, point_t endpoint)
{ // fixnode( , node, numnodes, grid, gridwidth, gridlines);
	int i=0;

	// Some nodes will show -1 instead of the correct value.
	// This is as the node is calculated before its surrounding nodes are calculated.
	//
	// These nodes can be fixed to reflect the correct value.
	// This error never happens to the starting node, so there's no problem with final ans.

	// Fixing process is by testing each -1 node to see
	// whether they point to nodes of positive values.

	point_t ptudlr[4];
	int stepsfromhere = node[nodenum].rsteps;

	for (i=0; i<4; i++)
		ptudlr[i] = dirmove(node[nodenum].pt, endpoint, i, grid, gridwidth, gridlines);


	for (i=0; i<4; i++)
	{
		// Checking all 4 reachable nodes.
		if (ptudlr[i].x != -1) // <-- dirmove function sets to -1 if no movement
		{// Ignore if the the direction doesn't go anywhere
			// Check through database of all known nodes
			int i2=0;
			for (i2=0; i2<*numnodes; i2++) // Sequential search. improvable??
			if (ptsequal(ptudlr[i], node[i2].pt))
			{
				// If a node is reachable,
				// stepstofinish will be based on that node.
				// stepstofinish = node's stepstofinish + 1

				if (node[i2].rsteps != -1)
					// Don't do anything if that node returns -1
				if ((stepsfromhere == -1) ||
						(stepsfromhere > node[i2].rsteps + 1))
					stepsfromhere = node[i2].rsteps + 1;
			}
		}
	}

	if (node[nodenum].rsteps != stepsfromhere)
	{
		//printf("\nFIX %d -> %d", node[nodenum].rsteps, stepsfromhere);
		node[nodenum].rsteps = stepsfromhere;
		return 1;
	}

	return 0;
}



void fixnodepos(point_t nodepos, node_t node[], int *numnodes,
		char grid[256][256], int gridwidth, int gridlines, point_t endpoint)
{
	int nodenum = -1;
	
	int i=0;
	for (i=0; i<*numnodes; i++)
		if (ptsequal(nodepos, node[i].pt))
			nodenum = i;

	if (nodenum == -1) return; // NOTE NOT DEFINED YET
	fixnode(nodenum, node, numnodes, grid, gridwidth, gridlines, endpoint);
}



int stepstofinish(point_t curpos, point_t endpoint, char grid[256][256],
		int gridwidth, int gridlines, node_t node[], int *numnodes) 
{
	//printf("NODECOUNT: %d", *numnodes);
	// A return value of -1 means the endpoint could not be reached from the current location.
	// A positive value would give the required number of steps to reach the finish.
	

	// TERMINAL CONDITION 1: FINISH REACHED
	// TERMINAL CONDITION 2: ALL SIDES HAVE KNOWN NODES

	// what happens if finish unreachable? <--- NEXT IN AGENDA
	// theres this problem of no nodes becoming solved until finish has been reached. I need to be able to get node data of every node that I've -passed-, not solved.
	
		
	// Mark the node. Initial value of -1.
	int currentnodenum = *numnodes;
	node[currentnodenum].pt.x = curpos.x;
	node[currentnodenum].pt.y = curpos.y;
	node[currentnodenum].rsteps = -1;
	*numnodes += 1;
	
	if (ptsequal(curpos, endpoint)) // Terminal condition - finish reached.
	{
		node[currentnodenum].rsteps = 0;
		return 0; // 0 steps to finish.
	}

	point_t ptudlr[4];
	int stepsfromhere = -1; // Initial value is -1. It means goal undefined


	int i=0;
	for (i=0; i<4; i++)
		ptudlr[i] = dirmove(curpos, endpoint, i, grid, gridwidth, gridlines);

	//printf("=====\nC: %d, %d\n", curpos.x, curpos.y);
/*	printf("=====\nC: %d, %d\n%d, %d\n%d, %d\n%d, %d\n%d, %d\n",
			curpos.x, curpos.y,
			ptudlr[0].x, ptudlr[0].y,
			ptudlr[1].x, ptudlr[1].y,
			ptudlr[2].x, ptudlr[2].y,
			ptudlr[3].x, ptudlr[3].y);*/

	for (i=0; i<4; i++)
	{
		// When calcdone is set to 1, all further checks halt.
		int calcdone = 0;

		// CHECK 1 - SEE IF DESTINATION IS THE SAME POINT (AKA HASNT MOVED)
		if (ptudlr[i].x == -1) // <-- dirmove function sets to -1 if no movement
		{
			calcdone = 1; // don't check any further.
		}

		// CHECK 2 - SEE IF THE DESTINATION IS ALREADY MARKED
		if ((!calcdone) && (*numnodes>0))
		{
			// Check through database of all known nodes
			int i2=0;
			for (i2=0; i2<*numnodes; i2++) // Sequential search. improvable??
				if (ptsequal(ptudlr[i], node[i2].pt))
				{
					// If a node is reachable,
					// stepstofinish will be based on that node.
					// stepstofinish = node's stepstofinish + 1

					if (node[i2].rsteps != -1)
						// Don't do anything if that node returns -1
					if ((stepsfromhere == -1) ||
							(stepsfromhere > node[i2].rsteps + 1))
						stepsfromhere = node[i2].rsteps + 1;

					calcdone = 1;
				}
		}

		// CHECK 3 - CALCULATE NODE YOURSELF
		if (!calcdone) // If no node was found, have to calculate the nodes yourself.
		{
			// Recursive function to calculate remaining steps for this new node.
			int calcdrsteps = stepstofinish(ptudlr[i], endpoint, grid,
				       	gridwidth, gridlines, node, numnodes);

				
			// If a node is reachable,
			// stepstofinish will be based on that node.
			// stepstofinish = node's stepstofinish + 1
			
			if (calcdrsteps != -1) // Don't do anything if that node returns -1
			if ((stepsfromhere == -1) || (stepsfromhere > calcdrsteps + 1))
				stepsfromhere = calcdrsteps + 1;			
		}
	}

	
	for (i=0; i<4; i++)
		fixnodepos(ptudlr[i], node, numnodes, grid, gridwidth, gridlines, endpoint);

	// Create a node.=
	node[currentnodenum].rsteps = stepsfromhere;

	return stepsfromhere;
}




int calcnodejump(int curnode, point_t endpoint, node_t node[], int numnodes, char grid[256][256],
		int gridwidth, int gridlines, int *countednodes, int nodecounted[])
{	
	if (node[curnode].rsteps == 0)
		return 0;


	//Calculates the net jump of a single node (no loops)
	int sumnetjump = 0;

	*countednodes += 1; //Counts the number of times netjump has been run.

	int i=0;
	point_t ptudlr[4];
	for (i=0; i<4; i++)
		ptudlr[i] = dirmove(node[curnode].pt, endpoint, i, grid, gridwidth, gridlines);
	
	
	for (i=0; i<4; i++)
	{
		if (ptudlr[i].x != -1) // Same location
		{
			int nodenum = getnodenum(ptudlr[i], node, numnodes);

			if (node[nodenum].rsteps == -1)
			{ // Trap Node
				
				sumnetjump += 5;
				// add 5 for reachable trapnodes from main path
			}
			else
			{ // Not a trap node

				// Add the difference between step of current pos and
				//                            step of adjacent pos
				sumnetjump += node[nodenum].rsteps - node[curnode].rsteps;
			}
		}
	}

	return sumnetjump;
}




int netjump(int curnode, point_t endpoint, node_t node[], int numnodes, char grid[256][256],
		int gridwidth, int gridlines, int *countednodes, int nodecounted[])
{// Used to calculate averagejump, a difficulty indicator
	// Calculates the netjump of all the nodes along the shortest path to the finish.

	if (node[curnode].rsteps == 0)
		return 0;
	//if (ptsequal(node[curnode].pt, endpoint))
		//return 0;
	
	// Don't recount nodes	
	if (nodecounted[curnode])
		return 0;
	else
		nodecounted[curnode] = 1;


	*countednodes += 1; //Counts the number of times netjump has been run.

	int sumnetjump = 0;

	point_t ptudlr[4];

	int i=0;
	for (i=0; i<4; i++)
		ptudlr[i] = dirmove(node[curnode].pt, endpoint, i, grid, gridwidth, gridlines);
	
	for (i=0; i<4; i++)
	{
		if (ptudlr[i].x != -1) // Same location
		{
			int nodenum = getnodenum(ptudlr[i], node, numnodes);

			if (node[nodenum].rsteps == -1)
			{ // Trap Node
				
				sumnetjump += 10;
				// add 10 for reachable trapnodes from main path
			}
			else
			{ // Not a trap node

				// Add the nodejump off all nodes 1 step away from main path.
				sumnetjump += calcnodejump(nodenum, endpoint, node, numnodes,
						grid, gridwidth, gridlines, countednodes,
						nodecounted);


				// Add the difference between step of current pos and
				//                            step of adjacent pos
				sumnetjump += node[nodenum].rsteps - node[curnode].rsteps;

				// Add the netjump of the next step.
				if (node[nodenum].rsteps < node[curnode].rsteps)
				{
					sumnetjump += netjump(nodenum, endpoint, node, numnodes,
							grid, gridwidth, gridlines, countednodes,
							nodecounted);
				}
			}
		}
	}

	return sumnetjump;
}


float calcnetjump(int curnode, point_t endpoint, node_t node[], int numnodes, char grid[256][256],
		int gridwidth, int gridlines, int minsteps)
{
	// Total value will depend on the number of counted nodes
	int countednodes = 0;
	int nodecounted[65536];

	int i=0;
	for (i=0; i<numnodes; i++)
		nodecounted[i] = 0;

	int totaljump = netjump(curnode, endpoint, node, numnodes, grid,
			gridwidth, gridlines, &countednodes, nodecounted);

	// Average Jump =
	// Total Jump * ( Min Number of Steps / Total nodes counted )
	//printf("Total Jump: %d\n", totaljump);
	//printf("Jumps: %d || Nodes: %d\n", minsteps, countednodes);

	return (float)totaljump * minsteps / countednodes;
}


void difficultycalculations(int *trapexist, float *netjump, point_t startpoint, point_t endpoint,
	       	int minsteps, node_t node[], int numnodes, char grid[256][256],
		int gridwidth, int gridlines)
{
	// MEASUREMENTS:
	// 1) Trap existence (nodes from which you cannot get to the goal from)
	// 2) Averagejump
	// 3) Number of steps
	
	*trapexist = 0;
	int i=0;
	for (i=0; i<numnodes; i++)
		if (node[i].rsteps == -1)
			*trapexist = 1;

	*netjump = calcnetjump(getnodenum(startpoint, node, numnodes),
				endpoint, node, numnodes, grid, gridwidth, gridlines, minsteps);


}








int stepcalculations(point_t startpoint, point_t endpoint, char grid[256][256],
		int gridwidth, int gridlines, int *trapexist, float *netjump,
		node_t node[65536], int *numnodes)
{
	//node_t node[65536];
	*numnodes = 0;
	// node[] and numnodes will be global for the whole calculation process

	int minstepsreq = stepstofinish(startpoint, endpoint, grid,
			gridwidth, gridlines, node, numnodes);

	int loops = 0;
	int fixedaru = 1;
	while ((fixedaru) && (loops<200))
	{
		fixedaru = 0;
		//printf("FIXING LOOP %d", loops);
		int i=0;
		for (i=0; i<*numnodes; i++)
			fixedaru += fixnode(i, node, numnodes,
					grid, gridwidth, gridlines, endpoint);
	
		//printf("\nFIXING DONE\n");
	
		loops++;
	}

	minstepsreq = node[0].rsteps;

	//printnodemap(node, *numnodes, grid, gridwidth, gridlines);

	if (minstepsreq == -1)
	{
		*trapexist = -1;
		*netjump = -1;
	}
	else
	{
		difficultycalculations(trapexist, netjump, startpoint, endpoint,
		       		minstepsreq, node, *numnodes, grid, gridwidth, gridlines);
	}

	return minstepsreq;
}




// For each node, the number of steps to the finish is calculated.
// 
// For some nodes, finish is unreachable. These places will have number of steps = -1
//
//
//DIFFICULTY MEASUREMENT
// A node map which has more possible movements from low stepsremaining to high stepsremaining will be generally harder.
// More difficulty will be added based on the size of the jumps. (e.g. 3steps remainign to 11steps remaining)
// Even Higher difficulty will be added for possible jumps to dead-ends
// No difficulty will be added if the jump to a higher stepsremaining is 1-step reversible.
//
//It will only be measured from the start point to the end point along nodes in decreasing step count.
//
//
// Difficulty will be reduced for each reducenumber jump.
//
// Looking at the above conditions, a "netjumpchangeinstepsremaining" value could work.
//
//DEAD-ENDS possible flag
//-> Set if jumps to dead-ends are possible


//3. MAZE ANALYSIS FUNCTIONS - END









//4. GENERATION FUNCTIONS - START

void generatemaze(point_t *startpoint, point_t *endpoint, char grid[256][256],
		float blockratio, int sizex, int sizey)
{
	int iy=0; int ix=0;

	int thousandtimesblockratio = blockratio*1000;

	for (iy=0; iy<sizey; iy++)
	for (ix=0; ix<sizex; ix++)
	{// RANDOM GENERATE
		if (rand()%1000 < thousandtimesblockratio)
			grid[iy][ix] = 1;
		else
			grid[iy][ix] = 0;
	}

	startpoint->x = rand()%sizex;
	startpoint->y = rand()%sizey;
	endpoint->x = rand()%sizex;
	endpoint->y = rand()%sizey;

	grid[startpoint->y][startpoint->x] = 0;
	grid[endpoint->y][endpoint->x] = 0;
}


void gridchangesplash(int splashnum, point_t startpoint, point_t endpoint, char grid[256][256],
			int gridwidth, int gridlines)
{//printf("GRID SPLASH\n");
	int i=0;
	for (i=0; i<splashnum; i++)
	{
		int loops = 0;

		//int ptindex[65536];
		//int ptindexnum = 0;
		
		// Seek out a space and make it a block
		while (loops < 50)
		{// Try a maximum of 50 times to find a space.
			point_t ptswitch;
			ptswitch.x = rand()%gridwidth;
			ptswitch.y = rand()%gridlines;

			if (grid[ptswitch.y][ptswitch.x] == 0)
			if (!ptsequal(startpoint,ptswitch) // Don't switch the finish/start space
				&& !ptsequal(endpoint,ptswitch))
			{
				grid[ptswitch.y][ptswitch.x] = 1;
				loops = 100; // breaks loop
			}

			loops++;
		}
		

		loops = 0;

		// Seek out a block and make it a space
		while (loops < 50)
		{// Try a maximum of 50 times to find a block.
			point_t ptswitch;
			ptswitch.x = rand()%gridwidth;
			ptswitch.y = rand()%gridlines;

			if (grid[ptswitch.y][ptswitch.x] == 1)
			{
				grid[ptswitch.y][ptswitch.x] = 0;
				loops = 100; // breaks loop
			}

			loops++;
		}
	}

}



void changestartpoint(point_t *startpoint, node_t node[], int numnodes, int minstep)
{//printf("CHANGESTART\n");
	int i=0;
	int numpossiblenodes = 0;

	for (i=0; i<numnodes; i++)
	{
		if (node[i].rsteps > minstep)
			numpossiblenodes += 1;
	}

	if (numpossiblenodes <= 1)
	{// If there are <= 1 nodes where the step is greater than minstep,
	// just take the node with the largest step.
		int maxnode = 0;
		int maxsteps = 0;
		for (i=0; i<numnodes; i++)
		{
			if (node[i].rsteps > maxsteps)
			{
				maxnode = i;
				maxsteps = node[i].rsteps;
			}
		}

		//... just take the node with the largest step...
		//unless that node is already the start point.
		//then take the node with the second largest step.
		if (!ptsequal(*startpoint,node[maxnode].pt))
		{
			startpoint->x = node[maxnode].pt.x;
			startpoint->y = node[maxnode].pt.y;
		}
		else
		{
			int maxsteps2 = 0;

			for (i=0; i<numnodes; i++)
			{ // Search for second highest
				if ((node[i].rsteps > maxsteps2) && (node[i].rsteps != maxsteps))
				{
					maxnode = i;
					maxsteps2 = node[i].rsteps;
				}
			}
			startpoint->x = node[maxnode].pt.x;
			startpoint->y = node[maxnode].pt.y;
		}
	}// If there are <= 1 nodes where the step is greater than minstep - END
	else
	{ // There are nodes where the step is greater than minstep

		// Randomly choose one of the nodes which has rsteps>minstep.
		int nodeindex[65536];
		int curindex = 0;

		int i=0;
		for (i=0; i<numnodes; i++)
		{
			if (node[i].rsteps > minstep)
			{
				nodeindex[curindex] = i;
				curindex++;
			}
		}

		int notenum = nodeindex[rand()%curindex];

		startpoint->x = node[notenum].pt.x;
		startpoint->y = node[notenum].pt.y;
	} // There are nodes where the step is greater than minstep - END
}



void changeendpoint(point_t *endpoint, char grid[256][256], int gridwidth, int gridlines)
{//printf("CHANGEEND\n");
	
	int loops = 0;

	while (loops < 50)
	{
		int ix = rand()%gridwidth;
		int iy = rand()%gridlines;

		if (grid[iy][ix] == 0)
		{
			endpoint->x = ix;
			endpoint->y = iy;
			loops = 100;
		}
		loops++;
	}

}


void modifymaze(point_t *startpoint, point_t *endpoint, char grid[256][256], int gridwidth, int gridlines, //node_t node[], int numnodes,
		int maxloops,
		int trapallowed,
		int minsteps,
		float blockratio,
		float minjump)
{
	/*
	int gridwidth = 10;
	int gridlines = 10;
	
	float blockratio = 0.3;
	int minsteps = 10;
	float minjump = 5;

	int trapallowed = 1;

	int maxloops = 10;
	int filestogenerate = 1;
	char filename[20] = "maze";
	*/
	
	int loops = 0;




	int trapexistP = 1;
	float netjumpP = 0;
	int numstepsP = -1;
	int numnodesP = 0;
				
	char gridP[256][256];
	point_t startpointP;
	point_t endpointP;


	while (loops < maxloops)
	{
		int trapexist = 0;
		float netjump = 0;
		node_t node[65536];
		int numnodes = 0;

		int numsteps = stepcalculations(*startpoint, *endpoint,
						grid, gridwidth, gridlines,
						&trapexist, &netjump, node, &numnodes);


		// PENDING: UNDO CHANGE IF BAD

		// Based on Above Calculations,
		// Decide whether to fix and how to fix


		// Positive solutionvector: change grid.
		// Negative solutionvector: change startpoint.
		int solutionvector = 0;

		// needfixing:  0 = all conditions met. BREAK.
		// 		1 = not all conditions met. CHANGE.
		// 		2 = conditions met in previous loop now not met. REVERT.
		int needfixing = 0;

		if ((trapallowed == 0) && (trapexist == 1))
		{ // ISSUE 1 - TRAPS EXIST
			if (trapexistP == 0) // previously 0
				needfixing = 2;
			else
				if (needfixing != 2) needfixing = 1;
			solutionvector += 1;
		}
		if (netjump < minjump)
		{ // ISSUE 2 - NETJUMP TOO LOW
			if (netjumpP > minjump) // previously high enough
				needfixing = 2;
			else
				if (needfixing != 2) needfixing = 1;
			solutionvector -= 1;
		}
		if (numsteps < minsteps)
		{ // ISSUE 3 - MIN STEPS TOO LOW
			if (numstepsP > minsteps) // previously high enough
				needfixing = 2;
			else
				if (needfixing != 2) needfixing = 1;

			if ((numsteps == -1) && (numstepsP != -1))
				needfixing = 2; // PREVIOUS CRASH CAUSE

			solutionvector -= 2;
		}
		if (numnodes < 0.3*(1 - blockratio))
		{ // ISSUE 4 - NODES TOO FEW - Excusable.
			solutionvector += 2;
		}

		// CRASH ORIGINATED HERE.
		if (needfixing == 2) 
		{ // Conditions previously met no longer met. Revert to previous maze.
			//printf("revert\n");
			trapexist = trapexistP;
			netjump = netjumpP;
			numsteps = numstepsP;
			numnodes = numnodesP;

			startpoint->x = startpointP.x;
			startpoint->y = startpointP.y;

			endpoint->x = endpointP.x;
			endpoint->y = endpointP.y;
	
			int ix,iy;
			for (ix=0;ix<gridwidth;ix++)
			for (iy=0;iy<gridlines;iy++)
				grid[iy][ix] = gridP[iy][ix];
		}


		// Make a record of the current loop for reference in next loop.
		trapexistP = trapexist;
		netjumpP = netjump;
		numstepsP = numsteps;
		numnodesP = numnodes;

		startpointP.x = startpoint->x;
		startpointP.y = startpoint->y;
				
		endpointP.x = endpoint->x;
		endpointP.y = endpoint->y;

		int ix,iy;
		for (ix=0;ix<gridwidth;ix++)
		for (iy=0;iy<gridlines;iy++)
			gridP[iy][ix] = grid[iy][ix];



		if (rand()%3 == 0) // 1/3 chance to do opposite fix.
		{
			solutionvector *= -1;
		}

//	printf("LOOP NUMBER %d\n", loops);
		if ((loops < maxloops) && (needfixing)) // only carry out if not final loop
		{// Fixing needed
			if (solutionvector > 0)
			{ // Change grid
				if (rand()%5 == 0) // 1/5 change of changing endpoint instead
					changeendpoint(endpoint, grid, gridwidth, gridlines);
				else
					gridchangesplash(gridwidth+gridlines,
							*startpoint, *endpoint, grid,
							gridwidth, gridlines);
			}
			else // solutionvector <= 0
			{ // Change Start Point
				changestartpoint(startpoint, node, numnodes, minsteps);
			}
		}
		else
		{// No Fixing Needed
			loops = maxloops; // Break loop
		}

	//printf("SOLUTION STEPS: %d\n", numsteps);
	//printf("TRAPS EXIST: %d\n", trapexist);
	//printf("NET JUMP: %f\n", netjump);



		loops++;
	}
	
}




















//4. GENERATION FUNCTIONS - END
























//5. MAIN - START

void main()
{
	srand(time(NULL));

	// Variable definitions
	char sbse[4] = {'_', '0', '+', '='};  // Space, Block, Start, End characters respectively


	// Map variables
	char filetext[65536];

	point_t startpoint = {0,0};
	point_t endpoint = {0,0};

	char grid[256][256]; // 0 = space, 1 = block


	// Generation Parameters
	int gridwidth = 20;
	int gridlines = 15;
	
	float blockratio = 0.15;
	int minsteps = 25;
	float minjump = 30;

	int trapallowed = 0;

	int maxloops = 100;
	int maxretries = 100000;
	int filestogenerate = 10;
	char filename[30] = "maze";
	
	loadparameters("genconfig.txt", &gridwidth, &gridlines, &blockratio, &minsteps,
			&minjump, &trapallowed, &maxloops, &maxretries, &filestogenerate,
			filename);

	// Actions
	//loadgridfromfile("sample1.txt", sbse, &startpoint, &endpoint,
	//			grid, &gridwidth, &gridlines);

	// Maze Data
	int trapexist = 0;
	float netjump = 0;
	int minstepsreq = 0;
	
	//node_t node[65536];
	//int numnodes = 0;

	// Counting variables
	int retrycount=-1;
	int generatedcount = 0;
	int filenameLength = strlen(filename);

	for (generatedcount=0; generatedcount<filestogenerate; generatedcount++)
	{// LOOP: Generate - START
		
		int retries = 0;
		int retrycount=-1;
	while (retries < maxretries)
	{ // LOOP One Retry - START
		retrycount++;
		if (retrycount%1==1) // print every x retries
			printf("RETRY NUMBER %d\n", retrycount);

		generatemaze(&startpoint, &endpoint, grid, blockratio, gridwidth, gridlines);

		modifymaze(&startpoint, &endpoint, grid, gridwidth, gridlines, 
			maxloops, trapallowed, minsteps, blockratio, minjump);

		node_t node[65536];
		int numnodes = 0;
		minstepsreq = stepcalculations(startpoint, endpoint, grid, gridwidth, gridlines,
					&trapexist, &netjump, node, &numnodes);


		if (((trapallowed == 0) && (trapexist == 1))
			|| (netjump < minjump) || (minstepsreq < minsteps))
			retries++; // MAZE DOES NOT MEET CONDITIONS. RETRY AGAIN.
		else
			retries = maxretries; // MAZE MEETS CONDITIONS. BREAK LOOP.
	} // LOOP One Retry - END

		// MAZE COMPLETE: PRINTING STATISTICS
	//	printnodemap(node, numnodes, grid, gridwidth, gridlines);
		printf("\n\n>> MAZE NUMBER %d <<\n", generatedcount+1);

		printf("RETRY COUNT: %d\n", retrycount);

		printf("\nMAP SIZE:   %d, %d\n", gridwidth, gridlines);
		printf("STARTPOINT: %d, %d\n", startpoint.x, startpoint.y);
		printf("ENDPOINT:   %d, %d\n\n", endpoint.x, endpoint.y);
		printf("SOLUTION STEPS: %d\n", minstepsreq);
		printf("TRAPS EXIST: %d\n", trapexist);
		printf("NET JUMP: %f\n", netjump);
		// MAZE COMPLETE: PRINTING STATISTICS - END


		// WRITE MAZE TO FILE
		if (filestogenerate==1) // Generate only one file
			writegridtofile(sbse, startpoint, endpoint, grid, gridwidth, gridlines,
			filename);
		else
		{
			if (generatedcount == 0)
				strcat(filename, "1");
			else
			{
				filename[filenameLength] = '\0';
				char intstr[5] = "";
				sprintf(intstr, "%d", generatedcount+1);

				strcat(filename, intstr);
				
			}
			//printf("\n%s", filename);
			writegridtofile(sbse, startpoint, endpoint, grid, gridwidth, gridlines,
			filename);
		}
		// WRITE MAZE TO FILE - END

	}// LOOP: Generate - END

	{
		printf("\n");
		int ix,iy;
		for (iy=0; iy<gridlines; iy++)
		{
			for (ix=0; ix<gridwidth; ix++)
			{
				if ((startpoint.x == ix) && (startpoint.y == iy))
					printf("S");
				else if ((endpoint.x == ix) && (endpoint.y == iy))
					printf("F");
				else
				{
					if (grid[iy][ix])
						printf("#");
					else
						printf(" ");
				}
			}
			printf("\n");
		}
	}



	scanf("%s", filetext);

}





