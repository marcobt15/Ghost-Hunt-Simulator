#include "defs.h"

void hunterThreadFunction(void* inputHunter){
	HunterType* hunter = (HunterType*) inputHunter;
	int amountOfGhostEvidence = 0;
	
	while (hunter->fear < 100 && hunter->boredomTimer > 0){
		//room has ghost
		if (hunter->room->ghost != NULL){
			hunter->fearTimer++;
			hunter->boredomTimer = BOREDOM_MAX;
		}
		
		//another hunter
		int choice;
		if (hunter->room->hunters.size > 1){
			choice = randInt(1,4);
			
			//choice
			if (choice == 1){
				amountOfGhostEvidence += collectEvidence(&hunter);
			}
			
			//choice 2 means move
			else if (choice == 2){
				moveHunter(&hunter);
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
				amountOfGhostEvidence += collectEvidence(&hunter);
			}
			
			//choice 2 move
			else if (choice == 2){
				moveHunter(&hunter);
			}
		}
		
		if (amountOfGhostEvidence == 3) break;
		
	}

	return (0);
}

void initHunter(HunterType** hunter, RoomNodeType* roomNode, EvidenceClassType readableEvidence, char* name){
	(*hunter)->room = roomNode->room;
	(*hunter)->readableEvidence = readableEvidence;
	
	EvidenceListType* evidenceList = malloc(sizeof(EvidenceListType));
	(*hunter)->evidence = evidenceList;
	initEvidenceList((*hunter)->evidence);
	
	strcpy((*hunter)->name, name);
	(*hunter)->fearTimer = 0;
	
	(*hunter)->boredomTimer = BOREDOME_MAX;
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
