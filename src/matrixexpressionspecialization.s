	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 15, 0	sdk_version 15, 5
	.globl	__ZN6bla_ga29micro_kernel_4x4_packedB_SIMDEPKdS1_Pdmmmmm ; -- Begin function _ZN6bla_ga29micro_kernel_4x4_packedB_SIMDEPKdS1_Pdmmmmm
	.p2align	2
__ZN6bla_ga29micro_kernel_4x4_packedB_SIMDEPKdS1_Pdmmmmm: ; @_ZN6bla_ga29micro_kernel_4x4_packedB_SIMDEPKdS1_Pdmmmmm
	.cfi_startproc
; %bb.0:
	cbz	x7, LBB0_18
; %bb.1:
	mov	x8, #0                          ; =0x0
	add	x9, x0, x3, lsl #3
	mov	w12, #24                        ; =0x18
	add	x10, x1, x7, lsl #3
	madd	x11, x3, x12, x0
	movi.2d	v7, #0000000000000000
	madd	x12, x7, x12, x1
	movi.2d	v6, #0000000000000000
	movi.2d	v5, #0000000000000000
	movi.2d	v4, #0000000000000000
	add	x13, x0, x3, lsl #4
	movi.2d	v3, #0000000000000000
	movi.2d	v2, #0000000000000000
	movi.2d	v1, #0000000000000000
	movi.2d	v0, #0000000000000000
	add	x14, x1, x7, lsl #4
	b	LBB0_4
LBB0_2:                                 ;   in Loop: Header=BB0_4 Depth=1
	fmla.2d	v7, v17, v18[0]
	fmla.2d	v3, v16, v18[0]
LBB0_3:                                 ;   in Loop: Header=BB0_4 Depth=1
	add	x8, x8, #1
	cmp	x7, x8
	b.eq	LBB0_19
LBB0_4:                                 ; =>This Inner Loop Header: Depth=1
	movi	d16, #0000000000000000
	movi	d18, #0000000000000000
	cbz	x5, LBB0_6
