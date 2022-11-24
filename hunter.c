#include "defs.h"

void initHunter(HunterType** hunter, RoomNodeType* roomNode, EvidenceClassType readableEvidence, char* name){
	(*hunter)->room = roomNode->room;
	(*hunter)->readableEvidence = readableEvidence;
	
	EvidenceListType* evidenceList = malloc(sizeof(EvidenceListType));
	(*hunter)->evidence = evidenceList;
	initEvidenceList((*hunter)->evidence);
	
	strcpy((*hunter)->name, name);
	(*hunter)->fearTimer = 0;
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
		free(hunterArray->hunters[i]->evidence);
		free(hunterArray->hunters[i]);
	}
}
