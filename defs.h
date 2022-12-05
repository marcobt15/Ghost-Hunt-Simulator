#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_STR            64
#define FEAR_RATE           1
#define MAX_FEAR          100
#define MAX_HUNTERS         4 // should be 4
#define USLEEP_TIME     50000
#define BOREDOM_MAX        99

#define N_G_E 		    0
#define G_E                 1

//GIVEN
// You may rename these types if you wish
typedef enum { EMF, TEMPERATURE, FINGERPRINTS, SOUND } EvidenceClassType;
typedef enum { POLTERGEIST, BANSHEE, BULLIES, PHANTOM } GhostClassType;

//ADDED

//ghost stuff
typedef struct{
	GhostClassType ghostType;
	struct roomType* room;
	int boredomTimer;
} GhostType;

//hunter stuff
typedef struct{
	struct roomType* room;
	EvidenceClassType readableEvidence;
	struct EvidenceList* evidence;
	char name[MAX_STR];
	int fearTimer;
	int boredomTimer;
} HunterType;

typedef struct{
	HunterType* hunters[MAX_HUNTERS];
	int size;
} HunterArrayType;

//evidence stuff
typedef struct{
	EvidenceClassType evidenceType;
	float value;
} EvidenceType;
 
typedef struct EvidenceNode{
	EvidenceType* evidence;
	struct EvidenceNode* next;
} EvidenceNodeType;

typedef struct EvidenceList{
	EvidenceNodeType* head;
	EvidenceNodeType* tail; //for insertion
} EvidenceListType;

//room stuff
typedef struct roomType{
	char name[MAX_STR];
	struct RoomList* rooms;
	EvidenceListType* evidence;
	HunterArrayType hunters;
	GhostType* ghost;
	sem_t mutex; //so only 1 thread can affect each room at a time
} RoomType;

typedef struct RoomNode{
	RoomType* room;
	struct RoomNode* next;
} RoomNodeType;

typedef struct RoomList{
	RoomNodeType* head;
	RoomNodeType* tail; //for insertion
	int totalRooms; //for moving in adjacent rooms and total adjacent rooms will not decrease
} RoomListType;

//building stuff
typedef struct{
	GhostType* ghost;
	HunterArrayType hunters;
	RoomListType* rooms;
} BuildingType;


void pData(HunterType*);



//main functions
int randInt(int, int);          // Generates a pseudorandom integer between the parameters
float randFloat(float, float);  // Generates a pseudorandom float between the parameters

//Building fucntions
void populateRooms(BuildingType*);   // Populates the building with sample data for rooms
void cleanupBuilding(BuildingType*);
void initBuilding(BuildingType*);

//Room functions
void initRoom(RoomType*, char*);
void initRoomList(RoomListType*);
void appendRoom(RoomListType*, RoomNodeType*);
void connectRooms(RoomType*, RoomType*);
void cleanupRoomData(RoomListType*);
void cleanupRoomList(RoomListType*);

//Evidence functions
void initEvidenceList(EvidenceListType*);
void initEvidenceType(EvidenceType*, EvidenceClassType, float);
void addEvidence(EvidenceListType*, EvidenceNodeType*);
void cleanupEvidenceData(EvidenceListType*);
void cleanupEvidenceList(EvidenceListType*);
int ghostEvidenceCheck(EvidenceClassType, float);

//Ghost functions
void* ghostThreadFunction(void*);
void leaveEvidence(GhostType*);
void moveRoom(GhostType*);
void initGhost(GhostType*, GhostClassType, RoomType*);
void getGhostName(GhostClassType, char*);

//Hunter functions
void* hunterThreadFunction(void*);
int checkForGhostEvidence(EvidenceListType*);
void removeStandardEvidence(HunterType*);
int collectEvidence(HunterType*);
void moveHunter(HunterType*);
void communicateEvidence(HunterType*);
void initHunter(HunterType*, RoomNodeType*, EvidenceClassType, char*);
void removeHunterFromRoom(RoomType*, HunterType*);
void determineGhostType(HunterType*, GhostClassType*);
void initHunterArray(HunterArrayType*);
void addHunter(HunterArrayType*, HunterType*);
void cleanupHunterArray(HunterArrayType*);
