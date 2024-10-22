#include "defs.h"

void* hunterThreadFunction(void* inputHunter){
	HunterType* hunter = (HunterType*) inputHunter;
	
	while(1){
		RoomType* currRoom = hunter->room;
		sem_wait(&(currRoom->mutex));
		
		if (currRoom->ghost != NULL){
			hunter->fearTimer += FEAR_RATE;
			if (hunter->fearTimer == MAX_FEAR){
				printf("%s got too scared and left the building\n", hunter->name);
				sem_post(&(currRoom->mutex));
				break;
			}
			hunter->boredomTimer = BOREDOM_MAX;
		}
		
		if (currRoom->hunters.size > 1){
			int choice = randInt(0,3);
			if (choice == 0){
				collectEvidence(hunter);
				//printEvidence(hunter);
			}
			
			else if (choice == 1){
				moveHunter(hunter);
			}
			
			else{
				communicateEvidence(hunter);
			}
		}
		else{
			int choice = randInt(0,2);
			if (choice == 0){
				collectEvidence(hunter);
				//printEvidence(hunter);
			}
			
			else{
				moveHunter(hunter);
			}
		}
		sem_post(&(currRoom->mutex));
		
		if (checkForGhostEvidence(hunter->evidence) == 3){
			printf("%s has found enough evidence and left the building\n", hunter->name);
			break;
		}
		
		else if (hunter->boredomTimer <= 0){
			printf("%s got too bored and left the building\n", hunter->name);
			break;
		}
		
		usleep(USLEEP_TIME);
	}
	
	removeHunterFromRoom(hunter->room, hunter);
	
	return (0);
}

void communicateEvidence(HunterType* hunter){
	
	int hunterChoice = randInt(0, hunter->room->hunters.size);
	while (hunter->room->hunters.hunters[hunterChoice] == hunter){
		hunterChoice = randInt(0, hunter->room->hunters.size);
	}
	HunterType* otherHunter = hunter->room->hunters.hunters[hunterChoice];
	
	//To keep evidence lists shorter
	removeStandardEvidence(hunter);
	if(hunter->evidence->head == NULL) return;
	
	if(otherHunter->evidence->head == NULL){
		otherHunter->evidence->head = hunter->evidence->head;
		otherHunter->evidence->tail = hunter->evidence->tail;
	}
	
	else{
		if (otherHunter->evidence->head != NULL && otherHunter->evidence->tail == NULL) printf("ISSUE"); 
		otherHunter->evidence->tail->next = hunter->evidence->head;
		otherHunter->evidence->tail = hunter->evidence->tail;
	}
	
	hunter->evidence->tail = NULL;
	hunter->evidence->head = NULL;
	printf("%s just gave their evidence to %s\n", hunter->name, otherHunter->name);
	//printEvidence(otherHunter);
}

void removeStandardEvidence(HunterType* hunter){
	EvidenceNodeType* currEvi = hunter->evidence->head;
	EvidenceNodeType* prev = NULL;
	while(currEvi != NULL){
		if (ghostEvidenceCheck(currEvi->evidence->evidenceType, currEvi->evidence->value) == N_G_E){
			free(currEvi->evidence);
			if (currEvi == hunter->evidence->head && currEvi == hunter->evidence->tail){
				free(currEvi);
				hunter->evidence->head = NULL;
				hunter->evidence->tail = NULL;
				break;
			}
			else if (currEvi == hunter->evidence->head){
				hunter->evidence->head = currEvi->next;
				free(currEvi);
				currEvi = hunter->evidence->head;
			}
			else if (currEvi == hunter->evidence->tail){
				hunter->evidence->tail = prev;
				free(currEvi);
				prev->next = NULL;
				break;
			}
			else{
				prev->next = currEvi->next;
				free(currEvi);
				currEvi = prev->next;
			}
		}
		else{
			prev = currEvi;
			currEvi = currEvi->next;
		}
	}
}

void moveHunter(HunterType* hunter){
	RoomType* currRoom = hunter->room;
	
	int adjacentRoomNum = currRoom->rooms->totalRooms;
	RoomNodeType* currRoomChoice;
	
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
		hunter->room = currRoomChoice->room;
		addHunter(&(currRoomChoice->room->hunters), hunter);
		removeHunterFromRoom(currRoom, hunter);
		
		printf("%s moved to %s \n", hunter->name, hunter->room->name);
		sem_post(&(currRoomChoice->room->mutex));
		return;
	}

	printf("%s could not move\n", hunter->name);
	
}

