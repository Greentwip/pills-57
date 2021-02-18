void	st_message_proc(void)
{
	// メッセージ表示
	if ( st_mes_ptr->frame > st_message_count ) {
		if ( st_mes_ptr[st_message_count+1].frame < framecont ) {
			msgWnd_clear(&mess_st);
			msgWnd_addStr(&mess_st, st_mes_ptr[st_message_count+1].mes);
			st_message_count++;

			if ( joynew[main_joy[0]] & ( CONT_A ) ) msgWnd_skip(&mess_st);

		}
	} else {
		st_message_count = 9999;
	}
}



//-------------------------------------------------------------------------
// Stage new
//-------------------------------------------------------------------------
void	story_st_new_op(Gfx **glp, int flg)
{
	Gfx			*pgfx;
	LMatrix 	pm;
	static		int	fin_frame, fin_demo;
	int			demo_no, mes_no, wk;

	pgfx = *glp;

	gSPDisplayList(pgfx++, normal_texture_init_dl);
	gSPDisplayList(pgfx++, story_setup);

	makeTransrateMatrix(pm, 0 * FIXPOINT, -25 * FIXPOINT, -1900 * FIXPOINT);

	switch ( story_seq_step ) {
		case 0:
			// カーテンを上げとメッセージウインドウの表示
			if ( flg ) demo_no = LWS_STORY_PA1; else demo_no = LWS_STORY_PA2;
			mes_no = 0;
			story_st_start();
			lws_anim(&pgfx, pm, (SCENE_DATA *)( ( (u32)lws_data[demo_no] & 0x00ffffff ) + story_buffer ), 0, story_buffer);
			framecont = 0;
			fin_frame = 0;
			fin_demo = demo_no;
			break;
		case 1:
			if ( flg ) {
				demo_no = LWS_STORY_PA1;
				mes_no = 0;
			} else {
				demo_no = LWS_STORY_PA2;
				mes_no = 1;
			}
			story_time_cnt = framecont;
			fin_frame = story_time_cnt;
			fin_demo = demo_no;

			st_mes_ptr = mes_data[mes_no];

			// とりあえず何か押したら抜ける
			if ( joyupd[main_joy[0]] & ( CONT_START | CONT_B ) ) {
				story_seq_step += 99;
				story_time_cnt = framecont = 0;
				st_message_count = 0;
			}

			// 早送り
			if ( joynew[main_joy[0]] & ( CONT_A ) ) {
				story_time_cnt += STORY_FF_SPEED;
				framecont += STORY_FF_SPEED;
			}

			// メッセージ処理
			st_message_proc();

			if ( lws_anim(&pgfx, pm, (SCENE_DATA *)( ( (u32)lws_data[demo_no] & 0x00ffffff ) + story_buffer ), story_time_cnt, story_buffer) == TRUE ) {
				if ( msgWnd_isEnd(&mess_st) != 0 ) {
					story_seq_step++;
					story_time_cnt = framecont = 0;
					st_message_count = 0;
				}
			}
			break;
		case 2:
			demo_no = LWS_STORY_PB;
			mes_no = 2;
			story_time_cnt = framecont;
			fin_frame = story_time_cnt;
			fin_demo = demo_no;

			st_mes_ptr = mes_data[mes_no];

			// とりあえず何か押したら抜ける
			if ( joyupd[main_joy[0]] & ( CONT_START | CONT_B ) ) {
				story_seq_step += 99;
				story_time_cnt = framecont = 0;
				st_message_count = 0;
			}

			// 早送り
			if ( joynew[main_joy[0]] & ( CONT_A ) ) {
				story_time_cnt += STORY_FF_SPEED;
				framecont += STORY_FF_SPEED;
			}

			// メッセージ処理
			st_message_proc();

			if ( lws_anim(&pgfx, pm, (SCENE_DATA *)( ( (u32)lws_data[demo_no] & 0x00ffffff ) + story_buffer ), story_time_cnt, story_buffer) == TRUE ) {
				if ( msgWnd_isEnd(&mess_st) != 0 ) {
					story_seq_step++;
					story_time_cnt = framecont = 0;
					st_message_count = 0;
				}
			}

			if ( fin_frame > 1565 ) {
				//フェードアウト
				wk = fin_frame - 1565;
				wk = ( ( 30 - wk ) * 255 ) / 30;
				gSPDisplayList(pgfx++, normal_texture_init_dl);
				story_spot(&pgfx, 160, 86, wk, changestar_tex);
			}
			break;
		case 3:
			demo_no = LWS_STORY_PC;
			mes_no = 3;
			story_time_cnt = framecont;
			fin_frame = story_time_cnt;
			fin_demo = demo_no;

			st_mes_ptr = mes_data[mes_no];

			// とりあえず何か押したら抜ける
			if ( joyupd[main_joy[0]] & ( CONT_START | CONT_B ) ) {
				story_seq_step += 99;
				story_time_cnt = framecont = 0;
				st_message_count = 0;
			}

			// 早送り
			if ( joynew[main_joy[0]] & ( CONT_A ) ) {
				story_time_cnt += STORY_FF_SPEED;
				framecont += STORY_FF_SPEED;
			}

			// メッセージ処理
			st_message_proc();

			if ( lws_anim(&pgfx, pm, (SCENE_DATA *)( ( (u32)lws_data[demo_no] & 0x00ffffff ) + story_buffer ), story_time_cnt, story_buffer) == TRUE ) {
				if ( msgWnd_isEnd(&mess_st) != 0 ) {
					story_seq_step++;
					story_time_cnt = framecont = 0;
					st_message_count = 0;
				}
			}

			if ( fin_frame < 30 ) {
				//フェードイン
				wk = fin_frame;
				wk = ( wk * 255 ) / 30;
				gSPDisplayList(pgfx++, normal_texture_init_dl);
				story_spot(&pgfx, 160, 86, wk, changestar_tex);
			}
			if ( fin_frame > 610 ) {
				//フェードアウト
				wk = fin_frame - 610;
				wk = ( ( 30 - wk ) * 255 ) / 30;
				gSPDisplayList(pgfx++, normal_texture_init_dl);
				story_spot(&pgfx, 160, 86, wk, changestar_tex);
			}
			break;
		case 4:
			demo_no = LWS_STORY_M01a;
			mes_no = 4;
			story_time_cnt = framecont;
			fin_frame = story_time_cnt;
			fin_demo = demo_no;

			st_mes_ptr = mes_data[mes_no];

			// とりあえず何か押したら抜ける
			if ( joyupd[main_joy[0]] & ( CONT_START | CONT_B ) ) {
				story_seq_step++;
				story_time_cnt = 0;
			}

			// 早送り
			if ( joynew[main_joy[0]] & ( CONT_A ) ) {
				story_time_cnt += STORY_FF_SPEED;
				framecont += STORY_FF_SPEED;
			}

			// メッセージ処理
			st_message_proc();

			if ( lws_anim(&pgfx, pm, (SCENE_DATA *)( ( (u32)lws_data[demo_no] & 0x00ffffff ) + story_buffer ), story_time_cnt, story_buffer) == TRUE ) {
				if ( msgWnd_isEnd(&mess_st) != 0 ) {
					story_seq_step++;
					story_time_cnt = 0;
				}
			}

			if ( fin_frame < 30 ) {
				//フェードイン
				wk = fin_frame;
				wk = ( wk * 255 ) / 30;
				gSPDisplayList(pgfx++, normal_texture_init_dl);
				story_spot(&pgfx, 160, 86, wk, changestar_tex);
			}
			break;
		default:
			// 終わり
			story_st_end();

			lws_anim(&pgfx, pm, (SCENE_DATA *)( ( (u32)lws_data[fin_demo] & 0x00ffffff ) + story_buffer ), fin_frame, story_buffer);
			break;
	}

	gDPSetScissor(pgfx++, G_SC_NON_INTERLACE, 0, 0, 320-1, 240-1);

	*glp = pgfx;
}



