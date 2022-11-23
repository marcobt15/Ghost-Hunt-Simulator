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
#define MAX_HUNTERS         4
#define USLEEP_TIME     50000
#define BOREDOM_MAX        99

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
} HunterType;

typedef struct{
	HunterType* hunters[MAX_HUNTERS];
	int size;
} HunterArrayType;

//evidence stuff
//maybe add more to this not sure yet
typedef struct{
	EvidenceClassType evidenceType;
	int value;
} EvidenceType;
 
typedef struct{
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
} RoomType;

typedef struct RoomNode{
	RoomType* room;
	struct RoomNode* next;
} RoomNodeType;

typedef struct RoomList{
	RoomNodeType* head;
	RoomNodeType* tail; //for insertion
} RoomListType;

//building stuff
typedef struct{
	GhostType* ghost;
	HunterArrayType hunters;
	RoomListType rooms;
} BuildingType;

//main functions
int randInt(int, int);          // Generates a pseudorandom integer between the parameters
float randFloat(float, float);  // Generates a pseudorandom float between the parameters

//Building fucntions
void populateRooms(BuildingType*);   // Populates the building with sample data for rooms
void cleanupBuilding(BuildingType*);
void initBuilding(BuildingType*);

//Room functions
void initRoom(RoomType**, char*);
void initRoomList(RoomListType*);
void appendRoom(RoomListType*, RoomNodeType*);
void connectRooms(RoomType*, RoomType*);
void cleanupRoomData(RoomListType*);
void cleanupRoomList(RoomListType*);

//Evidence functions
//void initEvidence();
void initEvidenceList(EvidenceListType*);

//Ghost functions
void initGhost(GhostType*, GhostClassType, RoomType*);

//Hunter functions
void initHunter(HunterType**, RoomNodeType*, EvidenceClassType, char*);
void initHunterArray(HunterArrayType*);
void addHunter(HunterArrayType*, HunterType*);
void cleanupHunterArray(HunterArrayType*);
