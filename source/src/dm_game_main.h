//////////////////////////////////////////////////////////////////////////////
//	filename	:	dm_game_main.h
//	create		:	1999/08/02
//	modify		:	1999/11/11
//	created		:	Hiroyuki Watanabe
//////////////////////////////////////////////////////////////////////////////

#ifndef __dm_game_main_h__
#define __dm_game_main_h__

#include "dm_game_def.h"

//////////////////////////////////////////////////////////////////////////////
// �֐��̎Q��

// �J�v�Z���̃e�N�X�`�����p���b�g���擾
extern STexInfo *dm_game_get_capsel_tex(int sizeIndex);
extern STexInfo *dm_game_get_capsel_pal(int sizeIndex, int colorIndex);

// ���_���v�Z
extern u32 dm_make_score(game_state *state);

extern void dm_attack_se(game_state *state, int player_no); // �U���������֐�
extern void dm_warning_h_line(game_state *, game_map *);    // �ςݏグ�x���`�F�b�N
extern void dm_warning_h_line_se();                         // �ςݏグ�x���������֐�

extern void go_down(game_state *, game_map *, int); // �������֐�
extern void erase_anime(game_map *);                // �}�b�v����ŃA�j���[�V�����֐�

extern void translate_capsel(game_map *, game_state *, int, int); // �J�v�Z�����E�ړ��֐�
extern void rotate_capsel(game_map *, game_cap *, int);           // �J�v�Z�����E��]�֐�

extern void dm_set_capsel(game_state *state);               // �J�v�Z���ݒ�֐�
extern void dm_capsel_speed_up(game_state *);               // �J�v�Z�����x�㏸�ݒ�֐�
extern void dm_capsel_down(game_state *, game_map *);       // ����J�v�Z�����������֐�
extern int  dm_check_game_over(game_state *,game_map *);    // �Q�[���I�[�o�[(�ς݂�����)����֐�
extern void dm_virus_anime(game_state *, game_map *);       // �E�C���X�A�j���֐�
extern void dm_capsel_erase_anime(game_state *, game_map *);// �E�C���X(�J�v�Z��)���Ŋ֐�
extern int  dm_h_erase_chack(game_map *);                   // �c��������֐�
extern void dm_h_erase_chack_set(game_state *, game_map *); // �c��������Ə����J�n�֐�
extern int  dm_w_erase_chack(game_map *);                   // ����������֐�
extern void dm_w_erase_chack_set(game_state *, game_map *); // ����������Ə����J�n�֐�
extern void dm_h_ball_chack(game_map *);                    // �c�����菈���֐�
extern void dm_w_ball_chack(game_map *);                    // �������菈���֐�
extern int  dm_black_up(game_state *,game_map *);           // �Q�[���I�[�o�[�̎��̈Â��Ȃ�֐�

// �r��`��
extern void dm_calc_bottle_2p();
extern void dm_draw_bottle_2p(Gfx **gpp);

// �J�T�}����`��
extern void dm_draw_KaSaMaRu(Gfx **gpp, Mtx **mpp, Vtx **vpp,
	int speaking, int x, int y, int dir, int alpha);

// ����E�B���X�̈ʒu���v�Z
extern void dm_calc_big_virus_pos(game_state *state);

// ����E�B���X��`��
extern void dm_draw_big_virus(Gfx **gpp);

extern void dm_game_graphic_effect(game_state *, int, int); // �Q�[���`��֐�(���o�֌W�p)

extern void dm_game_graphic_p(game_state *, int, game_map *);    // �}�b�v���`��֐�
extern void key_control_main(game_state *, game_map *, int, int);// �J�v�Z������֐�
extern void make_ai_main();                                      // �v�l����

// ���ʌv�Z
void dm_effect_make(void);

// �q�[�v���������擾
extern void **dm_game_heap_top();

// �E�B���X�L�����A�j���[�V����
extern SAnimeState *get_virus_anime_state(int index);

// �E�B���X���Ŏ��̉��A�j���[�V����
extern SAnimeSmog *get_virus_smog_state(int index);

// �W�I���g���o�b�t�@���擾
extern Mtx *dm_get_mtx_buf();
extern Vtx *dm_get_vtx_buf();

extern void dm_game_init_heap();
extern void dm_game_init_static();
extern void dm_game_init_snap_bg();
extern void dm_game_draw_snap_bg(Gfx **gpp, Mtx **mpp, Vtx **vpp, int drawPanelFlag);
extern void dm_game_draw_snapped_bg(Gfx **gpp);

extern void dm_game_init(int reinit);
extern int  dm_game_main(NNSched *);
extern void dm_game_graphic();
extern void dm_game_graphic_onDoneSawp();
extern void dm_game_graphic_onDoneTask();

extern int  main_techmes(void *arg);
extern void graphic_techmes();

//////////////////////////////////////////////////////////////////////////////
//	�ϐ��̎Q��

// �E�C���X�̃A�j���[�V�����p�^�[���ԍ�
extern u8 virus_anime_table[][4];

// �r�̒��̏��
extern game_map game_map_data[4][GAME_MAP_W * (GAME_MAP_H + 1)];

// �E�C���X�ݒ�p�z��
extern virus_map virus_map_data[4][GAME_MAP_W * GAME_MAP_H];

// �e�v���C���[�̏�ԋy�ѐ���
extern game_state game_state_data[4];

// �E�C���X�ݒ菇�p�z��
extern u8 virus_map_disp_order[4][96];

//////////////////////////////////////////////////////////////////////////////

#endif