//-------------------------------------------------------------------------
// Stage new
//-------------------------------------------------------------------------
void	story_st_new(Gfx **glp, int demo_no, int mes_no)
{
	Gfx			*pgfx;
	LMatrix 	pm;
	static		int	fin_frame;
	SCENE_DATA	*scn_dat;

	pgfx = *glp;

	st_mes_ptr = mes_data[mes_no];

	gSPDisplayList(pgfx++, normal_texture_init_dl);
	gSPDisplayList(pgfx++, story_setup);

	makeTransrateMatrix(pm, 0 * FIXPOINT, -25 * FIXPOINT, -1900 * FIXPOINT);

	scn_dat = (SCENE_DATA *)( ( (u32)lws_data[demo_no] & 0x00ffffff ) + story_buffer );

	switch ( story_seq_step ) {
		case 0:
			// カーテンを上げとメッセージウインドウの表示
			story_st_start();

			lws_anim(&pgfx, pm, scn_dat, 0, story_buffer);
			framecont = 0;

			break;
		case 1:
			story_time_cnt = framecont;
			fin_frame = story_time_cnt;

			// とりあえず何か押したら抜ける
			if ( joyupd[main_joy[0]] & ( CONT_START | CONT_B ) ) {
				story_seq_step++;
				story_time_cnt = 0;
			}

			// 早送り
			if ( joynew[main_joy[0]] & ( CONT_A ) ) {
				story_time_cnt += STORY_FF_SPEED;
				framecont += STORY_FF_SPEED;
			}

			// メッセージ処理
			st_message_proc();

			if ( lws_anim(&pgfx, pm, scn_dat, story_time_cnt, story_buffer) == TRUE ) {
				if ( msgWnd_isEnd(&mess_st) != 0 ) {
					story_seq_step++;
					story_time_cnt = 0;
				}
			}
			break;
		default:
			// 終わり
			story_st_end();

			lws_anim(&pgfx, pm, scn_dat, fin_frame, story_buffer);
			break;
	}

	gDPSetScissor(pgfx++, G_SC_NON_INTERLACE, 0, 0, 320-1, 240-1);

	*glp = pgfx;
}


