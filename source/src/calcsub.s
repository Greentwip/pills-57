	.option	pic0
	.option	O2

 #---------------------------------------------------------------
 # int	waitEQ4(int*, int)
 #---------------------------------------------------------------
	.text
	.align	2
	.globl	waitEQ4
	.ent	waitEQ4 2
waitEQ4:
	.frame	$sp, 0, $31

$waitEQ4_lp:
	lw	$2, 0($4)
	subu	$2, $5
	bne	$2, $0, $waitEQ4_lp

	.livereg	0x0000FF0E,0x00000FFF
	j	$31
	.end	waitEQ4


 #---------------------------------------------------------------
 # long	sqrtS(unsigned long v)
 #---------------------------------------------------------------
	.text
	.align	2
	.globl	sqrtS
	.ent	sqrtS 2
sqrtS:
	.frame	$sp, 0, $31

	mtc1	$4, $f4
	cvt.s.w	$f0, $f4
	sqrt.s	$f6, $f0
	trunc.w.s	$f8, $f6, $13
	mfc1	$2, $f8

	.livereg	0x0000FF0E,0x00000FFF
	j	$31
	.end	sqrtS


 #---------------------------------------------------------------
 # long	sqrtl(unsigned long hi, unsigned long lo)
 #---------------------------------------------------------------
	.text
	.align	2
	.globl	sqrtl
	.ent	sqrtl 2
sqrtl:
	.frame	$sp, 0, $31

	move	$2, $0
	move	$3, $0
	li	$8, 32

	slt	$24, $5 ,$0
	sll	$5 ,1
	slt	$25, $4 ,$0
	sll	$4 ,1
	or	$4, $24
	sll	$3 ,1
	or	$3, $25

$sqrt_lp:
	slt	$24, $5 ,$0
	sll	$5 ,1
	slt	$25, $4 ,$0
	sll	$4 ,1
	or	$4, $24
	sll	$3 ,1
	or	$3, $25

	sll	$2, 1
	addiu	$2, 1

	bltu	$3, $2, $sqrt_dis

	subu	$3, $2
	addiu	$2, 2
$sqrt_dis:
	slt	$24, $5 ,$0
	sll	$5 ,1
	slt	$25, $4 ,$0
	sll	$4 ,1
	or	$4, $24
	sll	$3 ,1
	or	$3, $25

	addiu	$2, -1
	addiu	$8, -1
	bgtz	$8, $sqrt_lp

	srl	$2, 1
	.livereg	0x0000FF0E,0x00000FFF
	j	$31
	.end	sqrtl


 #---------------------------------------------------------------
 # s32 get_angleL(s32 x, s32 y)
 #---------------------------------------------------------------
	.data
	.align	2
	.align	0
start_tbl:
	.byte	0, 3, 6, 8
	.byte	10, 13, 15, 17
	.byte	19, 21, 23, 25
	.byte	27, 28, 30, 31
	.byte	32, 32, 32, 32

	.data
	.align	2
	.align	0
tan_table:
	.word	0x00000000, 0x00648d18, 0x00c9393c, 0x012e239c
	.word	0x01936bb8, 0x01f93183, 0x025f958e, 0x02c6b932
	.word	0x032ebebc, 0x0397c99d, 0x0401fe9d, 0x046d840f
	.word	0x04da820d, 0x054922b8, 0x05b9927d, 0x062c0065
	.word	0x06a09e66, 0x0717a1c6, 0x07914383, 0x080dc0cd
	.word	0x088d5b8c, 0x09105af7, 0x09970c44, 0x0a21c36d
	.word	0x0ab0dc15, 0x0b44ba8a, 0x0bddccf6, 0x0c7c8cbe
	.word	0x0d218015, 0x0dcd3be0, 0x0e8065e3, 0x0f3bb757
	.word	0x10000000, 0xffffffff

	.data
	.align	2
	.align	0
tan_div_table:
	.word	0x028bc48e, 0x028afb8e, 0x02896a18, 0x02871113
	.word	0x0283f1fb, 0x02800ebc, 0x027b69b9, 0x027605d4
	.word	0x026fe65e, 0x02690f18, 0x02618440, 0x02594a7d
	.word	0x025066df, 0x0246dee6, 0x023cb868, 0x0231f9b4
	.word	0x0226a964, 0x021ace77, 0x020e7038, 0x02019645
	.word	0x01f44893, 0x01e68f51, 0x01d872f9, 0x01c9fc39
	.word	0x01bb3404, 0x01ac2375, 0x019cd3d2, 0x018d4e95
	.word	0x017d9d49, 0x016dc99d, 0x015ddd57, 0x014de244
	.word	0x013de246

	.text	
	.align	2
	.globl	get_angleL
	.ent	get_angleL 2
get_angleL:
	.frame	$sp, 0, $31

	move	$12, $0		# flag = 0

	bne	$4, $0, $x_not_0
	bne	$5, $0, $y_not_0
	li	$2, 0		# X = 0 , Y = 0
	b	$return
$y_not_0:
	bgez	$5, $y_ge_0
	li	$2, 0xc000	# X = 0 , y < 0 ... -90deg
	b	$return
$y_ge_0:
	li	$2, 0x4000	# X = 0 , y >= 0 ... 90deg
	b	$return

$x_not_0:
	bgez	$4, $x_ge_0	# X < 0
	negu	$4, $4
	xori	$12, $12, 0x0110	#flag = 0x0110
$x_ge_0:

	bne	$5, $0, $y_not_0_1
	bne	$12, $0, $flag_0
	li	$2, 0			# X >= 0 , Y = 0 ... 0deg
	b	$return
$flag_0:
	li	$2, 0x8000		# X < 0 , Y = 0 ... 180deg
	b	$return

$y_not_0_1:
	bgez	$5, $y_ge_0_1
	negu	$5, $5
	xori	$12, $12, 0x0010	# flag xor 0x0010
$y_ge_0_1:

	subu	$24, $4, $5
	bgez	$24, $x_ge_y
	move	$24, $4
	move	$4, $5
	move	$5, $24
	xori	$12, $12, 0x0001	# flag xor 0x0001
$x_ge_y:

	#tmp = (u32)( ( 0x10000000 / x ) * y );
	lui	$24, 0x4000		# $15 = 0x40000000
	divu	$24, $24, $4
	multu	$24, $5
	mflo	$25
	srl	$4, $25, 2
	srl	$24, $4, 24
	lbu	$3, start_tbl($24)
	sll	$10, $3, 2
$loop:
	lw	$25, tan_table($10)	# while ( tan_table[i] <= tmp ) i++;
	addiu	$3, $3, 1
	addiu	$10, $10, 4
	subu	$24, $4, $25
	bgez	$24, $loop

	addiu	$3, $3, -2

	lw	$24, tan_table-8($10)
	sll	$2, $3, 8
	beq	$4, $24, $no_point	# if ( tmp != tan_table[i] )
	subu	$11, $4, $24		#tmp - tan_table[i]
	lw	$24, tan_div_table-8($10)
	multu	$11, $24		# * tan_div_table[i]
	mfhi	$24
	srl	$24, 8
	andi	$25, $24, 0x00ff
	or	$2, $2, $25
$no_point:

	# if ( flag & 0x0001 ) ang = 0x4000 - ang;
	andi	$24, $12, 0x0001
	beq	$24, $0, $chk1
	li	$24, 0x4000
	subu	$2, $24, $2
$chk1:

	# if ( flag & 0x0010 ) ang = -ang;
	andi	$24, $12, 0x0010
	beq	$24, $0, $chk2
	negu	$2, $2
$chk2:

	# if ( flag & 0x0100 ) ang += 0x8000;
	andi	$24, $12, 0x0100
	beq	$24, $0, $chk3
	addiu	$2, $2, 0x8000
$chk3:

