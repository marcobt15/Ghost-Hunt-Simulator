#include "defs.h"

void printb(BuildingType b){
	RoomNodeType* r = b.rooms->head;
	while (r != NULL){
		printf("%s\n", r->room->name);
		if (r->room->hunters.size > 0){
			for (int i = 0; i < r->room->hunters.size; i++){
				printf("\t%s\n", r->room->hunters.hunters[i]->name);
				EvidenceNodeType* e = r->room->hunters.hunters[i]->evidence->head;
				while(e != NULL){
					printf("\t%d    %f\n", e->evidence->evidenceType, e->evidence->value);
					e = e->next;
				}
			}
		}
		if (r->room->ghost != NULL){
			char n[MAX_STR];
			getGhostName(r->room->ghost->ghostType, n);
			printf("\t%s\n", n);
		}
		r = r->next;
	}
}

void printstate(BuildingType b){
	RoomNodeType* r = b.rooms->head;
	while (r != NULL){
		printf("%s\n", r->room->name);
		/*
		if (r->room->hunters.size > 0){
			for (int i = 0; i < r->room->hunters.size; i++){
				printf("\t%s\n", r->room->hunters.hunters[i]->name);
			}
		}
		*/
		/*
		if (r->room->ghost != NULL){
			char n[MAX_STR];
			getGhostName(r->room->ghost->ghostType, n);
			printf("\t%s\n", n);
		}
		*/
		EvidenceNodeType* e = r->room->evidence->head;
		while(e != NULL){
		
			printf("\t%d    %f\n", e->evidence->evidenceType, e->evidence->value);
			e = e->next;
		}
		
		r = r->next;
	}
}

void printEvidence(HunterType* hunter){
	EvidenceNodeType* e = hunter->evidence->head;
	if (e == NULL) printf("THERE IS NO EVIDENCE");
	printf("Here is %s evidence:\n", hunter->name);
	while(e != NULL){
		printf("\t%d    %f\n", e->evidence->evidenceType, e->evidence->value);
		e = e->next;
	}
	printf("this is the end of the evidence \n");
}

