#include "evsworks.h"

//#define	VRS_EQ_CNTa	15
//#define	VRS_EQ_CNTb	 8
//extern u8	aiVirusCnt;
//extern u8	aiVrsEqCnt;
//extern u8	aiVrsEqMax;

extern	u8	aiUsedFlag;
extern	u8	aiDebugP1;

extern	void aifMake(game_state *);
extern	void aifMakeFlagSet(game_state *);
extern	void aifKeyOut(game_state *);
extern	void aifGameInit(void);
extern	void aifFirstInit(void);

extern	void aifFieldCopy(game_state *);
//extern	void aifRecurCopy(void);
extern	void aifPlaceSearch(void);
extern	void aifMoveCheck(void);
extern	void aifTRecur(u8 ,u8 ,u8);
extern	void aifTRecurUP(u8 ,u8 ,u8);
extern	void aifYRecur(u8 ,u8 ,u8);
extern	void aifYRecurUP(u8 ,u8 ,u8);
//extern	int	 aifPriority(u8 own,u8 vir,u8 cap,u8 sub,u8 under,u8 tory);
//extern	int	 aifTEffect(u8 no,u8 ca,u8 cb);
//extern	int	 aifYEffect(u8 no,u8 ca,u8 cb);
extern	void aiHiruAllPriSet(game_state *);
extern	void aiSetCharacter(game_state *);

//extern	void aifPlaceDecide(void);
extern	void aifReMoveCheck(void);
extern	void aifKeyMake(game_state *);
//extern	void aifDownNumberReSet(int num);
//extern	void aifUpLimitCheck(void);
extern	void aiHiruSideLineEraser(game_state *);
