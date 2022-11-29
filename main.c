#include "defs.h"

int main(int argc, char *argv[])
{
    // Initialize a random seed for the random number generators
    srand(time(NULL));

    // You may change this code; this is for demonstration purposes
    BuildingType building;
    initBuilding(&building);
    populateRooms(&building);

    //Initializing all the hunters
    char name[MAX_STR];
    for (int i = 0; i < MAX_HUNTERS; i++){
    	printf("Enter the name of the hunter: ");
	fgets(name, MAX_STR, stdin);

	HunterType* newHunter = malloc(sizeof(HunterType));
	EvidenceClassType evidence = i;

	//testing
	//RoomNodeType* head = building.rooms.head->room->rooms->head;
	//if (head == NULL) printf("NULL\n");
	//printf("%s", head->room->name);

	initHunter(&newHunter, building.rooms.head, evidence, name);
	addHunter(&building.hunters, newHunter);
    }

    //Initializing the ghost
    GhostType ghost;

    
    //getting ghost type
    int ghostClassNumber = randInt(0, 4);
    printf("The ghost class is number: %d\n", ghostClassNumber);

    //getting random room
    int roomNumber = randInt(1, 12); //13 is the amount of rooms given so move 12 times to get there
    RoomNodeType* currRoom = building.rooms.head;
    for (int i = 0; i < roomNumber; i++){
    	currRoom = currRoom->next;
    }
    
    //inialize the ghost with the random ghost class and room
    initGhost(&ghost, ghostClassNumber, currRoom->room);

    currRoom->room->ghost = &ghost; //room has the ghost

    building.ghost = &ghost; //building has the ghost

    //THREADING
    
    //ghost thread
    pthread_t ghostThread;
    pthread_create(&ghostThread, NULL, ghostThreadFunction, building.ghost);
    
    //hunter threads
    pthread_t hunterThread1, hunterThread2, hunterThread3, hunterThread4;
    
    //joining threads
    pthread_join(ghostThread, NULL);
    
    /*
    //testing section
    //prints all rooms with all their evidence
    RoomNodeType* curr = building.rooms.head;
    while(curr != NULL){
    	if (curr->room->evidence->head == NULL) printf("no evidence in %s \n", curr->room->name);
    	else{
    		printf("This is room %s with this evidence:\n", curr->room->name);
    		EvidenceNodeType* currE = curr->room->evidence->head;
    		while(currE != NULL){
			printf("\tEvidence type: %d with the value: %f\n", currE->evidence->evidenceType, currE->evidence->value);
			currE = currE->next;
		}
    	}
    	curr = curr->next;
    }
    
    //end of testing section
    */

    cleanupBuilding(&building);
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
