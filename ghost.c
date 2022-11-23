#include "defs.h"

void initGhost(GhostType* ghost, GhostClassType ghostClass, RoomType* room){
	ghost->ghostType = ghostClass;
	ghost->room = room;
	ghost->boredomTimer = BOREDOM_MAX;
}
