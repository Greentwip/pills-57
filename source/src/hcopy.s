	.option	pic0
	.option	O2


#define	XSIZE	(320/2)
#define	YSIZE	240


 #---------------------------------------------------------------
 # void HardCopy(u16 *buff, u16 *vram)
 #---------------------------------------------------------------
	.text
	.align	2
	.globl	HardCopy
	.ent	HardCopy
HardCopy:
	.frame	$sp, 0, $31
	.mask	0x00000000,0
	.fmask	0x00000000,0

	li	$8, 0x00010001
	li	$24, 0x0000ffff
	li	$25, 0xffff0000
	li	$2, 240		# YSIZE
.L000:
	li	$6, 1
	move	$10, $0
	move	$11, $0
	move	$12, $0
	li	$3, 320/2	# XSIZE
.L001:
	lw	$7, 0($5)	# Point Get
	addiu	$5, 4

	srl	$13, $7, 16
	srl	$14, $7, 22
	srl	$15, $7, 27
	andi	$9, $13, 0x01	# First A
	andi	$13, 0x3e	# B
	andi	$14, 0x1f	# G
	andi	$15, 0x1f	# R

	beq	$9, $6, .Lnext
	addu	$12, $15
	addu	$11, $14
	addu	$10, $13
	srl	$12, 1		# R
	srl	$11, 1		# G
	srl	$10, 1		# B
	sll	$12, 27
	sll	$11, 22
	sll	$10, 16
	or	$12, $11
	and	$7, $24		# andi $7, 0x0000ffff
	or	$12, $10
	xori	$6, 1
	or	$7, $12

.Lnext:
	srl	$11, $7, 6
	srl	$12, $7, 11
	andi	$9, $7, 0x01	# Second A
	andi	$10, $7, 0x3e	# B
	andi	$11, 0x1f	# G
	andi	$12, 0x1f	# R

	beq	$9, $6, .Lstore
	addu	$15, $12
	addu	$14, $11
	addu	$13, $10
	srl	$15, 1		# R
	srl	$14, 1		# G
	srl	$13, 1		# B
	sll	$15, 11
	sll	$14, 6
	or	$15, $13
	and	$7, $25		# andi $7, 0xffff0000
	or	$15, $14
	xori	$6, 1
	or	$7, $15

.Lstore:
	or	$7, $8
	addiu	$3, -1
	sw	$7, 0($4)	# Point Set
	addiu	$4, 4
	bnez	$3, .L001

	addiu	$2, -1
	addiu	$4, 16		# X = 328 Dots
	bnez	$2, .L000

	j	$31

	.end	HardCopy



 #---------------------------------------------------------------
 # void HardCopy(u16 *buff, u16 *vram)
 #---------------------------------------------------------------
	.text
	.align	2
	.globl	HardCopy0
	.ent	HardCopy0
HardCopy0:
	.frame	$sp, 0, $31
	.mask	0x00000000,0
	.fmask	0x00000000,0

	li	$9, 0x00010001
	li	$24, 240	# YSIZE
.L010:
	li	$25, 320/2	# XSIZE
.L011:
	lw	$8, 0($5)	# Point Get
	addiu	$5, 4
	or	$8, $9
	addiu	$25, -1
	sw	$8, 0($4)	# Point Set
	addiu	$4, 4
	bnez	$25, .L011

	addiu	$24, -1
	addiu	$4, 16		# X = 328 Dots
	bnez	$24, .L010

	j	$31

	.end	HardCopy0


 #---------------------------------------------------------------
 # void HardCopy(u16 *buff, u16 *vram)
 #---------------------------------------------------------------
	.text
	.align	2
	.globl	HardCopy1
	.ent	HardCopy1
HardCopy1:
	.frame	$sp, 0, $31
	.mask	0x00000000,0
	.fmask	0x00000000,0

	li	$9, 0x00010001
	li	$24, 240*320/2
.L100:
	lw	$8, 0($5)	# Point Get
	addiu	$5, 4
	or	$8, $9
	addiu	$24, -1
	sw	$8, 0($4)	# Point Set
	addiu	$4, 4
	bnez	$24, .L100

	j	$31
	.end	HardCopy1
