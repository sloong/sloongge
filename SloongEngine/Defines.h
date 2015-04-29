#pragma once

#define WM_SENDEVENT WM_USER + 1

#define FULLSCREEN		false

#define WINDOW_WIDTH    800   // size of window
#define WINDOW_HEIGHT   600


#define SCREEN_WIDTH	1366
#define SCREEN_HEIGHT	768
#define SCREEN_BPP      32    // bits per pixel

#define OFFSURFACE_WIDTH	76
#define OFFSURFACE_HEIGHT	57

// defines for ants
#define NUM_ANTS         5 // just change this to whatever, but only 8 ants will be displayed
#define NUM_ANTS_FORM		9


#define RAND_RANGE(x,y) ( (x) + (rand()%((y)-(x)+1)))

