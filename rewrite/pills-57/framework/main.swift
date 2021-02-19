enum MAIN_NO: Int{
	case MAIN_11 = 0,
	MAIN_12,
	MAIN_STORY,
	MAIN_TITLE,
	MAIN_MANUAL,
	MAIN_GAME,
	MAIN_MENU,
	MAIN_TECHMES
}

class main{
    var main_no: MAIN_NO;
    var main_old: MAIN_NO?;
    var main_bak: MAIN_NO?;

	public init(){
		self.main_no = MAIN_NO.MAIN_11
    }

	func stopMainThread(){

    }

	func setIdleFunc(_ action: () -> void){

	}

	func update(){
		main_bak = main_no;

		switch(self.main_no){
			case .MAIN_11:
				self.main_no = main11();
			    break;

			case .MAIN12:
				self.main_no = main12();
			    break;

			case .MAIN_TITLE:
				self.main_no = dm_title_main();
			    break;

			case .MAIN_MANUAL:
				self.main_no = dm_manual_main();
				break;

			case .MAIN_MENU:
				self.main_no = main_menu();
				break;

			case .MAIN_TECHMES:
				self.main_no = main_techmes();




		
        }
    }

}