$return:
	sll	$24, $2, 16
	sra	$2, $24, 16

	.livereg	0x0000FF0E,0x00000FFF
	j	$31
	.end	get_angleL


 #---------------------------------------------------------------
 # sin table data
 #---------------------------------------------------------------
	.data
	.align	2
	.align	0
sintable:
	.half	0x0000, 0x00c9, 0x0192, 0x025b, 0x0324, 0x03ed, 0x04b6, 0x057f, 0x0648, 0x0711, 0x07d9, 0x08a2, 0x096b, 0x0a33, 0x0afb, 0x0bc4
	.half	0x0c8c, 0x0d54, 0x0e1c, 0x0ee4, 0x0fab, 0x1073, 0x113a, 0x1201, 0x12c8, 0x138f, 0x1455, 0x151c, 0x15e2, 0x16a8, 0x176e, 0x1833
	.half	0x18f9, 0x19be, 0x1a83, 0x1b47, 0x1c0c, 0x1cd0, 0x1d93, 0x1e57, 0x1f1a, 0x1fdd, 0x209f, 0x2162, 0x2224, 0x22e5, 0x23a7, 0x2467
	.half	0x2528, 0x25e8, 0x26a8, 0x2768, 0x2827, 0x28e5, 0x29a4, 0x2a62, 0x2b1f, 0x2bdc, 0x2c99, 0x2d55, 0x2e11, 0x2ecc, 0x2f87, 0x3042
	.half	0x30fc, 0x31b5, 0x326e, 0x3327, 0x33df, 0x3497, 0x354e, 0x3604, 0x36ba, 0x3770, 0x3825, 0x38d9, 0x398d, 0x3a40, 0x3af3, 0x3ba5
	.half	0x3c57, 0x3d08, 0x3db8, 0x3e68, 0x3f17, 0x3fc6, 0x4074, 0x4121, 0x41ce, 0x427a, 0x4326, 0x43d1, 0x447b, 0x4524, 0x45cd, 0x4675
	.half	0x471d, 0x47c4, 0x486a, 0x490f, 0x49b4, 0x4a58, 0x4afb, 0x4b9e, 0x4c40, 0x4ce1, 0x4d81, 0x4e21, 0x4ec0, 0x4f5e, 0x4ffb, 0x5098
	.half	0x5134, 0x51cf, 0x5269, 0x5303, 0x539b, 0x5433, 0x54ca, 0x5560, 0x55f6, 0x568a, 0x571e, 0x57b1, 0x5843, 0x58d4, 0x5964, 0x59f4
	.half	0x5a82, 0x5b10, 0x5b9d, 0x5c29, 0x5cb4, 0x5d3e, 0x5dc8, 0x5e50, 0x5ed7, 0x5f5e, 0x5fe4, 0x6068, 0x60ec, 0x616f, 0x61f1, 0x6272
	.half	0x62f2, 0x6371, 0x63ef, 0x646c, 0x64e9, 0x6564, 0x65de, 0x6657, 0x66d0, 0x6747, 0x67bd, 0x6832, 0x68a7, 0x691a, 0x698c, 0x69fd
	.half	0x6a6e, 0x6add, 0x6b4b, 0x6bb8, 0x6c24, 0x6c8f, 0x6cf9, 0x6d62, 0x6dca, 0x6e31, 0x6e97, 0x6efb, 0x6f5f, 0x6fc2, 0x7023, 0x7083
	.half	0x70e3, 0x7141, 0x719e, 0x71fa, 0x7255, 0x72af, 0x7308, 0x735f, 0x73b6, 0x740b, 0x7460, 0x74b3, 0x7505, 0x7556, 0x75a6, 0x75f4
	.half	0x7642, 0x768e, 0x76d9, 0x7723, 0x776c, 0x77b4, 0x77fb, 0x7840, 0x7885, 0x78c8, 0x790a, 0x794a, 0x798a, 0x79c9, 0x7a06, 0x7a42
	.half	0x7a7d, 0x7ab7, 0x7aef, 0x7b27, 0x7b5d, 0x7b92, 0x7bc6, 0x7bf9, 0x7c2a, 0x7c5a, 0x7c89, 0x7cb7, 0x7ce4, 0x7d0f, 0x7d3a, 0x7d63
	.half	0x7d8a, 0x7db1, 0x7dd6, 0x7dfb, 0x7e1e, 0x7e3f, 0x7e60, 0x7e7f, 0x7e9d, 0x7eba, 0x7ed6, 0x7ef0, 0x7f0a, 0x7f22, 0x7f38, 0x7f4e
	.half	0x7f62, 0x7f75, 0x7f87, 0x7f98, 0x7fa7, 0x7fb5, 0x7fc2, 0x7fce, 0x7fd9, 0x7fe2, 0x7fea, 0x7ff1, 0x7ff6, 0x7ffa, 0x7ffe, 0x7fff

	.half	0x7fff, 0x7fff, 0x7ffe, 0x7ffa, 0x7ff6, 0x7ff1, 0x7fea, 0x7fe2, 0x7fd9, 0x7fce, 0x7fc2, 0x7fb5, 0x7fa7, 0x7f98, 0x7f87, 0x7f75
	.half	0x7f62, 0x7f4e, 0x7f38, 0x7f22, 0x7f0a, 0x7ef0, 0x7ed6, 0x7eba, 0x7e9d, 0x7e7f, 0x7e60, 0x7e3f, 0x7e1e, 0x7dfb, 0x7dd6, 0x7db1
	.half	0x7d8a, 0x7d63, 0x7d3a, 0x7d0f, 0x7ce4, 0x7cb7, 0x7c89, 0x7c5a, 0x7c2a, 0x7bf9, 0x7bc6, 0x7b92, 0x7b5d, 0x7b27, 0x7aef, 0x7ab7
	.half	0x7a7d, 0x7a42, 0x7a06, 0x79c9, 0x798a, 0x794a, 0x790a, 0x78c8, 0x7885, 0x7840, 0x77fb, 0x77b4, 0x776c, 0x7723, 0x76d9, 0x768e
	.half	0x7642, 0x75f4, 0x75a6, 0x7556, 0x7505, 0x74b3, 0x7460, 0x740b, 0x73b6, 0x735f, 0x7308, 0x72af, 0x7255, 0x71fa, 0x719e, 0x7141
	.half	0x70e3, 0x7083, 0x7023, 0x6fc2, 0x6f5f, 0x6efb, 0x6e97, 0x6e31, 0x6dca, 0x6d62, 0x6cf9, 0x6c8f, 0x6c24, 0x6bb8, 0x6b4b, 0x6add
	.half	0x6a6e, 0x69fd, 0x698c, 0x691a, 0x68a7, 0x6832, 0x67bd, 0x6747, 0x66d0, 0x6657, 0x65de, 0x6564, 0x64e9, 0x646c, 0x63ef, 0x6371
	.half	0x62f2, 0x6272, 0x61f1, 0x616f, 0x60ec, 0x6068, 0x5fe4, 0x5f5e, 0x5ed7, 0x5e50, 0x5dc8, 0x5d3e, 0x5cb4, 0x5c29, 0x5b9d, 0x5b10
	.half	0x5a82, 0x59f4, 0x5964, 0x58d4, 0x5843, 0x57b1, 0x571e, 0x568a, 0x55f6, 0x5560, 0x54ca, 0x5433, 0x539b, 0x5303, 0x5269, 0x51cf
	.half	0x5134, 0x5098, 0x4ffb, 0x4f5e, 0x4ec0, 0x4e21, 0x4d81, 0x4ce1, 0x4c40, 0x4b9e, 0x4afb, 0x4a58, 0x49b4, 0x490f, 0x486a, 0x47c4
	.half	0x471d, 0x4675, 0x45cd, 0x4524, 0x447b, 0x43d1, 0x4326, 0x427a, 0x41ce, 0x4121, 0x4074, 0x3fc6, 0x3f17, 0x3e68, 0x3db8, 0x3d08
	.half	0x3c57, 0x3ba5, 0x3af3, 0x3a40, 0x398d, 0x38d9, 0x3825, 0x3770, 0x36ba, 0x3604, 0x354e, 0x3497, 0x33df, 0x3327, 0x326e, 0x31b5
	.half	0x30fc, 0x3042, 0x2f87, 0x2ecc, 0x2e11, 0x2d55, 0x2c99, 0x2bdc, 0x2b1f, 0x2a62, 0x29a4, 0x28e5, 0x2827, 0x2768, 0x26a8, 0x25e8
	.half	0x2528, 0x2467, 0x23a7, 0x22e5, 0x2224, 0x2162, 0x209f, 0x1fdd, 0x1f1a, 0x1e57, 0x1d93, 0x1cd0, 0x1c0c, 0x1b47, 0x1a83, 0x19be
	.half	0x18f9, 0x1833, 0x176e, 0x16a8, 0x15e2, 0x151c, 0x1455, 0x138f, 0x12c8, 0x1201, 0x113a, 0x1073, 0x0fab, 0x0ee4, 0x0e1c, 0x0d54
	.half	0x0c8c, 0x0bc4, 0x0afb, 0x0a33, 0x096b, 0x08a2, 0x07d9, 0x0711, 0x0648, 0x057f, 0x04b6, 0x03ed, 0x0324, 0x025b, 0x0192, 0x00c9

	.half	0x0000, 0xff37, 0xfe6e, 0xfda5, 0xfcdc, 0xfc13, 0xfb4a, 0xfa81, 0xf9b8, 0xf8ef, 0xf827, 0xf75e, 0xf695, 0xf5cd, 0xf505, 0xf43c
	.half	0xf374, 0xf2ac, 0xf1e4, 0xf11c, 0xf055, 0xef8d, 0xeec6, 0xedff, 0xed38, 0xec71, 0xebab, 0xeae4, 0xea1e, 0xe958, 0xe892, 0xe7cd
	.half	0xe707, 0xe642, 0xe57d, 0xe4b9, 0xe3f4, 0xe330, 0xe26d, 0xe1a9, 0xe0e6, 0xe023, 0xdf61, 0xde9e, 0xdddc, 0xdd1b, 0xdc59, 0xdb99
	.half	0xdad8, 0xda18, 0xd958, 0xd898, 0xd7d9, 0xd71b, 0xd65c, 0xd59e, 0xd4e1, 0xd424, 0xd367, 0xd2ab, 0xd1ef, 0xd134, 0xd079, 0xcfbe
	.half	0xcf04, 0xce4b, 0xcd92, 0xccd9, 0xcc21, 0xcb69, 0xcab2, 0xc9fc, 0xc946, 0xc890, 0xc7db, 0xc727, 0xc673, 0xc5c0, 0xc50d, 0xc45b
	.half	0xc3a9, 0xc2f8, 0xc248, 0xc198, 0xc0e9, 0xc03a, 0xbf8c, 0xbedf, 0xbe32, 0xbd86, 0xbcda, 0xbc2f, 0xbb85, 0xbadc, 0xba33, 0xb98b
	.half	0xb8e3, 0xb83c, 0xb796, 0xb6f1, 0xb64c, 0xb5a8, 0xb505, 0xb462, 0xb3c0, 0xb31f, 0xb27f, 0xb1df, 0xb140, 0xb0a2, 0xb005, 0xaf68
	.half	0xaecc, 0xae31, 0xad97, 0xacfd, 0xac65, 0xabcd, 0xab36, 0xaaa0, 0xaa0a, 0xa976, 0xa8e2, 0xa84f, 0xa7bd, 0xa72c, 0xa69c, 0xa60c
	.half	0xa57e, 0xa4f0, 0xa463, 0xa3d7, 0xa34c, 0xa2c2, 0xa238, 0xa1b0, 0xa129, 0xa0a2, 0xa01c, 0x9f98, 0x9f14, 0x9e91, 0x9e0f, 0x9d8e
	.half	0x9d0e, 0x9c8f, 0x9c11, 0x9b94, 0x9b17, 0x9a9c, 0x9a22, 0x99a9, 0x9930, 0x98b9, 0x9843, 0x97ce, 0x9759, 0x96e6, 0x9674, 0x9603
	.half	0x9592, 0x9523, 0x94b5, 0x9448, 0x93dc, 0x9371, 0x9307, 0x929e, 0x9236, 0x91cf, 0x9169, 0x9105, 0x90a1, 0x903e, 0x8fdd, 0x8f7d
	.half	0x8f1d, 0x8ebf, 0x8e62, 0x8e06, 0x8dab, 0x8d51, 0x8cf8, 0x8ca1, 0x8c4a, 0x8bf5, 0x8ba0, 0x8b4d, 0x8afb, 0x8aaa, 0x8a5a, 0x8a0c
	.half	0x89be, 0x8972, 0x8927, 0x88dd, 0x8894, 0x884c, 0x8805, 0x87c0, 0x877b, 0x8738, 0x86f6, 0x86b6, 0x8676, 0x8637, 0x85fa, 0x85be
	.half	0x8583, 0x8549, 0x8511, 0x84d9, 0x84a3, 0x846e, 0x843a, 0x8407, 0x83d6, 0x83a6, 0x8377, 0x8349, 0x831c, 0x82f1, 0x82c6, 0x829d
	.half	0x8276, 0x824f, 0x822a, 0x8205, 0x81e2, 0x81c1, 0x81a0, 0x8181, 0x8163, 0x8146, 0x812a, 0x8110, 0x80f6, 0x80de, 0x80c8, 0x80b2
	.half	0x809e, 0x808b, 0x8079, 0x8068, 0x8059, 0x804b, 0x803e, 0x8032, 0x8027, 0x801e, 0x8016, 0x800f, 0x800a, 0x8006, 0x8002, 0x8001

	.half	0x8000, 0x8001, 0x8002, 0x8006, 0x800a, 0x800f, 0x8016, 0x801e, 0x8027, 0x8032, 0x803e, 0x804b, 0x8059, 0x8068, 0x8079, 0x808b
	.half	0x809e, 0x80b2, 0x80c8, 0x80de, 0x80f6, 0x8110, 0x812a, 0x8146, 0x8163, 0x8181, 0x81a0, 0x81c1, 0x81e2, 0x8205, 0x822a, 0x824f
	.half	0x8276, 0x829d, 0x82c6, 0x82f1, 0x831c, 0x8349, 0x8377, 0x83a6, 0x83d6, 0x8407, 0x843a, 0x846e, 0x84a3, 0x84d9, 0x8511, 0x8549
	.half	0x8583, 0x85be, 0x85fa, 0x8637, 0x8676, 0x86b6, 0x86f6, 0x8738, 0x877b, 0x87c0, 0x8805, 0x884c, 0x8894, 0x88dd, 0x8927, 0x8972
	.half	0x89be, 0x8a0c, 0x8a5a, 0x8aaa, 0x8afb, 0x8b4d, 0x8ba0, 0x8bf5, 0x8c4a, 0x8ca1, 0x8cf8, 0x8d51, 0x8dab, 0x8e06, 0x8e62, 0x8ebf
	.half	0x8f1d, 0x8f7d, 0x8fdd, 0x903e, 0x90a1, 0x9105, 0x9169, 0x91cf, 0x9236, 0x929e, 0x9307, 0x9371, 0x93dc, 0x9448, 0x94b5, 0x9523
	.half	0x9592, 0x9603, 0x9674, 0x96e6, 0x9759, 0x97ce, 0x9843, 0x98b9, 0x9930, 0x99a9, 0x9a22, 0x9a9c, 0x9b17, 0x9b94, 0x9c11, 0x9c8f
	.half	0x9d0e, 0x9d8e, 0x9e0f, 0x9e91, 0x9f14, 0x9f98, 0xa01c, 0xa0a2, 0xa129, 0xa1b0, 0xa238, 0xa2c2, 0xa34c, 0xa3d7, 0xa463, 0xa4f0
	.half	0xa57e, 0xa60c, 0xa69c, 0xa72c, 0xa7bd, 0xa84f, 0xa8e2, 0xa976, 0xaa0a, 0xaaa0, 0xab36, 0xabcd, 0xac65, 0xacfd, 0xad97, 0xae31
	.half	0xaecc, 0xaf68, 0xb005, 0xb0a2, 0xb140, 0xb1df, 0xb27f, 0xb31f, 0xb3c0, 0xb462, 0xb505, 0xb5a8, 0xb64c, 0xb6f1, 0xb796, 0xb83c
	.half	0xb8e3, 0xb98b, 0xba33, 0xbadc, 0xbb85, 0xbc2f, 0xbcda, 0xbd86, 0xbe32, 0xbedf, 0xbf8c, 0xc03a, 0xc0e9, 0xc198, 0xc248, 0xc2f8
	.half	0xc3a9, 0xc45b, 0xc50d, 0xc5c0, 0xc673, 0xc727, 0xc7db, 0xc890, 0xc946, 0xc9fc, 0xcab2, 0xcb69, 0xcc21, 0xccd9, 0xcd92, 0xce4b
	.half	0xcf04, 0xcfbe, 0xd079, 0xd134, 0xd1ef, 0xd2ab, 0xd367, 0xd424, 0xd4e1, 0xd59e, 0xd65c, 0xd71b, 0xd7d9, 0xd898, 0xd958, 0xda18
	.half	0xdad8, 0xdb99, 0xdc59, 0xdd1b, 0xdddc, 0xde9e, 0xdf61, 0xe023, 0xe0e6, 0xe1a9, 0xe26d, 0xe330, 0xe3f4, 0xe4b9, 0xe57d, 0xe642
	.half	0xe707, 0xe7cd, 0xe892, 0xe958, 0xea1e, 0xeae4, 0xebab, 0xec71, 0xed38, 0xedff, 0xeec6, 0xef8d, 0xf055, 0xf11c, 0xf1e4, 0xf2ac
	.half	0xf374, 0xf43c, 0xf505, 0xf5cd, 0xf695, 0xf75e, 0xf827, 0xf8ef, 0xf9b8, 0xfa81, 0xfb4a, 0xfc13, 0xfcdc, 0xfda5, 0xfe6e, 0xff37
	.half	0x0000,	0x0000

 #---------------------------------------------------------------
 # long sinL(short angle)
 #	break:	$24,$25
 #---------------------------------------------------------------
	.text
	.align	2
	.globl	sinL
	.ent	sinL 2
