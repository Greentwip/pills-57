
let LEVLIMIT = 23;

let MFieldX  = 8;

let MFieldY  = 17



let ROOTCNT		= 50;

enum Lines{	

	case LnEraseLin,	

	LnEraseVrs,	

	LnRinsetsu,	

	LnLinesAll,	

	LnOnLinVrs,	

	LnLinSpace,	

	LnEraseVrsSide,

	LnHighCaps,	

	LnHighVrs,	

	LnNonCount,	

	LnTableMax

};

struct AI_FLAG{

	var	ok: UInt8?;

	var tory: UInt8?;

	var x: UInt8?;

	var y: UInt8?;

	var rev: UInt8?;

	var ccnt: UInt8?;

	var pri: Int32?;

	var dead: Int32?;

	var hei: [[UInt8]]?; //[2][LnTableMax]

	var wid: [[UInt8]]?; //[2][LnTableMax]

	var elin: [UInt8]?; //[2]

	var only: [UInt8]?; //[2]

	var wonly: [UInt8]?; //[2]

	var sub: UInt8;

	var rensa: UInt8;

} 



struct AI_ROOT{

	var x: UInt8?;

	var y: UInt8?;

}


let NUM_AI_EFFECT = 16;


let AIF_DAMAGE	= 0x01;

let AIF_DAMAGE2	= 0x02;

let AIF_AGAPE	= 0x04;

struct AIWORK{

	var aiFlagDecide: AI_FLAG?;

	var aiRootDecide: [AI_ROOT]?; //[ROOTCNT]

	var aiKeyCount: UInt8?;

	var aiSpeedCnt: UInt8?;

	var aiKRFlag: UInt8?;

	var aiRollCnt: UInt8?;

	var aiRollFinal: UInt8?;				

	var aiRollHabit: UInt8?;

	var aiSpUpFlag: UInt8?;

	var aiSpUpStart: UInt8?;

//	var aiSpUpCnt: UInt8?;

	var aivl: UInt8?;   // virus level

	var aiok: UInt8?;

	var aiRandFlag: UInt8?;

	var aiEX: UInt8?;

	var aiEY: UInt8?;

	var aiOldRollCnt: UInt8?;

//	var aiNum: UInt8?;

//	var aiTimer: UInt8?;


	var aiEffectNo: [UInt8]?; //[NUM_AI_EFFECT]

	var aiEffectParam: [Int16]?; //[NUM_AI_EFFECT]

	var aiEffectCount: [Int16]?; //[NUM_AI_EFFECT]



	var aiState: UInt8?;

	var	aiSelSpeed: UInt8?;

	var aiRootP: UInt8?;

	var aiPriOfs: UInt16?;

}


struct AICAPS{

	var mx: UInt8?
    var my: UInt8?;

	var ca: UInt8?; //COL_?(RED/YELLOW/...)

	var cb: UInt8?;

	var sp: UInt8?;			// FallSpeed

	var cn: UInt8?;

} 



struct AIBLK{

	var st: UInt8?;

	var co: UInt8?; //COL_?(RED/YELLOW/...)

}

