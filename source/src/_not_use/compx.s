	.option	pic0
	.option	O2

#define INDEX_BIT_COUNT		14
#define LENGTH_BIT_COUNT	4
#define WINDOW_SIZE		( 1 << INDEX_BIT_COUNT )
#define RAW_LOOK_AHEAD_SIZE	( 1 << LENGTH_BIT_COUNT )
#define BREAK_EVEN		( ( 1 + INDEX_BIT_COUNT + LENGTH_BIT_COUNT ) / 9 )
#define LOOK_AHEAD_SIZE		( RAW_LOOK_AHEAD_SIZE + BREAK_EVEN )
#define END_OF_STREAM		0
#define MOD_WINDOW(a)		( ( a ) & ( WINDOW_SIZE - 1 ) )
#define WINDOW_MASK		( WINDOW_SIZE - 1 )


 #---------------------------------------------------------------
 #static	void InitBitRead(u8 *addr)
 #{
 #	bit_data.addr = addr;	+0
 #	bit_data.mask = 0x80;	+4
 #	bit_data.rack = 0x00;	+5
 #}
 #---------------------------------------------------------------

 #---------------------------------------------------------------
 #static	int GetBit(void)
 #{
 #	int value;
 #
 #	if ( bit_data.mask == 0x80 ) {
 #		bit_data.rack = *bit_data.addr;
 #		bit_data.addr++;
 #	}
 #	value = (int)( bit_data.rack & bit_data.mask );
 #	bit_data.mask >>= 1;
 #	if ( bit_data.mask == 0 ) bit_data.mask = 0x80;
 #
 #	return( value );
 #}
 #---------------------------------------------------------------

 #---------------------------------------------------------------
 #static	u32 GetBits(int bit_count)
 #{
 #	u32	mask;
 #	u32	return_value;
 #
 #	mask = 1L << ( bit_count - 1 );
 #	return_value = 0;
 #	while ( mask != 0 ) {
 #		if ( bit_data.mask == 0x80 ) {
 #			bit_data.rack = *bit_data.addr;
 #			bit_data.addr++;
 #		}
 #		if ( bit_data.rack & bit_data.mask ) return_value |= mask;
 #		mask >>= 1;
 #		bit_data.mask >>= 1;
 #		if ( bit_data.mask == 0 ) bit_data.mask = 0x80;
 #	}
 #
 #	return( return_value );
 #}
 #---------------------------------------------------------------
 # u32 GetBits(int bit_count)
 #---------------------------------------------------------------
	.text
	.align	2
	.ent	GetBits
GetBits:
	.frame	$sp, 0, $31
	.mask	0x00000000,0
	.fmask	0x00000000,0

	addu	$5, $4, -1
	li	$2, 0x00000001		# 1
	#la	$18, bit_data
	#li	$6, 0x0080
	lbu	$3, 4($18)		# .mask
	lbu	$4, 5($18)		# .rack
	sll	$7, $2, $5
	move	$2, $0
	beq	$7, $0, .L000
.L001:
	bne	$3, $6, .L002
	lw	$5, 0($18)
	#nop
	lbu	$4, 0($5)
	addu	$5, $5, 1
	sw	$5, 0($18)
	sb	$4, 5($18)
.L002:
	and	$5, $4, $3
	beq	$5, $0, .L003
	or	$2, $7
.L003:
	srl	$3, $3, 1
	srl	$7, $7, 1
	bne	$3, $0, .L004
	move	$3, $6
.L004:
	bne	$7, $0, .L001