sinL:
	.frame	$sp, 0, $31

	srl	$24, $4, 5
	andi	$25, $4, 0x003f
	andi	$24, 0x07fe

	lh	$2, sintable($24)
	beqz	$25, $sin_ok

	lh	$24, sintable+2($24)
	subu	$24, $2
	mul	$24, $24, $25
	sra	$25, $24, 6
	addu	$2, $25
$sin_ok:
	.livereg	0x0000FF0E,0x00000FFF
	j	$31
	.end	sinL

 #---------------------------------------------------------------
 # long cosL(short angle)
 #	break:	$24,$25
 #---------------------------------------------------------------
	.text	
	.align	2
	.globl	cosL
	.ent	cosL 2
cosL:
	.frame	$sp, 0, $31

	addiu	$2, $4, 0x4000

	srl	$24, $2, 5
	andi	$25, $2, 0x003f
	andi	$24, 0x07fe

	lh	$2, sintable($24)
	beqz	$25, $cos_ok

	lh	$24, sintable+2($24)
	subu	$24, $2
	mul	$24, $24, $25
	sra	$25, $24, 6
	addu	$2, $25
$cos_ok:
	.livereg	0x0000FF0E,0x00000FFF
	j	$31
	.end	cosL


 #---------------------------------------------------------------
 # void	lc2wc(LMatrix m, long x, long y, long z, long *px, long *py, long *pz);
 #---------------------------------------------------------------
	.text	
	.align	2
	.globl	lc2wc
	.ent	lc2wc 2
