#include "defs.h"

void* ghostThreadFunction(void* inputGhost){
	GhostType* ghost = (GhostType*) inputGhost;
	
	while (ghost->boredomTimer > 0){
		RoomType* currRoom = ghost->room;
		sem_wait(&(currRoom->mutex));
		
		//If there is a hunter in the room
		if (ghost->room->hunters.size != 0){
			
			//reset the boredom timer
			ghost->boredomTimer = BOREDOM_MAX;
			
			int choice = randInt(1, 3);
			
			//leave evidence choice
			if (choice == 1){
				leaveEvidence(ghost);
			}
			
			//if choice is 2 do nothing
			else printf("The ghost does nothing");
		
		}
		
		//No hunter in the room
		else{
			//decrease boredom timer
			ghost->boredomTimer--;
			
			int choice = randInt(1, 4);
			
			//move room choice
			if (choice == 1){
				moveRoom(ghost);
			}
			
			//leave evidence choice
			else if (choice == 2){
				leaveEvidence(ghost);
			}
			
			//if choice is 3 do nothing
			else printf("The ghost does nothing");
		}
		sem_post(&(currRoom->mutex));
	}
	return (0);
}

void leaveEvidence(GhostType* ghost){

	int evidenceChoice;
	switch (ghost->ghostType){
	
		case POLTERGEIST:
			//Choices match to 0, 1, 2
			evidenceChoice = randInt(0,3);
			break;
			
		case BANSHEE:
			//Choices match to 0, 1, 3
			evidenceChoice = randInt(0,3);
			if (evidenceChoice == 2) evidenceChoice++;
			break;
		
		case BULLIES:
			//Choices match to 0, 2, 3
			evidenceChoice = randInt(1,4);
			if (evidenceChoice == 1) evidenceChoice--;
			break;
		
		case PHANTOM:
			//Choices match to 1, 2, 3
			evidenceChoice = randInt(1,4);	
			break;
	}
	
	EvidenceClassType evidenceDropped = evidenceChoice;
	
	float evidenceValue;
	switch (evidenceDropped){
		case EMF:
			evidenceValue = randFloat(4.70, 5.00);
			break;
		
		case TEMPERATURE:
			evidenceValue = randFloat(-10.00, 1.00);
			break;
		
		case FINGERPRINTS:
			evidenceValue = 1;
			break;
		
		case SOUND:
			evidenceValue = randFloat(65.00, 75.00);
			break;
	}
	
	EvidenceType* newEvidence = calloc(1, sizeof(EvidenceType));
	initEvidenceType(newEvidence, evidenceChoice, evidenceValue, 1);
	
	EvidenceNodeType* newEvidenceNode = calloc(1, sizeof(EvidenceNodeType));
    	newEvidenceNode->evidence = newEvidence;
	
	addEvidence(ghost->room->evidence, newEvidenceNode);
	printf("The ghost just dropped some evidence in: %s\n", ghost->room->name);
}

void moveRoom(GhostType* ghost){
	RoomType* currRoom = ghost->room;
	
	int adjacentRoomNum = currRoom->rooms->totalRooms;
	RoomNodeType* currRoomChoice;
	
	//divison by 0 in randInt function if this check isn't made
	if (adjacentRoomNum == 1){
		currRoomChoice = currRoom->rooms->head;
	}
	
	else{
		int roomChoice = randInt(0, adjacentRoomNum);
		currRoomChoice = currRoom->rooms->head;
		
		for (int i = 0; i < roomChoice; i++){
			currRoomChoice = currRoomChoice->next;
		}
		
	}
	
	if (sem_trywait(&(currRoomChoice->room->mutex)) == 0) {
		ghost->room = currRoomChoice->room;
		currRoomChoice->room->ghost = ghost;
		currRoom->ghost = NULL;
		
		
		sem_post(&(currRoomChoice->room->mutex));
		printf("The ghost just moved to %s \n", ghost->room->name);
		return;
	}
	printf("The ghost was unable to move rooms");
}

void initGhost(GhostType* ghost, GhostClassType ghostClass, RoomType* room){
	ghost->ghostType = ghostClass;
	ghost->room = room;
	ghost->boredomTimer = BOREDOM_MAX;
}