//-------------------------------------------------------------------------
// Stage new 2
//-------------------------------------------------------------------------
void	story_st_new2_f(Gfx **glp, int demo_no, int mes_no, int demo_no2, int mes_no2)
{
	Gfx			*pgfx;
	LMatrix 	pm;
	static		int	fin_frame, fin_demo;
	int			wk;

	pgfx = *glp;

	gSPDisplayList(pgfx++, normal_texture_init_dl);
	gSPDisplayList(pgfx++, story_setup);

	makeTransrateMatrix(pm, 0 * FIXPOINT, -25 * FIXPOINT, -1900 * FIXPOINT);

	lws_scene = (SCENE_DATA *)( ( (u32)lws_data[demo_no] & 0x00ffffff ) + story_buffer );

	switch ( story_seq_step ) {
		case 0:
			// カーテンを上げとメッセージウインドウの表示
			story_st_start();

			lws_anim(&pgfx, pm, (SCENE_DATA *)( ( (u32)lws_data[demo_no] & 0x00ffffff ) + story_buffer ), 0, story_buffer);
			framecont = 0;
			fin_frame = 0;
			fin_demo = demo_no;
			break;
		case 1:
			story_time_cnt = framecont;
			fin_frame = story_time_cnt;
			fin_demo = demo_no;

			st_mes_ptr = mes_data[mes_no];

			// とりあえず何か押したら抜ける
			if ( joyupd[main_joy[0]] & ( CONT_START | CONT_B ) ) {
				story_seq_step += 99;
				story_time_cnt = framecont = 0;
				st_message_count = 0;
			}

			// 早送り
			if ( joynew[main_joy[0]] & ( CONT_A ) ) {
				story_time_cnt += STORY_FF_SPEED;
				framecont += STORY_FF_SPEED;
			}

			// メッセージ処理
			st_message_proc();

			if ( lws_anim(&pgfx, pm, (SCENE_DATA *)( ( (u32)lws_data[demo_no] & 0x00ffffff ) + story_buffer ), story_time_cnt, story_buffer) == TRUE ) {
				if ( msgWnd_isEnd(&mess_st) != 0 ) {
					story_seq_step++;
					story_time_cnt = framecont = 0;
					st_message_count = 0;
				}
			}

			if ( fin_frame > ( lws_scene->last_frame - 32 ) ) {
				//フェードアウト
				wk = fin_frame - ( lws_scene->last_frame - 32 );
				wk = ( ( 30 - wk ) * 255 ) / 30;
				gSPDisplayList(pgfx++, normal_texture_init_dl);
				story_spot(&pgfx, 160, 86, wk, changestar_tex);
			}
			break;
		case 2:
			story_time_cnt = framecont;
			fin_frame = story_time_cnt;
			fin_demo = demo_no2;

			st_mes_ptr = mes_data[mes_no2];

			// とりあえず何か押したら抜ける
			if ( joyupd[main_joy[0]] & ( CONT_START | CONT_B ) ) {
				story_seq_step++;
				story_time_cnt = 0;
			}

			// 早送り
			if ( joynew[main_joy[0]] & ( CONT_A ) ) {
				story_time_cnt += STORY_FF_SPEED;
				framecont += STORY_FF_SPEED;
			}

			// メッセージ処理
			st_message_proc();

			if ( lws_anim(&pgfx, pm, (SCENE_DATA *)( ( (u32)lws_data[demo_no2] & 0x00ffffff ) + story_buffer ), story_time_cnt, story_buffer) == TRUE ) {
				if ( msgWnd_isEnd(&mess_st) != 0 ) {
					story_seq_step++;
					story_time_cnt = 0;
				}
			}

			if ( fin_frame < 30 ) {
				//フェードイン
				wk = fin_frame;
				wk = ( wk * 255 ) / 30;
				gSPDisplayList(pgfx++, normal_texture_init_dl);
				story_spot(&pgfx, 160, 86, wk, changestar_tex);
			}
			break;
		default:
			// 終わり
			story_st_end();

			lws_anim(&pgfx, pm, (SCENE_DATA *)( ( (u32)lws_data[fin_demo] & 0x00ffffff ) + story_buffer ), fin_frame, story_buffer);
			break;
	}

	gDPSetScissor(pgfx++, G_SC_NON_INTERLACE, 0, 0, 320-1, 240-1);

	*glp = pgfx;
}