lc2wc:
	.frame	$sp, 0, $31

	li	$2, 3
	addiu	$9, $sp, 16		## *px

	## *p[i] = m[3][i] + ( ( m[0][i]*x + m[1][i]*y + m[2][i]*z ) >> 15 ); 
$lp1:
	lw	$8, 0($4)
	lw	$15, 16($4)
	lw	$14, 32($4)
	mult	$8, $5
	mfhi	$24
	mflo	$25

	lw	$8, 0($9)		## *px
	addiu	$9, $9, 4
	mult	$15, $6
	mfhi	$10
	mflo	$11
	addu	$13, $11, $25		## $12,13 = $10,11 + $24,25
	sltu	$11, $13, $25
	addu	$11, $11, $10
	addu	$12, $11, $24

	mult	$14, $7
	mfhi	$10
	mflo	$11
	addu	$25, $11, $13		## $24,25 = $10,11 + $12,13
	sltu	$11, $25, $13
	addu	$11, $11, $10
	addu	$24, $11, $12

	lw	$14, 48($4)
	addiu	$4, $4, 4
	sll	$10, $24, 17
	srl	$11, $25, 15
	or	$25, $11, $10
	addu	$11, $14, $25

	sw	$11, 0($8)

	addiu	$2, $2 , -1
	bgtz	$2, $lp1

	.livereg	0x0000FF0E,0x00000FFF
	j	$31
	.end	lc2wc


 #---------------------------------------------------------------
 # void	wc2lc(LMatrix m, long x, long y, long z, long *px, long *py, long *pz);
 #---------------------------------------------------------------
	.text	
	.align	2
	.globl	wc2lc
	.ent	wc2lc 2
wc2lc:
	.frame	$sp, 0, $31

	li	$2, 3
	addiu	$9, $sp, 16		## *px

	## x -= m[3][0];
	## y -= m[3][1];
	## z -= m[3][2];
	lw	$13, 48($4)
	lw	$14, 52($4)
	lw	$15, 56($4)
	subu	$5, $5, $13
	subu	$6, $6, $14
	subu	$7, $7, $15

	## *p[i] = ( m[i][0]*x + m[i][1]*y + m[i][2]*z ) >> 15; 
$lp2:
	lw	$8, 0($4)
	lw	$15, 4($4)
	lw	$14, 8($4)
	addiu	$4, $4, 16
	mult	$8, $5
	mfhi	$24
	mflo	$25

	lw	$8, 0($9)		## *px
	addiu	$9, $9, 4
	mult	$15, $6
	mfhi	$10
	mflo	$11
	addu	$13, $11, $25		## $12,13 = $10,11 + $24,25
	sltu	$11, $13, $25
	addu	$11, $11, $10
	addu	$12, $11, $24

	mult	$14, $7
	mfhi	$10
	mflo	$11
	addu	$25, $11, $13		## $24,25 = $10,11 + $12,13
	sltu	$11, $25, $13
	addu	$11, $11, $10
	addu	$24, $11, $12

	sll	$10, $24, 17
	srl	$11, $25, 15
	or	$25, $11, $10

	sw	$25, 0($8)

	addiu	$2, $2 , -1
	bgtz	$2, $lp2

	.livereg	0x0000FF0E,0x00000FFF
	j	$31
	.end	wc2lc


 #---------------------------------------------------------------
 # void	matrixMulL(LMatrix m, LMatrix n, LMatrix r)
 #---------------------------------------------------------------
	.text
	.align	2
	.globl	matrixMulL
	.ent	matrixMulL 2
matrixMulL:
	.frame	$sp, 0, $31

	## r[0][3] = r[1][3] = r[2][3] = 0; r[3][3] = 0x00008000;
	li	$24, 0x8000
	sw	$0, 44($6)
	sw	$0, 28($6)
	sw	$0, 12($6)
	sw	$24, 60($6)

	## for ( i = 0; i < 3; i++ ) {
	li	$2, 3
	move	$3, $6
	move	$8, $5
	move	$9, $4
