#define C64_SCREEN_W	504
#define C64_SCREEN_H	312

#define C64_BORDER_W	404
#define C64_BORDER_H	284

#define C64_VIEW_W		320
#define C64_VIEW_H		200

#define C64_H_BLANK		((C64_SCREEN_W - C64_BORDER_W) / 2)
#define C64_V_BLANK		((C64_SCREEN_H - C64_BORDER_H) / 2)

#define C64_COL_CNT		40
#define C64_ROW_CNT		25

typedef U32 RGB_t;

int sdlThread(void * pData);
