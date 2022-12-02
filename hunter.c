#include "defs.h"

void* hunterThreadFunction(void* inputHunter){
	HunterType* hunter = (HunterType*) inputHunter;
	int amountOfGhostEvidence = 0;
	
	while (hunter->fearTimer < 100 && hunter->boredomTimer > 0){
		//room has ghost
		if (hunter->room->ghost != NULL){
			hunter->fearTimer++;
			hunter->boredomTimer = BOREDOM_MAX;
		}
		
		//another hunter
		int choice;
		int ghostEvidenceCheck;
		if (hunter->room->hunters.size > 1){
			choice = randInt(1,4);
			
			//choice 1 collects
			if (choice == 1){
				ghostEvidenceCheck = collectEvidence(&hunter);
				amountOfGhostEvidence += ghostEvidenceCheck;
				
				if (ghostEvidenceCheck == 1) hunter->boredomTimer = BOREDOM_MAX;
			}
			
			//choice 2 means move
			else if (choice == 2){
				moveHunter(&hunter);
				hunter->boredomTimer--;
			}
			
			//give evidence to other hunters
			else if (choice == 3){
				communicateEvidence(&hunter);
			}
		}
		
		//only yourself
		else{
			choice = randInt(1,3);
			
			//choice 1 collect
			if (choice == 1){
				ghostEvidenceCheck = collectEvidence(&hunter);
				amountOfGhostEvidence += ghostEvidenceCheck;
				
				if (ghostEvidenceCheck == 1) hunter->boredomTimer = BOREDOM_MAX;
			}
			
			//choice 2 move
			else if (choice == 2){
				moveHunter(&hunter);
				hunter->boredomTimer--;
			}
		}
		
		if (amountOfGhostEvidence == 3) break;
		
	}

	return (0);
}

int collectEvidence(HunterType **hunter){
	//move through rooms linked list and compare if evidenceType matches
	if((*hunter)->room->evidence->head != NULL){
		//walk through linked list
		//return 1 if ghostly evidence type matches 
		EvidenceNodeType *pointer = (*hunter)->room->evidence->head;
		EvidenceNodeType *follow = NULL;
		
		while(pointer != NULL){
			if(pointer->evidence->evidenceType == (*hunter)->readableEvidence){
				//put evidence from room into hunters evidence linked list
				//->use tail
				//can we just set it to pointer?
				if((*hunter)->evidence->head == NULL){
					(*hunter)->evidence->head = pointer; //?
					(*hunter)->evidence->tail = (*hunter)->evidence->head;
				}
				else{
					(*hunter)->evidence->tail->next = pointer; //?
					(*hunter)->evidence->tail = pointer;
				}
				
				
				follow->next = pointer->next;
				//found ghostly evidence
				return 1;
			}
			follow = pointer;
			pointer = pointer->next;
		}
	}
	
	//create random standard evidence
	//reused switch case
	
	float evidenceValue;
	switch ((*hunter)->readableEvidence){
		case EMF:
			evidenceValue = randFloat(0.0, 4.90);
			break;
		
		case TEMPERATURE:
			evidenceValue = randFloat(0.0, 27.00);
			break;
		
		case FINGERPRINTS:
			evidenceValue = 0;
			break;
		
		case SOUND:
			evidenceValue = randFloat(40.00, 70.00);
			break;
	}
	
	EvidenceType* newEvidence = calloc(1, sizeof(EvidenceType));
	initEvidenceType(newEvidence, (*hunter)->readableEvidence, evidenceValue, 0);
	
	EvidenceNodeType* newEvidenceNode = calloc(1, sizeof(EvidenceNodeType));
    	newEvidenceNode->evidence = newEvidence;
	
	addEvidence((*hunter)->evidence, newEvidenceNode);
	//no ghostly evidence 
	return 0;	
		
}

void moveHunter(HunterType** hunter){
	RoomType* currRoom = (*hunter)->room;
	//delete hunter from hunter array
	//loop through array and compare? - move all items in hunter array?
	
	//1 hunter in room
	if(currRoom->hunters.size == 1){
		//how to access hunters array as a pointer??
		currRoom->hunters.hunters[0] = NULL;
		currRoom->hunters.size = 0;
		return;
	}
	
	int hunterPos;
	for(int i = 0; i < currRoom->hunters.size; i++){
		//comparing addresses
		if(currRoom->hunters.hunters[i] == (*hunter)){
			hunterPos = i;
			break;
		}
		
	}
	
	for(int i = hunterPos; i < currRoom->hunters.size; i++){
		currRoom->hunters.hunters[i] = currRoom->hunters.hunters[i+1];
	}
	//1 less hunter in room now
	currRoom->hunters.size--;
	
	//move to different room
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
	
	(*hunter)->room = currRoomChoice->room;
	//update hunter arr for curr room 
	addHunter(&currRoomChoice->room->hunters, (*hunter));
}

//communicate function
void communicateEvidence(HunterType** hunter){
	//get random hunter number in size of arraylist
	int hunterChoice = randInt(0, (*hunter)->room->hunters.size);
	//if its the curr hunter try again
	while ((*hunter)->room->hunters.hunters[hunterChoice] == (*hunter)){
		hunterChoice = randInt(0, (*hunter)->room->hunters.size);
	}
	
	HunterType* otherHunter = (*hunter)->room->hunters.hunters[hunterChoice];
	
	//loop through current hunter's evidence and its its ghostly add it to the other hunters evidence list
	EvidenceNodeType* currEvidence = (*hunter)->evidence->head;
	while(currEvidence != NULL){
		//if its ghost evidence append to other hunter list
		//decided to duplicate evidence
		if(currEvidence->evidence->ghostEvidence){
			
			EvidenceType* newEvidence = calloc(1, sizeof(EvidenceType));
			initEvidenceType(newEvidence, currEvidence->evidence->evidenceType, currEvidence->evidence->value, 1);
			
			EvidenceNodeType* newEvidenceNode = calloc(1, sizeof(EvidenceNodeType));
		    	newEvidenceNode->evidence = newEvidence;
			
			addEvidence(otherHunter->evidence, newEvidenceNode);
		}
		currEvidence = currEvidence->next;
	}
}



void initHunter(HunterType** hunter, RoomNodeType* roomNode, EvidenceClassType readableEvidence, char* name){
	(*hunter)->room = roomNode->room;
	(*hunter)->readableEvidence = readableEvidence;
	
	EvidenceListType* evidenceList = malloc(sizeof(EvidenceListType));
	(*hunter)->evidence = evidenceList;
	initEvidenceList((*hunter)->evidence);
	
	strcpy((*hunter)->name, name);
	(*hunter)->fearTimer = 0;
	
	(*hunter)->boredomTimer = BOREDOM_MAX;
}

void initHunterArray(HunterArrayType* hunterList){
	hunterList->size = 0;
}

void addHunter(HunterArrayType* hunterList, HunterType* hunter){
	hunterList->hunters[hunterList->size] = hunter;
	hunterList->size++;
}

void cleanupHunterArray(HunterArrayType* hunterArray){
	for (int i = 0; i < hunterArray->size; i++){
		cleanupEvidenceData(hunterArray->hunters[i]->evidence);
		cleanupEvidenceList(hunterArray->hunters[i]->evidence);
		free(hunterArray->hunters[i]->evidence);
		free(hunterArray->hunters[i]);
	}
}