$lp3:
	## r[i][0] = ( m[i][0]*n[0][0] + m[i][1]*n[1][0] + m[i][2]*n[2][0] ) >> 15;
	lw	$15, 0($9)
	lw	$14, 4($9)

	lw	$25, 0($5)
	lw	$13, 16($5)
	mult	$15, $25
	mflo	$11
	mfhi	$10
	lw	$24, 8($9)
	lw	$25, 32($5)
	mult	$14, $13
	mflo	$12
	addu	$13, $11, $12		## $12,13 = $10,11 + $24,25
	sltu	$11, $13, $12
	mfhi	$12
	addu	$12, $10, $12
	addu	$12, $12, $11
	mult	$24, $25
	mflo	$11
	mfhi	$10
	addu	$25, $11, $13		## $24,25 = $10,11 + $12,13
	sltu	$11, $25, $13
	addu	$11, $11, $10
	addu	$24, $11, $12
	sll	$10, $24, 17
	srl	$11, $25, 15
	or	$24, $11, $10
	sw	$24, 0($3)

	## r[i][1] = ( m[i][0]*n[0][1] + m[i][1]*n[1][1] + m[i][2]*n[2][1] ) >> 15;
	lw	$25, 4($5)
	lw	$13, 20($5)
	mult	$15, $25
	mflo	$11
	mfhi	$10
	lw	$24, 8($9)
	lw	$25, 36($5)
	mult	$14, $13
	mflo	$12
	addu	$13, $11, $12		## $12,13 = $10,11 + $24,25
	sltu	$11, $13, $12
	mfhi	$12
	addu	$12, $10, $12
	addu	$12, $12, $11
	mult	$24, $25
	mflo	$11
	mfhi	$10
	addu	$25, $11, $13		## $24,25 = $10,11 + $12,13
	sltu	$11, $25, $13
	addu	$11, $11, $10
	addu	$24, $11, $12
	sll	$10, $24, 17
	srl	$11, $25, 15
	or	$24, $11, $10
	sw	$24, 4($3)

	## r[i][2] = ( m[i][0]*n[0][2] + m[i][1]*n[1][2] + m[i][2]*n[2][2] ) >> 15;
	lw	$25, 8($5)
	lw	$13, 24($5)
	mult	$15, $25
	mflo	$11
	mfhi	$10
	lw	$24, 8($9)
	lw	$25, 40($5)
	mult	$14, $13
	mflo	$12
	addu	$13, $11, $12		## $12,13 = $10,11 + $24,25
	sltu	$11, $13, $12
	mfhi	$12
	addu	$12, $10, $12
	addu	$12, $12, $11
	mult	$24, $25
	mflo	$11
	mfhi	$10
	addu	$25, $11, $13		## $24,25 = $10,11 + $12,13
	sltu	$11, $25, $13
	addu	$11, $11, $10
	addu	$24, $11, $12
	sll	$10, $24, 17
	srl	$11, $25, 15
	or	$24, $11, $10
	sw	$24, 8($3)

	## r[3][i] = ( ( m[3][0]*n[0][i] + m[3][1]*n[1][i] + m[3][2]*n[2][i] ) >> 15 ) + n[3][i];
	lw	$10, 48($4)
	lw	$11, 0($8)
	lw	$12, 52($4)
	lw	$13, 16($8)
	mult	$10, $11
	mflo	$25
	mfhi	$24
	lw	$14, 56($4)
	lw	$15, 32($8)
	mult	$12, $13
	mflo	$11
	mfhi	$10
	addu	$13, $11, $25		## $12,13 = $10,11 + $24,25
	sltu	$11, $13, $25
	addu	$11, $11, $10
	addu	$12, $11, $24
	mult	$14, $15
	mflo	$11
	mfhi	$10
	addu	$25, $11, $13		## $24,25 = $10,11 + $12,13
	sltu	$11, $25, $13
	addu	$11, $11, $10
	addu	$24, $11, $12
	lw	$14, 48($8)
	sll	$10, $24, 17
	srl	$11, $25, 15
	or	$25, $11, $10
	addu	$11, $14, $25
	sw	$11, 48($6)

	addiu	$2, $2, -1
	addiu	$3, $3, 16
	addiu	$9, $9, 16
	addiu	$6, $6, 4
	addiu	$8, $8, 4
	bgtz	$2, $lp3

	.livereg	0x0000FF0E,0x00000FFF
	j	$31
	.end	matrixMulL


 #---------------------------------------------------------------
 # void makeTransrateMatrix(LMatrix m, s32 xofs, s32 yofs, s32 zofs)
 #---------------------------------------------------------------
	.text	
	.align	2
	.globl	makeTransrateMatrix
	.ent	makeTransrateMatrix 2
makeTransrateMatrix:
	.frame	$sp, 0, $31

	ori	$2, $0, 0x8000
	sw	$2, 0($4)
	sw	$0, 4($4)
	sw	$0, 8($4)
	sw	$0, 12($4)
	sw	$0, 16($4)
	sw	$2, 20($4)
	sw	$0, 24($4)
	sw	$0, 28($4)
	sw	$0, 32($4)
	sw	$0, 36($4)
	sw	$2, 40($4)
	sw	$0, 44($4)
	sw	$5, 48($4)
	sw	$6, 52($4)
	sw	$7, 56($4)
	sw	$2, 60($4)

	.livereg	0x0000FF0E,0x00000FFF
	j	$31
	.end	makeTransrateMatrix


 #---------------------------------------------------------------
 # void makeScaleMatrix(LMatrix m, s32 scale)
 #---------------------------------------------------------------
	.text	
	.align	2
	.globl	makeScaleMatrix
	.ent	makeScaleMatrix 2
makeScaleMatrix:
	.frame	$sp, 0, $31

	ori	$2, $0, 0x8000
	sw	$5, 0($4)
	sw	$0, 4($4)
	sw	$0, 8($4)
	sw	$0, 12($4)
	sw	$0, 16($4)
	sw	$5, 20($4)
	sw	$0, 24($4)
	sw	$0, 28($4)
	sw	$0, 32($4)
	sw	$0, 36($4)
	sw	$5, 40($4)
	sw	$0, 44($4)
	sw	$0, 48($4)
	sw	$0, 52($4)
	sw	$0, 56($4)
	sw	$2, 60($4)

	.livereg	0x0000FF0E,0x00000FFF
	j	$31
	.end	makeScaleMatrix


 #---------------------------------------------------------------
 # void makeMatrix(LMatrix m, s16 xrot, s16 yrot, s16 zrot, s32 xofs, s32 yofs, s32 zofs);
 #---------------------------------------------------------------
	.text	
	.align	2
	.globl	makeMatrix
	.ent	makeMatrix 2