int main(int argc, char *argv[])
{
	// Initialize a random seed for the random number generators
	srand(time(NULL));

	BuildingType building;
	initBuilding(&building);
	populateRooms(&building);
	
	//Initializing the ghost
	GhostType ghost;
	GhostClassType ghostClass = randInt(0, 4);    //getting ghost type

	//getting random room
	int roomNumber = randInt(1, 12); //13 is the amount of rooms given so move 12 times to get to room 13
	RoomNodeType* currRoom = building.rooms->head;
	for (int i = 0; i < roomNumber; i++){
		currRoom = currRoom->next;
	}

	initGhost(&ghost, ghostClass, currRoom->room);
	
	building.ghost = &ghost; //building has the ghost
	
	printf("Ghost is in %s\n", ghost.room->name);
	//Initializing all the hunters
	char name[MAX_STR];
	for (int i = 0; i < MAX_HUNTERS; i++){
		printf("Enter the name of the hunter: ");
		fgets(name, MAX_STR, stdin);

		//getting rid of the \n in the name[]
		int strLength = strlen(name);
		if (name[strLength-1] == '\n') name[strLength-1] = '\0';

		HunterType* newHunter = malloc(sizeof(HunterType));
		EvidenceClassType evidence = i;

		initHunter(newHunter, building.rooms->head, evidence, name);
		addHunter(&building.hunters, newHunter);
	}

	pthread_t ghostThread, hunterThread1, hunterThread2, hunterThread3, hunterThread4;

	pthread_create(&ghostThread, NULL, ghostThreadFunction, building.ghost);
	pthread_create(&hunterThread1, NULL, hunterThreadFunction, building.hunters.hunters[0]);
	pthread_create(&hunterThread2, NULL, hunterThreadFunction, building.hunters.hunters[1]);
	pthread_create(&hunterThread3, NULL, hunterThreadFunction, building.hunters.hunters[2]);
	pthread_create(&hunterThread4, NULL, hunterThreadFunction, building.hunters.hunters[3]);

	//joining threads
	pthread_join(ghostThread, NULL);
	pthread_join(hunterThread1, NULL);
	pthread_join(hunterThread2, NULL);
	pthread_join(hunterThread3, NULL);
	pthread_join(hunterThread4, NULL);
	
	
	printf("Here are all the Hunter who got too scared:\n");
	int scaredHunters = 0;
	for (int i = 0; i < MAX_HUNTERS; i++){
	if (building.hunters.hunters[i]->fearTimer >= 100){
		printf("\t%s\n", building.hunters.hunters[i]->name);
		scaredHunters++;
		}
	}

	if (scaredHunters == 0) printf("No hunters were scared off by the ghost!\n");

	char ghostName[MAX_STR];
	getGhostName(building.ghost->ghostType, ghostName);
	
	if (scaredHunters == MAX_HUNTERS){
		printf("The %s has won! All the huntes got scared and left", ghostName);
	}

	else{
		printf("The real ghost type is: %s\n", ghostName);
		
		GhostClassType suspectedGhostType;
		for (int i = 0; i < MAX_HUNTERS; i++){
			if (building.hunters.hunters[i]->fearTimer < MAX_FEAR && building.hunters.hunters[i]->boredomTimer > 0){
				int amountOfGhostEvidence = checkForGhostEvidence(building.hunters.hunters[i]->evidence);
				if (amountOfGhostEvidence == 3){
					determineGhostType(building.hunters.hunters[i], &suspectedGhostType);
					break;
				}
				
			}
		}
		
		char suspectedGhostName[MAX_STR];
		getGhostName(suspectedGhostType, suspectedGhostName);
		printf("The ghost type that the hunters suspect is: %s\n", suspectedGhostName);
		
		if (strcmp(ghostName, suspectedGhostName) == 0) printf("The hunters have won! They determined what ghost was present\n");
		else printf("The hunter have not guessed the right ghost\n"); //should never get here
		cleanupBuilding(&building);
		
		
	}
	/*
	for (int i = 0; i < 10; i++){
		leaveEvidence(&ghost);
	}
	printstate(building);
	for (int i = 0; i < 4; i++){
		moveHunter(building.hunters.hunters[i]);
	}
	printb(building);
	
	for (int i = 0; i < 4; i++){
		for(int j = 0 ; j < 3; j++){
			collectEvidence(building.hunters.hunters[i]);
		} 
	}
	printstate(building);
	
	for (int i = 0; i < 4; i++){
		printEvidence(building.hunters.hunters[i]);
	}
	
	
	for (int i = 0; i < 4; i++){
		communicateEvidence(building.hunters.hunters[i]);
	}
	
	for (int i = 0; i < 4; i++){
		printEvidence(building.hunters.hunters[i]);
	}
	
	
	for (int i = 0; i < 4; i++){
		removeStandardEvidence(building.hunters.hunters[i]);
	}
	printf("\nhere is the evidence lists after removing standard evidence:\n");
	for (int i = 0; i < 4; i++){
		printEvidence(building.hunters.hunters[i]);
	}
	
	int h = -1;
	while(1){
		for (int i = 0; i < 4; i++){
			int d = checkForGhostEvidence(building.hunters.hunters[i]->evidence);
			if (d == 3){
				h=i;
				break;
			}
		}
		if (h != -1) break;

		for (int j = 0; j < 4; j++){
			communicateEvidence(building.hunters.hunters[j]);
		}
		
	}
	
	printf("\nevidence after sharing a lot\n");
	
	for (int k = 0; k < 4; k++){
		printEvidence(building.hunters.hunters[k]);
	}
	
	printf("\n\n %d \n\n", h);
	
	GhostClassType g;
	determineGhostType(building.hunters.hunters[h], &g);
	
	
	char suspectedGhostName[MAX_STR];
	getGhostName(g, suspectedGhostName);
	printf("The ghost type that the hunters suspect is: %s\n", suspectedGhostName);
	
	char ghostName[MAX_STR];
	getGhostName(building.ghost->ghostType, ghostName);

	printf("The real ghost type is: %s\n", ghostName);
	
	if (strcmp(ghostName, suspectedGhostName) == 0) printf("The huntes have won! They determined what ghost was present\n");
	else printf("The hunter have not guessed the right ghost\n"); //should never get here
	
	*/
	return 0;
}


/*
  Function:  randInt
  Purpose:   returns a pseudo randomly generated number,
             in the range min to (max - 1), inclusively
       in:   upper end of the range of the generated number
   return:   randomly generated integer in the range [min, max-1)
*/
int randInt(int min, int max)
{
	return rand() % (max - min) + min;
}

/*
  Function:  randFloat
  Purpose:   returns a pseudo randomly generated number,
             in the range min to max, inclusively
       in:   upper end of the range of the generated number
   return:   randomly generated integer in the range [0, max-1)
*/
float randFloat(float a, float b) {
	// Get a percentage between rand() and the maximum
	float random = ((float) rand()) / (float) RAND_MAX;
	// Scale it to the range we want, and shift it
	return random * (b - a) + a;
}
