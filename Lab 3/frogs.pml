// there are five positions 
# define POSITIONS 5

// index is the frog: 0 is empty space, 1 is frog 1, 2 is frog2 etc
// the value tells us the position of each frog 
int frog[POSITIONS];

// this tells us whether the frog is stuck or not
bool frogsStuck[POSITIONS] = {true, false, false, false, false};

proctype hop (int frogNum; int direction) {

	// this will print the start line of each frog
	if 
	::(direction == 1) ->
		printf("START FROG %d AT %d GOING RIGHT\n", frogNum, frog[frogNum]);
	:: else ->
		printf("START FROG %d AT %d GOING LEFT\n", frogNum, frog[frogNum]);
	fi;

	// temporary value to switch positions
	int temp;
	
	end: do :: true -> 
		atomic {
			// if the frog can move spaces
			if ::(frog[0] == (frog[frogNum] + direction) || frog[0] == frog[frogNum]+direction+direction) ->
				
				// switch the positions
				temp = frog[frogNum];
				frog[frogNum] = frog[0];
				frog[0] = temp

				// print the lines
				printf("MOVE FROG%d FROM %d TO %d\n", frogNum, temp, frog[frogNum]);
				printf("EMPTY %d, FROG1@%d, FROG2@%d, FROG3@%d, FROG4@%d\n",
					frog[0],
					frog[1],
					frog[2],
					frog[3],
					frog[4]
					);

				// the current frog is NOT stuck
				frogsStuck[frogNum] = false;

			// the frog is stuck 
			:: else -> frogsStuck[frogNum] = true;
			fi; 

			// if the frogs aren't stuck and not correct do nothing
			if :: (!( frog[0] == 4 && frog[1] == 5 
				&& (frogsStuck[1] && frogsStuck[2] && frogsStuck[3] && frogsStuck[4]) 
				&& (frog[0] != frog[1] + 1 || frog[0] != frog[1] + 2)
				&& (frog[0] != frog[2] - 1 || frog[0] != frog[2] - 2)
				&& (frog[0] != frog[3] - 1 || frog[0] != frog[3] - 2) 
				&& (frog[0] != frog[4] - 1 || frog[0] != frog[4] - 2)) ) -> skip;

			// if they are all correct and stuck break the loop
			:: else -> break;
			fi;
		}
	od
}


init {
	// initialise all the spaces
	frog[0] = 2;
	frog[1] = 1;
	frog[2] = 3;
	frog[3] = 4;
	frog[4] = 5;

	// print the state
	printf("EMPTY %d, FROG1@%d, FROG2@%d, FROG3@%d, FROG4@%d\n", 
		frog[0], 
		frog[1], 
		frog[2], 
		frog[3], 
		frog[4]);

	// call all the frog proctypes
	run hop(1, 1);
	run hop(2, -1);
	run hop(3, -1);
	run hop(4, -1);

	// wait until the frog processes are finished
	(_nr_pr == 1);
	
	// last print line
	printf("DONE!\n" );

	// assert true when the final state is not valid
	assert(!(frog[1] == 5 && frog[0] == 4))	
}
