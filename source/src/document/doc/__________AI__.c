ドクマリＮ６４ ＡＩ処理アイドルスレッドへの移行方法

１、３３３行の以下のプログラムをコメントアウト
	if ( aiUsedFlag ) return;								// １イント中に誰かが思考処理を行なった？
			↓	↓
//	if ( aiUsedFlag ) return;								// １イント中に誰かが思考処理を行なった？

２、アイドルスレッドから「aifMake」を呼び出すように変更

３、アイドルスレッド起動前に「aifFirstInit」を呼び出すように変更

以上。
