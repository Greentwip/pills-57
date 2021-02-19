
let ANIME_STACK_DEPTH = 4;

struct SAnimeSeq {


	var labelStack: [UInt8]?; //[ANIME_STACK_DEPTH]
	var countStack: [UInt8]?; //[ANIME_STACK_DEPTH]
	var stackDepth: Int;

	var seqArray: [[UInt8]]?;

	
	var animeNo: Int?;

	
	var seqCount: Int?;

	
	var textureNo: Int;
};


struct SAnimeState {
	var animeSeq: [SAnimeSeq]?; //[1]
	var texArray: [STexInfo]?;
	var frameCount: UInt32?;
	var center: [Int]?; //[2]
	var charNo: Int?;
	var color: [Int]?; //[4]
};
