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



void readfile(char filetext[], char filename[])
{
	FILE *filep;

	char filename2[101] = "";
	strcat(filename2, filename);
	strcat(filename2, ".txt");

	filep = fopen(filename2, "r");

	fseek( filep, 0, SEEK_END);
	long fsize = ftell( filep);
	rewind( filep);

	//printf("%d\n", fsize);
	fread( filetext, 1, fsize, filep);
	filetext[fsize] = '\0';
}


/*void readfile(char filetext[])
{
	FILE *filep;

	filep = fopen("sample1.txt", "r");

	fseek( filep, 0, SEEK_END);
	long fsize = ftell( filep);
	rewind( filep);

	printf("%d\n", fsize);
	fread( filetext, 1, fsize, filep);
	filetext[fsize] = '\0';
}*/


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


	printf("\n @");
	for (ix=0; ix<gridwidth; ix++) printf("-");
	printf("@\n");

	for (iy=0; iy<gridlines; iy++)
	{
		printf(" |");
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
		printf("|\n");
	}
	
	printf(" @");
	for (ix=0; ix<gridwidth; ix++) printf("-");
	printf("@\n");
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

//	printf("=====\nC: %d, %d\n", curpos.x, curpos.y);
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
		int gridwidth, int gridlines, int *trapexist, float *netjump)
{
	node_t node[65536];
	int numnodes = 0;
	// node[] and numnodes will be global for the whole calculation process

	int minstepsreq = stepstofinish(startpoint, endpoint, grid,
			gridwidth, gridlines, node, &numnodes);

	int loops = 0;
	int fixedaru = 1;
	while ((fixedaru) && (loops<200))
	{
		fixedaru = 0;
		//printf("FIXING LOOP %d", loops);
		int i=0;
		for (i=0; i<numnodes; i++)
			fixedaru += fixnode(i, node, &numnodes,
					grid, gridwidth, gridlines, endpoint);
	
		//printf("\nFIXING DONE\n");
	
		loops++;
	}
	
	minstepsreq = node[0].rsteps;

	printnodemap(node, numnodes, grid, gridwidth, gridlines);

	if (minstepsreq == -1)
	{
		*trapexist = -1;
		*netjump = -1;
	}
	else
	{
		difficultycalculations(trapexist, netjump, startpoint, endpoint,
		       		minstepsreq, node, numnodes, grid, gridwidth, gridlines);
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

	printf("Enter name of file (excluding .txt extension):\n");
	char filename[] = "maze";

	scanf("%s", filename);

	// Actions start

	readfile(filetext, filename);

	// gridwidth represents the position of the terminating character of a line
	int gridwidth = 0;

	while(gridwidth<256)
	{
		int cont = 0;
		int i=0;

		for (i=0; i<4; i++)
			if (filetext[gridwidth] == sbse[i]) cont = 1;

		if (!cont) break;
	
		gridwidth++;
	}

	// lines


	int i=0;
	int gridlines = 0;

	{
		char filetextread[65536];
		for (i=0; i<strlen(filetext); i++)
			filetextread[i] = filetext[i];

		
		int i2=0;
		while (i2 != -1)
		{
			for (i=0; i<gridwidth; i++)
			{
				if (filetextread[i] == sbse[0]) grid[i2][i] = 0;
				else if (filetextread[i] == sbse[1]) grid[i2][i] = 1;
				else if (filetextread[i] == sbse[2])
				{	
					grid[i2][i] = 0;
					startpoint.x = i;
					startpoint.y = i2;
				}
				else if (filetextread[i] == sbse[3])
				{
					grid[i2][i] = 0;
					endpoint.x = i;
					endpoint.y = i2;
				}
	
			}

			cutline(filetextread);
			i2++;
			if (filetextread[0] == '\0')
			{
				gridlines = i2;
				i2 = -1;
			}
		}
	}

	int trapexist = 0;
	float netjump = 0;
	int minstepsreq = stepcalculations(startpoint, endpoint, grid, gridwidth, gridlines,
					&trapexist, &netjump);



//	printf("\n\n%s\n\n", filetext);

	/*int i2=0;
	for (i2=0; i2<gridlines; i2++)
	{
		for (i=0; i<gridwidth; i++)
			printf("%d", grid[i2][i]);
		printf("\n");
	}*/

	printf("\nMAP SIZE:   %d, %d\n", gridwidth, gridlines);
	printf("STARTPOINT: %d, %d\n", startpoint.x, startpoint.y);
	printf("ENDPOINT:   %d, %d\n\n", endpoint.x, endpoint.y);
	printf("SOLUTION STEPS: %d\n", minstepsreq);
	printf("TRAPS EXIST: %d\n", trapexist);
	printf("NET JUMP: %f\n", netjump);




	scanf("%s", filetext);

}





