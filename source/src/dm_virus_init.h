//////////////////////////////////////////////////////////////////////////////
//	filename	:	dm_virus_init.h
//	create		:	1999/08/03
//	modify		:	1999/11/26
//	created		:	Hiroyuki Watanabe
//////////////////////////////////////////////////////////////////////////////

#ifndef	__dm_virus_init_h__
#define	__dm_virus_init_h__

//////////////////////////////////////////////////////////////////////////////
//	�錾

#define		dm_up_weight		1		// ��1/3�̏d��
#define		dm_middle_weight 	2		// ��  ..
#define		dm_low_weight		3		// ��  ..

//////////////////////////////////////////////////////////////////////////////
//	�֐��̎Q��

//## �}�b�v����������
extern void init_map_all(game_map *map);

//	�}�b�v���S���N���A�֐�
//	����	map	:�N���A����}�b�v�ւ̃|�C���^
extern	void	clear_map_all( game_map *map );

//	�}�b�v���ꕔ�N���A�֐�
//	����	map	:�N���A����}�b�v�ւ̃|�C���^
//			x	:�w���W(0�`7)
//			y	:�x���W(1�`16)
extern	void	clear_map( game_map *map, int x, int y );

//	�}�b�v���擾�֐�
//	����	map	:�N���A����}�b�v�ւ̃|�C���^
//			x	:�w���W(0�`7)
//			y	:�x���W(1�`16)
//	�Ԓl	0:��������
//			1:�����L��
extern	int		get_map_info( game_map *map, int x, int y );

// �w�肵���w���W����ŁA��ԏ�ɂ���
// �E�B���X or �J�v�Z���̐F���擾
extern int get_map_top_color(
	game_map *map, // �}�b�v�f�[�^�z��̐擪�A�h���X
	int       x    // �F���擾����w���W
	);
// �߂�l : �F�ԍ� (-1 �͖��F)

// �}�b�v��̈�ԏ�̒i�ɂ���F���擾
extern void get_map_top_colors(
	game_map *map,         // �}�b�v�f�[�^�z��̐擪�A�h���X
	int colors[GAME_MAP_W] // �F�f�[�^�o�͐�
	);

//	�E�C���X���擾�֐�
//	����	map:�����擾����}�b�v�ւ̃|�C���^
//	�Ԓl	�E�C���X�̑���
extern	int		get_virus_count( game_map *map );

//	�F�ʃE�C���X���擾�֐�
//	����	map	:�����擾����}�b�v�ւ̃|�C���^
//			r	:�ԐF�E�C���X�̐����i�[����ϐ��̃|�C���^
//			y	:���F�E�C���X�̐����i�[����ϐ��̃|�C���^
//			b	:�F�E�C���X�̐����i�[����ϐ��̃|�C���^
//	�Ԓl �E�C���X�̑���
extern	int		get_virus_color_count(game_map *map, u8 *r, u8 *y, u8 *b);

//	�}�b�v���ݒ�֐�
//	����	map	:����ݒ肷��}�b�v�ւ̃|�C���^
//			x	:�w���W(0�`7)
//			y	:�x���W(1�`16)
//			item:�ݒ肷�����( dm_game_main.h �ŗ񋓂���Ă������ )
//			col :�F( dm_game_main.h �ŗ񋓂���Ă������ )
extern	void	set_map( game_map *map, int x, int y, int item, int col);

//	�E�C���X�}�b�v���ݒ�֐�
//	����	map	:�E�C���X����ݒ肷��}�b�v�ւ̃|�C���^
//			x	:�w���W(0�`7)
//			y	:�x���W(1�`16)
//			vi	:�E�C���X( dm_game_main.h �ŗ񋓂���Ă������ )
//			an	:�A�j���ԍ�
extern	void	set_virus( game_map *map, int x, int y, int vi, int an );

//	�}�b�v��񕡐��֐�
//	����	m_a	:�R�s�[���}�b�v�ւ̃|�C���^
//			m_b	:�R�s�[��}�b�v�ւ̃|�C���^
//			o_a	:�R�s�[���\����
//			o_b	:�R�s�[��\����
extern	void	dm_virus_map_copy( virus_map *m_a,virus_map *m_b,u8 *o_a,u8 *o_b);

// �Q�[���J�n���̃E�B���X�����擾
extern int dm_get_first_virus_count(
	GAME_MODE mode, // �Q�[�����[�h
	game_state *state // �Q�[���X�e�[�^�X
	);
// �߂�l: �Q�[���J�n���̃E�B���X��

// �Q�[���J�n���̃E�B���X�����擾(�t���b�V���A�X�R�A�A�^�b�N)
extern int dm_get_first_virus_count_in_new_mode(
	int level // �Q�[�����x��
	);
// �߂�l: �Q�[���J�n���̃E�B���X��

//	�E�C���X���쐬�֐�
//	����	st	:�v���C���[���̃|�C���^
//			map	:�쐬�����E�C���X�����i�[����ꏊ�̃|�C���^
//			od	:�\�����i�[��|�C���^
extern void _dm_virus_init(GAME_MODE mode, game_state *state, virus_map *v_map, u8 *order, int special);
extern void dm_virus_init(GAME_MODE mode, game_state *st, virus_map *map, u8 *order);


// �t���b�V���E�B���X�̈ʒu�����߂�
extern int make_flash_virus_pos(game_state *state, virus_map *vmap, u8 *order);

//////////////////////////////////////////////////////////////////////////////

#endif