makeMatrix:
	.frame	$sp, 0, $31

	move	$9, $31
	move	$8, $4

	lw	$10, 16($sp)
	lw	$11, 20($sp)
	lw	$12, 24($sp)
 # m[0][3] = m[1][3] = m[2][3] = 0
	sw	$0, 44($8)
	sw	$0, 28($8)
	sw	$0, 12($8)
 # m[3][3] = 0x00008000
	ori	$24, $0, 0x8000
	sw	$24, 60($8)
 # m[3][0] = X
	sw	$10, 48($8)
 # m[3][0] = Y
	sw	$11, 52($8)
 # m[3][0] = Z
	sw	$12, 56($8)

 # sy = sin(yrot);
	move	$4, $6
	bal	sinL
	move	$10, $2
 # cy = cos(yrot);
	bal	cosL
	move	$11, $2

 # sz = sin(zrot);
	move	$4, $7
	bal	sinL
	move	$12, $2
 # cz = cos(zrot);
	bal	cosL
	move	$13, $2

 # sx = sin(xrot);
	move	$4, $5
	bal	sinL
	move	$14, $2
 # cx = cos(xrot);
	bal	cosL

 # m[2][1] = ( -sx );
	negu	$24, $14
	sw	$24, 36($8)

 # m[0][1] = ( cx * sz ) >> 15;
	mul	$24, $12, $2
	sra	$25, $24, 15
	sw	$25, 4($8)
 # m[1][1] = ( cx * cz ) >> 15;
	mul	$24, $13, $2
	sra	$25, $24, 15
	sw	$25, 20($8)
 # m[2][0] = ( cx * sy ) >> 15;
	mul	$24, $10, $2
	sra	$25, $24, 15
	sw	$25, 32($8)

 # m[2][2] = ( cx * cy ) >> 15;
	mul	$24, $11, $2
	sra	$25, $24, 15
	sw	$25, 40($8)

 # wk = ( sx * sy ) >> 15;
	mul	$24, $10, $14
	sra	$2, $24, 15
 # m[0][0] = ( sz*wk + cz*cy ) >> 15;
	mul	$25, $13, $11
	mul	$24, $12, $2
	addu	$15, $24, $25
	sra	$24, $15, 15
	sw	$24, 0($8)
 # m[1][0] = ( cz*wk - sz*cy ) >> 15;
	mul	$25, $12, $11
	mul	$24, $13, $2
	subu	$15, $24, $25
	sra	$24, $15, 15
	sw	$24, 16($8)

 # wk = ( sx * cy ) >> 15;
	mul	$24, $11, $14
	sra	$2, $24, 15
 # m[0][2] = ( sz*wk - cz*sy ) >> 15;
	mul	$25, $13, $10
	mul	$24, $12, $2
	subu	$15, $24, $25
	sra	$24, $15, 15
	sw	$24, 8($8)
 # m[1][2] = ( cz*wk + sz*sy ) >> 15;
	mul	$25, $12, $10
	mul	$24, $13, $2
	addu	$15, $24, $25
	sra	$24, $15, 15
	sw	$24, 24($8)

	move	$31, $9
	.livereg	0x0000FF0E,0x00000FFF
	j	$31
	.end	makeMatrix


 #---------------------------------------------------------------
 # void makeXrotMatrix(LMatrix m, s16 xrot, s32 xofs, s32 yofs, s32 zofs);
 #---------------------------------------------------------------
	.text	
	.align	2
	.globl	makeXrotMatrix
	.ent	makeXrotMatrix 2
makeXrotMatrix:
	.frame	$sp, 0, $31

	move	$9, $31
	move	$8, $4

	lw	$10, 16($sp)
 # m[0][3] = m[1][3] = m[2][3] = 0
	sw	$0, 44($8)
	sw	$0, 28($8)
	sw	$0, 12($8)
 # m[3][0] = X
	sw	$6, 48($8)
 # m[3][0] = Y
	sw	$7, 52($8)
 # m[3][0] = Z
	sw	$10, 56($8)
 # m[3][3] = 0x00008000
	ori	$10, $0, 0x8000
	sw	$10, 60($8)

 # sx = sin(xrot);
	move	$4, $5
	bal	sinL
	move	$11, $2
 # cx = cos(xrot);
	bal	cosL

 # m[0][0] = 1
	sw	$10, 0($8)

 # m[0][1] = 0
	sw	$0, 4($8)

 # m[0][2] = 0
	sw	$0, 8($8)

 # m[1][0] = 0
	sw	$0, 16($8)

 # m[1][1] = cx;
	sw	$2, 20($8)

 # m[1][2] = sx
	sw	$11, 24($8)

 # m[2][0] = 0
	sw	$0, 32($8)

 # m[2][1] = ( -sx );
	negu	$24, $11
	sw	$24, 36($8)

 # m[2][2] = cx
	sw	$2, 40($8)

	move	$31, $9
	.livereg	0x0000FF0E,0x00000FFF
	j	$31
	.end	makeXrotMatrix


 #---------------------------------------------------------------
 # void makeYrotMatrix(LMatrix m, s16 yrot, s32 xofs, s32 yofs, s32 zofs);
 #---------------------------------------------------------------
	.text	
	.align	2
	.globl	makeYrotMatrix
	.ent	makeYrotMatrix 2
makeYrotMatrix:
	.frame	$sp, 0, $31

	move	$9, $31
	move	$8, $4

	lw	$10, 16($sp)
 # m[0][3] = m[1][3] = m[2][3] = 0
	sw	$0, 44($8)
	sw	$0, 28($8)
	sw	$0, 12($8)
 # m[3][0] = X
	sw	$6, 48($8)
 # m[3][0] = Y
	sw	$7, 52($8)
 # m[3][0] = Z
	sw	$10, 56($8)
 # m[3][3] = 0x00008000
	ori	$10, $0, 0x8000
	sw	$10, 60($8)

 # sy = sin(yrot);
	move	$4, $5
	bal	sinL
	move	$11, $2
 # cy = cos(yrot);
	bal	cosL

 # m[0][0] = cy
	sw	$2, 0($8)

 # m[0][1] = 0
	sw	$0, 4($8)

 # m[0][2] = -sy
	negu	$24, $11
	sw	$24, 8($8)

 # m[1][0] = 0
	sw	$0, 16($8)

 # m[1][1] = 1;
	sw	$10, 20($8)

 # m[1][2] = 0
	sw	$0, 24($8)

 # m[2][0] = sy
	sw	$11, 32($8)

 # m[2][1] = 0;
	sw	$0, 36($8)

 # m[2][2] = cy
	sw	$2, 40($8)

	move	$31, $9
	.livereg	0x0000FF0E,0x00000FFF
	j	$31
	.end	makeYrotMatrix


 #---------------------------------------------------------------
 # void makeZrotMatrix(LMatrix m, s16 zrot, s32 xofs, s32 yofs, s32 zofs);
 #---------------------------------------------------------------
	.text	
	.align	2
	.globl	makeZrotMatrix
	.ent	makeZrotMatrix 2
makeZrotMatrix:
	.frame	$sp, 0, $31

	move	$9, $31
	move	$8, $4

	lw	$10, 16($sp)
 # m[0][3] = m[1][3] = m[2][3] = 0
	sw	$0, 44($8)
	sw	$0, 28($8)
	sw	$0, 12($8)
 # m[3][0] = X
	sw	$6, 48($8)
 # m[3][0] = Y
	sw	$7, 52($8)
 # m[3][0] = Z
	sw	$10, 56($8)
 # m[3][3] = 0x00008000
	ori	$10, $0, 0x8000
	sw	$10, 60($8)

 # sz = sin(zrot);
	move	$4, $5
	bal	sinL
	move	$11, $2
 # cz = cos(zrot);
	bal	cosL

 # m[0][0] = cz
	sw	$2, 0($8)

 # m[0][1] = sz
	sw	$11, 4($8)

 # m[0][2] = 0
	sw	$0, 8($8)

 # m[1][0] = -sz
	negu	$24, $11
	sw	$24, 16($8)

 # m[1][1] = cz
	sw	$2, 20($8)

 # m[1][2] = 0
	sw	$0, 24($8)

 # m[2][0] = 0
	sw	$0, 32($8)

 # m[2][1] = 0;
	sw	$0, 36($8)

 # m[2][2] = 1
	sw	$10, 40($8)

	move	$31, $9
	.livereg	0x0000FF0E,0x00000FFF
	j	$31
	.end	makeZrotMatrix


 #---------------------------------------------------------------
 # void makeXZMatrix(LMatrix m, s16 xrot, s16 zrot);
 #---------------------------------------------------------------
	.text
	.align	2
	.globl	makeXZMatrix
	.ent	makeXZMatrix 2
