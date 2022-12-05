#include "defs.h"

void initEvidenceList(EvidenceListType* evidenceList){
	evidenceList->head = NULL;
	evidenceList->tail = NULL;
}

void initEvidenceType(EvidenceType* evidence, EvidenceClassType evidenceType, float value){
	evidence->evidenceType = evidenceType;
	evidence->value = value;
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

int ghostEvidenceCheck(EvidenceClassType e, float value){
	switch(e){
		case EMF:
			if (value <= 4.9) return N_G_E;
			break;
		case TEMPERATURE:
			if (value >= 0) return N_G_E;
			break;
		case FINGERPRINTS:
			if (value == 0) return N_G_E;
			break;
		case SOUND:
			if (value <= 70) return N_G_E;
			break;
	}
	return G_E;
}

char* getEvidenceName(EvidenceClassType e){
	switch(e){
		case EMF:
			return "EMF";
		case TEMPERATURE:
			return "TEMPERATURE";
		case FINGERPRINTS:
			return "FINGERPRINT";
		case SOUND:
			return "SOUND";
	}
}