//-------------------------------------------------------------------------
// Stage new 2
//-------------------------------------------------------------------------
void	story_st_new2(Gfx **glp, int demo_no, int mes_no, int demo_no2, int mes_no2)
{
	Gfx			*pgfx;
	LMatrix 	pm;
	static		int	fin_frame, fin_demo;

	pgfx = *glp;

	gSPDisplayList(pgfx++, normal_texture_init_dl);
	gSPDisplayList(pgfx++, story_setup);

	makeTransrateMatrix(pm, 0 * FIXPOINT, -25 * FIXPOINT, -1900 * FIXPOINT);

	switch ( story_seq_step ) {
		case 0:
			// カーテンを上げとメッセージウインドウの表示
			story_st_start();

			lws_anim(&pgfx, pm, (SCENE_DATA *)( ( (u32)lws_data[demo_no] & 0x00ffffff ) + story_buffer ), 0, story_buffer);
			framecont = 0;
			fin_frame = 0;
			fin_demo = demo_no;
			break;
		case 1:
			story_time_cnt = framecont;
			fin_frame = story_time_cnt;
			fin_demo = demo_no;

			st_mes_ptr = mes_data[mes_no];

			// とりあえず何か押したら抜ける
			if ( joyupd[main_joy[0]] & ( CONT_START | CONT_B ) ) {
				story_seq_step += 99;
				story_time_cnt = framecont = 0;
				st_message_count = 0;
			}

			// 早送り
			if ( joynew[main_joy[0]] & ( CONT_A ) ) {
				story_time_cnt += STORY_FF_SPEED;
				framecont += STORY_FF_SPEED;
			}

			// メッセージ処理
			st_message_proc();

			if ( lws_anim(&pgfx, pm, (SCENE_DATA *)( ( (u32)lws_data[demo_no] & 0x00ffffff ) + story_buffer ), story_time_cnt, story_buffer) == TRUE ) {
				if ( msgWnd_isEnd(&mess_st) != 0 ) {
					story_seq_step++;
					story_time_cnt = framecont = 0;
					st_message_count = 0;
				}
			}
			break;
		case 2:
			story_time_cnt = framecont;
			fin_frame = story_time_cnt;
			fin_demo = demo_no2;

			st_mes_ptr = mes_data[mes_no2];

			// とりあえず何か押したら抜ける
			if ( joyupd[main_joy[0]] & ( CONT_START | CONT_B ) ) {
				story_seq_step++;
				story_time_cnt = 0;
			}

			// 早送り
			if ( joynew[main_joy[0]] & ( CONT_A ) ) {
				story_time_cnt += STORY_FF_SPEED;
				framecont += STORY_FF_SPEED;
			}

			// メッセージ処理
			st_message_proc();

			if ( lws_anim(&pgfx, pm, (SCENE_DATA *)( ( (u32)lws_data[demo_no2] & 0x00ffffff ) + story_buffer ), story_time_cnt, story_buffer) == TRUE ) {
				if ( msgWnd_isEnd(&mess_st) != 0 ) {
					story_seq_step++;
					story_time_cnt = 0;
				}
			}
			break;
		default:
			// 終わり
			story_st_end();

			lws_anim(&pgfx, pm, (SCENE_DATA *)( ( (u32)lws_data[fin_demo] & 0x00ffffff ) + story_buffer ), fin_frame, story_buffer);
			break;
	}

	gDPSetScissor(pgfx++, G_SC_NON_INTERLACE, 0, 0, 320-1, 240-1);

	*glp = pgfx;
}