void collectEvidence(HunterType* hunter){
	EvidenceNodeType* currEvi = hunter->room->evidence->head;
	EvidenceNodeType* prevEvi = NULL;
	
	if(hunter->room->evidence->head != NULL){
		while(currEvi != NULL){
			if (currEvi->evidence->evidenceType == hunter->readableEvidence){
				
				addEvidence(hunter->evidence, currEvi);
				/*
				if (hunter->evidence->head == NULL){
					hunter->evidence->head == currEvi;
					hunter->evidence->tail == currEvi;
				}
				
				else{
					hunter->evidence->tail->next == currEvi;
					hunter->evidence->tail == currEvi;
				}
				*/
				
				//1 element in room
				if (hunter->room->evidence->head == currEvi && hunter->room->evidence->tail == currEvi){
					hunter->room->evidence->head = NULL;
					hunter->room->evidence->tail = NULL;
					break;
				}
				
				else if (hunter->room->evidence->head == currEvi){
					hunter->room->evidence->head = currEvi->next;
					currEvi = hunter->room->evidence->head;
				}
				
				else if (hunter->room->evidence->tail == currEvi){
					hunter->room->evidence->tail = prevEvi;
					prevEvi->next = NULL;
					break;
				}
				
				else{
					prevEvi->next = currEvi->next;
					currEvi = currEvi->next;
				}
				
				hunter->evidence->tail->next = NULL;
				
				
				if (ghostEvidenceCheck(hunter->evidence->tail->evidence->evidenceType, hunter->evidence->tail->evidence->value) == G_E){
					printf("%s has collected GHOSTLY %s evidence from %s\n", hunter->name, getEvidenceName(hunter->readableEvidence), hunter->room->name);
					hunter->boredomTimer = BOREDOM_MAX;
				}
				else{
					printf("%s has collected %s evidence from %s\n", hunter->name, getEvidenceName(hunter->readableEvidence), hunter->room->name);
					hunter->boredomTimer--;
				}
				
			}
			
			else{
				prevEvi = currEvi;
				currEvi = currEvi->next;
			}
			
		}
		return;
	}
	
	float evidenceValue;
	switch (hunter->readableEvidence){
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
	initEvidenceType(newEvidence, hunter->readableEvidence, evidenceValue);
	
	EvidenceNodeType* newEvidenceNode = calloc(1, sizeof(EvidenceNodeType));
    	newEvidenceNode->evidence = newEvidence;
    	newEvidenceNode->next = NULL;
	
	addEvidence(hunter->evidence, newEvidenceNode);
	printf("%s collects RANDOM %s evidence in %s \n", hunter->name, getEvidenceName(hunter->readableEvidence), hunter->room->name);
	hunter->boredomTimer--;
}

int checkForGhostEvidence(EvidenceListType* list){
	EvidenceNodeType* e = list->head;
	
	int diffEvidenceCollected = 0;
	int foundEMF = 0;
	int foundTemp = 0;
	int foundPrints = 0; 
	int foundSound = 0; 
	
	while(e != NULL){
		if (ghostEvidenceCheck(e->evidence->evidenceType, e->evidence->value) == G_E){
			switch(e->evidence->evidenceType){
				case EMF:
					if (foundEMF == 0){
						diffEvidenceCollected++;
						foundEMF = 1;
					}
					break;
					
				case TEMPERATURE:
					if (foundTemp == 0){
						diffEvidenceCollected++;
						foundTemp = 1;
					}
					break;
					
				case FINGERPRINTS:
					if (foundPrints == 0){
						diffEvidenceCollected++;
						foundPrints = 1;
					}
					break;
					
				case SOUND:
					if (foundSound == 0){
						diffEvidenceCollected++;
						foundSound = 1;
					}
					break;
			}
		}
		if (diffEvidenceCollected == 3) break;
		e = e->next;
	}
	
	return diffEvidenceCollected;
}

void initHunterArray(HunterArrayType* hunterList){
	hunterList->size = 0;
}

void initHunter(HunterType* hunter, RoomNodeType* roomNode, EvidenceClassType readableEvidence, char* name){
	hunter->room = roomNode->room;
	
	addHunter(&(roomNode->room->hunters), hunter);
	
	hunter->readableEvidence = readableEvidence;
	
	EvidenceListType* evidenceList = malloc(sizeof(EvidenceListType));
	hunter->evidence = evidenceList;
	initEvidenceList(hunter->evidence);
	
	strcpy(hunter->name, name);
	hunter->fearTimer = 0;
	hunter->boredomTimer = BOREDOM_MAX;
}


void addHunter(HunterArrayType* hunterList, HunterType* hunter){
	hunterList->hunters[hunterList->size] = hunter;
	hunterList->size++;
}

void removeHunterFromRoom(RoomType* currRoom, HunterType* hunter){
	//1 hunter in room
	if(currRoom->hunters.size == 1){
		//how to access hunters array as a pointer??
		currRoom->hunters.hunters[0] = NULL;
		currRoom->hunters.size = 0;
	}
	
	else{
		int hunterPos;
		for(int i = 0; i < currRoom->hunters.size; i++){
			//comparing addresses
			if(currRoom->hunters.hunters[i] == hunter){
				hunterPos = i;
				break;
			}
			
		}
		
		//1 less hunter in room now
		currRoom->hunters.size--;
		for(int i = hunterPos; i < currRoom->hunters.size; i++){
			currRoom->hunters.hunters[i] = currRoom->hunters.hunters[i+1];
		}
	}
}


void determineGhostType(HunterType* hunter, GhostClassType* ghostClass){
	EvidenceNodeType* currEvi = hunter->evidence->head;
	int diffEvidenceCollected = 0;
	int foundEMF = 0;
	int foundTemp = 0;
	int foundPrints = 0;
	int foundSound = 0;
	while(currEvi != NULL && diffEvidenceCollected < 3){
		if (ghostEvidenceCheck(currEvi->evidence->evidenceType, currEvi->evidence->value) == G_E){
			switch(currEvi->evidence->evidenceType){
				case EMF:
					if (!foundEMF){
						diffEvidenceCollected++;
						foundEMF = 1;
					}
					break;
					
				case TEMPERATURE:
					if (!foundTemp){
						diffEvidenceCollected++;
						foundTemp = 1;
					}
					break;
					
				case FINGERPRINTS:
					if (!foundPrints){
						diffEvidenceCollected++;
						foundPrints = 1;
					}
					break;
					
				case SOUND:
					if (!foundSound){
						diffEvidenceCollected++;
						foundSound = 1;
					}
					break;
			}
		}
		currEvi = currEvi->next;
	}
	
	
	if (foundEMF && foundTemp && foundPrints){
		*ghostClass = 0;
	}
	
	else if (foundEMF && foundTemp && foundSound){
		*ghostClass = 1;
	}
	
	else if (foundEMF &&foundPrints && foundSound){
		*ghostClass = 2;
	}
	
	else if (foundTemp && foundPrints && foundSound){
		*ghostClass = 3;
	}
	
}

void cleanupHunterArray(HunterArrayType* hunterArray){
	for (int i = 0; i < hunterArray->size; i++){
		cleanupEvidenceData(hunterArray->hunters[i]->evidence);
		cleanupEvidenceList(hunterArray->hunters[i]->evidence);
		free(hunterArray->hunters[i]->evidence);
		free(hunterArray->hunters[i]);
	}
}

/*
void* hunterThreadFunction(void* inputHunter){
	HunterType* hunter = (HunterType*) inputHunter;
	
	//for checking ghost evidence it has
	int diffEvidenceCollected = 0;
	
	while (hunter->fearTimer < MAX_FEAR && hunter->boredomTimer > 0){
		RoomType* currRoom = hunter->room;
		sem_wait(&(currRoom->mutex));
	
		//room has ghost
		if (hunter->room->ghost != NULL){
			hunter->fearTimer++;
			hunter->boredomTimer = BOREDOM_MAX;
		}
		
		//another hunter
		int choice;
		if (hunter->room->hunters.size > 1){
			choice = randInt(1,4);
			
			//choice 1 collects
			if (choice == 1){
				if (collectEvidence(hunter)){
					hunter->boredomTimer = BOREDOM_MAX;
					removeStandardEvidence(hunter);
				}
				else hunter->boredomTimer--;
			}
			
			//choice 2 means move
			else if (choice == 2){
				moveHunter(hunter);
				hunter->boredomTimer--;
			}
			
			//give evidence to other hunters
			else if (choice == 3){
				communicateEvidence(hunter);
				hunter->boredomTimer--;
			}
		}
		
		//only yourself
		else{		
			choice = randInt(1,3);
			
			//choice 1 collect
			if (choice == 1){
				if (collectEvidence(hunter)){
					hunter->boredomTimer = BOREDOM_MAX;
					removeStandardEvidence(hunter);
				}
				else hunter->boredomTimer--;
			}
			
			//choice 2 move
			else if (choice == 2){
				moveHunter(hunter);
				hunter->boredomTimer--;
			}
		}
		
		sem_post(&(currRoom->mutex));
		
		diffEvidenceCollected = checkForGhostEvidence(hunter->evidence);
		
		if (diffEvidenceCollected == 3) break;
		
		usleep(USLEEP_TIME);
		//pData(hunter);
		//usleep(USLEEP_TIME*4);
		
	}
	
	removeHunterFromRoom(hunter->room, hunter);
	if (hunter->fearTimer >= MAX_FEAR) printf("%s GOT TOO SCARED AND LEFT THE BUILDING\n", hunter->name);
	else if (hunter->boredomTimer <= 0) printf("%s GOT TOO BORED AND LEFT THE BUILDING\n", hunter->name);
	else printf("%s COLLECTED ALL THE EVIDENCE IT NEEDED AND LEFT THE BUILDING\n", hunter->name);
	
	usleep(5000000);

	return (0);
}





int collectEvidence(HunterType* hunter){
	//move through rooms linked list and compare if evidenceType matches
	if(hunter->room->evidence->head != NULL){
		//walk through linked list
		//return 1 if ghostly evidence type matches 
		EvidenceNodeType *pointer = hunter->room->evidence->head;
		EvidenceNodeType *follow = NULL;
		
		while(pointer != NULL){
			if(pointer->evidence->evidenceType == hunter->readableEvidence){

				//put evidence from room into hunters evidence linked list
				//->use tail
				//can we just set it to pointer?
				if(hunter->evidence->head == NULL){
					hunter->evidence->head = pointer; //?
					hunter->evidence->tail = hunter->evidence->head;
				}
				else{
					hunter->evidence->tail->next = pointer; //?
					hunter->evidence->tail = pointer;
				}
					
				//1 element list
				if (pointer == hunter->room->evidence->head && pointer == hunter->room->evidence->tail){
					hunter->room->evidence->head = NULL;
					hunter->room->evidence->tail = NULL;
				}
				
				//>1 element and removing from tail
				else if (pointer == hunter->room->evidence->tail){
					hunter->room->evidence->tail = follow;
	
				}
				//>1 element and removing from middle
				else if (follow != NULL){ 
					follow->next = pointer->next;
				}
				
				//>1 element and removing from head
				else if (follow == NULL){ 
					hunter->room->evidence->head = pointer->next;
				}
				pointer->next = NULL; //If its added to new evidence list
			/*
				int dup = checkDuplicates(hunter->evidence, pointer->evidence->value);
				if (dup == N_DUPLICATE){
					//put evidence from room into hunters evidence linked list
					//->use tail
					//can we just set it to pointer?
					if(hunter->evidence->head == NULL){
						hunter->evidence->head = pointer; //?
						hunter->evidence->tail = hunter->evidence->head;
					}
					else{
						hunter->evidence->tail->next = pointer; //?
						hunter->evidence->tail = pointer;
					}
					//pointer->next = NULL; //If its added to new evidence list
				}
				
					
				//1 element list
				if (pointer == hunter->room->evidence->head && pointer == hunter->room->evidence->tail){
					hunter->room->evidence->head = NULL;
					hunter->room->evidence->tail = NULL;
					if (dup == DUPLICATE){
						free(pointer->evidence);
						free(pointer);
					}
					//else pointer->next = NULL; //If its added to new evidence list
					break;
	
				}
				
				//>1 element and removing from tail
				else if (pointer == hunter->room->evidence->tail){
					hunter->room->evidence->tail = follow;
					follow->next = NULL;
					if (dup == DUPLICATE) free(pointer);
					//else pointer->next = NULL; //If its added to new evidence list
					break;
	
				}
				//>1 element and removing from middle
				else if (follow != NULL){ 
					follow->next = pointer->next;
					if (dup == DUPLICATE){
						free(pointer);
						pointer = follow->next;
					}
					else{
						pointer->next = NULL; //If its added to new evidence list
					}
	
				}
				
				//>1 element and removing from head
				else if (follow == NULL){ 
					hunter->room->evidence->head = pointer->next;
					if (dup == DUPLICATE){
						follow = NULL;
						free(pointer);
						pointer = hunter->room->evidence->head;
					}
					else{
						pointer->next = NULL; //If its added to new evidence list
					}
					
				}
				
				if (dup == N_DUPLICATE){
					follow = pointer;
					pointer = pointer->next;
				}*/
				/*
			}
			
			
			follow = pointer;
			pointer = pointer->next;
			
		}
		
		
		//check if that new evidence is actually ghostly evidence for boredom timer
		
		printf("%s collects ghost evidence \n", hunter->name);
		return 1;
	}
	
	//create random standard evidence
	//reused switch case
	float evidenceValue;
	switch (hunter->readableEvidence){
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
	initEvidenceType(newEvidence, hunter->readableEvidence, evidenceValue);
	
	EvidenceNodeType* newEvidenceNode = calloc(1, sizeof(EvidenceNodeType));
    	newEvidenceNode->evidence = newEvidence;
    	newEvidenceNode->next = NULL;
	
	addEvidence(hunter->evidence, newEvidenceNode);
	printf("%s collects standard evidence \n", hunter->name);
	//no ghostly evidence 
	return 0;	
		
}

void moveHunter(HunterType* hunter){
	RoomType* currRoom = hunter->room;
	
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
	
	//if the room cannot be accessed don't move
	if (sem_trywait(&(currRoomChoice->room->mutex)) != 0) {
		printf("%s could not move\n", hunter->name);
		return;
	}
	
	//otherwise continue to do things
	
	hunter->room = currRoomChoice->room;
	//update hunter arr for curr room 
	addHunter(&(currRoomChoice->room->hunters), hunter);
	
	//delete hunter from hunter array
	//loop through array and compare? - move all items in hunter array?
	
	removeHunterFromRoom(currRoom, hunter);
	
	printf("%s moved to %s \n", hunter->name, hunter->room->name);
	sem_post(&(currRoomChoice->room->mutex));
}

void communicateEvidence(HunterType* hunter){
	//get random hunter number in size of arraylist
	int hunterChoice = randInt(0, hunter->room->hunters.size);
	//if its the curr hunter try again
	while (hunter->room->hunters.hunters[hunterChoice] == hunter){
		hunterChoice = randInt(0, hunter->room->hunters.size);
	}
	
	HunterType* otherHunter = hunter->room->hunters.hunters[hunterChoice];
	
	//loop through current hunter's evidence and its its ghostly add it to the other hunters evidence list
	EvidenceNodeType* currEvidence = hunter->evidence->head;
	while(currEvidence != NULL){
		//if its ghost evidence append to other hunter list
		//decided to duplicate evidence
		if(ghostEvidenceCheck(currEvidence->evidence->evidenceType, currEvidence->evidence->value) == G_E){
			if (checkDuplicates(otherHunter->evidence, currEvidence->evidence->value) == N_DUPLICATE){
				EvidenceType* newEvidence = calloc(1, sizeof(EvidenceType));
				initEvidenceType(newEvidence, currEvidence->evidence->evidenceType, currEvidence->evidence->value);
				
				EvidenceNodeType* newEvidenceNode = calloc(1, sizeof(EvidenceNodeType));
			    	newEvidenceNode->evidence = newEvidence;
				
				addEvidence(otherHunter->evidence, newEvidenceNode);
			}
		}
		currEvidence = currEvidence->next;
	}
	printf("%s gave their ghost evidence to %s\n", hunter->name, otherHunter->name);
}








int checkDuplicates(EvidenceListType* list, float value){
	EvidenceNodeType* currEvi = list->head;
	while(currEvi != NULL){
		switch (currEvi->evidence->evidenceType){
			case EMF:
				if (currEvi->evidence->value == value) return DUPLICATE;
				break;
				
			case TEMPERATURE:
				if (currEvi->evidence->value == value) return DUPLICATE;
				break;
				
			case FINGERPRINTS:
				if (currEvi->evidence->value == value) return DUPLICATE;
				break;
				
			case SOUND:
				if (currEvi->evidence->value == value) return DUPLICATE;
				break;
		}
		currEvi = currEvi->next;
	}
	return N_DUPLICATE;
}



*/