makeXZMatrix:
	.frame	$sp, 0, $31

	move	$9, $31
	move	$8, $4

 # m[0][3] = m[1][3] = m[2][3] = 0
	sw	$0, 44($8)
	sw	$0, 28($8)
	sw	$0, 12($8)
 # m[3][0] = X
	sw	$0, 48($8)
 # m[3][0] = Y
	sw	$0, 52($8)
 # m[3][0] = Z
	sw	$0, 56($8)
 # m[3][3] = 0x00008000
	ori	$10, $0, 0x8000
	sw	$10, 60($8)

 # sz = sin(zrot);
	move	$4, $6
	bal	sinL
	move	$10, $2
 # cz = cos(zrot);
	bal	cosL
	move	$11, $2

 # sx = sin(xrot);
	move	$4, $5
	bal	sinL
	move	$12, $2
 # cx = cos(xrot);
	bal	cosL

 # m[0][0] = cz
	sw	$11, 0($8)

 # m[0][1] = sz
	sw	$10, 4($8)

 # m[0][2] = 0
	sw	$0, 8($8)

 # m[1][0] = -sz*cx
	negu	$24, $10
	mul	$25, $24, $2
	sra	$24, $25, 15
	sw	$24, 16($8)

 # m[1][1] = cx*cz
	mul	$25, $11, $2
	sra	$24, $25, 15
	sw	$24, 20($8)

 # m[1][2] = sx
	sw	$12, 24($8)

 # m[2][0] = sz*sx
	mul	$25, $10, $12
	sra	$24, $25, 15
	sw	$24, 32($8)

 # m[2][1] = -sx*cz
	negu	$24, $12
	mul	$25, $24, $11
	sra	$24, $25, 15
	sw	$24, 36($8)

 # m[2][2] = cx
	sw	$2, 40($8)

	move	$31, $9
	.livereg	0x0000FF0E,0x00000FFF
	j	$31
	.end	makeXZMatrix


 #---------------------------------------------------------------
 # void	matrixConv(LMatrix m, Mtx *mtx, s32 shift);
 #---------------------------------------------------------------
	.text	
	.align	2
	.globl	matrixConv
	.ent	matrixConv 2
matrixConv:
	.frame	$sp, 0, $31

	lui	$8, 0xffff
	li	$2, 8
	beqz	$6, $cnv_lp0

	li	$2, 6
	bgtz	$6, $cnv_lbl1

 #----- Matrix Scale < 1
	negu	$6
	addiu	$6, -1
$cnv_lp2:
	lw	$24, 0($4)
	lw	$25, 4($4)
	addiu	$4, 8

	addiu	$2, -1

	sra	$10, $24, $6
	sra	$11, $25, $6
	and	$12, $10, $8
	srl	$13, $11, 16
	sll	$14, $10, 16
	andi	$15, $11, 0xffff
	or	$24, $12, $13
	or	$25, $14, $15
	sw	$24, 0($5)
	sw	$25, 32($5)
	addiu	$5, 4

	bgtz	$2, $cnv_lp2
	b	$cnv_lbl0

 #----- Matrix Scale > 1
$cnv_lbl1:
	addiu	$6, 1
$cnv_lp1:
	lw	$24, 0($4)
	lw	$25, 4($4)
	addiu	$4, 8

	addiu	$2, -1

	sll	$10, $24, $6
	sll	$11, $25, $6
	and	$12, $10, $8
	srl	$13, $11, 16
	sll	$14, $10, 16
	andi	$15, $11, 0xffff
	or	$24, $12, $13
	or	$25, $14, $15
	sw	$24, 0($5)
	sw	$25, 32($5)
	addiu	$5, 4

	bgtz	$2, $cnv_lp1

$cnv_lbl0:
	li	$2, 2

 #----- Matrix Scale = 1
$cnv_lp0:
	lw	$24, 0($4)
	lw	$25, 4($4)
	addiu	$4, 8

	addiu	$2, -1

	sll	$10, $24, 1
	sll	$11, $25, 1
	and	$12, $10, $8
	srl	$13, $11, 16
	sll	$14, $10, 16
	andi	$15, $11, 0xffff
	or	$24, $12, $13
	or	$25, $14, $15
	sw	$24, 0($5)
	sw	$25, 32($5)
	addiu	$5, 4

	bgtz	$2, $cnv_lp0

$cnv_end:
	.livereg	0x0000FF0E,0x00000FFF
	j	$31
	.end	matrixConv


 #---------------------------------------------------------------
 # void	matrixCopyL(LMatrix dst, LMatrix src);
 #---------------------------------------------------------------
	.text	
	.align	2
	.globl	matrixCopyL
	.ent	matrixCopyL 2
matrixCopyL:
	.frame	$sp, 0, $31

	li	$24, 4
$lp0:
	lw	$10, 0($5)
	lw	$11, 4($5)
	lw	$12, 8($5)
	lw	$13, 12($5)
	sw	$10, 0($4)
	sw	$11, 4($4)
	sw	$12, 8($4)
	sw	$13, 12($4)

	addi	$24, -1
	addi	$4, 16
	addi	$5, 16
	bgtz	$24, $lp0

	.livereg	0x0000FF0E,0x00000FFF
	j	$31
	.end	matrixCopyL


 #---------------------------------------------------------------
 # void	rotpointL(short a, long ox, long oy, long *x, long *y);
 #---------------------------------------------------------------
	.text
	.align	2
	.globl	rotpointL
	.ent	rotpointL 2
rotpointL:
	.frame	$sp, 0, $31

	move	$9, $31
	lw	$8, 16($sp)		# *y

	# cos(a);
	bal	cosL
	move	$3, $2
	# sin(a);
	bal	sinL

	lw	$15, 0($8)		# y
	lw	$14, 0($7)		# x

    	# dy = *y - oy;
	subu	$25, $15, $6

    	# *y = ( dy * ca + dx * sa ) + y0;
	mult	$25, $3
	mfhi	$10
	mflo	$11

    	# dx = *x - ox;
	subu	$24, $14, $5
	negu	$25

	mult	$24, $2
	mfhi	$12
	mflo	$13

	addu	$4, $11, $13		## $14,4 = $10,11 + $12,13
	sltu	$14, $4, $13
	addu	$15, $14, $10
	addu	$14, $15, $12
	srl	$13, $4, 15
	sll	$12, $14, 17

    	# *x = ( dx * ca - dy * sa ) + x0;
	mult	$24, $3
	mfhi	$10
	mflo	$11

	or	$3, $12, $13
	addu	$6, $3

	mult	$25, $2
	mfhi	$12
	mflo	$13

	addu	$4, $11, $13		## $3,4 = $10,11 + $12,13
	sltu	$14, $4, $13
	addu	$15, $14, $10
	addu	$3, $15, $12
	srl	$13, $4, 15
	sll	$12, $3, 17
	or	$3, $12, $13
	addu	$5, $3

	sw	$6, 0($8)
	sw	$5, 0($7)

	move	$31, $9
	.livereg	0x0000FF0E,0x00000FFF
	j	$31
	.end	rotpointL


 #---------------------------------------------------------------
 # long	defangleL(short a1, short a2);
 #	break:	$3, $4, $5, $10, $11, $12, $13
 #---------------------------------------------------------------
	.text
	.align	2
	.globl	defangleL
	.ent	defangleL 2
defangleL:
	.frame	$sp, 0, $31

	# angle1 &= 0x0000ffff;
	# angle2 &= 0x0000ffff;
	andi	$10, $4, 0x0000ffff
	andi	$11, $5, 0x0000ffff
	lui	$2, 0x0001		# $2 = 0x00010000

	# if ( angle1 < angle2 )
	bge	$10, $11, $less
	# angle01 = angle1 + 0x10000;
	# angle02 = angle2;
	addu	$12 ,$10, $2
	move	$13 ,$11
	b	$chk
$less:
	# angle01 = angle1;
	# angle02 = angle2 + 0x10000;
	move	$12 ,$10
	addu	$13 ,$11, $2