//-------------------------------------------------------------------------
// マリオＥＤ
//-------------------------------------------------------------------------
static	void	story_m_end(Gfx **glp, int type, int mes)
{
	Gfx			*pgfx;
	LMatrix 	pm;
	static		int	fin_frame;
	int			a, r, g, b;

	pgfx = *glp;

	gSPDisplayList(pgfx++, normal_texture_init_dl);
	gSPDisplayList(pgfx++, story_setup);

	makeTransrateMatrix(pm, 0 * FIXPOINT, -25 * FIXPOINT, -1900 * FIXPOINT);

	switch ( story_seq_step ) {
		case 0:
			// カーテンを上げとメッセージウインドウの表示
			story_st_start();

			lws_anim(&pgfx, pm, (SCENE_DATA *)( ( (u32)lws_data[type] & 0x00ffffff ) + story_buffer ), 0, story_buffer);
			framecont = 0;
			story_spot_cnt = 256;
			st_mes_ptr = mes_data[mes];

			break;
		case 1:
			story_time_cnt = framecont;
			fin_frame = story_time_cnt;

			// メッセージ処理
			st_message_proc();

			// とりあえず何か押したら抜ける
			if ( joyupd[main_joy[0]] & ( CONT_START | CONT_B ) ) {
				story_seq_step++;
				story_time_cnt = framecont = 0;
				st_message_count = 0;
			}

			if ( lws_anim(&pgfx, pm, (SCENE_DATA *)( ( (u32)lws_data[type] & 0x00ffffff ) + story_buffer ), story_time_cnt, story_buffer) == TRUE ) {
				story_seq_step++;
				story_time_cnt = framecont = 0;
				st_message_count = 0;
			}
			break;
		default:
			story_time_cnt = framecont;
			fin_frame = story_time_cnt;

			st_mes_ptr = mes_data[22];

			// 背景の色
			r = g = b = 255;
			if ( story_time_cnt > 350 ) {
				if ( story_time_cnt < 500 ) {
					// 昼から夕方
					// 350 - 499
					a = story_time_cnt - 350;
					r = 255 + ( ( 150 - 255 ) * a ) / ( 499 - 350 );
					g = 255 + ( (  95 - 255 ) * a ) / ( 499 - 350 );
					b = 255 + ( (  80 - 255 ) * a ) / ( 499 - 350 );
				} else if ( story_time_cnt < 700 ) {
					// 夕方から夜
					// 500 - 699
					a = story_time_cnt - 500;
					r = 150 + ( (  55 - 150 ) * a ) / ( 699 - 500 );
					g =  95 + ( (  79 -  95 ) * a ) / ( 699 - 500 );
					b =  80 + ( ( 110 -  80 ) * a ) / ( 699 - 500 );
				} else if ( story_time_cnt < 1000 ) {
					r =  55;
					g =  79;
					b = 110;
				} else if ( story_time_cnt < 1250 ) {
					// 夜から昼
					// 1000 - 1250
					a = story_time_cnt - 1000;
					r =  55 + ( ( 255 -  55 ) * a ) / ( 1249 - 1000 );
					g =  79 + ( ( 255 -  79 ) * a ) / ( 1249 - 1000 );
					b = 110 + ( ( 255 - 110 ) * a ) / ( 1249 - 1000 );
				}
			}
			a = 255;
			if ( story_time_cnt > 630 && story_time_cnt < 1000 ) {
				if ( story_time_cnt < 700 ) {
					// 点灯
					// 630 - 699
					a = story_time_cnt - 630;
					a = 255 + ( (  0 - 255 ) * a ) / ( 699 - 630 );
				} else if ( story_time_cnt > 950 ) {
					// 消灯
					// 950 - 999
					a = story_time_cnt - 950;
					a = 0 + ( ( 255 - 0 ) * a ) / ( 999 - 950 );
				} else {
					a = 0;
				}
			}
			gDPSetEnvColor(pgfx++, r, g, b, a);

			// メッセージ処理
			if ( story_staff_roll != 0 ) {
				if ( st_message_count == 9999 ) {
//					if ( msgWnd_isEnd(&mess_st) != 0 ) {
//						msgWnd_addStr(&mess_st, EndingLastMessage);
//					}
				} else {
					st_message_proc();
				}
			}

			if ( lws_anim(&pgfx, pm, (SCENE_DATA *)( ( (u32)lws_data[LWS_STORY_ED_Mb] & 0x00ffffff ) + story_buffer ), story_time_cnt, story_buffer) == TRUE ) {
				story_time_cnt = framecont = 279;
			}

			// スタッフロール開始
			if ( story_time_cnt > 30 && story_staff_roll == 0 ) story_staff_roll = 1;

			break;
	}

	gDPSetScissor(pgfx++, G_SC_NON_INTERLACE, 0, 0, 320-1, 240-1);

	// スタッフロールが終わった
	if ( story_staff_roll == 2 ) {
		//if ( msgWnd_isEnd(&mess_roll_st) != 0 ) story_staff_roll = -1;
		if ( !msgWnd_isScroll(&mess_roll_st) ) story_staff_roll = -1;
/*
		// とりあえず何か押したら抜ける
		if ( joyupd[main_joy[0]] & ( CONT_START | CONT_A | CONT_B ) ) {
			story_seq_step += 99;
			story_time_cnt = framecont = 0;
			st_message_count = 0;
		}
*/
	}

	// スタッフロール開始
	if ( story_staff_roll == 1 ) {
		msgWnd_addStr(&mess_roll_st, st_staffroll_txt);
		msgWnd_skip(&mess_roll_st);
		mess_roll_st.contFlags = 0;
		mess_roll_st.scrSpeed = 1.0 / 60.0;
		story_staff_roll = 2;
	}

	*glp = pgfx;
}
