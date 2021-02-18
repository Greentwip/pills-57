
#if !defined(_DEBUG_H_)
#define _DEBUG_H_

// 最終提出定義
#undef NINTENDO
#if defined(NDEBUG)
#define NINTENDO
#endif

// ROMバージョン表示
#undef ROM_VERSION
//#define ROM_VERSION

// EEPRomを使用する
#undef USE_EEP
#define USE_EEP

// HALTを実行する
#undef USE_HALT
//#define USE_HALT

// コンソール出力マクロ
#if defined(DEBUG)
#define PRT0(fmt)                 osSyncPrintf(fmt)
#define PRT1(fmt,a1)              osSyncPrintf(fmt,(a1))
#define PRT2(fmt,a1,a2)           osSyncPrintf(fmt,(a1),(a2))
#define PRT3(fmt,a1,a2,a3)        osSyncPrintf(fmt,(a1),(a2),(a3))
#define PRT4(fmt,a1,a2,a3,a4)     osSyncPrintf(fmt,(a1),(a2),(a3),(a4))
#define PRTFLN()                  PRT1(__FILE__"(%d)\n",__LINE__)
#define PRTFL()                   PRT1(__FILE__"(%d): ",__LINE__)
#define DBG0(flg,fmt)             (!(flg)?((void)0):(PRTFL(),PRT0(fmt)))
#define DBG1(flg,fmt,a1)          (!(flg)?((void)0):(PRTFL(),PRT1(fmt,(a1))))
#define DBG2(flg,fmt,a1,a2)       (!(flg)?((void)0):(PRTFL(),PRT2(fmt,(a1),(a2))))
#define DBG3(flg,fmt,a1,a2,a3)    (!(flg)?((void)0):(PRTFL(),PRT3(fmt,(a1),(a2),(a3))))
#define DBG4(flg,fmt,a1,a2,a3,a4) (!(flg)?((void)0):(PRTFL(),PRT4(fmt,(a1),(a2),(a3),(a4))))
#else // DEBUG
#define PRT0(fmt)                 ((void)0)
#define PRT1(fmt,a1)              ((void)0)
#define PRT2(fmt,a1,a2)           ((void)0)
#define PRT3(fmt,a1,a2,a3)        ((void)0)
#define PRT4(fmt,a1,a2,a3,a4)     ((void)0)
#define PRTFLN()                  ((void)0)
#define PRTFL()                   ((void)0)
#define DBG0(flg,fmt)             ((void)0)
#define DBG1(flg,fmt,a1)          ((void)0)
#define DBG2(flg,fmt,a1,a2)       ((void)0)
#define DBG3(flg,fmt,a1,a2,a3)    ((void)0)
#define DBG4(flg,fmt,a1,a2,a3,a4) ((void)0)
#endif // DEBUG

// バージョン情報
#if defined(ROM_VERSION)
extern const char _debugVersionDate[];
extern const char _debugVersionTime[];
#endif

#endif // _DEBUG_H_