$chk:
	# angle2 -= angle1;
	# angle02 -= angle01;
	subu	$2, $11, $10
	subu	$3, $13, $12

	#if ( ABS(angle2) > ABS(angle02) ) d = angle02; else d = angle2;
	abs	$10, $2
	abs	$11, $3

	ble	$10, $11, $def_ok
	move	$2, $3
$def_ok:
	.livereg	0x0000FF0E,0x00000FFF
	j	$31
	.end	defangleL


 #---------------------------------------------------------------
 # long	distanceS(long x1, long y1, long z1, long x2, long y2, long z2)
 #---------------------------------------------------------------
	.text	
	.align	2
	.globl	distanceS
	.ent	distanceS 2
distanceS:
	.frame	$sp, 0, $31

	subu	$3, $7, $4
	lw	$14, 16($sp)
	mult	$3, $3
	mflo	$24
	subu	$8, $14, $5
	lw	$15, 20($sp)
	mult	$8, $8
	mflo	$25
	subu	$9, $15, $6
	addu	$10, $24, $25
	mult	$9, $9
	mflo	$11
	addu	$12, $10, $11

	mtc1	$12, $f4
	cvt.s.w	$f0, $f4
	sqrt.s	$f6, $f0
	trunc.w.s	$f8, $f6, $13
	mfc1	$2, $f8

	.livereg	0x0000FF0E,0x00000FFF
	j	$31
	.end	distanceS


 #---------------------------------------------------------------
 # long	distanceL(long x1, long y1, long z1, long x2, long y2, long z2)
 #---------------------------------------------------------------
	.text	
	.align	2
	.globl	distanceL
	.ent	distanceL 2
distanceL:
	.frame	$sp, 0, $31

	lw	$24, 16($sp)
	lw	$25, 20($sp)
	subu	$2, $7, $4
	subu	$24, $24, $5
	subu	$25, $25, $6
	mtc1	$2, $f4
	mtc1	$24, $f6
	mtc1	$25, $f8
	cvt.s.w	$f0, $f4
	cvt.s.w	$f4, $f6
	cvt.s.w	$f6, $f8
	mul.s	$f0, $f0, $f0
	mul.s	$f4, $f4, $f4
	mul.s	$f6, $f6, $f6
	add.s	$f8, $f0, $f4
	add.s	$f0, $f6, $f8
	sqrt.s	$f6, $f0
	trunc.w.s	$f8, $f6, $13
	mfc1	$2, $f8

	#subu	$3, $7, $4
	#lw	$14, 16($sp)
	#lw	$15, 20($sp)
	#mult	$3, $3
	#mfhi	$24
	#mflo	$25
	#subu	$8, $14, $5
	#subu	$9, $15, $6
	#move	$7, $31
	#mult	$8, $8
	#mfhi	$10
	#mflo	$11
	#addu	$13, $11, $25		## $12,13 = $10,11 + $24,25
	#sltu	$14, $13, $25
	#addu	$15, $14, $10
	#addu	$12, $15, $24
	#mult	$9, $9
	#mfhi	$10
	#mflo	$11
	#addu	$5, $11, $13		## $4,5 = $10,11 + $12,13
	#sltu	$14, $5, $13
	#addu	$15, $14, $10
	#addu	$4, $15, $12
	#bal	sqrtl
	#move	$31, $7

	.livereg	0x0000FF0E,0x00000FFF
	j	$31
	.end	distanceL


 #---------------------------------------------------------------
 # long	sqrt_a2b2(long a, long b)
 #	return = sqrt( a*a + b*b )
 #---------------------------------------------------------------
	.text
	.align	2
	.globl	sqrt_a2b2
	.ent	sqrt_a2b2 2
sqrt_a2b2:
	.frame	$sp, 0, $31

	mtc1	$4, $f4
	mtc1	$5, $f6
	cvt.s.w	$f0, $f4
	cvt.s.w	$f4, $f6
	mul.s	$f6, $f0, $f0
	mul.s	$f8, $f4, $f4
	add.s	$f0, $f6, $f8
	sqrt.s	$f6, $f0
	trunc.w.s	$f8, $f6, $13
	mfc1	$2, $f8

	#mult	$4, $4
	#mfhi	$10
	#mflo	$11
	#move	$7, $31
	#sll	$0, $0, 1		# nop
	#mult	$5, $5
	#mfhi	$12
	#mflo	$13
	#addu	$5, $11, $13		## $4,5 = $10,11 + $12,13
	#sltu	$14, $5, $13
	#addu	$15, $14, $10
	#addu	$4, $15, $12
	#bal	sqrtl
	#move	$31, $7

	.livereg	0x0000FF0E,0x00000FFF
	j	$31
	.end	sqrt_a2b2


 #---------------------------------------------------------------
 # long	sqrt_abc(long a, long b, long c)
 #	return = sqrt( a*a + b*b + c*c)
 #---------------------------------------------------------------
	.text
	.align	2
	.globl	sqrt_abc
	.ent	sqrt_abc 2
sqrt_abc:
	.frame	$sp, 0, $31

	mtc1	$4, $f4
	mtc1	$5, $f6
	mtc1	$6, $f8
	cvt.s.w	$f0, $f4
	cvt.s.w	$f4, $f6
	mul.s	$f6, $f0, $f0
	mul.s	$f0, $f4, $f4
	cvt.s.w	$f4, $f8
	mul.s	$f8, $f4, $f4
	add.s	$f4, $f6, $f0
	add.s	$f0, $f4, $f8
	sqrt.s	$f6, $f0
	trunc.w.s	$f8, $f6, $13
	mfc1	$2, $f8

	.livereg	0x0000FF0E,0x00000FFF
	j	$31
	.end	sqrt_abc


 #---------------------------------------------------------------
 # long	muldiv(long a, long b, long c)
 #	return (a*b)/c
 #---------------------------------------------------------------
	.text	
	.align	2
	.globl	muldiv
	.ent	muldiv 2
muldiv:
	.frame	$sp, 0, $31

	xor	$25, $4, $5
	xor	$24, $25, $6		# sign flag
	abs	$10, $4
	abs	$11, $5
	abs	$15, $6

	mult	$10, $11
	li	$25, 32
	move	$2, $0
	mflo	$9
	mfhi	$8

$mdl0:	sll	$2, 1
	bgt	$15, $8, $mdl1
	or	$2, 1
	subu	$8, $15

$mdl1:	addiu	$25, $25, -1
	srl	$10, $9, 31
	sll	$11, $8, 1
	sll	$9, 1
	or	$8, $10, $11
	bgez	$25, $mdl0

	bgez	$24, $mdret
	neg	$2
$mdret:
	.livereg	0x0000FF0E,0x00000FFF
	j	$31
	.end	muldiv


 #---------------------------------------------------------------
 # void makeVect(s16 xrot, s16 yrot, s32 *vx, s32 *vy, s32 *vz);
 #---------------------------------------------------------------
	.text	
	.align	2
	.globl	makeVect
	.ent	makeVect 2
makeVect:
	.frame	$sp, 0, $31

	move	$9, $31
	lw	$8, 16($sp)

 # sx = sin(xrot);
	bal	sinL
	negu	$12, $2
 # cx = cos(xrot);
	bal	cosL
	move	$13, $2

 # sy = sin(yrot);
	move	$4, $5
	bal	sinL
	move	$10, $2
 # cy = cos(yrot);
	bal	cosL
	move	$11, $2

 # Y = ( -sx );
	sw	$12, 0($7)

 # X = ( cx * sy ) >> 15;
	mul	$24, $10, $13
	sra	$25, $24, 15
	sw	$25, 0($6)

 # Z = ( cx * cy ) >> 15;
	mul	$24, $11, $13
	sra	$25, $24, 15
	sw	$25, 0($8)

	move	$31, $9
	.livereg	0x0000FF0E,0x00000FFF
	j	$31
	.end	makeVect
