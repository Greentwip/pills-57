//-------------------------------------------------------------------------
// Stage new
//-------------------------------------------------------------------------
void	story_st_new_w9(Gfx **glp, int demo_no, int mes_no)
{
	Gfx			*pgfx;
	LMatrix 	pm;
	static		int	fin_frame;
	SCENE_DATA	*scn_dat;
	int			r, g, b, a;

	pgfx = *glp;

	st_mes_ptr = mes_data[mes_no];

	gSPDisplayList(pgfx++, normal_texture_init_dl);
	gSPDisplayList(pgfx++, story_setup);

	makeTransrateMatrix(pm, 0 * FIXPOINT, -25 * FIXPOINT, -1900 * FIXPOINT);

	scn_dat = (SCENE_DATA *)( ( (u32)lws_data[demo_no] & 0x00ffffff ) + story_buffer );

	switch ( story_seq_step ) {
		case 0:
			// �J�[�e�����グ�ƃ��b�Z�[�W�E�C���h�E�̕\��
			story_st_start();

			lws_anim(&pgfx, pm, scn_dat, 0, story_buffer);
			framecont = 0;

			break;
		case 1:
			story_time_cnt = framecont;
			fin_frame = story_time_cnt;

			// Env�F
			r = g = b = 0;
			if ( story_time_cnt >= 759 && story_time_cnt <= 819 ) {
				// 759 - 819
				a = story_time_cnt - 759;
				r = ( 255 * a ) / ( 819 - 759 );
				g = ( 255 * a ) / ( 819 - 759 );
				b = ( 255 * a ) / ( 819 - 759 );
			} else if ( story_time_cnt > 819 ) {
				r = g = b = 255;
			}
			a = 255;
			gDPSetEnvColor(pgfx++, r, g, b, a);

			// �Ƃ肠���������������甲����
			if ( joyupd[main_joy[0]] & ( CONT_START | CONT_B ) ) {
				story_seq_step++;
				story_time_cnt = 0;
			}

			// ������
			if ( joynew[main_joy[0]] & ( CONT_A ) ) {
				story_time_cnt += STORY_FF_SPEED;
				framecont += STORY_FF_SPEED;
			}

			// ���b�Z�[�W����
			st_message_proc();

			if ( lws_anim(&pgfx, pm, scn_dat, story_time_cnt, story_buffer) == TRUE ) {
				story_seq_step++;
				story_time_cnt = 0;
			}
			break;
		default:
			// �I���
			story_st_end();

			lws_anim(&pgfx, pm, scn_dat, fin_frame, story_buffer);
			break;
	}

	gDPSetScissor(pgfx++, G_SC_NON_INTERLACE, 0, 0, 320-1, 240-1);

	*glp = pgfx;
}


//-------------------------------------------------------------------------
// �����I�d�c
//-------------------------------------------------------------------------
static	void	story_w_end(Gfx **glp, int type)
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
			// �J�[�e�����グ�ƃ��b�Z�[�W�E�C���h�E�̕\��
			story_st_start();

			lws_anim(&pgfx, pm, (SCENE_DATA *)( ( (u32)lws_data[type] & 0x00ffffff ) + story_buffer ), 0, story_buffer);
			framecont = 0;
			story_spot_cnt = 256;
			st_mes_ptr = mes_data[23];

			break;
		case 1:
			story_time_cnt = framecont;
			fin_frame = story_time_cnt;

			// ���b�Z�[�W����
			st_message_proc();

			// �Ƃ肠���������������甲����
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

			st_mes_ptr = mes_data[24];

			// �w�i�̐F
			r = g = b = 255;
			if ( story_time_cnt >= 610 ) {
				if ( story_time_cnt < 795 ) {
					// ������[��
					// 350 - 794
					a = story_time_cnt - 610;
					r = 255 + ( ( 150 - 255 ) * a ) / ( 794 - 610 );
					g = 255 + ( (  95 - 255 ) * a ) / ( 794 - 610 );
					b = 255 + ( (  80 - 255 ) * a ) / ( 794 - 610 );
				} else if ( story_time_cnt < 980 ) {
					// �[�������
					// 795 - 979
					a = story_time_cnt - 795;
					r = 150 + ( (  55 - 150 ) * a ) / ( 979 - 795 );
					g =  95 + ( (  79 -  95 ) * a ) / ( 979 - 795 );
					b =  80 + ( ( 110 -  80 ) * a ) / ( 979 - 795 );
				} else if ( story_time_cnt < 1480 ) {
					// 980 - 1479
					r =  55;
					g =  79;
					b = 110;
				} else if ( story_time_cnt < 1800 ) {
					// �邩�璋
					// 1480 - 1799
					a = story_time_cnt - 1480;
					r =  55 + ( ( 255 -  55 ) * a ) / ( 1799 - 1480 );
					g =  79 + ( ( 255 -  79 ) * a ) / ( 1799 - 1480 );
					b = 110 + ( ( 255 - 110 ) * a ) / ( 1799 - 1480 );
				}
			}
			a = 255;
/*
			if ( story_time_cnt > 630 && story_time_cnt < 1000 ) {
				if ( story_time_cnt < 700 ) {
					// �_��
					// 630 - 699
					a = story_time_cnt - 630;
					a = 255 + ( (  0 - 255 ) * a ) / ( 699 - 630 );
				} else if ( story_time_cnt > 950 ) {
					// ����
					// 950 - 999
					a = story_time_cnt - 950;
					a = 0 + ( ( 255 - 0 ) * a ) / ( 999 - 950 );
				} else {
					a = 0;
				}
			}
*/
			gDPSetEnvColor(pgfx++, r, g, b, a);

			// ���b�Z�[�W����
			if ( story_staff_roll != 0 ) {
				if ( st_message_count == 9999 ) {
//					if ( msgWnd_isEnd(&mess_st) != 0 ) {
//						msgWnd_addStr(&mess_st, EndingLastMessage);
//					}
				} else {
					st_message_proc();
				}
			}

			if ( lws_anim(&pgfx, pm, (SCENE_DATA *)( ( (u32)lws_data[LWS_STORY_ED_Wb] & 0x00ffffff ) + story_buffer ), story_time_cnt, story_buffer) == TRUE ) {
				story_time_cnt = framecont = 109;
			}

			// �X�^�b�t���[���J�n
			if ( story_time_cnt > 30 && story_staff_roll == 0 ) story_staff_roll = 1;

			break;
	}

	gDPSetScissor(pgfx++, G_SC_NON_INTERLACE, 0, 0, 320-1, 240-1);

	// �X�^�b�t���[�����I�����
	if ( story_staff_roll == 2 ) {
		//if ( msgWnd_isEnd(&mess_roll_st) != 0 ) story_staff_roll = -1;
		if ( !msgWnd_isScroll(&mess_roll_st) ) story_staff_roll = -1;
/*
		// �Ƃ肠���������������甲����
		if ( joyupd[main_joy[0]] & ( CONT_START | CONT_A | CONT_B ) ) {
			story_seq_step += 99;
			story_time_cnt = framecont = 0;
			st_message_count = 0;
		}
*/
	}

	// �X�^�b�t���[���J�n
	if ( story_staff_roll == 1 ) {
		msgWnd_addStr(&mess_roll_st, st_staffroll_txt);
		msgWnd_skip(&mess_roll_st);
		mess_roll_st.contFlags = 0;
		mess_roll_st.scrSpeed = 1.0 / 60.0;
		story_staff_roll = 2;
	}

	*glp = pgfx;
}