; %bb.5:                                ;   in Loop: Header=BB0_4 Depth=1
	ldr	d18, [x0, x8, lsl #3]
	cmp	x5, #1
	b.ne	LBB0_12
LBB0_6:                                 ;   in Loop: Header=BB0_4 Depth=1
	movi.2d	v17, #0000000000000000
	mov.d	v17[0], v18[0]
	movi	d18, #0000000000000000
LBB0_7:                                 ;   in Loop: Header=BB0_4 Depth=1
	cbz	x6, LBB0_3
; %bb.8:                                ;   in Loop: Header=BB0_4 Depth=1
	mov.d	v16[1], v18[0]
	ldr	d18, [x1, x8, lsl #3]
	cmp	x6, #1
	b.eq	LBB0_2
; %bb.9:                                ;   in Loop: Header=BB0_4 Depth=1
	ldr	d19, [x10, x8, lsl #3]
	cmp	x6, #3
	b.lo	LBB0_15
; %bb.10:                               ;   in Loop: Header=BB0_4 Depth=1
	ldr	d20, [x14, x8, lsl #3]
	b.ne	LBB0_17
; %bb.11:                               ;   in Loop: Header=BB0_4 Depth=1
	fmla.2d	v7, v17, v18[0]
	fmla.2d	v6, v17, v19[0]
	fmla.2d	v5, v17, v20[0]
	fmla.2d	v3, v16, v18[0]
	fmla.2d	v2, v16, v19[0]
	fmla.2d	v1, v16, v20[0]
	b	LBB0_3
LBB0_12:                                ;   in Loop: Header=BB0_4 Depth=1
	add	x15, x9, x8, lsl #3
	mov.16b	v17, v18
	ld1.d	{ v17 }[1], [x15]
	movi	d18, #0000000000000000
	cmp	x5, #3
	b.lo	LBB0_16
; %bb.13:                               ;   in Loop: Header=BB0_4 Depth=1
	ldr	d16, [x13, x8, lsl #3]
	b.eq	LBB0_7
; %bb.14:                               ;   in Loop: Header=BB0_4 Depth=1
	ldr	d18, [x11, x8, lsl #3]
	b	LBB0_7
LBB0_15:                                ;   in Loop: Header=BB0_4 Depth=1
	fmla.2d	v7, v17, v18[0]
	fmla.2d	v6, v17, v19[0]
	fmla.2d	v3, v16, v18[0]
	fmla.2d	v2, v16, v19[0]
	b	LBB0_3
LBB0_16:                                ;   in Loop: Header=BB0_4 Depth=1
	movi	d16, #0000000000000000
	b	LBB0_7
LBB0_17:                                ;   in Loop: Header=BB0_4 Depth=1
	ldr	d21, [x12, x8, lsl #3]
	fmla.2d	v7, v17, v18[0]
	fmla.2d	v6, v17, v19[0]
	fmla.2d	v5, v17, v20[0]
	fmla.2d	v4, v17, v21[0]
	fmla.2d	v3, v16, v18[0]
	fmla.2d	v2, v16, v19[0]
	fmla.2d	v1, v16, v20[0]
	fmla.2d	v0, v16, v21[0]
	b	LBB0_3
LBB0_18:
	movi.2d	v0, #0000000000000000
	movi.2d	v1, #0000000000000000
	movi.2d	v2, #0000000000000000
	movi.2d	v3, #0000000000000000
	movi.2d	v4, #0000000000000000
	movi.2d	v5, #0000000000000000
	movi.2d	v6, #0000000000000000
	movi.2d	v7, #0000000000000000
LBB0_19:
	cbz	x6, LBB0_25
; %bb.20:
	cbz	x5, LBB0_25
; %bb.21:
	ldr	d16, [x2]
	fadd	d16, d7, d16
	str	d16, [x2]
	cmp	x5, #1
	b.ne	LBB0_26
; %bb.22:
	cmp	x6, #1
	b.eq	LBB0_25
; %bb.23:
	ldr	d0, [x2, #8]
	fadd	d0, d6, d0
	str	d0, [x2, #8]
	cmp	x6, #2
	b.ls	LBB0_25
; %bb.24:
	ldr	d0, [x2, #16]
	fadd	d0, d5, d0
	str	d0, [x2, #16]
	cmp	x6, #3
	b.ne	LBB0_52
LBB0_25:
	ret
LBB0_26:
	mov	d7, v7[1]
	add	x8, x2, x4, lsl #3
	ldr	d16, [x8]
	fadd	d7, d7, d16
	str	d7, [x8]
	cmp	x6, #1
	b.eq	LBB0_30
; %bb.27:
	ldr	d7, [x2, #8]
	fadd	d7, d6, d7
	str	d7, [x2, #8]
	mov	d6, v6[1]
	ldr	d7, [x8, #8]
	fadd	d6, d6, d7
	str	d6, [x8, #8]
	cmp	x6, #2
	b.ls	LBB0_30
; %bb.28:
	ldr	d6, [x2, #16]
	fadd	d6, d5, d6
	str	d6, [x2, #16]
	mov	d5, v5[1]
	ldr	d6, [x8, #16]
	fadd	d5, d5, d6
	str	d5, [x8, #16]
	cmp	x6, #3
	b.ne	LBB0_41
; %bb.29:
	mov	w8, #0                          ; =0x0
	mov	w9, #1                          ; =0x1
	b	LBB0_31
LBB0_30:
	mov	w9, #0                          ; =0x0
	mov	w8, #0                          ; =0x0
LBB0_31:
	cmp	x5, #2
	cset	w12, hi
	b.ls	LBB0_35
; %bb.32:
	lsl	x10, x4, #4
	add	x11, x2, x10
	ldr	d4, [x11]
	fadd	d4, d3, d4
	str	d4, [x11]
	cmp	x5, #3
	b.ne	LBB0_36
; %bb.33:
	cmp	x6, #1
	b.ne	LBB0_42
; %bb.34:
	tbnz	w9, #0, LBB0_43
	b	LBB0_48
LBB0_35:
	cmp	x6, #1
	b.ne	LBB0_25
	b	LBB0_37
LBB0_36:
	mov	d3, v3[1]
	mov	w13, #24                        ; =0x18
	madd	x13, x4, x13, x2
	ldr	d4, [x13]
	fadd	d3, d3, d4
	str	d3, [x13]
	cmp	x6, #1
	b.ne	LBB0_45
LBB0_37:
	tbz	w9, #0, LBB0_49
; %bb.38:
	cmp	x5, #2
	b.ls	LBB0_25
; %bb.39:
	lsl	x10, x4, #4
	add	x9, x2, x10
	ldr	d2, [x9, #16]
	fadd	d2, d1, d2
	str	d2, [x9, #16]
	cmp	x5, #3
	b.eq	LBB0_48
; %bb.40:
	mov	d1, v1[1]
	mov	w9, #24                         ; =0x18
	madd	x9, x4, x9, x2
	ldr	d2, [x9, #16]
	fadd	d1, d1, d2
	str	d1, [x9, #16]
	cbz	w8, LBB0_25
	b	LBB0_47
LBB0_41:
	ldr	d5, [x2, #24]
	fadd	d5, d4, d5
	str	d5, [x2, #24]
	mov	d4, v4[1]
	ldr	d5, [x8, #24]
	fadd	d4, d4, d5
	str	d4, [x8, #24]
	mov	w9, #1                          ; =0x1
	mov	w8, #1                          ; =0x1
	b	LBB0_31
LBB0_42:
	add	x10, x2, x4, lsl #4
	ldr	d3, [x10, #8]
	fadd	d2, d2, d3
	str	d2, [x10, #8]
	tbz	w9, #0, LBB0_48
LBB0_43:
	ldr	d2, [x11, #16]
	fadd	d1, d1, d2
	str	d1, [x11, #16]
	cbz	w8, LBB0_25
; %bb.44:
	mov	x2, x11
	fmov	d4, d0
	b	LBB0_52
LBB0_45:
	add	x12, x2, x4, lsl #4
	ldr	d3, [x12, #8]
	fadd	d3, d2, d3
	str	d3, [x12, #8]
	mov	d2, v2[1]
	ldr	d3, [x13, #8]
	fadd	d2, d2, d3
	str	d2, [x13, #8]
	cbz	w9, LBB0_48
; %bb.46:
	ldr	d2, [x11, #16]
	fadd	d2, d1, d2
	str	d2, [x11, #16]
	mov	d1, v1[1]
	ldr	d2, [x13, #16]
	fadd	d1, d1, d2
	str	d1, [x13, #16]
	tbz	w8, #0, LBB0_25
LBB0_47:
	add	x8, x2, x10
	ldr	d1, [x8, #24]
	fadd	d1, d0, d1
	str	d1, [x8, #24]
	b	LBB0_51
LBB0_48:
	mov	w12, #1                         ; =0x1
LBB0_49:
	and	w8, w12, w8
	cmp	w8, #1
	b.ne	LBB0_25
; %bb.50:
	add	x8, x2, x4, lsl #4
	ldr	d1, [x8, #24]
	fadd	d1, d0, d1
	str	d1, [x8, #24]
	cmp	x5, #4
	b.lo	LBB0_25
LBB0_51:
	mov	d4, v0[1]
	mov	w8, #24                         ; =0x18
	madd	x2, x4, x8, x2
LBB0_52:
	ldr	d0, [x2, #24]
	fadd	d0, d4, d0
	str	d0, [x2, #24]
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	_main                           ; -- Begin function main
	.p2align	2
_main:                                  ; @main
	.cfi_startproc
; %bb.0:
	mov	w0, #0                          ; =0x0
	ret
	.cfi_endproc
                                        ; -- End function
.subsections_via_symbols
