#include "defs.h"

void initEvidenceList(EvidenceListType* evidenceList){
	evidenceList->head = NULL;
	evidenceList->tail = NULL;
}

void initEvidenceType(EvidenceType* evidence, EvidenceClassType evidenceType, float value, int ghostEvidence){
	evidence->evidenceType = evidenceType;
	evidence->value = value;
	evidence->ghostEvidence = ghostEvidence;
}


void addEvidence(EvidenceListType* list, EvidenceNodeType* evidence){
	//If an empty list, the evidence will be the head and tail
	if (list->head == NULL){
		list->head = evidence;
		list->tail = evidence;
		return;
	}
	
	//Not an empty list
	list->tail->next = evidence;
	list->tail = evidence;
}

void cleanupEvidenceData(EvidenceListType* list){
	EvidenceNodeType* curr = list->head;
	if (curr == NULL) return;
	
	while(1){
		free(curr->evidence);
		if (curr->next == NULL) break;
		curr = curr->next;
	}
}

void cleanupEvidenceList(EvidenceListType* list){
	EvidenceNodeType* curr = list->head;
	EvidenceNodeType* next = NULL;
	
	while(1){
		if (curr == NULL) break;
		next = curr->next;
		free(curr);
		curr = next;
	}
}