.L000:
	sb	$3, 4($18)

	j	$31
	.end	GetBits



 #---------------------------------------------------------------
 #void	ExpandData(u8 *input, u8 *output)
 #{
 #	u8	c;
 #	int	i;
 #	int	current_position;
 #	int	match_length;
 #	int	match_position;
 #
 #	InitBitRead(input);
 #
 #	current_position = 1;
 #	for (;;) {
 #		if ( GetBit() != 0 ) {
 #			c = (u8)GetBits(8);
 #			*output = c;
 #			output++;
 #			window[current_position] = c;
 #			current_position = MOD_WINDOW( current_position + 1 );
 #		} else {
 #			match_position = (int)GetBits(INDEX_BIT_COUNT);
 #			if ( match_position == END_OF_STREAM ) break;
 #			match_length = (int)GetBits(LENGTH_BIT_COUNT);
 #			match_length += BREAK_EVEN;
 #			for ( i = 0 ; i <= match_length ; i++ ) {
 #				c = window[MOD_WINDOW( match_position + i )];
 #				*output = c;
 #				output++;
 #				window[current_position] = c;
 #				current_position = MOD_WINDOW( current_position + 1 );
 #			}
 #		}
 #	}
 #}
 #---------------------------------------------------------------
 # データの展開
 # int	ExpandData(u8 *input, u8 *output)
 #	u8	*input	ソースデータアドレス
 #	u8	*output	展開先アドレス
 #---------------------------------------------------------------
	.text
	.align	2
	.globl	ExpandData
	.ent	ExpandData
ExpandData:
	.frame	$sp,32,$31		# vars= 0, regs= 4/0, args= 16, extra= 0
	.mask	0x80070000,-4
	.fmask	0x00000000,0

	subu	$sp, $sp, 32
	sw	$31, 28($sp)
	sw	$18, 24($sp)
	sw	$17, 20($sp)
	sw	$16, 16($sp)

	la	$18, bit_data
	move	$17, $5
	li	$16, 0x00000001		# 1

 #InitBitRead:
	li	$6, 0x0080
	sw	$4, 0($18)
	sb	$0, 5($18)
	sb	$6, 4($18)
 #InitBitRead_End:

.Loop:
 # GetBit:
	#li	$6, 0x0080
	lbu	$3, 4($18)		# .mask
	lbu	$4, 5($18)		# .rack
	bne	$3, $6, .L3
	lw	$2, 0($18)
	#nop
	lbu	$4, 0($2)
	addu	$2, $2, 1
	sw	$2, 0($18)
	sb	$4, 5($18)
.L3:
	and	$2, $4, $3
	srl	$3, $3, 1
	bne	$3, $0, .L4
	move	$3, $6
.L4:
	sb	$3, 4($18)
 # GetBit_End:

	beq	$2, $0, .L01

	.set	noreorder
	.set	nomacro
	jal	GetBits
	li	$4,0x00000008		# 8
	.set	macro
	.set	reorder

	sb	$2, 0($17)
	addu	$17, $17, 1
	sb	$2, window($16)
	addu	$2, $16, 1

	.set	noreorder
	.set	nomacro
	j	.Loop
	andi	$16, $2, 0x3fff		# WINDOW_MASK
	.set	macro
	.set	reorder

.L01:
	.set	noreorder
	.set	nomacro
	jal	GetBits
	li	$4, 0x000e		# INDEX_BIT_COUNT
	.set	macro
	.set	reorder

	move	$8, $2
	beq	$8, $0, .Exit

	.set	noreorder
	.set	nomacro
	jal	GetBits
	li	$4,0x0004		# LENGTH_BIT_COUNT
	.set	macro
	.set	reorder

	addu	$4, $2, (2+1)		# BREAK_EVEN + 1
	#bltz	$4, .Loop		# Absolute ( $4 >= 2+1 )

.L00:
	andi	$2, $8, 0x3fff		# WINDOW_MASK
	lbu	$2, window($2)
	addu	$8, $8, 1
	addu	$4, $4, -1
	sb	$2, 0($17)
	sb	$2, window($16)
	addu	$2, $16, 1
	andi	$16, $2, 0x3fff		# WINDOW_MASK
	.set	noreorder
	.set	nomacro
	bne	$4, $0, .L00
	addu	$17, $17, 1
	.set	macro
	.set	reorder

	j	.Loop

.Exit:
	move	$2, $17
	lw	$31,28($sp)
	lw	$18,24($sp)
	lw	$17,20($sp)
	lw	$16,16($sp)
	addu	$sp, $sp, 32

	j	$31

	.end	ExpandData

 #---------------------------------------------------------------
 #---------------------------------------------------------------
	.comm	bit_data,8,4
	.comm	window,16384,1
 #---------------------------------------------------------------
