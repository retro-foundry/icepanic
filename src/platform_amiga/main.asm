	idnt	"src/platform_amiga/main.c"
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	section	"CODE",code
	cnop	0,4
l15
	movem.l	l22,-(a7)
	move.w	(4+l24,a7),d0
	cmp.w	(6+l24,a7),d0
	bge	l19
l18
	move.w	(6+l24,a7),d0
	bra	l16
l19
	move.w	(4+l24,a7),d0
	cmp.w	(8+l24,a7),d0
	ble	l21
l20
	move.w	(8+l24,a7),d0
	bra	l16
l21
	move.w	(4+l24,a7),d0
l16
l22	reg
l24	equ	0
	rts
; stacksize=0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l25
	movem.l	l32,-(a7)
	cmp.w	#3,(4+l34,a7)
	bne	l29
l28
	moveq	#-1,d0
	bra	l26
l29
	cmp.w	#4,(4+l34,a7)
	bne	l31
l30
	moveq	#1,d0
	bra	l26
l31
	moveq	#0,d0
l26
l32	reg
l34	equ	0
	rts
; stacksize=0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l35
	movem.l	l42,-(a7)
	cmp.w	#1,(4+l44,a7)
	bne	l39
l38
	moveq	#-1,d0
	bra	l36
l39
	cmp.w	#2,(4+l44,a7)
	bne	l41
l40
	moveq	#1,d0
	bra	l36
l41
	moveq	#0,d0
l36
l42	reg
l44	equ	0
	rts
; stacksize=0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l45
	movem.l	l54,-(a7)
	move.l	(4+l56,a7),a0
	move.l	(8+l56,a7),a1
	move.b	(a0),d0
	cmp.b	(a1),d0
	bne	l50
l53
	move.l	(4+l56,a7),a0
	move.l	(8+l56,a7),a1
	move.b	(1,a0),d0
	cmp.b	(1,a1),d0
	bne	l50
l52
	move.l	(4+l56,a7),a0
	move.l	(8+l56,a7),a1
	move.b	(2,a0),d0
	cmp.b	(2,a1),d0
	bne	l50
l48
	move.l	(4+l56,a7),a0
	move.l	(8+l56,a7),a1
	move.b	(3,a0),d0
	cmp.b	(3,a1),d0
	bne	l50
l49
	moveq	#1,d0
	bra	l51
l50
	moveq	#0,d0
l51
l46
l54	reg
l56	equ	0
	rts
; stacksize=0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l57
	movem.l	l72,-(a7)
	tst.w	(4+l74,a7)
	bgt	l61
l60
	moveq	#0,d0
	bra	l58
l61
	cmp.w	#100,(4+l74,a7)
	ble	l63
l62
	moveq	#1,d0
	bra	l58
l63
	cmp.w	#50,(4+l74,a7)
	ble	l65
l64
	move.w	(4+l74,a7),d0
	bge	l75
	addq.w	#7,d0
l75:
	asr.w	#3,d0
	and.w	#1,d0
	beq	l66
l68
	moveq	#0,d0
	bra	l67
l66
	moveq	#1,d0
l67
	bra	l58
l65
	move.w	(4+l74,a7),d0
	bge	l76
	addq.w	#3,d0
l76:
	asr.w	#2,d0
	and.w	#1,d0
	beq	l69
l71
	moveq	#0,d0
	bra	l70
l69
	moveq	#1,d0
l70
l58
l72	reg
l74	equ	0
	rts
; stacksize=0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l77
	movem.l	l93,-(a7)
	cmp.w	#50,(4+l95,a7)
	bgt	l81
l80
	moveq	#1,d0
	and.w	(6+l95,a7),d0
	bne	l83
l82
	moveq	#32,d0
	bra	l84
l83
	moveq	#31,d0
l84
	bra	l78
l81
	cmp.w	#100,(4+l95,a7)
	bgt	l86
l85
	moveq	#2,d0
	and.w	(6+l95,a7),d0
	bne	l88
l87
	moveq	#31,d0
	bra	l89
l88
	moveq	#30,d0
l89
	bra	l78
l86
	moveq	#7,d0
	and.w	(6+l95,a7),d0
	bne	l91
l90
	moveq	#31,d0
	bra	l92
l91
	moveq	#30,d0
l92
l78
l93	reg
l95	equ	0
	rts
; stacksize=0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l96
	movem.l	l117,-(a7)
	move.w	(4+l119,a7),d0
	subq.w	#1,d0
	beq	l100
	subq.w	#1,d0
	beq	l104
	subq.w	#1,d0
	beq	l108
	subq.w	#1,d0
	beq	l112
	bra	l113
l100
	tst.b	(7+l119,a7)
	beq	l102
l101
	moveq	#8,d0
	bra	l103
l102
	moveq	#7,d0
l103
	bra	l97
l104
	tst.b	(7+l119,a7)
	beq	l106
l105
	moveq	#10,d0
	bra	l107
l106
	moveq	#9,d0
l107
	bra	l97
l108
	tst.b	(7+l119,a7)
	beq	l110
l109
	moveq	#12,d0
	bra	l111
l110
	moveq	#11,d0
l111
	bra	l97
l112
l113
	tst.b	(7+l119,a7)
	beq	l115
l114
	moveq	#14,d0
	bra	l116
l115
	moveq	#13,d0
l116
l99
l97
l117	reg
l119	equ	0
	rts
; stacksize=0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l120
	subq.w	#4,a7
	movem.l	l134,-(a7)
	move.l	(8+l136,a7),a0
	move.l	(8+l136,a7),a1
	move.l	(4,a0),d0
	add.l	(8,a1),d0
	moveq	#9,d1
	asr.l	d1,d0
	move.w	d0,(0+l136,a7)
	moveq	#1,d0
	and.w	(0+l136,a7),d0
	bne	l123
l125
	moveq	#0,d0
	bra	l124
l123
	moveq	#1,d0
l124
	move.b	d0,(2+l136,a7)
	move.l	(8+l136,a7),a0
	cmp.w	#1,(22,a0)
	bne	l127
l126
	tst.b	(2+l136,a7)
	beq	l129
l128
	moveq	#18,d0
	bra	l130
l129
	moveq	#17,d0
l130
	bra	l121
l127
	tst.b	(2+l136,a7)
	beq	l132
l131
	moveq	#16,d0
	bra	l133
l132
	moveq	#15,d0
l133
l121
l134	reg
l136	equ	0
	addq.w	#4,a7
	rts
; stacksize=4
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l137
	subq.w	#4,a7
	movem.l	l151,-(a7)
	move.l	(8+l153,a7),a0
	move.l	(8+l153,a7),a1
	move.w	(10,a0),d0
	sub.w	(18,a1),d0
	move.w	d0,(0+l153,a7)
	move.l	(8+l153,a7),a0
	move.w	(10,a0),d0
	bge	l154
	addq.w	#1,d0
l154:
	asr.w	#1,d0
	cmp.w	(0+l153,a7),d0
	ble	l140
l142
	moveq	#0,d0
	bra	l141
l140
	moveq	#1,d0
l141
	move.b	d0,(2+l153,a7)
	move.l	(8+l153,a7),a0
	cmp.w	#8,(8,a0)
	bne	l144
l143
	tst.b	(2+l153,a7)
	beq	l146
l145
	moveq	#22,d0
	bra	l147
l146
	moveq	#21,d0
l147
	bra	l138
l144
	tst.b	(2+l153,a7)
	beq	l149
l148
	moveq	#20,d0
	bra	l150
l149
	moveq	#19,d0
l150
l138
l151	reg
l153	equ	0
	addq.w	#4,a7
	rts
; stacksize=4
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l155
	movem.l	l171,-(a7)
	move.w	(4+l173,a7),d0
	subq.w	#1,d0
	beq	l159
	subq.w	#1,d0
	beq	l161
	subq.w	#1,d0
	beq	l163
	subq.w	#1,d0
	beq	l165
	subq.w	#1,d0
	beq	l167
	bra	l169
l159
	move.l	#l160,d0
	bra	l156
l161
	move.l	#l162,d0
	bra	l156
l163
	move.l	#l164,d0
	bra	l156
l165
	move.l	#l166,d0
	bra	l156
l167
	move.l	#l168,d0
	bra	l156
l169
	move.l	#l170,d0
l158
l156
l171	reg
l173	equ	0
	rts
; stacksize=0
	cnop	0,4
l160
	dc.b	76
	dc.b	73
	dc.b	70
	dc.b	69
	dc.b	0
	cnop	0,4
l162
	dc.b	84
	dc.b	73
	dc.b	77
	dc.b	69
	dc.b	0
	cnop	0,4
l164
	dc.b	83
	dc.b	67
	dc.b	79
	dc.b	82
	dc.b	69
	dc.b	0
	cnop	0,4
l166
	dc.b	83
	dc.b	76
	dc.b	79
	dc.b	87
	dc.b	0
	cnop	0,4
l168
	dc.b	77
	dc.b	73
	dc.b	78
	dc.b	69
	dc.b	83
	dc.b	0
	cnop	0,4
l170
	dc.b	77
	dc.b	89
	dc.b	83
	dc.b	84
	dc.b	0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l174
	movem.l	l190,-(a7)
	move.w	(4+l192,a7),d0
	subq.w	#1,d0
	beq	l178
	subq.w	#1,d0
	beq	l180
	subq.w	#1,d0
	beq	l182
	subq.w	#1,d0
	beq	l184
	subq.w	#1,d0
	beq	l186
	bra	l188
l178
	move.l	#l179,d0
	bra	l175
l180
	move.l	#l181,d0
	bra	l175
l182
	move.l	#l183,d0
	bra	l175
l184
	move.l	#l185,d0
	bra	l175
l186
	move.l	#l187,d0
	bra	l175
l188
	move.l	#l189,d0
l177
l175
l190	reg
l192	equ	0
	rts
; stacksize=0
	cnop	0,4
l179
	dc.b	71
	dc.b	65
	dc.b	73
	dc.b	78
	dc.b	32
	dc.b	49
	dc.b	32
	dc.b	76
	dc.b	73
	dc.b	70
	dc.b	69
	dc.b	32
	dc.b	77
	dc.b	65
	dc.b	88
	dc.b	32
	dc.b	55
	dc.b	0
	cnop	0,4
l181
	dc.b	65
	dc.b	68
	dc.b	68
	dc.b	32
	dc.b	84
	dc.b	73
	dc.b	77
	dc.b	69
	dc.b	32
	dc.b	76
	dc.b	69
	dc.b	83
	dc.b	83
	dc.b	32
	dc.b	69
	dc.b	65
	dc.b	67
	dc.b	72
	dc.b	32
	dc.b	76
	dc.b	86
	dc.b	0
	cnop	0,4
l183
	dc.b	83
	dc.b	67
	dc.b	79
	dc.b	82
	dc.b	69
	dc.b	32
	dc.b	66
	dc.b	79
	dc.b	78
	dc.b	85
	dc.b	83
	dc.b	32
	dc.b	84
	dc.b	65
	dc.b	80
	dc.b	69
	dc.b	82
	dc.b	83
	dc.b	0
	cnop	0,4
l185
	dc.b	83
	dc.b	76
	dc.b	79
	dc.b	87
	dc.b	32
	dc.b	66
	dc.b	79
	dc.b	78
	dc.b	85
	dc.b	83
	dc.b	32
	dc.b	84
	dc.b	65
	dc.b	80
	dc.b	69
	dc.b	82
	dc.b	83
	dc.b	0
	cnop	0,4
l187
	dc.b	70
	dc.b	73
	dc.b	82
	dc.b	69
	dc.b	32
	dc.b	68
	dc.b	82
	dc.b	79
	dc.b	80
	dc.b	32
	dc.b	77
	dc.b	73
	dc.b	78
	dc.b	69
	dc.b	0
	cnop	0,4
l189
	dc.b	78
	dc.b	79
	dc.b	32
	dc.b	69
	dc.b	70
	dc.b	70
	dc.b	69
	dc.b	67
	dc.b	84
	dc.b	0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l193
	movem.l	l203,-(a7)
	move.w	(4+l205,a7),d0
	subq.w	#1,d0
	beq	l197
	subq.w	#1,d0
	beq	l198
	subq.w	#1,d0
	beq	l199
	subq.w	#1,d0
	beq	l200
	subq.w	#1,d0
	beq	l201
	bra	l202
l197
	moveq	#23,d0
	bra	l194
l198
	moveq	#14,d0
	bra	l194
l199
	moveq	#8,d0
	bra	l194
l200
	moveq	#11,d0
	bra	l194
l201
	moveq	#22,d0
	bra	l194
l202
	moveq	#30,d0
l196
l194
l203	reg
l205	equ	0
	rts
; stacksize=0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l206
	movem.l	l216,-(a7)
	move.w	(4+l218,a7),d0
	subq.w	#1,d0
	beq	l210
	subq.w	#1,d0
	beq	l211
	subq.w	#1,d0
	beq	l212
	subq.w	#1,d0
	beq	l213
	subq.w	#1,d0
	beq	l214
	bra	l215
l210
	moveq	#28,d0
	bra	l207
l211
	moveq	#24,d0
	bra	l207
l212
	moveq	#25,d0
	bra	l207
l213
	moveq	#23,d0
	bra	l207
l214
	moveq	#29,d0
	bra	l207
l215
	moveq	#27,d0
l209
l207
l216	reg
l218	equ	0
	rts
; stacksize=0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l219
	movem.l	l233,-(a7)
	move.w	(4+l235,a7),d0
	subq.w	#1,d0
	beq	l223
	subq.w	#1,d0
	beq	l225
	subq.w	#1,d0
	beq	l227
	subq.w	#1,d0
	beq	l229
	bra	l231
l223
	move.l	#l224,d0
	bra	l220
l225
	move.l	#l226,d0
	bra	l220
l227
	move.l	#l228,d0
	bra	l220
l229
	move.l	#l230,d0
	bra	l220
l231
	move.l	#l232,d0
l222
l220
l233	reg
l235	equ	0
	rts
; stacksize=0
	cnop	0,4
l224
	dc.b	76
	dc.b	73
	dc.b	70
	dc.b	69
	dc.b	0
	cnop	0,4
l226
	dc.b	84
	dc.b	73
	dc.b	77
	dc.b	69
	dc.b	0
	cnop	0,4
l228
	dc.b	83
	dc.b	67
	dc.b	79
	dc.b	82
	dc.b	69
	dc.b	0
	cnop	0,4
l230
	dc.b	83
	dc.b	76
	dc.b	79
	dc.b	87
	dc.b	0
	cnop	0,4
l232
	dc.b	78
	dc.b	79
	dc.b	78
	dc.b	69
	dc.b	0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l236
	movem.l	l250,-(a7)
	move.w	(4+l252,a7),d0
	subq.w	#1,d0
	beq	l240
	subq.w	#1,d0
	beq	l242
	subq.w	#1,d0
	beq	l244
	subq.w	#1,d0
	beq	l246
	bra	l248
l240
	move.l	#l241,d0
	bra	l237
l242
	move.l	#l243,d0
	bra	l237
l244
	move.l	#l245,d0
	bra	l237
l246
	move.l	#l247,d0
	bra	l237
l248
	move.l	#l249,d0
l239
l237
l250	reg
l252	equ	0
	rts
; stacksize=0
	cnop	0,4
l241
	dc.b	78
	dc.b	69
	dc.b	87
	dc.b	32
	dc.b	82
	dc.b	85
	dc.b	78
	dc.b	32
	dc.b	49
	dc.b	32
	dc.b	76
	dc.b	73
	dc.b	70
	dc.b	69
	dc.b	0
	cnop	0,4
l243
	dc.b	78
	dc.b	69
	dc.b	87
	dc.b	32
	dc.b	82
	dc.b	85
	dc.b	78
	dc.b	32
	dc.b	84
	dc.b	73
	dc.b	77
	dc.b	69
	dc.b	0
	cnop	0,4
l245
	dc.b	78
	dc.b	69
	dc.b	87
	dc.b	32
	dc.b	82
	dc.b	85
	dc.b	78
	dc.b	32
	dc.b	83
	dc.b	67
	dc.b	79
	dc.b	82
	dc.b	69
	dc.b	0
	cnop	0,4
l247
	dc.b	78
	dc.b	69
	dc.b	87
	dc.b	32
	dc.b	82
	dc.b	85
	dc.b	78
	dc.b	32
	dc.b	83
	dc.b	76
	dc.b	79
	dc.b	87
	dc.b	0
	cnop	0,4
l249
	dc.b	78
	dc.b	79
	dc.b	32
	dc.b	69
	dc.b	70
	dc.b	70
	dc.b	69
	dc.b	67
	dc.b	84
	dc.b	0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l253
	movem.l	l262,-(a7)
	move.w	(4+l264,a7),d0
	subq.w	#1,d0
	beq	l257
	subq.w	#1,d0
	beq	l258
	subq.w	#1,d0
	beq	l259
	subq.w	#1,d0
	beq	l260
	bra	l261
l257
	moveq	#23,d0
	bra	l254
l258
	moveq	#14,d0
	bra	l254
l259
	moveq	#8,d0
	bra	l254
l260
	moveq	#11,d0
	bra	l254
l261
	moveq	#30,d0
l256
l254
l262	reg
l264	equ	0
	rts
; stacksize=0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l265
	movem.l	l274,-(a7)
	move.w	(4+l276,a7),d0
	subq.w	#1,d0
	beq	l269
	subq.w	#1,d0
	beq	l270
	subq.w	#1,d0
	beq	l271
	subq.w	#1,d0
	beq	l272
	bra	l273
l269
	moveq	#28,d0
	bra	l266
l270
	moveq	#24,d0
	bra	l266
l271
	moveq	#25,d0
	bra	l266
l272
	moveq	#23,d0
	bra	l266
l273
	moveq	#27,d0
l268
l266
l274	reg
l276	equ	0
	rts
; stacksize=0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l277
	movem.l	l287,-(a7)
	move.w	(4+l289,a7),d0
	subq.w	#1,d0
	beq	l281
	subq.w	#1,d0
	beq	l282
	subq.w	#1,d0
	beq	l283
	subq.w	#1,d0
	beq	l284
	subq.w	#1,d0
	beq	l285
	bra	l286
l281
	moveq	#23,d0
	bra	l278
l282
	moveq	#27,d0
	bra	l278
l283
	moveq	#29,d0
	bra	l278
l284
	moveq	#8,d0
	bra	l278
l285
	moveq	#12,d0
	bra	l278
l286
	moveq	#3,d0
l280
l278
l287	reg
l289	equ	0
	rts
; stacksize=0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l290
	subq.w	#4,a7
	movem.l	l296,-(a7)
	move.l	#14675970,(0+l298,a7)
l293
l294
	move.l	(0+l298,a7),a0
	move.w	#16384,d0
	and.w	(a0),d0
	bne	l293
l295
l291
l296	reg
l298	equ	0
	addq.w	#4,a7
	rts
; stacksize=4
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l299
	subq.w	#8,a7
	movem.l	l305,-(a7)
l302
	move.l	#14675968,a0
	move.w	(6,a0),(2+l307,a7)
	move.l	#14675968,a0
	move.w	(4,a0),(0+l307,a7)
	move.l	#14675968,a0
	move.w	(6,a0),(4+l307,a7)
l304
	move.w	(4+l307,a7),d1
	move.w	(2+l307,a7),d0
	eor.w	d1,d0
	and.w	#65280,d0
	bne	l302
l303
	moveq	#1,d0
	and.w	(0+l307,a7),d0
	lsl.w	#8,d0
	move.w	(4+l307,a7),d1
	lsr.w	#8,d1
	and.w	#255,d1
	or.w	d1,d0
l300
l305	reg
l307	equ	0
	addq.w	#8,a7
	rts
; stacksize=8
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l308
	movem.l	l317,-(a7)
l311
l312
	jsr	l299
	cmp.w	#250,d0
	bcs	l311
l313
l314
l315
	jsr	l299
	cmp.w	#250,d0
	bcc	l314
l316
l309
l317	reg
l319	equ	0
	rts
; stacksize=12
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l320
	movem.l	l323,-(a7)
	jsr	l299
	move.l	(4+l325,a7),a0
	move.w	d0,(a0)
	move.l	(4+l325,a7),a0
	move.b	#0,(2,a0)
l321
l323	reg
l325	equ	0
	rts
; stacksize=12
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l326
	subq.w	#4,a7
	movem.l	l331,-(a7)
	jsr	l299
	move.w	d0,(0+l333,a7)
	moveq	#8,d0
	add.w	(0+l333,a7),d0
	move.l	(8+l333,a7),a0
	cmp.w	(a0),d0
	bcc	l330
l329
	move.l	(8+l333,a7),a0
	move.b	#1,(2,a0)
l330
	move.l	(8+l333,a7),a0
	move.w	(0+l333,a7),(a0)
l327
l331	reg
l333	equ	0
	addq.w	#4,a7
	rts
; stacksize=16
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l334
	movem.l	l339,-(a7)
	move.l	(4+l341,a7),-(a7)
	jsr	l326
	move.l	(8+l341,a7),a0
	addq.w	#4,a7
	tst.b	(2,a0)
	bne	l338
l337
	jsr	l308
l338
l335
l339	reg
l341	equ	0
	rts
; stacksize=24
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l342
	movem.l	l345,-(a7)
	move.l	(4+l347,a7),a0
	move.w	(8+l347,a7),d0
	ext.l	d0
	move.l	#320,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	move.l	(16,a0),a0
	add.l	d0,a0
	move.w	(10+l347,a7),d0
	ext.l	d0
	lsl.l	#2,d0
	add.l	d0,a0
	move.l	a0,d0
l343
l345	reg	d2/d3
	movem.l	(a7)+,d2/d3
l347	equ	8
	rts
; stacksize=8
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l348
	movem.l	l351,-(a7)
	move.l	(4+l353,a7),a0
	move.w	(8+l353,a7),d0
	ext.l	d0
	move.l	#320,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	move.l	(20,a0),a0
	add.l	d0,a0
	move.l	a0,d0
l349
l351	reg	d2/d3
	movem.l	(a7)+,d2/d3
l353	equ	8
	rts
; stacksize=8
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l354
	movem.l	l357,-(a7)
	move.l	(4+l359,a7),a0
	move.w	(8+l359,a7),d0
	ext.l	d0
	move.l	#160,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	move.l	(8,a0),a0
	add.l	d0,a0
	move.l	a0,d0
l355
l357	reg	d2/d3
	movem.l	(a7)+,d2/d3
l359	equ	8
	rts
; stacksize=8
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l360
	movem.l	l363,-(a7)
	move.w	(10+l365,a7),d0
	ext.l	d0
	move.l	#200,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	move.l	(4+l365,a7),a0
	add.l	d0,a0
	move.w	(12+l365,a7),d0
	ext.l	d0
	moveq	#40,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	add.l	d0,a0
	move.w	(8+l365,a7),d0
	asr.w	#4,d0
	ext.l	d0
	lsl.l	#1,d0
	add.l	d0,a0
	move.l	a0,d0
l361
l363	reg	d2/d3
	movem.l	(a7)+,d2/d3
l365	equ	8
	rts
; stacksize=8
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l366
	subq.w	#4,a7
	movem.l	l371,-(a7)
	move.l	#65540,d1
	move.l	(8+l373,a7),d0
	move.l	_SysBase,a6
	jsr	-198(a6)
	move.l	d0,a0
	move.l	a0,(0+l373,a7)
	bne	l370
l369
	move.l	#65537,d1
	move.l	(8+l373,a7),d0
	move.l	_SysBase,a6
	jsr	-198(a6)
	move.l	d0,a0
	move.l	a0,(0+l373,a7)
l370
	move.l	(0+l373,a7),d0
l367
l371	reg	a6
	movem.l	(a7)+,a6
l373	equ	4
	addq.w	#4,a7
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l374
	subq.w	#8,a7
	movem.l	l377,-(a7)
	move.w	#304,-(a7)
	move.w	#0,-(a7)
	move.w	(26+l379,a7),-(a7)
	jsr	l15
	move.w	d0,(28+l379,a7)
	move.w	#184,-(a7)
	move.w	#0,-(a7)
	move.w	(34+l379,a7),-(a7)
	jsr	l15
	move.w	d0,(36+l379,a7)
	and.w	#-16,(34+l379,a7)
	move.w	(32+l379,a7),-(a7)
	move.l	(26+l379,a7),-(a7)
	jsr	l354
	move.l	d0,(18+l379,a7)
	move.w	(42+l379,a7),d0
	ext.l	d0
	move.l	#200,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	move.l	(34+l379,a7),a0
	add.l	d0,a0
	move.w	(40+l379,a7),d0
	asr.w	#4,d0
	ext.l	d0
	lsl.l	#1,d0
	add.l	d0,a0
	move.l	a0,(22+l379,a7)
	jsr	l290
	move.l	#14675968,a0
	move.w	#1484,(64,a0)
	move.l	#14675968,a0
	move.w	#0,(66,a0)
	move.l	#14675968,a0
	move.w	#65535,(68,a0)
	move.l	#14675968,a0
	move.w	#65535,(70,a0)
	move.l	#14675968,a0
	move.l	(18+l379,a7),(76,a0)
	move.l	#14675968,a0
	move.l	(22+l379,a7),(84,a0)
	move.l	#14675968,a0
	move.w	#0,(98,a0)
	move.l	#14675968,a0
	move.w	#38,(102,a0)
	move.l	#14675968,a0
	move.w	#5121,(88,a0)
	add.w	#18,a7
l375
l377	reg	d2/d3
	movem.l	(a7)+,d2/d3
l379	equ	8
	addq.w	#8,a7
	rts
; stacksize=46
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l380
	sub.w	#16,a7
	movem.l	l383,-(a7)
	move.w	#288,-(a7)
	move.w	#0,-(a7)
	move.w	(34+l385,a7),-(a7)
	jsr	l15
	move.w	d0,(36+l385,a7)
	move.w	#184,-(a7)
	move.w	#0,-(a7)
	move.w	(42+l385,a7),-(a7)
	jsr	l15
	move.w	d0,(44+l385,a7)
	moveq	#15,d0
	and.w	(42+l385,a7),d0
	move.w	d0,(12+l385,a7)
	move.l	(32+l385,a7),a0
	move.w	(40+l385,a7),d0
	ext.l	d0
	move.l	#320,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	move.l	(20,a0),a0
	add.l	d0,a0
	move.l	a0,(14+l385,a7)
	move.l	(32+l385,a7),a0
	move.w	(40+l385,a7),d0
	ext.l	d0
	move.l	#320,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	move.l	(16,a0),a0
	add.l	d0,a0
	move.l	a0,(18+l385,a7)
	move.w	(44+l385,a7),d0
	ext.l	d0
	move.l	#200,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	move.l	(36+l385,a7),a0
	add.l	d0,a0
	move.w	(42+l385,a7),d0
	asr.w	#4,d0
	ext.l	d0
	lsl.l	#1,d0
	add.l	d0,a0
	move.l	a0,(22+l385,a7)
	jsr	l290
	moveq	#12,d1
	move.w	(12+l385,a7),d0
	lsl.w	d1,d0
	or.w	#4042,d0
	move.l	#14675968,a0
	move.w	d0,(64,a0)
	moveq	#12,d1
	move.w	(12+l385,a7),d0
	lsl.w	d1,d0
	move.l	#14675968,a0
	move.w	d0,(66,a0)
	move.l	#14675968,a0
	move.w	#65535,(68,a0)
	move.l	#14675968,a0
	move.w	#65535,(70,a0)
	move.l	#14675968,a0
	move.l	(14+l385,a7),(80,a0)
	move.l	#14675968,a0
	move.l	(18+l385,a7),(76,a0)
	move.l	#14675968,a0
	move.l	(22+l385,a7),(72,a0)
	move.l	#14675968,a0
	move.l	(22+l385,a7),(84,a0)
	move.l	#14675968,a0
	move.w	#0,(100,a0)
	move.l	#14675968,a0
	move.w	#0,(98,a0)
	move.l	#14675968,a0
	move.w	#36,(96,a0)
	move.l	#14675968,a0
	move.w	#36,(102,a0)
	move.l	#14675968,a0
	move.w	#5122,(88,a0)
	add.w	#12,a7
l381
l383	reg	d2/d3
	movem.l	(a7)+,d2/d3
l385	equ	8
	add.w	#16,a7
	rts
; stacksize=44
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l386
	subq.w	#8,a7
	movem.l	l392,-(a7)
	move.l	(12+l394,a7),a0
	tst.l	(24,a0)
	beq	l389
l391
	move.l	(12+l394,a7),a0
	tst.l	(28,a0)
	bne	l390
l389
	bra	l387
l390
	move.w	#176,-(a7)
	move.w	#0,-(a7)
	move.w	(24+l394,a7),-(a7)
	jsr	l15
	move.w	d0,(26+l394,a7)
	move.w	#117,-(a7)
	move.w	#0,-(a7)
	move.w	(32+l394,a7),-(a7)
	jsr	l15
	move.w	d0,(34+l394,a7)
	moveq	#15,d0
	and.w	(32+l394,a7),d0
	move.w	d0,(12+l394,a7)
	move.w	(34+l394,a7),d0
	ext.l	d0
	move.l	#200,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	move.l	(28+l394,a7),a0
	add.l	d0,a0
	move.w	(32+l394,a7),d0
	asr.w	#4,d0
	ext.l	d0
	lsl.l	#1,d0
	add.l	d0,a0
	move.l	a0,(14+l394,a7)
	jsr	l290
	moveq	#12,d1
	move.w	(12+l394,a7),d0
	lsl.w	d1,d0
	or.w	#4042,d0
	move.l	#14675968,a0
	move.w	d0,(64,a0)
	moveq	#12,d1
	move.w	(12+l394,a7),d0
	lsl.w	d1,d0
	move.l	#14675968,a0
	move.w	d0,(66,a0)
	move.l	#14675968,a0
	move.w	#65535,(68,a0)
	move.l	#14675968,a0
	move.w	#65535,(70,a0)
	move.l	(24+l394,a7),a0
	move.l	#14675968,a1
	move.l	(28,a0),(80,a1)
	move.l	(24+l394,a7),a0
	move.l	#14675968,a1
	move.l	(24,a0),(76,a1)
	move.l	#14675968,a0
	move.l	(14+l394,a7),(72,a0)
	move.l	#14675968,a0
	move.l	(14+l394,a7),(84,a0)
	move.l	#14675968,a0
	move.w	#0,(100,a0)
	move.l	#14675968,a0
	move.w	#0,(98,a0)
	move.l	#14675968,a0
	move.w	#22,(96,a0)
	move.l	#14675968,a0
	move.w	#22,(102,a0)
	move.l	#14675968,a0
	move.w	#26569,(88,a0)
	add.w	#12,a7
l387
l392	reg	d2/d3
	movem.l	(a7)+,d2/d3
l394	equ	8
	addq.w	#8,a7
	rts
; stacksize=36
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l395
	movem.l	l398,-(a7)
	move.l	(4+l400,a7),a0
	moveq	#0,d0
	move.b	(a0),d0
	lsl.w	#8,d0
	move.l	(4+l400,a7),a0
	moveq	#0,d1
	move.b	(1,a0),d1
	or.w	d1,d0
l396
l398	reg
l400	equ	0
	rts
; stacksize=0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l401
	movem.l	l408,-(a7)
	cmp.w	#15,(4+l410,a7)
	beq	l406
l404
	cmp.w	#16,(4+l410,a7)
	beq	l406
l405
	moveq	#0,d0
	bra	l407
l406
	moveq	#1,d0
l407
l402
l408	reg
l410	equ	0
	rts
; stacksize=0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l411
	movem.l	l432,-(a7)
	move.l	(8+l434,a7),a0
	move.b	#0,(a0)
	move.l	(12+l434,a7),a0
	move.b	#0,(a0)
	move.w	(4+l434,a7),d0
	sub.w	#15,d0
	beq	l415
	subq.w	#1,d0
	beq	l416
	bra	l431
l415
l416
	cmp.b	#2,(7+l434,a7)
	bne	l418
l417
	move.l	(12+l434,a7),a0
	move.b	#1,(a0)
	bra	l419
l418
	cmp.b	#17,(7+l434,a7)
	bne	l421
l420
	move.l	(8+l434,a7),a0
	move.b	#1,(a0)
	bra	l422
l421
	cmp.b	#18,(7+l434,a7)
	bne	l424
l423
	move.l	(8+l434,a7),a0
	move.b	#2,(a0)
	bra	l425
l424
	cmp.b	#19,(7+l434,a7)
	bne	l427
l426
	move.l	(8+l434,a7),a0
	move.b	#3,(a0)
	bra	l428
l427
	cmp.b	#31,(7+l434,a7)
	bne	l430
l429
	move.l	(12+l434,a7),a0
	move.b	#3,(a0)
l430
l428
l425
l422
l419
	bra	l414
l431
l414
l412
l432	reg
l434	equ	0
	rts
; stacksize=0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l435
	sub.w	#28,a7
	movem.l	l468,-(a7)
	move.l	#4224,d2
	moveq	#0,d0
	move.l	(32+l470,a7),a0
	add.w	#40,a0
	inline
	move.l	a0,a1
	cmp.l	#16,d2
	blo	.l3
	move.l	a0,d1
	and.b	#1,d1
	beq	.l1
	move.b	d0,(a0)+
	subq.l	#1,d2
.l1
	move.b	d0,d1
	lsl.w	#8,d0
	move.b	d1,d0
	move.w	d0,d1
	swap	d0
	move.w	d1,d0
	moveq	#3,d1
	and.l	d2,d1
	sub.l	d1,d2
.l2
	move.l	d0,(a0)+
	subq.l	#4,d2
	bne	.l2
	move.w	d1,d2
.l3
	subq.w	#1,d2
	blo	.l5
.l4
	move.b	d0,(a0)+
	dbf	d2,.l4
.l5
	move.l	a1,d0
	einline
	move.l	d0,a1
	move.w	#0,(0+l470,a7)
	bra	l439
l438
	move.w	#0,(2+l470,a7)
	bra	l443
l442
	move.w	#0,(6+l470,a7)
	bra	l447
l446
	move.w	(0+l470,a7),d0
	ext.l	d0
	move.l	#160,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	lea	_g_amiga_tile_data,a0
	add.l	d0,a0
	move.w	(2+l470,a7),d0
	ext.l	d0
	moveq	#10,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	add.l	d0,a0
	move.w	(6+l470,a7),d0
	ext.l	d0
	lsl.l	#1,d0
	add.l	d0,a0
	move.l	a0,(18+l470,a7)
	move.l	(18+l470,a7),-(a7)
	jsr	l395
	lea	(12+l470,a7),a0
	move.w	(10+l470,a7),d1
	ext.l	d1
	lsl.l	#1,d1
	move.w	d0,(0,a0,d1.l)
	addq.w	#4,a7
l449
	addq.w	#1,(6+l470,a7)
l447
	cmp.w	#5,(6+l470,a7)
	blt	l446
l448
	move.w	#0,(4+l470,a7)
	bra	l451
l450
	move.b	#0,(18+l470,a7)
	moveq	#15,d0
	sub.w	(4+l470,a7),d0
	moveq	#1,d1
	lsl.w	d0,d1
	move.w	d1,(22+l470,a7)
	moveq	#15,d0
	sub.w	(4+l470,a7),d0
	move.w	d0,(24+l470,a7)
	move.w	#0,(6+l470,a7)
	bra	l455
l454
	lea	(8+l470,a7),a0
	move.w	(6+l470,a7),d0
	ext.l	d0
	lsl.l	#1,d0
	add.l	d0,a0
	move.w	(a0),d0
	and.w	(22+l470,a7),d0
	beq	l459
l458
	move.w	(6+l470,a7),d1
	moveq	#1,d0
	lsl.b	d1,d0
	or.b	d0,(18+l470,a7)
l459
l457
	addq.w	#1,(6+l470,a7)
l455
	cmp.w	#5,(6+l470,a7)
	blt	l454
l456
	lea	(20+l470,a7),a0
	move.l	a0,-(a7)
	lea	(23+l470,a7),a0
	move.l	a0,-(a7)
	moveq	#0,d0
	move.b	(26+l470,a7),d0
	move.w	d0,-(a7)
	move.w	(10+l470,a7),-(a7)
	jsr	l411
	moveq	#0,d0
	move.b	(31+l470,a7),d0
	and.w	#2,d0
	add.w	#12,a7
	beq	l461
l460
	move.l	(32+l470,a7),a0
	add.w	#40,a0
	move.w	(0+l470,a7),d0
	ext.l	d0
	lsl.l	#7,d0
	add.l	d0,a0
	move.w	(2+l470,a7),d0
	ext.l	d0
	lsl.l	#3,d0
	add.l	d0,a0
	move.w	(24+l470,a7),d1
	moveq	#1,d0
	lsl.w	d1,d0
	or.w	d0,(a0)
l461
	moveq	#0,d0
	move.b	(19+l470,a7),d0
	and.w	#1,d0
	beq	l463
l462
	move.l	(32+l470,a7),a0
	add.w	#40,a0
	move.w	(0+l470,a7),d0
	ext.l	d0
	lsl.l	#7,d0
	add.l	d0,a0
	move.w	(2+l470,a7),d0
	ext.l	d0
	lsl.l	#3,d0
	add.l	d0,a0
	addq.l	#2,a0
	move.w	(24+l470,a7),d1
	moveq	#1,d0
	lsl.w	d1,d0
	or.w	d0,(a0)
l463
	moveq	#0,d0
	move.b	(20+l470,a7),d0
	and.w	#2,d0
	beq	l465
l464
	move.l	(32+l470,a7),a0
	add.w	#40,a0
	move.w	(0+l470,a7),d0
	ext.l	d0
	lsl.l	#7,d0
	add.l	d0,a0
	move.w	(2+l470,a7),d0
	ext.l	d0
	lsl.l	#3,d0
	add.l	d0,a0
	addq.l	#4,a0
	move.w	(24+l470,a7),d1
	moveq	#1,d0
	lsl.w	d1,d0
	or.w	d0,(a0)
l465
	moveq	#0,d0
	move.b	(20+l470,a7),d0
	and.w	#1,d0
	beq	l467
l466
	move.l	(32+l470,a7),a0
	add.w	#40,a0
	move.w	(0+l470,a7),d0
	ext.l	d0
	lsl.l	#7,d0
	add.l	d0,a0
	move.w	(2+l470,a7),d0
	ext.l	d0
	lsl.l	#3,d0
	add.l	d0,a0
	addq.l	#6,a0
	move.w	(24+l470,a7),d1
	moveq	#1,d0
	lsl.w	d1,d0
	or.w	d0,(a0)
l467
l453
	addq.w	#1,(4+l470,a7)
l451
	cmp.w	#16,(4+l470,a7)
	blt	l450
l452
l445
	addq.w	#1,(2+l470,a7)
l443
	cmp.w	#16,(2+l470,a7)
	blt	l442
l444
l441
	addq.w	#1,(0+l470,a7)
l439
	cmp.w	#33,(0+l470,a7)
	blt	l438
l440
l436
l468	reg	d2/d3
	movem.l	(a7)+,d2/d3
l470	equ	8
	add.w	#28,a7
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l471
	subq.w	#4,a7
	movem.l	l474,-(a7)
	move.w	(12+l476,a7),d0
	ext.l	d0
	moveq	#72,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	move.l	(8+l476,a7),a0
	add.l	d0,a0
	move.l	a0,(0+l476,a7)
	move.l	(0+l476,a7),a0
	move.w	#0,(a0)
	move.l	(0+l476,a7),a0
	move.w	#0,(2,a0)
	move.l	(0+l476,a7),a0
	move.w	#0,(68,a0)
	move.l	(0+l476,a7),a0
	move.w	#0,(70,a0)
l472
l474	reg	d2/d3
	movem.l	(a7)+,d2/d3
l476	equ	8
	addq.w	#4,a7
	rts
; stacksize=12
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l477
	subq.w	#4,a7
	movem.l	l484,-(a7)
	move.w	#0,(0+l486,a7)
	bra	l481
l480
	move.w	(0+l486,a7),-(a7)
	move.l	(10+l486,a7),-(a7)
	jsr	l471
	addq.w	#6,a7
l483
	addq.w	#1,(0+l486,a7)
l481
	cmp.w	#8,(0+l486,a7)
	blt	l480
l482
l478
l484	reg
l486	equ	0
	addq.w	#4,a7
	rts
; stacksize=26
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l487
	subq.w	#8,a7
	movem.l	l499,-(a7)
	move.w	#304,-(a7)
	move.w	#0,-(a7)
	move.w	(20+l501,a7),-(a7)
	jsr	l15
	move.w	#128,d1
	add.w	d0,d1
	move.w	d1,(6+l501,a7)
	move.w	#176,-(a7)
	move.w	#0,-(a7)
	move.w	(28+l501,a7),-(a7)
	jsr	l15
	moveq	#44,d1
	add.w	d0,d1
	move.w	d1,(14+l501,a7)
	moveq	#16,d0
	add.w	(14+l501,a7),d0
	move.w	d0,(16+l501,a7)
	move.w	#255,d0
	and.w	(14+l501,a7),d0
	lsl.w	#8,d0
	move.w	(12+l501,a7),d1
	asr.w	#1,d1
	and.w	#255,d1
	or.w	d1,d0
	move.l	(24+l501,a7),a0
	move.w	d0,(a0)
	move.w	#255,d0
	and.w	(16+l501,a7),d0
	lsl.w	#8,d0
	move.w	#256,d1
	and.w	(14+l501,a7),d1
	add.w	#12,a7
	beq	l491
l490
	moveq	#4,d1
	bra	l492
l491
	moveq	#0,d1
l492
	or.w	d1,d0
	move.w	#256,d1
	and.w	(4+l501,a7),d1
	beq	l494
l493
	moveq	#2,d1
	bra	l495
l494
	moveq	#0,d1
l495
	or.w	d1,d0
	moveq	#1,d1
	and.w	(0+l501,a7),d1
	or.w	d1,d0
	tst.b	(21+l501,a7)
	beq	l497
l496
	move.w	#128,d1
	bra	l498
l497
	moveq	#0,d1
l498
	or.w	d1,d0
	move.l	(12+l501,a7),a0
	move.w	d0,(2,a0)
l488
l499	reg
l501	equ	0
	addq.w	#8,a7
	rts
; stacksize=24
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l502
	sub.w	#20,a7
	movem.l	l509,-(a7)
	move.l	(24+l511,a7),a0
	move.w	(28+l511,a7),d0
	ext.l	d0
	lsl.l	#2,d0
	move.l	(32,a0,d0.l),(0+l511,a7)
	move.w	(30+l511,a7),(4+l511,a7)
	moveq	#6,d0
	add.w	(30+l511,a7),d0
	move.w	d0,(6+l511,a7)
	move.w	(4+l511,a7),d0
	ext.l	d0
	moveq	#72,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	move.l	(0+l511,a7),a0
	add.l	d0,a0
	move.l	a0,(8+l511,a7)
	move.w	(6+l511,a7),d0
	ext.l	d0
	moveq	#72,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	move.l	(0+l511,a7),a0
	add.l	d0,a0
	move.l	a0,(12+l511,a7)
	move.w	#0,-(a7)
	move.w	(38+l511,a7),-(a7)
	move.w	(38+l511,a7),-(a7)
	move.l	(14+l511,a7),-(a7)
	jsr	l487
	move.w	#0,-(a7)
	move.w	(48+l511,a7),-(a7)
	move.w	(48+l511,a7),-(a7)
	move.l	(28+l511,a7),-(a7)
	jsr	l487
	move.w	#0,(36+l511,a7)
	add.w	#20,a7
	bra	l506
l505
	move.l	(24+l511,a7),a0
	add.w	#40,a0
	move.w	(32+l511,a7),d0
	ext.l	d0
	lsl.l	#7,d0
	add.l	d0,a0
	move.w	(16+l511,a7),d0
	ext.l	d0
	lsl.l	#3,d0
	add.l	d0,a0
	move.w	(16+l511,a7),d0
	lsl.w	#1,d0
	addq.w	#2,d0
	ext.l	d0
	lsl.l	#1,d0
	move.l	(8+l511,a7),a1
	move.w	(a0),(0,a1,d0.l)
	move.l	(24+l511,a7),a0
	add.w	#40,a0
	move.w	(32+l511,a7),d0
	ext.l	d0
	lsl.l	#7,d0
	add.l	d0,a0
	move.w	(16+l511,a7),d0
	ext.l	d0
	lsl.l	#3,d0
	add.l	d0,a0
	move.w	(16+l511,a7),d0
	lsl.w	#1,d0
	addq.w	#3,d0
	ext.l	d0
	lsl.l	#1,d0
	move.l	(8+l511,a7),a1
	move.w	(2,a0),(0,a1,d0.l)
	move.l	(24+l511,a7),a0
	add.w	#40,a0
	move.w	(32+l511,a7),d0
	ext.l	d0
	lsl.l	#7,d0
	add.l	d0,a0
	move.w	(16+l511,a7),d0
	ext.l	d0
	lsl.l	#3,d0
	add.l	d0,a0
	move.w	(16+l511,a7),d0
	lsl.w	#1,d0
	addq.w	#2,d0
	ext.l	d0
	lsl.l	#1,d0
	move.l	(12+l511,a7),a1
	move.w	(4,a0),(0,a1,d0.l)
	move.l	(24+l511,a7),a0
	add.w	#40,a0
	move.w	(32+l511,a7),d0
	ext.l	d0
	lsl.l	#7,d0
	add.l	d0,a0
	move.w	(16+l511,a7),d0
	ext.l	d0
	lsl.l	#3,d0
	add.l	d0,a0
	move.w	(16+l511,a7),d0
	lsl.w	#1,d0
	addq.w	#3,d0
	ext.l	d0
	lsl.l	#1,d0
	move.l	(12+l511,a7),a1
	move.w	(6,a0),(0,a1,d0.l)
l508
	addq.w	#1,(16+l511,a7)
l506
	cmp.w	#16,(16+l511,a7)
	blt	l505
l507
	move.l	(8+l511,a7),a0
	move.w	#0,(68,a0)
	move.l	(8+l511,a7),a0
	move.w	#0,(70,a0)
	move.l	(12+l511,a7),a0
	move.w	#0,(68,a0)
	move.l	(12+l511,a7),a0
	move.w	#0,(70,a0)
l503
l509	reg	d2/d3
	movem.l	(a7)+,d2/d3
l511	equ	8
	add.w	#20,a7
	rts
; stacksize=76
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l512
	movem.l	l515,-(a7)
	move.b	(5+l517,a7),d1
	moveq	#1,d0
	lsl.w	d1,d0
l513
l515	reg
l517	equ	0
	rts
; stacksize=0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l518
	subq.w	#8,a7
	movem.l	l532,-(a7)
	cmp.w	#20,(12+l534,a7)
	bge	l522
l521
	move.w	#20,(12+l534,a7)
l522
	tst.w	(14+l534,a7)
	bgt	l524
l523
	move.w	#8,(14+l534,a7)
l524
	move.w	(12+l534,a7),d0
	ext.l	d0
	move.w	(14+l534,a7),d1
	ext.l	d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	move.l	d0,(0+l534,a7)
	beq	l526
l525
	move.l	(0+l534,a7),d1
	move.l	#3546895,d0
	jsr	__divu
	move.l	d0,d2
	bra	l527
l526
	move.l	#65535,d2
l527
	move.l	d2,(4+l534,a7)
	moveq	#124,d0
	cmp.l	(4+l534,a7),d0
	bls	l529
l528
	moveq	#124,d0
	move.l	d0,(4+l534,a7)
l529
	cmp.l	#65535,(4+l534,a7)
	bls	l531
l530
	move.l	#65535,(4+l534,a7)
l531
	move.w	(6+l534,a7),d0
l519
l532	reg	d2/d3
	movem.l	(a7)+,d2/d3
l534	equ	8
	addq.w	#8,a7
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l535
	subq.w	#4,a7
	movem.l	l538,-(a7)
	move.w	#180,d0
	add.w	(8+l540,a7),d0
	ext.l	d0
	divs.w	#260,d0
	move.w	d0,(0+l540,a7)
	move.w	#64,-(a7)
	move.w	#1,-(a7)
	move.w	(4+l540,a7),-(a7)
	jsr	l15
	addq.w	#6,a7
l536
l538	reg
l540	equ	0
	addq.w	#4,a7
	rts
; stacksize=14
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l541
	subq.w	#4,a7
	movem.l	l546,-(a7)
	tst.w	(8+l548,a7)
	bgt	l545
l544
	moveq	#0,d0
	bra	l542
l545
	moveq	#50,d0
	muls.w	(8+l548,a7),d0
	add.w	#999,d0
	ext.l	d0
	divs.w	#1000,d0
	move.w	d0,(0+l548,a7)
	move.w	#255,-(a7)
	move.w	#1,-(a7)
	move.w	(4+l548,a7),-(a7)
	jsr	l15
	addq.w	#6,a7
l542
l546	reg
l548	equ	0
	addq.w	#4,a7
	rts
; stacksize=14
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l549
	movem.l	l556,-(a7)
	move.l	(4+l558,a7),a0
	tst.l	(a0)
	bne	l553
l552
	moveq	#0,d0
	bra	l550
l553
	cmp.b	#2,(9+l558,a7)
	bne	l555
l554
	move.l	(4+l558,a7),a1
	move.l	(a1),a0
	addq.l	#8,a0
	move.l	a0,d0
	bra	l550
l555
	move.l	(4+l558,a7),a0
	move.l	(a0),d0
l550
l556	reg
l558	equ	0
	rts
; stacksize=0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l559
	movem.l	l564,-(a7)
	cmp.b	#2,(5+l566,a7)
	bne	l563
l562
	moveq	#32,d0
	bra	l560
l563
	moveq	#4,d0
l560
l564	reg
l566	equ	0
	rts
; stacksize=0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l567
	movem.l	l570,-(a7)
	move.l	#14675968,a0
	add.w	#160,a0
	moveq	#0,d0
	move.b	(5+l572,a7),d0
	lsl.l	#4,d0
	move.w	#0,(8,a0,d0.l)
	moveq	#0,d0
	move.b	(5+l572,a7),d0
	move.w	d0,-(a7)
	jsr	l512
	move.l	#14675968,a0
	move.w	d0,(150,a0)
	addq.w	#2,a7
l568
l570	reg
l572	equ	0
	rts
; stacksize=6
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l573
	subq.w	#4,a7
	movem.l	l576,-(a7)
	move.l	(8+l578,a7),a0
	move.b	(276,a0),(0+l578,a7)
	move.l	#282,d2
	moveq	#0,d0
	move.l	(8+l578,a7),a0
	inline
	move.l	a0,a1
	cmp.l	#16,d2
	blo	.l3
	move.l	a0,d1
	and.b	#1,d1
	beq	.l1
	move.b	d0,(a0)+
	subq.l	#1,d2
.l1
	move.b	d0,d1
	lsl.w	#8,d0
	move.b	d1,d0
	move.w	d0,d1
	swap	d0
	move.w	d1,d0
	moveq	#3,d1
	and.l	d2,d1
	sub.l	d1,d2
.l2
	move.l	d0,(a0)+
	subq.l	#4,d2
	bne	.l2
	move.w	d1,d2
.l3
	subq.w	#1,d2
	blo	.l5
.l4
	move.b	d0,(a0)+
	dbf	d2,.l4
.l5
	move.l	a1,d0
	einline
	move.l	d0,a1
	move.l	(8+l578,a7),a0
	move.b	(0+l578,a7),(276,a0)
	moveq	#0,d0
	move.b	(0+l578,a7),d0
	move.w	d0,-(a7)
	jsr	l567
	addq.w	#2,a7
l574
l576	reg	d2
	movem.l	(a7)+,d2
l578	equ	4
	addq.w	#4,a7
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l579
	movem.l	l585,-(a7)
	tst.l	(4+l587,a7)
	beq	l582
l584
	move.l	(4+l587,a7),a0
	tst.b	(568,a0)
	bne	l583
l582
	bra	l580
l583
	move.l	(4+l587,a7),a0
	pea	(4,a0)
	jsr	l573
	move.l	(8+l587,a7),a0
	addq.w	#4,a0
	pea	(282,a0)
	jsr	l573
	addq.w	#8,a7
l580
l585	reg
l587	equ	0
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l588
	subq.w	#4,a7
	movem.l	l598,-(a7)
	tst.l	(12+l600,a7)
	beq	l591
l593
	move.l	(12+l600,a7),a0
	tst.w	(4,a0)
	bne	l592
l591
	moveq	#0,d0
	bra	l589
l592
	move.l	(8+l600,a7),a0
	cmp.b	#32,(278,a0)
	bcs	l595
l594
	moveq	#0,d0
	bra	l589
l595
	move.l	(8+l600,a7),a0
	move.l	(8+l600,a7),a1
	move.b	(277,a0),d0
	add.b	(278,a1),d0
	move.b	d0,(0+l600,a7)
	cmp.b	#32,(0+l600,a7)
	bcs	l597
l596
	sub.b	#32,(0+l600,a7)
l597
	moveq	#0,d0
	move.b	(0+l600,a7),d0
	lsl.l	#3,d0
	move.l	(8+l600,a7),a0
	move.l	(12+l600,a7),a1
	lea	(a1),a2
	lea	(0,a0,d0.l),a3
	move.l	(a2)+,(a3)+
	move.l	(a2)+,(a3)+
	move.l	(8+l600,a7),a0
	add.w	#278,a0
	addq.b	#1,(a0)
	moveq	#1,d0
l589
l598	reg	a2/a3
	movem.l	(a7)+,a2/a3
l600	equ	8
	addq.w	#4,a7
	rts
; stacksize=12
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l601
	subq.w	#8,a7
	movem.l	l604,-(a7)
	moveq	#8,d2
	moveq	#0,d0
	lea	(0+l606,a7),a0
	inline
	move.l	a0,a1
	cmp.l	#16,d2
	blo	.l3
	move.l	a0,d1
	and.b	#1,d1
	beq	.l1
	move.b	d0,(a0)+
	subq.l	#1,d2
.l1
	move.b	d0,d1
	lsl.w	#8,d0
	move.b	d1,d0
	move.w	d0,d1
	swap	d0
	move.w	d1,d0
	moveq	#3,d1
	and.l	d2,d1
	sub.l	d1,d2
.l2
	move.l	d0,(a0)+
	subq.l	#4,d2
	bne	.l2
	move.w	d1,d2
.l3
	subq.w	#1,d2
	blo	.l5
.l4
	move.b	d0,(a0)+
	dbf	d2,.l4
.l5
	move.l	a1,d0
	einline
	move.l	d0,a1
	move.b	#0,(7+l606,a7)
	move.w	(16+l606,a7),-(a7)
	jsr	l541
	move.w	d0,(6+l606,a7)
	lea	(2+l606,a7),a0
	move.l	a0,-(a7)
	move.l	(18+l606,a7),-(a7)
	jsr	l588
	add.w	#10,a7
l602
l604	reg	d2
	movem.l	(a7)+,d2
l606	equ	4
	addq.w	#8,a7
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l607
	subq.w	#8,a7
	movem.l	l615,-(a7)
	tst.l	(12+l617,a7)
	beq	l610
l612
	move.l	(12+l617,a7),a0
	tst.b	(568,a0)
	bne	l611
l610
	bra	l608
l611
	tst.w	(18+l617,a7)
	bgt	l614
l613
	move.w	(16+l617,a7),(18+l617,a7)
l614
	moveq	#8,d2
	moveq	#0,d0
	lea	(0+l617,a7),a0
	inline
	move.l	a0,a1
	cmp.l	#16,d2
	blo	.l3
	move.l	a0,d1
	and.b	#1,d1
	beq	.l1
	move.b	d0,(a0)+
	subq.l	#1,d2
.l1
	move.b	d0,d1
	lsl.w	#8,d0
	move.b	d1,d0
	move.w	d0,d1
	swap	d0
	move.w	d1,d0
	moveq	#3,d1
	and.l	d2,d1
	sub.l	d1,d2
.l2
	move.l	d0,(a0)+
	subq.l	#4,d2
	bne	.l2
	move.w	d1,d2
.l3
	subq.w	#1,d2
	blo	.l5
.l4
	move.b	d0,(a0)+
	dbf	d2,.l4
.l5
	move.l	a1,d0
	einline
	move.l	d0,a1
	move.b	#1,(7+l617,a7)
	move.w	#8,-(a7)
	move.w	(18+l617,a7),-(a7)
	jsr	l518
	move.w	d0,(4+l617,a7)
	move.w	#8,-(a7)
	move.w	(24+l617,a7),-(a7)
	jsr	l518
	move.w	d0,(10+l617,a7)
	move.w	(28+l617,a7),-(a7)
	jsr	l541
	move.w	d0,(14+l617,a7)
	move.w	#80,-(a7)
	move.w	#20,-(a7)
	move.w	(38+l617,a7),-(a7)
	jsr	l15
	muls.w	(38+l617,a7),d0
	add.w	#25,d0
	ext.l	d0
	divs.w	#50,d0
	move.w	d0,(38+l617,a7)
	move.w	(38+l617,a7),-(a7)
	jsr	l535
	move.b	d0,(24+l617,a7)
	lea	(18+l617,a7),a0
	move.l	a0,-(a7)
	move.l	(34+l617,a7),a0
	pea	(4,a0)
	jsr	l588
	add.w	#26,a7
l608
l615	reg	d2
	movem.l	(a7)+,d2
l617	equ	4
	addq.w	#8,a7
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l618
	movem.l	l621,-(a7)
	move.w	#50,-(a7)
	move.w	(14+l623,a7),-(a7)
	move.w	(14+l623,a7),-(a7)
	move.w	(14+l623,a7),-(a7)
	move.w	(16+l623,a7),-(a7)
	move.l	(14+l623,a7),-(a7)
	jsr	l607
	add.w	#14,a7
l619
l621	reg
l623	equ	0
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l624
	movem.l	l630,-(a7)
	tst.l	(4+l632,a7)
	beq	l627
l629
	move.l	(4+l632,a7),a0
	tst.b	(568,a0)
	bne	l628
l627
	bra	l625
l628
	move.w	(8+l632,a7),-(a7)
	move.l	(6+l632,a7),a0
	pea	(4,a0)
	jsr	l601
	addq.w	#6,a7
l625
l630	reg
l632	equ	0
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l633
	subq.w	#8,a7
	movem.l	l639,-(a7)
	tst.l	(12+l641,a7)
	beq	l636
l638
	move.l	(12+l641,a7),a0
	tst.b	(568,a0)
	bne	l637
l636
	bra	l634
l637
	moveq	#8,d2
	moveq	#0,d0
	lea	(0+l641,a7),a0
	inline
	move.l	a0,a1
	cmp.l	#16,d2
	blo	.l3
	move.l	a0,d1
	and.b	#1,d1
	beq	.l1
	move.b	d0,(a0)+
	subq.l	#1,d2
.l1
	move.b	d0,d1
	lsl.w	#8,d0
	move.b	d1,d0
	move.w	d0,d1
	swap	d0
	move.w	d1,d0
	moveq	#3,d1
	and.l	d2,d1
	sub.l	d1,d2
.l2
	move.l	d0,(a0)+
	subq.l	#4,d2
	bne	.l2
	move.w	d1,d2
.l3
	subq.w	#1,d2
	blo	.l5
.l4
	move.b	d0,(a0)+
	dbf	d2,.l4
.l5
	move.l	a1,d0
	einline
	move.l	d0,a1
	move.b	#2,(7+l641,a7)
	move.w	#64,-(a7)
	move.w	#2200,-(a7)
	jsr	l518
	move.w	d0,(4+l641,a7)
	move.w	#64,-(a7)
	move.w	#900,-(a7)
	jsr	l518
	move.w	d0,(10+l641,a7)
	move.w	(24+l641,a7),-(a7)
	jsr	l541
	move.w	d0,(14+l641,a7)
	move.w	(28+l641,a7),-(a7)
	jsr	l535
	move.b	d0,(18+l641,a7)
	lea	(12+l641,a7),a0
	move.l	a0,-(a7)
	move.l	(28+l641,a7),a0
	addq.w	#4,a0
	pea	(282,a0)
	jsr	l588
	add.w	#20,a7
l634
l639	reg	d2
	movem.l	(a7)+,d2
l641	equ	4
	addq.w	#8,a7
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l642
	subq.w	#8,a7
	movem.l	l662,-(a7)
	tst.l	(12+l664,a7)
	beq	l645
l649
	move.l	(12+l664,a7),a0
	tst.b	(568,a0)
	beq	l645
l648
	tst.l	(16+l664,a7)
	beq	l645
l647
	tst.w	(20+l664,a7)
	bgt	l646
l645
	bra	l643
l646
	move.w	#0,(0+l664,a7)
	bra	l651
l650
	move.w	(0+l664,a7),d0
	ext.l	d0
	lsl.l	#3,d0
	move.l	(16+l664,a7),a0
	add.l	d0,a0
	move.l	a0,(2+l664,a7)
	move.l	(2+l664,a7),a0
	tst.w	(a0)
	bls	l655
l656
	move.l	(2+l664,a7),a0
	tst.w	(2,a0)
	bls	l655
l654
	move.l	(2+l664,a7),a0
	move.w	(4,a0),-(a7)
	move.l	(4+l664,a7),a0
	move.w	(2,a0),-(a7)
	move.l	(6+l664,a7),a0
	move.w	(a0),-(a7)
	move.l	(18+l664,a7),-(a7)
	jsr	l618
	add.w	#10,a7
	bra	l657
l655
	move.l	(2+l664,a7),a0
	tst.w	(2,a0)
	bls	l659
l658
	move.l	(2+l664,a7),a0
	move.w	(2,a0),-(a7)
	move.l	(14+l664,a7),-(a7)
	jsr	l624
	addq.w	#6,a7
l659
l657
	move.l	(2+l664,a7),a0
	tst.w	(6,a0)
	bls	l661
l660
	move.l	(2+l664,a7),a0
	move.w	(6,a0),-(a7)
	move.l	(14+l664,a7),-(a7)
	jsr	l624
	addq.w	#6,a7
l661
l653
	addq.w	#1,(0+l664,a7)
l651
	move.w	(0+l664,a7),d0
	cmp.w	(20+l664,a7),d0
	blt	l650
l652
l643
l662	reg
l664	equ	0
	addq.w	#8,a7
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l665
	movem.l	l668,-(a7)
	move.w	#36,-(a7)
	move.w	#13600,-(a7)
	move.w	#30,-(a7)
	move.w	#760,-(a7)
	move.w	#620,-(a7)
	move.l	(14+l670,a7),-(a7)
	jsr	l607
	move.w	#6,-(a7)
	move.l	(20+l670,a7),-(a7)
	jsr	l624
	move.w	#34,-(a7)
	move.w	#14600,-(a7)
	move.w	#34,-(a7)
	move.w	#1020,-(a7)
	move.w	#820,-(a7)
	move.l	(34+l670,a7),-(a7)
	jsr	l607
	move.w	#6,-(a7)
	move.l	(40+l670,a7),-(a7)
	jsr	l624
	move.w	#31,-(a7)
	move.w	#15600,-(a7)
	move.w	#38,-(a7)
	move.w	#1360,-(a7)
	move.w	#1080,-(a7)
	move.l	(54+l670,a7),-(a7)
	jsr	l607
	move.w	#8,-(a7)
	move.l	(60+l670,a7),-(a7)
	jsr	l624
	move.w	#28,-(a7)
	move.w	#16800,-(a7)
	move.w	#58,-(a7)
	move.w	#1880,-(a7)
	move.w	#1420,-(a7)
	move.l	(74+l670,a7),-(a7)
	jsr	l607
	add.w	#74,a7
l666
l668	reg
l670	equ	0
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l671
	movem.l	l674,-(a7)
	move.w	#32,-(a7)
	move.w	#12800,-(a7)
	move.w	#34,-(a7)
	move.w	#860,-(a7)
	move.w	#700,-(a7)
	move.l	(14+l676,a7),-(a7)
	jsr	l607
	move.w	#6,-(a7)
	move.l	(20+l676,a7),-(a7)
	jsr	l624
	move.w	#30,-(a7)
	move.w	#13600,-(a7)
	move.w	#36,-(a7)
	move.w	#1120,-(a7)
	move.w	#920,-(a7)
	move.l	(34+l676,a7),-(a7)
	jsr	l607
	move.w	#6,-(a7)
	move.l	(40+l676,a7),-(a7)
	jsr	l624
	move.w	#28,-(a7)
	move.w	#14600,-(a7)
	move.w	#40,-(a7)
	move.w	#1460,-(a7)
	move.w	#1160,-(a7)
	move.l	(54+l676,a7),-(a7)
	jsr	l607
	move.w	#8,-(a7)
	move.l	(60+l676,a7),-(a7)
	jsr	l624
	move.w	#24,-(a7)
	move.w	#16600,-(a7)
	move.w	#74,-(a7)
	move.w	#1980,-(a7)
	move.w	#1540,-(a7)
	move.l	(74+l676,a7),-(a7)
	jsr	l607
	add.w	#74,a7
l672
l674	reg
l676	equ	0
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l677
	movem.l	l681,-(a7)
	move.w	#6,-(a7)
	pea	l680
	move.l	(10+l683,a7),-(a7)
	jsr	l642
	move.w	#18,-(a7)
	move.w	#13200,-(a7)
	move.w	#86,-(a7)
	move.w	#1760,-(a7)
	move.w	#1319,-(a7)
	move.l	(24+l683,a7),-(a7)
	jsr	l607
	move.w	#10,-(a7)
	move.l	(30+l683,a7),-(a7)
	jsr	l624
	move.w	#20,-(a7)
	move.w	#12600,-(a7)
	move.w	#84,-(a7)
	move.w	#1568,-(a7)
	move.w	#1175,-(a7)
	move.l	(44+l683,a7),-(a7)
	jsr	l607
	move.w	#12,-(a7)
	move.l	(50+l683,a7),-(a7)
	jsr	l624
	move.w	#22,-(a7)
	move.w	#13800,-(a7)
	move.w	#96,-(a7)
	move.w	#1976,-(a7)
	move.w	#1568,-(a7)
	move.l	(64+l683,a7),-(a7)
	jsr	l607
	add.w	#64,a7
l678
l681	reg
l683	equ	0
	rts
	cnop	0,4
l680
	dc.w	587
	dc.w	52
	dc.w	8600
	dc.w	5
	dc.w	740
	dc.w	56
	dc.w	9000
	dc.w	5
	dc.w	880
	dc.w	60
	dc.w	9500
	dc.w	6
	dc.w	988
	dc.w	64
	dc.w	10000
	dc.w	6
	dc.w	1175
	dc.w	74
	dc.w	10600
	dc.w	8
	dc.w	1319
	dc.w	84
	dc.w	11400
	dc.w	10
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l684
	movem.l	l687,-(a7)
	move.w	#11000,-(a7)
	move.w	#44,-(a7)
	move.l	(8+l689,a7),-(a7)
	jsr	l633
	move.w	#48,-(a7)
	move.w	#13800,-(a7)
	move.w	#74,-(a7)
	move.w	#220,-(a7)
	move.w	#360,-(a7)
	move.l	(22+l689,a7),-(a7)
	jsr	l607
	move.w	#8,-(a7)
	move.l	(28+l689,a7),-(a7)
	jsr	l624
	move.w	#54,-(a7)
	move.w	#14600,-(a7)
	move.w	#118,-(a7)
	move.w	#110,-(a7)
	move.w	#220,-(a7)
	move.l	(42+l689,a7),-(a7)
	jsr	l607
	add.w	#42,a7
l685
l687	reg
l689	equ	0
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l690
	movem.l	l693,-(a7)
	move.w	#11400,-(a7)
	move.w	#56,-(a7)
	move.l	(8+l695,a7),-(a7)
	jsr	l633
	move.w	#50,-(a7)
	move.w	#13400,-(a7)
	move.w	#76,-(a7)
	move.w	#230,-(a7)
	move.w	#300,-(a7)
	move.l	(22+l695,a7),-(a7)
	jsr	l607
	move.w	#6,-(a7)
	move.l	(28+l695,a7),-(a7)
	jsr	l624
	move.w	#52,-(a7)
	move.w	#13800,-(a7)
	move.w	#92,-(a7)
	move.w	#160,-(a7)
	move.w	#230,-(a7)
	move.l	(42+l695,a7),-(a7)
	jsr	l607
	move.w	#8,-(a7)
	move.l	(48+l695,a7),-(a7)
	jsr	l624
	move.w	#56,-(a7)
	move.w	#14600,-(a7)
	move.w	#140,-(a7)
	move.w	#90,-(a7)
	move.w	#160,-(a7)
	move.l	(62+l695,a7),-(a7)
	jsr	l607
	add.w	#62,a7
l691
l693	reg
l695	equ	0
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l696
	movem.l	l700,-(a7)
	move.w	#4,-(a7)
	pea	l699
	move.l	(10+l702,a7),-(a7)
	jsr	l642
	move.w	#8,-(a7)
	move.l	(16+l702,a7),-(a7)
	jsr	l624
	move.w	#36,-(a7)
	move.w	#11000,-(a7)
	move.w	#82,-(a7)
	move.w	#146,-(a7)
	move.w	#196,-(a7)
	move.l	(30+l702,a7),-(a7)
	jsr	l607
	add.w	#30,a7
l697
l700	reg
l702	equ	0
	rts
	cnop	0,4
l699
	dc.w	392
	dc.w	66
	dc.w	9000
	dc.w	10
	dc.w	330
	dc.w	74
	dc.w	9400
	dc.w	10
	dc.w	262
	dc.w	84
	dc.w	9900
	dc.w	14
	dc.w	196
	dc.w	108
	dc.w	10600
	dc.w	0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l703
	movem.l	l707,-(a7)
	move.w	#3,-(a7)
	pea	l706
	move.l	(10+l709,a7),-(a7)
	jsr	l642
	move.w	#22,-(a7)
	move.w	#10400,-(a7)
	move.w	#38,-(a7)
	move.w	#1620,-(a7)
	move.w	#1320,-(a7)
	move.l	(24+l709,a7),-(a7)
	jsr	l607
	add.w	#24,a7
l704
l707	reg
l709	equ	0
	rts
	cnop	0,4
l706
	dc.w	620
	dc.w	28
	dc.w	8200
	dc.w	4
	dc.w	860
	dc.w	32
	dc.w	9000
	dc.w	5
	dc.w	1180
	dc.w	42
	dc.w	9800
	dc.w	0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l710
	movem.l	l713,-(a7)
	move.w	#24,-(a7)
	move.w	#6800,-(a7)
	move.w	#24,-(a7)
	move.w	#820,-(a7)
	move.w	#1240,-(a7)
	move.l	(14+l715,a7),-(a7)
	jsr	l607
	move.w	#20,-(a7)
	move.w	#6200,-(a7)
	move.w	#20,-(a7)
	move.w	#560,-(a7)
	move.w	#820,-(a7)
	move.l	(28+l715,a7),-(a7)
	jsr	l607
	add.w	#28,a7
l711
l713	reg
l715	equ	0
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l716
	movem.l	l720,-(a7)
	move.l	(4+l722,a7),-(a7)
	jsr	l579
	move.w	#7,-(a7)
	pea	l719
	move.l	(14+l722,a7),-(a7)
	jsr	l642
	add.w	#14,a7
l717
l720	reg
l722	equ	0
	rts
	cnop	0,4
l719
	dc.w	1047
	dc.w	54
	dc.w	6200
	dc.w	4
	dc.w	1319
	dc.w	56
	dc.w	6700
	dc.w	4
	dc.w	1568
	dc.w	62
	dc.w	7100
	dc.w	8
	dc.w	2093
	dc.w	92
	dc.w	7800
	dc.w	12
	dc.w	1760
	dc.w	52
	dc.w	6600
	dc.w	4
	dc.w	2093
	dc.w	68
	dc.w	7600
	dc.w	8
	dc.w	2637
	dc.w	118
	dc.w	8400
	dc.w	0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l723
	movem.l	l727,-(a7)
	move.w	#2,-(a7)
	pea	l726
	move.l	(10+l729,a7),-(a7)
	jsr	l642
	add.w	#10,a7
l724
l727	reg
l729	equ	0
	rts
	cnop	0,4
l726
	dc.w	1568
	dc.w	24
	dc.w	5600
	dc.w	4
	dc.w	2093
	dc.w	34
	dc.w	6400
	dc.w	0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l730
	movem.l	l771,-(a7)
	tst.l	(4+l773,a7)
	beq	l733
l736
	move.l	(4+l773,a7),a0
	tst.b	(568,a0)
	beq	l733
l735
	tst.l	(8+l773,a7)
	bne	l734
l733
	bra	l731
l734
	move.l	#128,d0
	and.l	(8+l773,a7),d0
	beq	l738
l737
	move.l	(4+l773,a7),-(a7)
	jsr	l579
	move.l	(8+l773,a7),-(a7)
	jsr	l690
	addq.w	#8,a7
	bra	l731
l738
	move.l	#16384,d0
	and.l	(8+l773,a7),d0
	beq	l740
l739
	move.l	(4+l773,a7),-(a7)
	jsr	l579
	move.l	(8+l773,a7),-(a7)
	jsr	l696
	and.l	#32508,(16+l773,a7)
	addq.w	#8,a7
l740
	moveq	#16,d0
	and.l	(8+l773,a7),d0
	beq	l742
l741
	move.l	(4+l773,a7),-(a7)
	jsr	l579
	move.l	(8+l773,a7),-(a7)
	jsr	l684
	and.l	#32508,(16+l773,a7)
	addq.w	#8,a7
l742
	moveq	#32,d0
	and.l	(8+l773,a7),d0
	beq	l744
l743
	move.l	(4+l773,a7),-(a7)
	jsr	l579
	move.l	(8+l773,a7),-(a7)
	jsr	l677
	and.l	#32508,(16+l773,a7)
	addq.w	#8,a7
l744
	moveq	#8,d0
	and.l	(8+l773,a7),d0
	beq	l746
l745
	move.l	(4+l773,a7),-(a7)
	jsr	l671
	addq.w	#4,a7
l746
	move.l	#4294934528,d0
	and.l	(8+l773,a7),d0
	beq	l748
l747
	move.l	(4+l773,a7),-(a7)
	jsr	l703
	and.l	#4294967294,(12+l773,a7)
	addq.w	#4,a7
l748
	moveq	#1,d0
	and.l	(8+l773,a7),d0
	beq	l750
l749
	move.w	#7000,-(a7)
	move.w	#24,-(a7)
	move.l	(8+l773,a7),-(a7)
	jsr	l633
	move.w	#56,-(a7)
	move.w	#13200,-(a7)
	move.w	#36,-(a7)
	move.w	#230,-(a7)
	move.w	#300,-(a7)
	move.l	(22+l773,a7),-(a7)
	jsr	l607
	move.w	#30,-(a7)
	move.w	#11200,-(a7)
	move.w	#28,-(a7)
	move.w	#560,-(a7)
	move.w	#420,-(a7)
	move.l	(36+l773,a7),-(a7)
	jsr	l607
	add.w	#36,a7
l750
	moveq	#2,d0
	and.l	(8+l773,a7),d0
	beq	l752
l751
	move.l	(4+l773,a7),-(a7)
	jsr	l665
	addq.w	#4,a7
l752
	moveq	#4,d0
	and.l	(8+l773,a7),d0
	beq	l754
l753
	move.w	#44,-(a7)
	move.w	#9000,-(a7)
	move.w	#34,-(a7)
	move.w	#900,-(a7)
	move.w	#980,-(a7)
	move.l	(14+l773,a7),-(a7)
	jsr	l607
	move.w	#8,-(a7)
	move.l	(20+l773,a7),-(a7)
	jsr	l624
	move.w	#44,-(a7)
	move.w	#8600,-(a7)
	move.w	#42,-(a7)
	move.w	#760,-(a7)
	move.w	#880,-(a7)
	move.l	(34+l773,a7),-(a7)
	jsr	l607
	add.w	#34,a7
l754
	moveq	#64,d0
	and.l	(8+l773,a7),d0
	beq	l756
l755
	move.w	#34,-(a7)
	move.w	#10200,-(a7)
	move.w	#34,-(a7)
	move.w	#560,-(a7)
	move.w	#420,-(a7)
	move.l	(14+l773,a7),-(a7)
	jsr	l607
	move.w	#8,-(a7)
	move.l	(20+l773,a7),-(a7)
	jsr	l624
	move.w	#30,-(a7)
	move.w	#11200,-(a7)
	move.w	#46,-(a7)
	move.w	#860,-(a7)
	move.w	#620,-(a7)
	move.l	(34+l773,a7),-(a7)
	jsr	l607
	add.w	#34,a7
l756
l758
	move.l	#4096,d0
	and.l	(8+l773,a7),d0
	beq	l760
l759
	move.w	#30,-(a7)
	move.w	#9800,-(a7)
	move.w	#32,-(a7)
	move.w	#720,-(a7)
	move.w	#520,-(a7)
	move.l	(14+l773,a7),-(a7)
	jsr	l607
	and.l	#4294967039,(22+l773,a7)
	add.w	#14,a7
l760
	move.l	#8192,d0
	and.l	(8+l773,a7),d0
	beq	l762
l761
	move.w	#14400,-(a7)
	move.w	#56,-(a7)
	move.l	(8+l773,a7),-(a7)
	jsr	l633
	move.w	#58,-(a7)
	move.w	#15600,-(a7)
	move.w	#96,-(a7)
	move.w	#150,-(a7)
	move.w	#280,-(a7)
	move.l	(22+l773,a7),-(a7)
	jsr	l607
	move.w	#62,-(a7)
	move.w	#14800,-(a7)
	move.w	#126,-(a7)
	move.w	#90,-(a7)
	move.w	#190,-(a7)
	move.l	(36+l773,a7),-(a7)
	jsr	l607
	and.l	#4294967039,(44+l773,a7)
	add.w	#36,a7
l762
	move.l	#256,d0
	and.l	(8+l773,a7),d0
	beq	l764
l763
	move.w	#4600,-(a7)
	move.w	#18,-(a7)
	move.l	(8+l773,a7),-(a7)
	jsr	l633
	move.w	#56,-(a7)
	move.w	#8600,-(a7)
	move.w	#26,-(a7)
	move.w	#170,-(a7)
	move.w	#240,-(a7)
	move.l	(22+l773,a7),-(a7)
	jsr	l607
	add.w	#22,a7
l764
	move.l	#512,d0
	and.l	(8+l773,a7),d0
	beq	l766
l765
	move.l	(4+l773,a7),-(a7)
	jsr	l671
	addq.w	#4,a7
l766
	move.l	#2048,d0
	and.l	(8+l773,a7),d0
	beq	l768
l767
	move.w	#30,-(a7)
	move.w	#9800,-(a7)
	move.w	#20,-(a7)
	move.w	#920,-(a7)
	move.w	#760,-(a7)
	move.l	(14+l773,a7),-(a7)
	jsr	l607
	move.w	#4,-(a7)
	move.l	(20+l773,a7),-(a7)
	jsr	l624
	move.w	#26,-(a7)
	move.w	#10800,-(a7)
	move.w	#30,-(a7)
	move.w	#1220,-(a7)
	move.w	#940,-(a7)
	move.l	(34+l773,a7),-(a7)
	jsr	l607
	add.w	#34,a7
l768
	move.l	#1024,d0
	and.l	(8+l773,a7),d0
	beq	l770
l769
	move.w	#40,-(a7)
	move.w	#8600,-(a7)
	move.w	#22,-(a7)
	move.w	#500,-(a7)
	move.w	#540,-(a7)
	move.l	(14+l773,a7),-(a7)
	jsr	l607
	move.w	#5,-(a7)
	move.l	(20+l773,a7),-(a7)
	jsr	l624
	move.w	#40,-(a7)
	move.w	#9000,-(a7)
	move.w	#24,-(a7)
	move.w	#620,-(a7)
	move.w	#660,-(a7)
	move.l	(34+l773,a7),-(a7)
	jsr	l607
	move.w	#6,-(a7)
	move.l	(40+l773,a7),-(a7)
	jsr	l624
	move.w	#36,-(a7)
	move.w	#9600,-(a7)
	move.w	#32,-(a7)
	move.w	#900,-(a7)
	move.w	#760,-(a7)
	move.l	(54+l773,a7),-(a7)
	jsr	l607
	add.w	#54,a7
l770
l731
l771	reg
l773	equ	0
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l774
	sub.w	#12,a7
	movem.l	l795,-(a7)
	tst.l	(16+l797,a7)
	beq	l777
l780
	tst.l	(20+l797,a7)
	beq	l777
l779
	move.l	(20+l797,a7),a0
	tst.b	(278,a0)
	bne	l778
l777
	tst.l	(20+l797,a7)
	beq	l782
	move.l	(20+l797,a7),a0
	moveq	#0,d0
	move.b	(276,a0),d0
	move.w	d0,-(a7)
	jsr	l567
	addq.w	#2,a7
l782
	bra	l775
l778
	move.l	(20+l797,a7),a0
	moveq	#0,d0
	move.b	(277,a0),d0
	lsl.l	#3,d0
	move.l	(20+l797,a7),a0
	move.l	(20+l797,a7),a1
	lea	(0,a0,d0.l),a2
	lea	(256,a1),a3
	move.l	(a2)+,(a3)+
	move.l	(a2)+,(a3)+
	move.l	(20+l797,a7),a0
	add.w	#277,a0
	addq.b	#1,(a0)
	move.l	(20+l797,a7),a0
	cmp.b	#32,(277,a0)
	bcs	l784
l783
	move.l	(20+l797,a7),a0
	move.b	#0,(277,a0)
l784
	move.l	(20+l797,a7),a0
	add.w	#278,a0
	subq.b	#1,(a0)
	move.l	(20+l797,a7),a0
	move.w	#0,(280,a0)
	move.l	(20+l797,a7),a0
	move.l	(20+l797,a7),a1
	move.w	(256,a0),(264,a1)
	move.l	(20+l797,a7),a0
	move.w	#0,(266,a0)
	move.l	(20+l797,a7),a0
	move.w	#0,(268,a0)
	move.l	(20+l797,a7),a0
	move.w	#1,(270,a0)
	move.l	(20+l797,a7),a0
	move.w	#0,(272,a0)
	move.l	(20+l797,a7),a0
	move.w	#0,(274,a0)
	move.l	(20+l797,a7),a0
	add.w	#256,a0
	cmp.w	#1,(4,a0)
	bls	l786
l785
	move.l	(20+l797,a7),a0
	add.w	#256,a0
	move.l	(20+l797,a7),a1
	move.w	(2,a0),d0
	sub.w	(256,a1),d0
	move.w	d0,(6+l797,a7)
	move.l	(20+l797,a7),a0
	add.w	#256,a0
	move.w	(4,a0),d0
	subq.w	#1,d0
	move.l	(20+l797,a7),a0
	move.w	d0,(270,a0)
	move.l	(20+l797,a7),a0
	move.w	(6+l797,a7),d0
	ext.l	d0
	divs.w	(270,a0),d0
	move.l	(20+l797,a7),a0
	move.w	d0,(266,a0)
	tst.w	(6+l797,a7)
	bge	l788
l787
	move.w	(6+l797,a7),d0
	neg.w	d0
	move.w	d0,(8+l797,a7)
	move.l	(20+l797,a7),a0
	move.w	#-1,(268,a0)
	bra	l789
l788
	move.w	(6+l797,a7),(8+l797,a7)
	move.l	(20+l797,a7),a0
	move.w	#1,(268,a0)
l789
	move.l	(20+l797,a7),a0
	move.w	(8+l797,a7),d0
	and.l	#65535,d0
	divu.w	(270,a0),d0
	swap	d0
	move.l	(20+l797,a7),a0
	move.w	d0,(272,a0)
l786
	move.l	(20+l797,a7),a0
	move.b	#1,(279,a0)
	move.l	(20+l797,a7),a0
	move.b	(276,a0),(0+l797,a7)
	move.l	(20+l797,a7),a0
	add.w	#256,a0
	tst.b	(7,a0)
	beq	l790
l792
	move.l	(20+l797,a7),a0
	add.w	#256,a0
	tst.b	(6,a0)
	bne	l791
l790
	moveq	#0,d0
	move.b	(0+l797,a7),d0
	move.w	d0,-(a7)
	jsr	l567
	addq.w	#2,a7
	bra	l775
l791
	move.l	(20+l797,a7),a0
	add.w	#256,a0
	moveq	#0,d0
	move.b	(7,a0),d0
	move.w	d0,-(a7)
	move.l	(18+l797,a7),-(a7)
	jsr	l549
	move.l	d0,(8+l797,a7)
	addq.w	#6,a7
	bne	l794
l793
	moveq	#0,d0
	move.b	(0+l797,a7),d0
	move.w	d0,-(a7)
	jsr	l567
	move.l	(22+l797,a7),a0
	move.b	#0,(279,a0)
	addq.w	#2,a7
	bra	l775
l794
	moveq	#0,d0
	move.b	(0+l797,a7),d0
	move.w	d0,-(a7)
	jsr	l512
	move.l	#14675968,a0
	move.w	d0,(150,a0)
	move.l	#14675968,a0
	add.w	#160,a0
	moveq	#0,d0
	move.b	(2+l797,a7),d0
	lsl.l	#4,d0
	move.l	(4+l797,a7),(0,a0,d0.l)
	move.l	(22+l797,a7),a0
	add.w	#256,a0
	moveq	#0,d0
	move.b	(7,a0),d0
	move.w	d0,-(a7)
	jsr	l559
	move.l	#14675968,a0
	add.w	#160,a0
	moveq	#0,d1
	move.b	(4+l797,a7),d1
	lsl.l	#4,d1
	move.w	d0,(4,a0,d1.l)
	move.l	(24+l797,a7),a0
	move.l	#14675968,a1
	add.w	#160,a1
	moveq	#0,d0
	move.b	(4+l797,a7),d0
	lsl.l	#4,d0
	move.w	(256,a0),(6,a1,d0.l)
	move.l	(24+l797,a7),a0
	add.w	#256,a0
	move.l	#14675968,a1
	add.w	#160,a1
	moveq	#0,d0
	move.b	(4+l797,a7),d0
	lsl.l	#4,d0
	add.l	d0,a1
	moveq	#0,d0
	move.b	(6,a0),d0
	move.w	d0,(8,a1)
	moveq	#0,d0
	move.b	(4+l797,a7),d0
	move.w	d0,-(a7)
	jsr	l512
	or.w	#32768,d0
	move.l	#14675968,a0
	move.w	d0,(150,a0)
	addq.w	#6,a7
l775
l795	reg	a2/a3
	movem.l	(a7)+,a2/a3
l797	equ	8
	add.w	#12,a7
	rts
; stacksize=32
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l798
	movem.l	l817,-(a7)
	move.l	(8+l819,a7),a0
	tst.b	(279,a0)
	bne	l802
l801
	move.l	(8+l819,a7),-(a7)
	move.l	(8+l819,a7),-(a7)
	jsr	l774
	addq.w	#8,a7
l802
	move.l	(8+l819,a7),a0
	tst.b	(279,a0)
	bne	l804
l803
	bra	l799
l804
	move.l	(8+l819,a7),a0
	add.w	#256,a0
	tst.b	(7,a0)
	beq	l806
l807
	move.l	(8+l819,a7),a0
	add.w	#256,a0
	tst.b	(6,a0)
	bls	l806
l805
	move.l	(8+l819,a7),a0
	move.l	#14675968,a1
	add.w	#160,a1
	move.l	(8+l819,a7),a2
	moveq	#0,d0
	move.b	(276,a2),d0
	lsl.l	#4,d0
	move.w	(264,a0),(6,a1,d0.l)
	move.l	(8+l819,a7),a0
	add.w	#256,a0
	move.l	#14675968,a1
	add.w	#160,a1
	move.l	(8+l819,a7),a2
	moveq	#0,d0
	move.b	(276,a2),d0
	lsl.l	#4,d0
	add.l	d0,a1
	moveq	#0,d0
	move.b	(6,a0),d0
	move.w	d0,(8,a1)
	move.l	(8+l819,a7),a0
	add.w	#256,a0
	cmp.w	#1,(4,a0)
	bls	l809
l808
	move.l	(8+l819,a7),a0
	move.l	(8+l819,a7),a1
	move.w	(264,a0),d0
	add.w	(266,a1),d0
	move.l	(8+l819,a7),a0
	move.w	d0,(264,a0)
	move.l	(8+l819,a7),a0
	move.l	(8+l819,a7),a1
	move.w	(274,a0),d0
	add.w	(272,a1),d0
	move.l	(8+l819,a7),a0
	move.w	d0,(274,a0)
	move.l	(8+l819,a7),a0
	move.l	(8+l819,a7),a1
	move.w	(274,a0),d0
	cmp.w	(270,a1),d0
	bcs	l811
l810
	move.l	(8+l819,a7),a0
	move.l	(8+l819,a7),a1
	move.w	(274,a0),d0
	sub.w	(270,a1),d0
	move.l	(8+l819,a7),a0
	move.w	d0,(274,a0)
	move.l	(8+l819,a7),a0
	move.l	(8+l819,a7),a1
	move.w	(264,a0),d0
	add.w	(268,a1),d0
	move.l	(8+l819,a7),a0
	move.w	d0,(264,a0)
l811
l809
l806
	move.l	(8+l819,a7),a0
	add.w	#280,a0
	addq.w	#1,(a0)
	move.l	(8+l819,a7),a0
	move.l	(8+l819,a7),a1
	add.w	#256,a1
	move.w	(280,a0),d0
	cmp.w	(4,a1),d0
	bcs	l813
l812
	move.l	(8+l819,a7),a0
	move.b	#0,(279,a0)
	move.l	(8+l819,a7),a0
	tst.b	(278,a0)
	bne	l815
l814
	move.l	(8+l819,a7),a0
	moveq	#0,d0
	move.b	(276,a0),d0
	move.w	d0,-(a7)
	jsr	l567
	addq.w	#2,a7
	bra	l816
l815
	move.l	(8+l819,a7),-(a7)
	move.l	(8+l819,a7),-(a7)
	jsr	l774
	addq.w	#8,a7
l816
l813
l799
l817	reg	a2
	movem.l	(a7)+,a2
l819	equ	4
	rts
; stacksize=48
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l820
	movem.l	l826,-(a7)
	tst.l	(4+l828,a7)
	beq	l823
l825
	move.l	(4+l828,a7),a0
	tst.b	(568,a0)
	bne	l824
l823
	bra	l821
l824
	move.l	(4+l828,a7),a0
	pea	(4,a0)
	move.l	(8+l828,a7),-(a7)
	jsr	l798
	move.l	(12+l828,a7),a0
	addq.w	#4,a0
	pea	(282,a0)
	move.l	(16+l828,a7),-(a7)
	jsr	l798
	add.w	#16,a7
l821
l826	reg
l828	equ	0
	rts
; stacksize=68
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l829
	subq.w	#8,a7
	movem.l	l839,-(a7)
	move.l	#1831565813,(0+l841,a7)
	tst.l	(12+l841,a7)
	bne	l834
l833
	bra	l830
l834
	moveq	#8,d2
	lea	l832,a1
	move.l	(12+l841,a7),a0
	inline
	move.l	a0,d0
	cmp.l	#16,d2
	blo	.l5
	moveq	#1,d1
	and.b	d0,d1
	beq	.l1
	move.b	(a1)+,(a0)+
	subq.l	#1,d2
.l1
	move.l	a1,d1
	and.b	#1,d1
	beq	.l3
	cmp.l	#$10000,d2
	blo	.l5
.l2
	move.b	(a1)+,(a0)+
	subq.l	#1,d2
	bne	.l2
	bra	.l7
.l3
	moveq	#3,d1
	and.l	d2,d1
	sub.l	d1,d2
.l4
	move.l	(a1)+,(a0)+
	subq.l	#4,d2
	bne	.l4
	move.w	d1,d2
.l5
	subq.w	#1,d2
	blo	.l7
.l6
	move.b	(a1)+,(a0)+
	dbf	d2,.l6
.l7
	einline
	move.w	#0,(4+l841,a7)
	bra	l836
l835
	move.l	#1664525,d0
	move.l	(0+l841,a7),d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	move.l	#1013904223,d1
	add.l	d0,d1
	move.l	d1,(0+l841,a7)
	moveq	#24,d1
	move.l	(0+l841,a7),d0
	lsr.l	d1,d0
	moveq	#8,d1
	add.w	(4+l841,a7),d1
	move.l	(12+l841,a7),a0
	move.b	d0,(0,a0,d1.w)
l838
	addq.w	#1,(4+l841,a7)
l836
	cmp.w	#64,(4+l841,a7)
	blt	l835
l837
l830
l839	reg	d2/d3
	movem.l	(a7)+,d2/d3
l841	equ	8
	addq.w	#8,a7
	rts
	cnop	0,4
l832
	dc.b	88
	dc.b	104
	dc.b	104
	dc.b	88
	dc.b	168
	dc.b	152
	dc.b	152
	dc.b	168
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l842
	movem.l	l847,-(a7)
	move.l	#570,d2
	moveq	#0,d0
	move.l	(4+l849,a7),a0
	inline
	move.l	a0,a1
	cmp.l	#16,d2
	blo	.l3
	move.l	a0,d1
	and.b	#1,d1
	beq	.l1
	move.b	d0,(a0)+
	subq.l	#1,d2
.l1
	move.b	d0,d1
	lsl.w	#8,d0
	move.b	d1,d0
	move.w	d0,d1
	swap	d0
	move.w	d1,d0
	moveq	#3,d1
	and.l	d2,d1
	sub.l	d1,d2
.l2
	move.l	d0,(a0)+
	subq.l	#4,d2
	bne	.l2
	move.w	d1,d2
.l3
	subq.w	#1,d2
	blo	.l5
.l4
	move.b	d0,(a0)+
	dbf	d2,.l4
.l5
	move.l	a1,d0
	einline
	move.l	d0,a1
	move.l	#65538,d1
	moveq	#72,d0
	move.l	_SysBase,a6
	jsr	-198(a6)
	move.l	d0,a0
	move.l	(4+l849,a7),a1
	move.l	a0,(a1)
	move.l	(4+l849,a7),a0
	tst.l	(a0)
	bne	l846
l845
	moveq	#0,d0
	bra	l843
l846
	move.l	(4+l849,a7),a0
	move.l	(a0),-(a7)
	jsr	l829
	move.l	(8+l849,a7),a0
	addq.w	#4,a0
	move.b	#0,(276,a0)
	move.l	(8+l849,a7),a0
	addq.w	#4,a0
	add.l	#282,a0
	move.b	#1,(276,a0)
	move.l	(8+l849,a7),a0
	move.b	#1,(568,a0)
	move.w	#0,-(a7)
	jsr	l567
	move.w	#1,-(a7)
	jsr	l567
	moveq	#1,d0
	addq.w	#8,a7
l843
l847	reg	a6/d2
	movem.l	(a7)+,a6/d2
l849	equ	8
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l850
	movem.l	l857,-(a7)
	tst.l	(4+l859,a7)
	bne	l854
l853
	bra	l851
l854
	move.w	#0,-(a7)
	jsr	l567
	move.w	#1,-(a7)
	jsr	l567
	addq.w	#4,a7
	move.l	(4+l859,a7),a0
	tst.l	(a0)
	beq	l856
	moveq	#72,d0
	move.l	(4+l859,a7),a0
	move.l	(a0),a1
	move.l	_SysBase,a6
	jsr	-210(a6)
l856
	move.l	#570,d2
	moveq	#0,d0
	move.l	(4+l859,a7),a0
	inline
	move.l	a0,a1
	cmp.l	#16,d2
	blo	.l3
	move.l	a0,d1
	and.b	#1,d1
	beq	.l1
	move.b	d0,(a0)+
	subq.l	#1,d2
.l1
	move.b	d0,d1
	lsl.w	#8,d0
	move.b	d1,d0
	move.w	d0,d1
	swap	d0
	move.w	d1,d0
	moveq	#3,d1
	and.l	d2,d1
	sub.l	d1,d2
.l2
	move.l	d0,(a0)+
	subq.l	#4,d2
	bne	.l2
	move.w	d1,d2
.l3
	subq.w	#1,d2
	blo	.l5
.l4
	move.b	d0,(a0)+
	dbf	d2,.l4
.l5
	move.l	a1,d0
	einline
	move.l	d0,a1
l851
l857	reg	a6/d2
	movem.l	(a7)+,a6/d2
l859	equ	8
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l860
	sub.w	#12,a7
	movem.l	l878,-(a7)
	moveq	#7,d0
	and.w	(20+l880,a7),d0
	move.w	#128,d1
	lsr.w	d0,d1
	move.w	d1,d0
	move.b	d0,(0+l880,a7)
	moveq	#0,d0
	move.b	(0+l880,a7),d0
	not.w	d0
	move.b	d0,(1+l880,a7)
	move.w	(22+l880,a7),d0
	ext.l	d0
	move.l	#200,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	move.w	(20+l880,a7),d1
	asr.w	#3,d1
	ext.l	d1
	add.l	d1,d0
	move.l	d0,(6+l880,a7)
	move.l	(16+l880,a7),a0
	add.l	(6+l880,a7),a0
	move.l	a0,(2+l880,a7)
	moveq	#0,d0
	move.b	(25+l880,a7),d0
	and.w	#1,d0
	beq	l864
	move.l	(2+l880,a7),a0
	move.b	(0+l880,a7),d0
	or.b	d0,(a0)
	bra	l865
l864
	move.l	(2+l880,a7),a0
	move.b	(1+l880,a7),d0
	and.b	d0,(a0)
l865
	moveq	#0,d0
	move.b	(25+l880,a7),d0
	and.w	#2,d0
	beq	l867
	move.l	(2+l880,a7),a0
	move.b	(40,a0),d0
	or.b	(0+l880,a7),d0
	move.l	(2+l880,a7),a0
	move.b	d0,(40,a0)
	bra	l868
l867
	move.l	(2+l880,a7),a0
	move.b	(40,a0),d0
	and.b	(1+l880,a7),d0
	move.l	(2+l880,a7),a0
	move.b	d0,(40,a0)
l868
	moveq	#0,d0
	move.b	(25+l880,a7),d0
	and.w	#4,d0
	beq	l870
	move.l	(2+l880,a7),a0
	move.b	(80,a0),d0
	or.b	(0+l880,a7),d0
	move.l	(2+l880,a7),a0
	move.b	d0,(80,a0)
	bra	l871
l870
	move.l	(2+l880,a7),a0
	move.b	(80,a0),d0
	and.b	(1+l880,a7),d0
	move.l	(2+l880,a7),a0
	move.b	d0,(80,a0)
l871
	moveq	#0,d0
	move.b	(25+l880,a7),d0
	and.w	#8,d0
	beq	l873
	move.l	(2+l880,a7),a0
	move.b	(120,a0),d0
	or.b	(0+l880,a7),d0
	move.l	(2+l880,a7),a0
	move.b	d0,(120,a0)
	bra	l874
l873
	move.l	(2+l880,a7),a0
	move.b	(120,a0),d0
	and.b	(1+l880,a7),d0
	move.l	(2+l880,a7),a0
	move.b	d0,(120,a0)
l874
	moveq	#0,d0
	move.b	(25+l880,a7),d0
	and.w	#16,d0
	beq	l876
	move.l	(2+l880,a7),a0
	move.b	(160,a0),d0
	or.b	(0+l880,a7),d0
	move.l	(2+l880,a7),a0
	move.b	d0,(160,a0)
	bra	l877
l876
	move.l	(2+l880,a7),a0
	move.b	(160,a0),d0
	and.b	(1+l880,a7),d0
	move.l	(2+l880,a7),a0
	move.b	d0,(160,a0)
l877
l861
l878	reg	d2/d3
	movem.l	(a7)+,d2/d3
l880	equ	8
	add.w	#12,a7
	rts
; stacksize=20
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l881
	movem.l	l889,-(a7)
	tst.w	(8+l891,a7)
	blt	l884
l888
	tst.w	(10+l891,a7)
	blt	l884
l887
	cmp.w	#320,(8+l891,a7)
	bge	l884
l886
	cmp.w	#200,(10+l891,a7)
	blt	l885
l884
	bra	l882
l885
	moveq	#0,d0
	move.b	(13+l891,a7),d0
	move.w	d0,-(a7)
	move.w	(12+l891,a7),-(a7)
	move.w	(12+l891,a7),-(a7)
	move.l	(10+l891,a7),-(a7)
	jsr	l860
	add.w	#10,a7
l882
l889	reg
l891	equ	0
	rts
; stacksize=34
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l892
	sub.w	#16,a7
	movem.l	l917,-(a7)
	tst.w	(28+l919,a7)
	blt	l895
l897
	cmp.w	#200,(28+l919,a7)
	blt	l896
l895
	bra	l893
l896
	move.w	#319,-(a7)
	move.w	#0,-(a7)
	move.w	(28+l919,a7),-(a7)
	jsr	l15
	move.w	d0,(30+l919,a7)
	move.w	#319,-(a7)
	move.w	#0,-(a7)
	move.w	(36+l919,a7),-(a7)
	jsr	l15
	move.w	d0,(38+l919,a7)
	add.w	#12,a7
	move.w	(26+l919,a7),d0
	cmp.w	(24+l919,a7),d0
	bge	l899
l898
	bra	l893
l899
	move.w	(24+l919,a7),d0
	asr.w	#3,d0
	move.w	d0,(2+l919,a7)
	move.w	(26+l919,a7),d0
	asr.w	#3,d0
	move.w	d0,(4+l919,a7)
	moveq	#7,d0
	and.w	(24+l919,a7),d0
	move.w	#255,d1
	lsr.w	d0,d1
	move.w	d1,d0
	move.b	d0,(6+l919,a7)
	moveq	#7,d0
	and.w	(26+l919,a7),d0
	neg.w	d0
	addq.w	#7,d0
	move.b	#255,d1
	lsl.b	d0,d1
	move.b	d1,(7+l919,a7)
	move.w	#0,(0+l919,a7)
	bra	l901
l900
	move.w	(28+l919,a7),d0
	ext.l	d0
	move.l	#200,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	move.l	(20+l919,a7),a0
	add.l	d0,a0
	move.w	(0+l919,a7),d0
	ext.l	d0
	moveq	#40,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	add.l	d0,a0
	move.l	a0,(10+l919,a7)
	moveq	#0,d0
	move.b	(31+l919,a7),d0
	move.w	(0+l919,a7),d1
	asr.w	d1,d0
	and.w	#1,d0
	beq	l905
l904
	move.w	#255,d0
	bra	l906
l905
	moveq	#0,d0
l906
	move.b	d0,(14+l919,a7)
	move.w	(2+l919,a7),d0
	cmp.w	(4+l919,a7),d0
	bne	l908
l907
	move.b	(6+l919,a7),d0
	and.b	(7+l919,a7),d0
	move.b	d0,(8+l919,a7)
	tst.b	(14+l919,a7)
	beq	l910
	move.l	(10+l919,a7),a0
	add.w	(2+l919,a7),a0
	move.b	(a0),d0
	or.b	(8+l919,a7),d0
	move.l	(10+l919,a7),a0
	add.w	(2+l919,a7),a0
	move.b	d0,(a0)
	bra	l911
l910
	move.l	(10+l919,a7),a0
	add.w	(2+l919,a7),a0
	moveq	#0,d0
	move.b	(8+l919,a7),d0
	not.w	d0
	and.b	(a0),d0
	move.l	(10+l919,a7),a0
	add.w	(2+l919,a7),a0
	move.b	d0,(a0)
l911
	bra	l903
l908
	tst.b	(14+l919,a7)
	beq	l913
	move.l	(10+l919,a7),a0
	add.w	(2+l919,a7),a0
	move.b	(a0),d0
	or.b	(6+l919,a7),d0
	move.l	(10+l919,a7),a0
	add.w	(2+l919,a7),a0
	move.b	d0,(a0)
	move.l	(10+l919,a7),a0
	add.w	(4+l919,a7),a0
	move.b	(a0),d0
	or.b	(7+l919,a7),d0
	move.l	(10+l919,a7),a0
	add.w	(4+l919,a7),a0
	move.b	d0,(a0)
	bra	l914
l913
	move.l	(10+l919,a7),a0
	add.w	(2+l919,a7),a0
	moveq	#0,d0
	move.b	(6+l919,a7),d0
	not.w	d0
	and.b	(a0),d0
	move.l	(10+l919,a7),a0
	add.w	(2+l919,a7),a0
	move.b	d0,(a0)
	move.l	(10+l919,a7),a0
	add.w	(4+l919,a7),a0
	moveq	#0,d0
	move.b	(7+l919,a7),d0
	not.w	d0
	and.b	(a0),d0
	move.l	(10+l919,a7),a0
	add.w	(4+l919,a7),a0
	move.b	d0,(a0)
l914
	moveq	#1,d0
	add.w	(2+l919,a7),d0
	cmp.w	(4+l919,a7),d0
	bge	l916
l915
	move.w	(4+l919,a7),d0
	sub.w	(2+l919,a7),d0
	subq.w	#1,d0
	ext.l	d0
	move.l	d0,d2
	moveq	#0,d0
	move.b	(14+l919,a7),d0
	move.l	(10+l919,a7),a0
	add.w	(2+l919,a7),a0
	addq.l	#1,a0
	inline
	move.l	a0,a1
	cmp.l	#16,d2
	blo	.l3
	move.l	a0,d1
	and.b	#1,d1
	beq	.l1
	move.b	d0,(a0)+
	subq.l	#1,d2
.l1
	move.b	d0,d1
	lsl.w	#8,d0
	move.b	d1,d0
	move.w	d0,d1
	swap	d0
	move.w	d1,d0
	moveq	#3,d1
	and.l	d2,d1
	sub.l	d1,d2
.l2
	move.l	d0,(a0)+
	subq.l	#4,d2
	bne	.l2
	move.w	d1,d2
.l3
	subq.w	#1,d2
	blo	.l5
.l4
	move.b	d0,(a0)+
	dbf	d2,.l4
.l5
	move.l	a1,d0
	einline
	move.l	d0,a1
l916
l903
	addq.w	#1,(0+l919,a7)
l901
	cmp.w	#5,(0+l919,a7)
	blt	l900
l902
l893
l917	reg	d2/d3
	movem.l	(a7)+,d2/d3
l919	equ	8
	add.w	#16,a7
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l920
	subq.w	#4,a7
	movem.l	l930,-(a7)
	move.w	#319,-(a7)
	move.w	#0,-(a7)
	move.w	(16+l932,a7),-(a7)
	jsr	l15
	move.w	d0,(18+l932,a7)
	move.w	#199,-(a7)
	move.w	#0,-(a7)
	move.w	(24+l932,a7),-(a7)
	jsr	l15
	move.w	d0,(26+l932,a7)
	move.w	#319,-(a7)
	move.w	#0,-(a7)
	move.w	(32+l932,a7),-(a7)
	jsr	l15
	move.w	d0,(34+l932,a7)
	move.w	#199,-(a7)
	move.w	#0,-(a7)
	move.w	(40+l932,a7),-(a7)
	jsr	l15
	move.w	d0,(42+l932,a7)
	add.w	#24,a7
	move.w	(16+l932,a7),d0
	cmp.w	(12+l932,a7),d0
	blt	l923
l925
	move.w	(18+l932,a7),d0
	cmp.w	(14+l932,a7),d0
	bge	l924
l923
	bra	l921
l924
	move.w	(14+l932,a7),(0+l932,a7)
	bra	l927
l926
	moveq	#0,d0
	move.b	(21+l932,a7),d0
	move.w	d0,-(a7)
	move.w	(2+l932,a7),-(a7)
	move.w	(20+l932,a7),-(a7)
	move.w	(18+l932,a7),-(a7)
	move.l	(16+l932,a7),-(a7)
	jsr	l892
	add.w	#12,a7
l929
	addq.w	#1,(0+l932,a7)
l927
	move.w	(0+l932,a7),d0
	cmp.w	(18+l932,a7),d0
	ble	l926
l928
l921
l930	reg
l932	equ	0
	addq.w	#4,a7
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l933
	subq.w	#8,a7
	movem.l	l949,-(a7)
	tst.w	(20+l951,a7)
	blt	l936
l938
	cmp.w	#9,(20+l951,a7)
	ble	l937
l936
	bra	l934
l937
	move.w	#0,(0+l951,a7)
	bra	l940
l939
	move.w	(20+l951,a7),d0
	ext.l	d0
	moveq	#5,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	lea	l14,a0
	add.l	d0,a0
	add.w	(0+l951,a7),a0
	move.b	(a0),(4+l951,a7)
	move.w	#0,(2+l951,a7)
	bra	l944
l943
	moveq	#2,d0
	sub.w	(2+l951,a7),d0
	moveq	#0,d1
	move.b	(4+l951,a7),d1
	asr.w	d0,d1
	and.w	#1,d1
	beq	l948
	moveq	#0,d0
	move.b	(23+l951,a7),d0
	move.w	d0,-(a7)
	move.w	(20+l951,a7),d0
	add.w	(2+l951,a7),d0
	move.w	d0,-(a7)
	move.w	(20+l951,a7),d0
	add.w	(6+l951,a7),d0
	move.w	d0,-(a7)
	move.l	(18+l951,a7),-(a7)
	jsr	l881
	add.w	#10,a7
l948
l946
	addq.w	#1,(2+l951,a7)
l944
	cmp.w	#3,(2+l951,a7)
	blt	l943
l945
l942
	addq.w	#1,(0+l951,a7)
l940
	cmp.w	#5,(0+l951,a7)
	blt	l939
l941
l934
l949	reg	d2/d3
	movem.l	(a7)+,d2/d3
l951	equ	8
	addq.w	#8,a7
	rts
; stacksize=64
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l952
	movem.l	l990,-(a7)
	move.b	(5+l992,a7),d0
	cmp.b	#43,d0
	beq	l984
	cmp.b	#45,d0
	beq	l983
	cmp.b	#46,d0
	beq	l986
	cmp.b	#47,d0
	beq	l985
	move.b	d0,d1
	ext.w	d1
	sub.w	#58,d1
	cmp.w	#32,d1
	bhi	l989
	lsl.w	#2,d1
	move.l	l993(pc,d1.w),a0
	jmp	(a0)
	cnop	0,4
l993
	dc.l	l982
	dc.l	l989
	dc.l	l987
	dc.l	l989
	dc.l	l988
	dc.l	l989
	dc.l	l989
	dc.l	l956
	dc.l	l957
	dc.l	l958
	dc.l	l959
	dc.l	l960
	dc.l	l961
	dc.l	l962
	dc.l	l963
	dc.l	l964
	dc.l	l965
	dc.l	l966
	dc.l	l967
	dc.l	l968
	dc.l	l969
	dc.l	l970
	dc.l	l971
	dc.l	l972
	dc.l	l973
	dc.l	l974
	dc.l	l975
	dc.l	l976
	dc.l	l977
	dc.l	l978
	dc.l	l979
	dc.l	l980
	dc.l	l981
l956
	move.l	(6+l992,a7),a0
	move.b	#7,(a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(1,a0)
	move.l	(6+l992,a7),a0
	move.b	#7,(2,a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(3,a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(4,a0)
	moveq	#1,d0
	bra	l953
l957
	move.l	(6+l992,a7),a0
	move.b	#6,(a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(1,a0)
	move.l	(6+l992,a7),a0
	move.b	#6,(2,a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(3,a0)
	move.l	(6+l992,a7),a0
	move.b	#6,(4,a0)
	moveq	#1,d0
	bra	l953
l958
	move.l	(6+l992,a7),a0
	move.b	#7,(a0)
	move.l	(6+l992,a7),a0
	move.b	#4,(1,a0)
	move.l	(6+l992,a7),a0
	move.b	#4,(2,a0)
	move.l	(6+l992,a7),a0
	move.b	#4,(3,a0)
	move.l	(6+l992,a7),a0
	move.b	#7,(4,a0)
	moveq	#1,d0
	bra	l953
l959
	move.l	(6+l992,a7),a0
	move.b	#6,(a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(1,a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(2,a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(3,a0)
	move.l	(6+l992,a7),a0
	move.b	#6,(4,a0)
	moveq	#1,d0
	bra	l953
l960
	move.l	(6+l992,a7),a0
	move.b	#7,(a0)
	move.l	(6+l992,a7),a0
	move.b	#4,(1,a0)
	move.l	(6+l992,a7),a0
	move.b	#6,(2,a0)
	move.l	(6+l992,a7),a0
	move.b	#4,(3,a0)
	move.l	(6+l992,a7),a0
	move.b	#7,(4,a0)
	moveq	#1,d0
	bra	l953
l961
	move.l	(6+l992,a7),a0
	move.b	#7,(a0)
	move.l	(6+l992,a7),a0
	move.b	#4,(1,a0)
	move.l	(6+l992,a7),a0
	move.b	#6,(2,a0)
	move.l	(6+l992,a7),a0
	move.b	#4,(3,a0)
	move.l	(6+l992,a7),a0
	move.b	#4,(4,a0)
	moveq	#1,d0
	bra	l953
l962
	move.l	(6+l992,a7),a0
	move.b	#7,(a0)
	move.l	(6+l992,a7),a0
	move.b	#4,(1,a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(2,a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(3,a0)
	move.l	(6+l992,a7),a0
	move.b	#7,(4,a0)
	moveq	#1,d0
	bra	l953
l963
	move.l	(6+l992,a7),a0
	move.b	#5,(a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(1,a0)
	move.l	(6+l992,a7),a0
	move.b	#7,(2,a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(3,a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(4,a0)
	moveq	#1,d0
	bra	l953
l964
	move.l	(6+l992,a7),a0
	move.b	#7,(a0)
	move.l	(6+l992,a7),a0
	move.b	#2,(1,a0)
	move.l	(6+l992,a7),a0
	move.b	#2,(2,a0)
	move.l	(6+l992,a7),a0
	move.b	#2,(3,a0)
	move.l	(6+l992,a7),a0
	move.b	#7,(4,a0)
	moveq	#1,d0
	bra	l953
l965
	move.l	(6+l992,a7),a0
	move.b	#1,(a0)
	move.l	(6+l992,a7),a0
	move.b	#1,(1,a0)
	move.l	(6+l992,a7),a0
	move.b	#1,(2,a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(3,a0)
	move.l	(6+l992,a7),a0
	move.b	#7,(4,a0)
	moveq	#1,d0
	bra	l953
l966
	move.l	(6+l992,a7),a0
	move.b	#5,(a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(1,a0)
	move.l	(6+l992,a7),a0
	move.b	#6,(2,a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(3,a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(4,a0)
	moveq	#1,d0
	bra	l953
l967
	move.l	(6+l992,a7),a0
	move.b	#4,(a0)
	move.l	(6+l992,a7),a0
	move.b	#4,(1,a0)
	move.l	(6+l992,a7),a0
	move.b	#4,(2,a0)
	move.l	(6+l992,a7),a0
	move.b	#4,(3,a0)
	move.l	(6+l992,a7),a0
	move.b	#7,(4,a0)
	moveq	#1,d0
	bra	l953
l968
	move.l	(6+l992,a7),a0
	move.b	#5,(a0)
	move.l	(6+l992,a7),a0
	move.b	#7,(1,a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(2,a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(3,a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(4,a0)
	moveq	#1,d0
	bra	l953
l969
	move.l	(6+l992,a7),a0
	move.b	#5,(a0)
	move.l	(6+l992,a7),a0
	move.b	#7,(1,a0)
	move.l	(6+l992,a7),a0
	move.b	#7,(2,a0)
	move.l	(6+l992,a7),a0
	move.b	#7,(3,a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(4,a0)
	moveq	#1,d0
	bra	l953
l970
	move.l	(6+l992,a7),a0
	move.b	#7,(a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(1,a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(2,a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(3,a0)
	move.l	(6+l992,a7),a0
	move.b	#7,(4,a0)
	moveq	#1,d0
	bra	l953
l971
	move.l	(6+l992,a7),a0
	move.b	#6,(a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(1,a0)
	move.l	(6+l992,a7),a0
	move.b	#6,(2,a0)
	move.l	(6+l992,a7),a0
	move.b	#4,(3,a0)
	move.l	(6+l992,a7),a0
	move.b	#4,(4,a0)
	moveq	#1,d0
	bra	l953
l972
	move.l	(6+l992,a7),a0
	move.b	#7,(a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(1,a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(2,a0)
	move.l	(6+l992,a7),a0
	move.b	#7,(3,a0)
	move.l	(6+l992,a7),a0
	move.b	#1,(4,a0)
	moveq	#1,d0
	bra	l953
l973
	move.l	(6+l992,a7),a0
	move.b	#6,(a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(1,a0)
	move.l	(6+l992,a7),a0
	move.b	#6,(2,a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(3,a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(4,a0)
	moveq	#1,d0
	bra	l953
l974
	move.l	(6+l992,a7),a0
	move.b	#7,(a0)
	move.l	(6+l992,a7),a0
	move.b	#4,(1,a0)
	move.l	(6+l992,a7),a0
	move.b	#7,(2,a0)
	move.l	(6+l992,a7),a0
	move.b	#1,(3,a0)
	move.l	(6+l992,a7),a0
	move.b	#7,(4,a0)
	moveq	#1,d0
	bra	l953
l975
	move.l	(6+l992,a7),a0
	move.b	#7,(a0)
	move.l	(6+l992,a7),a0
	move.b	#2,(1,a0)
	move.l	(6+l992,a7),a0
	move.b	#2,(2,a0)
	move.l	(6+l992,a7),a0
	move.b	#2,(3,a0)
	move.l	(6+l992,a7),a0
	move.b	#2,(4,a0)
	moveq	#1,d0
	bra	l953
l976
	move.l	(6+l992,a7),a0
	move.b	#5,(a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(1,a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(2,a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(3,a0)
	move.l	(6+l992,a7),a0
	move.b	#7,(4,a0)
	moveq	#1,d0
	bra	l953
l977
	move.l	(6+l992,a7),a0
	move.b	#5,(a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(1,a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(2,a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(3,a0)
	move.l	(6+l992,a7),a0
	move.b	#2,(4,a0)
	moveq	#1,d0
	bra	l953
l978
	move.l	(6+l992,a7),a0
	move.b	#5,(a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(1,a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(2,a0)
	move.l	(6+l992,a7),a0
	move.b	#7,(3,a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(4,a0)
	moveq	#1,d0
	bra	l953
l979
	move.l	(6+l992,a7),a0
	move.b	#5,(a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(1,a0)
	move.l	(6+l992,a7),a0
	move.b	#2,(2,a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(3,a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(4,a0)
	moveq	#1,d0
	bra	l953
l980
	move.l	(6+l992,a7),a0
	move.b	#5,(a0)
	move.l	(6+l992,a7),a0
	move.b	#5,(1,a0)
	move.l	(6+l992,a7),a0
	move.b	#2,(2,a0)
	move.l	(6+l992,a7),a0
	move.b	#2,(3,a0)
	move.l	(6+l992,a7),a0
	move.b	#2,(4,a0)
	moveq	#1,d0
	bra	l953
l981
	move.l	(6+l992,a7),a0
	move.b	#7,(a0)
	move.l	(6+l992,a7),a0
	move.b	#1,(1,a0)
	move.l	(6+l992,a7),a0
	move.b	#2,(2,a0)
	move.l	(6+l992,a7),a0
	move.b	#4,(3,a0)
	move.l	(6+l992,a7),a0
	move.b	#7,(4,a0)
	moveq	#1,d0
	bra	l953
l982
	move.l	(6+l992,a7),a0
	move.b	#0,(a0)
	move.l	(6+l992,a7),a0
	move.b	#2,(1,a0)
	move.l	(6+l992,a7),a0
	move.b	#0,(2,a0)
	move.l	(6+l992,a7),a0
	move.b	#2,(3,a0)
	move.l	(6+l992,a7),a0
	move.b	#0,(4,a0)
	moveq	#1,d0
	bra	l953
l983
	move.l	(6+l992,a7),a0
	move.b	#0,(a0)
	move.l	(6+l992,a7),a0
	move.b	#0,(1,a0)
	move.l	(6+l992,a7),a0
	move.b	#7,(2,a0)
	move.l	(6+l992,a7),a0
	move.b	#0,(3,a0)
	move.l	(6+l992,a7),a0
	move.b	#0,(4,a0)
	moveq	#1,d0
	bra	l953
l984
	move.l	(6+l992,a7),a0
	move.b	#0,(a0)
	move.l	(6+l992,a7),a0
	move.b	#2,(1,a0)
	move.l	(6+l992,a7),a0
	move.b	#7,(2,a0)
	move.l	(6+l992,a7),a0
	move.b	#2,(3,a0)
	move.l	(6+l992,a7),a0
	move.b	#0,(4,a0)
	moveq	#1,d0
	bra	l953
l985
	move.l	(6+l992,a7),a0
	move.b	#1,(a0)
	move.l	(6+l992,a7),a0
	move.b	#1,(1,a0)
	move.l	(6+l992,a7),a0
	move.b	#2,(2,a0)
	move.l	(6+l992,a7),a0
	move.b	#4,(3,a0)
	move.l	(6+l992,a7),a0
	move.b	#4,(4,a0)
	moveq	#1,d0
	bra	l953
l986
	move.l	(6+l992,a7),a0
	move.b	#0,(a0)
	move.l	(6+l992,a7),a0
	move.b	#0,(1,a0)
	move.l	(6+l992,a7),a0
	move.b	#0,(2,a0)
	move.l	(6+l992,a7),a0
	move.b	#0,(3,a0)
	move.l	(6+l992,a7),a0
	move.b	#2,(4,a0)
	moveq	#1,d0
	bra	l953
l987
	move.l	(6+l992,a7),a0
	move.b	#1,(a0)
	move.l	(6+l992,a7),a0
	move.b	#2,(1,a0)
	move.l	(6+l992,a7),a0
	move.b	#4,(2,a0)
	move.l	(6+l992,a7),a0
	move.b	#2,(3,a0)
	move.l	(6+l992,a7),a0
	move.b	#1,(4,a0)
	moveq	#1,d0
	bra	l953
l988
	move.l	(6+l992,a7),a0
	move.b	#4,(a0)
	move.l	(6+l992,a7),a0
	move.b	#2,(1,a0)
	move.l	(6+l992,a7),a0
	move.b	#1,(2,a0)
	move.l	(6+l992,a7),a0
	move.b	#2,(3,a0)
	move.l	(6+l992,a7),a0
	move.b	#4,(4,a0)
	moveq	#1,d0
	bra	l953
l989
	moveq	#0,d0
l955
l953
l990	reg
l992	equ	0
	rts
; stacksize=0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l994
	sub.w	#12,a7
	movem.l	l1017,-(a7)
	cmp.b	#48,(25+l1019,a7)
	blt	l998
l999
	cmp.b	#57,(25+l1019,a7)
	bgt	l998
l997
	moveq	#0,d0
	move.b	(27+l1019,a7),d0
	move.w	d0,-(a7)
	move.b	(27+l1019,a7),d0
	ext.w	d0
	sub.w	#48,d0
	move.w	d0,-(a7)
	move.w	(26+l1019,a7),-(a7)
	move.w	(26+l1019,a7),-(a7)
	move.l	(24+l1019,a7),-(a7)
	jsr	l933
	add.w	#12,a7
	bra	l995
l998
	cmp.b	#97,(25+l1019,a7)
	blt	l1001
l1002
	cmp.b	#122,(25+l1019,a7)
	bgt	l1001
l1000
	move.b	(25+l1019,a7),d0
	sub.b	#97,d0
	moveq	#65,d1
	add.b	d0,d1
	move.b	d1,(25+l1019,a7)
l1001
	cmp.b	#32,(25+l1019,a7)
	bne	l1004
l1003
	bra	l995
l1004
	lea	(0+l1019,a7),a0
	move.l	a0,-(a7)
	move.b	(29+l1019,a7),d0
	ext.w	d0
	move.w	d0,-(a7)
	jsr	l952
	addq.w	#6,a7
	tst.b	d0
	bne	l1006
l1005
	bra	l995
l1006
	move.w	#0,(6+l1019,a7)
	bra	l1008
l1007
	move.w	#0,(8+l1019,a7)
	bra	l1012
l1011
	lea	(0+l1019,a7),a0
	add.w	(6+l1019,a7),a0
	moveq	#2,d0
	sub.w	(8+l1019,a7),d0
	moveq	#0,d1
	move.b	(a0),d1
	asr.w	d0,d1
	and.w	#1,d1
	beq	l1016
	moveq	#0,d0
	move.b	(27+l1019,a7),d0
	move.w	d0,-(a7)
	move.w	(24+l1019,a7),d0
	add.w	(8+l1019,a7),d0
	move.w	d0,-(a7)
	move.w	(24+l1019,a7),d0
	add.w	(12+l1019,a7),d0
	move.w	d0,-(a7)
	move.l	(22+l1019,a7),-(a7)
	jsr	l881
	add.w	#10,a7
l1016
l1014
	addq.w	#1,(8+l1019,a7)
l1012
	cmp.w	#3,(8+l1019,a7)
	blt	l1011
l1013
l1010
	addq.w	#1,(6+l1019,a7)
l1008
	cmp.w	#5,(6+l1019,a7)
	blt	l1007
l1009
l995
l1017	reg
l1019	equ	0
	add.w	#12,a7
	rts
; stacksize=92
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1020
	movem.l	l1026,-(a7)
	bra	l1024
l1023
	moveq	#0,d0
	move.b	(17+l1028,a7),d0
	move.w	d0,-(a7)
	move.l	(14+l1028,a7),a0
	move.b	(a0),d0
	ext.w	d0
	move.w	d0,-(a7)
	move.w	(14+l1028,a7),-(a7)
	move.w	(14+l1028,a7),-(a7)
	move.l	(12+l1028,a7),-(a7)
	jsr	l994
	addq.w	#4,(20+l1028,a7)
	addq.l	#1,(24+l1028,a7)
	add.w	#12,a7
l1024
	move.l	(12+l1028,a7),a0
	tst.b	(a0)
	bne	l1023
l1025
l1021
l1026	reg
l1028	equ	0
	rts
; stacksize=108
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1029
	subq.w	#4,a7
	movem.l	l1038,-(a7)
	move.w	#0,(0+l1040,a7)
	bra	l1033
l1032
	addq.w	#1,(0+l1040,a7)
l1033
	move.l	(8+l1040,a7),a0
	add.w	(0+l1040,a7),a0
	tst.b	(a0)
	bne	l1032
l1034
	tst.w	(0+l1040,a7)
	ble	l1036
l1035
	move.w	(0+l1040,a7),d0
	lsl.w	#2,d0
	subq.w	#1,d0
	bra	l1037
l1036
	moveq	#0,d0
l1037
l1030
l1038	reg
l1040	equ	0
	addq.w	#4,a7
	rts
; stacksize=4
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1041
	subq.w	#4,a7
	movem.l	l1044,-(a7)
	move.l	(18+l1046,a7),-(a7)
	jsr	l1029
	move.w	d0,(4+l1046,a7)
	move.w	(18+l1046,a7),d0
	sub.w	(16+l1046,a7),d0
	addq.w	#1,d0
	sub.w	(4+l1046,a7),d0
	tst.w	d0
	bge	l1047
	addq.w	#1,d0
l1047:
	asr.w	#1,d0
	add.w	(16+l1046,a7),d0
	move.w	d0,(6+l1046,a7)
	moveq	#0,d0
	move.b	(27+l1046,a7),d0
	move.w	d0,-(a7)
	move.l	(24+l1046,a7),-(a7)
	move.w	(26+l1046,a7),-(a7)
	move.w	(14+l1046,a7),-(a7)
	move.l	(22+l1046,a7),-(a7)
	jsr	l1020
	add.w	#18,a7
l1042
l1044	reg
l1046	equ	0
	addq.w	#4,a7
	rts
; stacksize=134
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1048
	movem.l	l1051,-(a7)
	moveq	#0,d0
	move.b	(19+l1053,a7),d0
	move.w	d0,-(a7)
	move.l	(14+l1053,a7),-(a7)
	move.w	(16+l1053,a7),-(a7)
	move.w	(16+l1053,a7),d0
	subq.w	#1,d0
	move.w	d0,-(a7)
	move.l	(14+l1053,a7),-(a7)
	jsr	l1020
	moveq	#0,d0
	move.b	(33+l1053,a7),d0
	move.w	d0,-(a7)
	move.l	(28+l1053,a7),-(a7)
	move.w	(30+l1053,a7),-(a7)
	moveq	#1,d0
	add.w	(30+l1053,a7),d0
	move.w	d0,-(a7)
	move.l	(28+l1053,a7),-(a7)
	jsr	l1020
	moveq	#0,d0
	move.b	(47+l1053,a7),d0
	move.w	d0,-(a7)
	move.l	(42+l1053,a7),-(a7)
	move.w	(44+l1053,a7),d0
	subq.w	#1,d0
	move.w	d0,-(a7)
	move.w	(44+l1053,a7),-(a7)
	move.l	(42+l1053,a7),-(a7)
	jsr	l1020
	moveq	#0,d0
	move.b	(61+l1053,a7),d0
	move.w	d0,-(a7)
	move.l	(56+l1053,a7),-(a7)
	moveq	#1,d0
	add.w	(58+l1053,a7),d0
	move.w	d0,-(a7)
	move.w	(58+l1053,a7),-(a7)
	move.l	(56+l1053,a7),-(a7)
	jsr	l1020
	moveq	#0,d0
	move.b	(73+l1053,a7),d0
	move.w	d0,-(a7)
	move.l	(70+l1053,a7),-(a7)
	move.w	(72+l1053,a7),-(a7)
	move.w	(72+l1053,a7),-(a7)
	move.l	(70+l1053,a7),-(a7)
	jsr	l1020
	add.w	#70,a7
l1049
l1051	reg
l1053	equ	0
	rts
; stacksize=182
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1054
	subq.w	#4,a7
	movem.l	l1057,-(a7)
	move.l	(18+l1059,a7),-(a7)
	jsr	l1029
	move.w	d0,(4+l1059,a7)
	move.w	(18+l1059,a7),d0
	sub.w	(16+l1059,a7),d0
	addq.w	#1,d0
	sub.w	(4+l1059,a7),d0
	tst.w	d0
	bge	l1060
	addq.w	#1,d0
l1060:
	asr.w	#1,d0
	add.w	(16+l1059,a7),d0
	move.w	d0,(6+l1059,a7)
	moveq	#0,d0
	move.b	(29+l1059,a7),d0
	move.w	d0,-(a7)
	moveq	#0,d0
	move.b	(29+l1059,a7),d0
	move.w	d0,-(a7)
	move.l	(26+l1059,a7),-(a7)
	move.w	(28+l1059,a7),-(a7)
	move.w	(16+l1059,a7),-(a7)
	move.l	(24+l1059,a7),-(a7)
	jsr	l1048
	add.w	#20,a7
l1055
l1057	reg
l1059	equ	0
	addq.w	#4,a7
	rts
; stacksize=210
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1061
	subq.w	#4,a7
	movem.l	l1070,-(a7)
	move.w	#0,(0+l1072,a7)
	move.w	#4,-(a7)
	move.w	#1,-(a7)
	move.w	(16+l1072,a7),-(a7)
	jsr	l15
	move.w	d0,(18+l1072,a7)
	addq.w	#6,a7
	bra	l1065
l1064
	addq.w	#1,(0+l1072,a7)
l1065
	move.l	(8+l1072,a7),a0
	add.w	(0+l1072,a7),a0
	tst.b	(a0)
	bne	l1064
l1066
	tst.w	(0+l1072,a7)
	ble	l1068
l1067
	move.w	(0+l1072,a7),d0
	lsl.w	#2,d0
	muls.w	(12+l1072,a7),d0
	sub.w	(12+l1072,a7),d0
	bra	l1069
l1068
	moveq	#0,d0
l1069
l1062
l1070	reg
l1072	equ	0
	addq.w	#4,a7
	rts
; stacksize=14
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1073
	sub.w	#16,a7
	movem.l	l1105,-(a7)
	move.w	#4,-(a7)
	move.w	#1,-(a7)
	move.w	(36+l1107,a7),-(a7)
	jsr	l15
	move.w	d0,(38+l1107,a7)
	addq.w	#6,a7
	cmp.b	#48,(29+l1107,a7)
	blt	l1077
l1078
	cmp.b	#57,(29+l1107,a7)
	bgt	l1077
l1076
	moveq	#5,d2
	move.b	(29+l1107,a7),d0
	ext.w	d0
	sub.w	#48,d0
	ext.l	d0
	moveq	#5,d1
	move.l	d0,d3
	move.l	d1,d4
	swap	d3
	swap	d4
	mulu.w	d1,d3
	mulu.w	d0,d4
	mulu.w	d1,d0
	add.w	d4,d3
	swap	d3
	clr.w	d3
	add.l	d3,d0
	lea	l14,a0
	add.l	d0,a0
	move.l	a0,a1
	lea	(0+l1107,a7),a0
	inline
	move.l	a0,d0
	cmp.l	#16,d2
	blo	.l5
	moveq	#1,d1
	and.b	d0,d1
	beq	.l1
	move.b	(a1)+,(a0)+
	subq.l	#1,d2
.l1
	move.l	a1,d1
	and.b	#1,d1
	beq	.l3
	cmp.l	#$10000,d2
	blo	.l5
.l2
	move.b	(a1)+,(a0)+
	subq.l	#1,d2
	bne	.l2
	bra	.l7
.l3
	moveq	#3,d1
	and.l	d2,d1
	sub.l	d1,d2
.l4
	move.l	(a1)+,(a0)+
	subq.l	#4,d2
	bne	.l4
	move.w	d1,d2
.l5
	subq.w	#1,d2
	blo	.l7
.l6
	move.b	(a1)+,(a0)+
	dbf	d2,.l6
.l7
	einline
	bra	l1079
l1077
	cmp.b	#97,(29+l1107,a7)
	blt	l1081
l1082
	cmp.b	#122,(29+l1107,a7)
	bgt	l1081
l1080
	move.b	(29+l1107,a7),d0
	sub.b	#97,d0
	moveq	#65,d1
	add.b	d0,d1
	move.b	d1,(29+l1107,a7)
l1081
	cmp.b	#32,(29+l1107,a7)
	bne	l1084
l1083
	bra	l1074
l1084
	lea	(0+l1107,a7),a0
	move.l	a0,-(a7)
	move.b	(33+l1107,a7),d0
	ext.w	d0
	move.w	d0,-(a7)
	jsr	l952
	addq.w	#6,a7
	tst.b	d0
	bne	l1086
l1085
	bra	l1074
l1086
l1079
	move.w	#0,(6+l1107,a7)
	bra	l1088
l1087
	move.w	#0,(8+l1107,a7)
	bra	l1092
l1091
	lea	(0+l1107,a7),a0
	add.w	(6+l1107,a7),a0
	moveq	#2,d0
	sub.w	(8+l1107,a7),d0
	moveq	#0,d1
	move.b	(a0),d1
	asr.w	d0,d1
	and.w	#1,d1
	bne	l1096
l1095
	bra	l1094
l1096
	move.w	#0,(12+l1107,a7)
	bra	l1098
l1097
	move.w	#0,(10+l1107,a7)
	bra	l1102
l1101
	moveq	#0,d0
	move.b	(31+l1107,a7),d0
	move.w	d0,-(a7)
	move.w	(8+l1107,a7),d0
	muls.w	(34+l1107,a7),d0
	add.w	(28+l1107,a7),d0
	add.w	(14+l1107,a7),d0
	move.w	d0,-(a7)
	move.w	(12+l1107,a7),d0
	muls.w	(36+l1107,a7),d0
	add.w	(28+l1107,a7),d0
	add.w	(14+l1107,a7),d0
	move.w	d0,-(a7)
	move.l	(26+l1107,a7),-(a7)
	jsr	l881
	add.w	#10,a7
l1104
	addq.w	#1,(10+l1107,a7)
l1102
	move.w	(10+l1107,a7),d0
	cmp.w	(32+l1107,a7),d0
	blt	l1101
l1103
l1100
	addq.w	#1,(12+l1107,a7)
l1098
	move.w	(12+l1107,a7),d0
	cmp.w	(32+l1107,a7),d0
	blt	l1097
l1099
l1094
	addq.w	#1,(8+l1107,a7)
l1092
	cmp.w	#3,(8+l1107,a7)
	blt	l1091
l1093
l1090
	addq.w	#1,(6+l1107,a7)
l1088
	cmp.w	#5,(6+l1107,a7)
	blt	l1087
l1089
l1074
l1105	reg	d2/d3/d4
	movem.l	(a7)+,d2/d3/d4
l1107	equ	12
	add.w	#16,a7
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1108
	movem.l	l1114,-(a7)
	move.w	#4,-(a7)
	move.w	#1,-(a7)
	move.w	(22+l1116,a7),-(a7)
	jsr	l15
	move.w	d0,(24+l1116,a7)
	addq.w	#6,a7
	bra	l1112
l1111
	move.w	(18+l1116,a7),-(a7)
	moveq	#0,d0
	move.b	(19+l1116,a7),d0
	move.w	d0,-(a7)
	move.l	(16+l1116,a7),a0
	move.b	(a0),d0
	ext.w	d0
	move.w	d0,-(a7)
	move.w	(16+l1116,a7),-(a7)
	move.w	(16+l1116,a7),-(a7)
	move.l	(14+l1116,a7),-(a7)
	jsr	l1073
	move.w	(32+l1116,a7),d0
	lsl.w	#2,d0
	add.w	d0,(22+l1116,a7)
	addq.l	#1,(26+l1116,a7)
	add.w	#14,a7
l1112
	move.l	(12+l1116,a7),a0
	tst.b	(a0)
	bne	l1111
l1113
l1109
l1114	reg
l1116	equ	0
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1117
	movem.l	l1120,-(a7)
	move.w	(20+l1122,a7),-(a7)
	moveq	#0,d0
	move.b	(21+l1122,a7),d0
	move.w	d0,-(a7)
	move.l	(16+l1122,a7),-(a7)
	move.w	(18+l1122,a7),-(a7)
	move.w	(18+l1122,a7),d0
	subq.w	#1,d0
	move.w	d0,-(a7)
	move.l	(16+l1122,a7),-(a7)
	jsr	l1108
	move.w	(36+l1122,a7),-(a7)
	moveq	#0,d0
	move.b	(37+l1122,a7),d0
	move.w	d0,-(a7)
	move.l	(32+l1122,a7),-(a7)
	move.w	(34+l1122,a7),-(a7)
	moveq	#1,d0
	add.w	(34+l1122,a7),d0
	move.w	d0,-(a7)
	move.l	(32+l1122,a7),-(a7)
	jsr	l1108
	move.w	(52+l1122,a7),-(a7)
	moveq	#0,d0
	move.b	(53+l1122,a7),d0
	move.w	d0,-(a7)
	move.l	(48+l1122,a7),-(a7)
	move.w	(50+l1122,a7),d0
	subq.w	#1,d0
	move.w	d0,-(a7)
	move.w	(50+l1122,a7),-(a7)
	move.l	(48+l1122,a7),-(a7)
	jsr	l1108
	move.w	(68+l1122,a7),-(a7)
	moveq	#0,d0
	move.b	(69+l1122,a7),d0
	move.w	d0,-(a7)
	move.l	(64+l1122,a7),-(a7)
	moveq	#1,d0
	add.w	(66+l1122,a7),d0
	move.w	d0,-(a7)
	move.w	(66+l1122,a7),-(a7)
	move.l	(64+l1122,a7),-(a7)
	jsr	l1108
	move.w	(84+l1122,a7),-(a7)
	moveq	#0,d0
	move.b	(83+l1122,a7),d0
	move.w	d0,-(a7)
	move.l	(80+l1122,a7),-(a7)
	move.w	(82+l1122,a7),-(a7)
	move.w	(82+l1122,a7),-(a7)
	move.l	(80+l1122,a7),-(a7)
	jsr	l1108
	add.w	#80,a7
l1118
l1120	reg
l1122	equ	0
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1123
	subq.w	#4,a7
	movem.l	l1126,-(a7)
	move.w	(26+l1128,a7),-(a7)
	move.l	(20+l1128,a7),-(a7)
	jsr	l1061
	move.w	d0,(6+l1128,a7)
	move.w	(20+l1128,a7),d0
	sub.w	(18+l1128,a7),d0
	addq.w	#1,d0
	sub.w	(6+l1128,a7),d0
	tst.w	d0
	bge	l1129
	addq.w	#1,d0
l1129:
	asr.w	#1,d0
	add.w	(18+l1128,a7),d0
	move.w	d0,(8+l1128,a7)
	move.w	(32+l1128,a7),-(a7)
	moveq	#0,d0
	move.b	(33+l1128,a7),d0
	move.w	d0,-(a7)
	moveq	#0,d0
	move.b	(33+l1128,a7),d0
	move.w	d0,-(a7)
	move.l	(30+l1128,a7),-(a7)
	move.w	(32+l1128,a7),-(a7)
	move.w	(20+l1128,a7),-(a7)
	move.l	(28+l1128,a7),-(a7)
	jsr	l1117
	add.w	#24,a7
l1124
l1126	reg
l1128	equ	0
	addq.w	#4,a7
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1130
	sub.w	#24,a7
	movem.l	l1145,-(a7)
	move.w	#0,(20+l1147,a7)
l1133
	moveq	#10,d1
	move.l	(36+l1147,a7),d0
	jsr	__divu
	move.l	d1,d0
	move.l	d0,d2
	lea	(0+l1147,a7),a0
	move.w	(20+l1147,a7),d0
	addq.w	#1,(20+l1147,a7)
	ext.l	d0
	lsl.l	#1,d0
	move.w	d2,(0,a0,d0.l)
	moveq	#10,d1
	move.l	(36+l1147,a7),d0
	jsr	__divu
	move.l	d0,d2
	move.l	d2,(36+l1147,a7)
l1135
	tst.l	(36+l1147,a7)
	bls	l1134
l1136
	cmp.w	#10,(20+l1147,a7)
	blt	l1133
l1134
	bra	l1138
l1137
	lea	(0+l1147,a7),a0
	move.w	(20+l1147,a7),d0
	addq.w	#1,(20+l1147,a7)
	ext.l	d0
	lsl.l	#1,d0
	move.w	#0,(0,a0,d0.l)
l1138
	move.w	(20+l1147,a7),d0
	cmp.w	(40+l1147,a7),d0
	bge	l1139
l1140
	cmp.w	#10,(20+l1147,a7)
	blt	l1137
l1139
	move.w	(20+l1147,a7),d0
	subq.w	#1,d0
	move.w	d0,(22+l1147,a7)
	bra	l1142
l1141
	moveq	#0,d0
	move.b	(43+l1147,a7),d0
	move.w	d0,-(a7)
	lea	(2+l1147,a7),a0
	move.w	(24+l1147,a7),d0
	ext.l	d0
	lsl.l	#1,d0
	move.w	(0,a0,d0.l),-(a7)
	move.w	(38+l1147,a7),-(a7)
	move.w	(38+l1147,a7),-(a7)
	move.l	(36+l1147,a7),-(a7)
	jsr	l933
	addq.w	#4,(44+l1147,a7)
	add.w	#12,a7
l1144
	subq.w	#1,(22+l1147,a7)
l1142
	tst.w	(22+l1147,a7)
	bge	l1141
l1143
l1131
l1145	reg	d2
	movem.l	(a7)+,d2
l1147	equ	4
	add.w	#24,a7
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1148
	movem.l	l1151,-(a7)
	moveq	#0,d0
	move.b	(21+l1153,a7),d0
	move.w	d0,-(a7)
	move.w	(18+l1153,a7),-(a7)
	move.l	(16+l1153,a7),-(a7)
	move.w	(18+l1153,a7),-(a7)
	move.w	(18+l1153,a7),d0
	subq.w	#1,d0
	move.w	d0,-(a7)
	move.l	(16+l1153,a7),-(a7)
	jsr	l1130
	moveq	#0,d0
	move.b	(37+l1153,a7),d0
	move.w	d0,-(a7)
	move.w	(34+l1153,a7),-(a7)
	move.l	(32+l1153,a7),-(a7)
	move.w	(34+l1153,a7),-(a7)
	moveq	#1,d0
	add.w	(34+l1153,a7),d0
	move.w	d0,-(a7)
	move.l	(32+l1153,a7),-(a7)
	jsr	l1130
	moveq	#0,d0
	move.b	(53+l1153,a7),d0
	move.w	d0,-(a7)
	move.w	(50+l1153,a7),-(a7)
	move.l	(48+l1153,a7),-(a7)
	move.w	(50+l1153,a7),d0
	subq.w	#1,d0
	move.w	d0,-(a7)
	move.w	(50+l1153,a7),-(a7)
	move.l	(48+l1153,a7),-(a7)
	jsr	l1130
	moveq	#0,d0
	move.b	(69+l1153,a7),d0
	move.w	d0,-(a7)
	move.w	(66+l1153,a7),-(a7)
	move.l	(64+l1153,a7),-(a7)
	moveq	#1,d0
	add.w	(66+l1153,a7),d0
	move.w	d0,-(a7)
	move.w	(66+l1153,a7),-(a7)
	move.l	(64+l1153,a7),-(a7)
	jsr	l1130
	moveq	#0,d0
	move.b	(83+l1153,a7),d0
	move.w	d0,-(a7)
	move.w	(82+l1153,a7),-(a7)
	move.l	(80+l1153,a7),-(a7)
	move.w	(82+l1153,a7),-(a7)
	move.w	(82+l1153,a7),-(a7)
	move.l	(80+l1153,a7),-(a7)
	jsr	l1130
	add.w	#80,a7
l1149
l1151	reg
l1153	equ	0
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1154
	subq.w	#4,a7
	movem.l	l1168,-(a7)
	move.w	#0,(0+l1170,a7)
l1157
	addq.w	#1,(0+l1170,a7)
	moveq	#10,d1
	move.l	(8+l1170,a7),d0
	jsr	__divu
	move.l	d0,d2
	move.l	d2,(8+l1170,a7)
l1159
	tst.l	(8+l1170,a7)
	bls	l1158
l1160
	cmp.w	#10,(0+l1170,a7)
	blt	l1157
l1158
	bra	l1162
l1161
	addq.w	#1,(0+l1170,a7)
l1162
	move.w	(0+l1170,a7),d0
	cmp.w	(12+l1170,a7),d0
	bge	l1163
l1164
	cmp.w	#10,(0+l1170,a7)
	blt	l1161
l1163
	tst.w	(0+l1170,a7)
	ble	l1166
l1165
	move.w	(0+l1170,a7),d0
	lsl.w	#2,d0
	subq.w	#1,d0
	bra	l1167
l1166
	moveq	#0,d0
l1167
l1155
l1168	reg	d2
	movem.l	(a7)+,d2
l1170	equ	4
	addq.w	#4,a7
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1171
	movem.l	l1174,-(a7)
	moveq	#0,d0
	move.b	(19+l1176,a7),d0
	move.w	d0,-(a7)
	move.w	(18+l1176,a7),-(a7)
	move.l	(16+l1176,a7),-(a7)
	move.w	(18+l1176,a7),-(a7)
	move.w	(26+l1176,a7),-(a7)
	move.l	(24+l1176,a7),-(a7)
	jsr	l1154
	addq.w	#6,a7
	neg.w	d0
	add.w	(18+l1176,a7),d0
	addq.w	#1,d0
	move.w	d0,-(a7)
	move.l	(16+l1176,a7),-(a7)
	jsr	l1130
	add.w	#16,a7
l1172
l1174	reg
l1176	equ	0
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1177
	sub.w	#12,a7
	movem.l	l1201,-(a7)
	tst.w	(24+l1203,a7)
	blt	l1180
l1182
	cmp.w	#9,(24+l1203,a7)
	ble	l1181
l1180
	bra	l1178
l1181
	move.w	#6,-(a7)
	move.w	#1,-(a7)
	move.w	(32+l1203,a7),-(a7)
	jsr	l15
	move.w	d0,(34+l1203,a7)
	move.w	#0,(6+l1203,a7)
	addq.w	#6,a7
	bra	l1184
l1183
	move.w	(24+l1203,a7),d0
	ext.l	d0
	moveq	#5,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	lea	l14,a0
	add.l	d0,a0
	add.w	(0+l1203,a7),a0
	move.b	(a0),(8+l1203,a7)
	move.w	#0,(2+l1203,a7)
	bra	l1188
l1187
	moveq	#2,d0
	sub.w	(2+l1203,a7),d0
	moveq	#0,d1
	move.b	(8+l1203,a7),d1
	asr.w	d0,d1
	and.w	#1,d1
	bne	l1192
l1191
	bra	l1190
l1192
	move.w	#0,(6+l1203,a7)
	bra	l1194
l1193
	move.w	#0,(4+l1203,a7)
	bra	l1198
l1197
	moveq	#0,d0
	move.b	(27+l1203,a7),d0
	move.w	d0,-(a7)
	move.w	(2+l1203,a7),d0
	muls.w	(30+l1203,a7),d0
	add.w	(24+l1203,a7),d0
	add.w	(8+l1203,a7),d0
	move.w	d0,-(a7)
	move.w	(6+l1203,a7),d0
	muls.w	(32+l1203,a7),d0
	add.w	(24+l1203,a7),d0
	add.w	(8+l1203,a7),d0
	move.w	d0,-(a7)
	move.l	(22+l1203,a7),-(a7)
	jsr	l881
	add.w	#10,a7
l1200
	addq.w	#1,(4+l1203,a7)
l1198
	move.w	(4+l1203,a7),d0
	cmp.w	(28+l1203,a7),d0
	blt	l1197
l1199
l1196
	addq.w	#1,(6+l1203,a7)
l1194
	move.w	(6+l1203,a7),d0
	cmp.w	(28+l1203,a7),d0
	blt	l1193
l1195
l1190
	addq.w	#1,(2+l1203,a7)
l1188
	cmp.w	#3,(2+l1203,a7)
	blt	l1187
l1189
l1186
	addq.w	#1,(0+l1203,a7)
l1184
	cmp.w	#5,(0+l1203,a7)
	blt	l1183
l1185
l1178
l1201	reg	d2/d3
	movem.l	(a7)+,d2/d3
l1203	equ	8
	add.w	#12,a7
	rts
; stacksize=68
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1204
	subq.w	#4,a7
	movem.l	l1213,-(a7)
	move.w	#1,(0+l1215,a7)
	move.w	#6,-(a7)
	move.w	#1,-(a7)
	move.w	(18+l1215,a7),-(a7)
	jsr	l15
	move.w	d0,(20+l1215,a7)
	addq.w	#6,a7
	bra	l1208
l1207
	moveq	#10,d1
	move.l	(8+l1215,a7),d0
	jsr	__divu
	move.l	d0,d2
	move.l	d2,(8+l1215,a7)
	addq.w	#1,(0+l1215,a7)
l1208
	moveq	#10,d0
	cmp.l	(8+l1215,a7),d0
	bhi	l1209
l1210
	cmp.w	#10,(0+l1215,a7)
	blt	l1207
l1209
	move.w	(0+l1215,a7),d0
	cmp.w	(12+l1215,a7),d0
	bge	l1212
l1211
	move.w	(12+l1215,a7),(0+l1215,a7)
l1212
	move.w	(14+l1215,a7),d0
	lsl.w	#2,d0
	muls.w	(0+l1215,a7),d0
	sub.w	(14+l1215,a7),d0
l1205
l1213	reg	d2
	movem.l	(a7)+,d2
l1215	equ	4
	addq.w	#4,a7
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1216
	sub.w	#24,a7
	movem.l	l1231,-(a7)
	move.w	#0,(20+l1233,a7)
	move.w	#6,-(a7)
	move.w	#1,-(a7)
	move.w	(48+l1233,a7),-(a7)
	jsr	l15
	move.w	d0,(50+l1233,a7)
	addq.w	#6,a7
l1219
	moveq	#10,d1
	move.l	(36+l1233,a7),d0
	jsr	__divu
	move.l	d1,d0
	move.l	d0,d2
	lea	(0+l1233,a7),a0
	move.w	(20+l1233,a7),d0
	addq.w	#1,(20+l1233,a7)
	ext.l	d0
	lsl.l	#1,d0
	move.w	d2,(0,a0,d0.l)
	moveq	#10,d1
	move.l	(36+l1233,a7),d0
	jsr	__divu
	move.l	d0,d2
	move.l	d2,(36+l1233,a7)
l1221
	tst.l	(36+l1233,a7)
	bls	l1220
l1222
	cmp.w	#10,(20+l1233,a7)
	blt	l1219
l1220
	bra	l1224
l1223
	lea	(0+l1233,a7),a0
	move.w	(20+l1233,a7),d0
	addq.w	#1,(20+l1233,a7)
	ext.l	d0
	lsl.l	#1,d0
	move.w	#0,(0,a0,d0.l)
l1224
	move.w	(20+l1233,a7),d0
	cmp.w	(40+l1233,a7),d0
	bge	l1225
l1226
	cmp.w	#10,(20+l1233,a7)
	blt	l1223
l1225
	move.w	(20+l1233,a7),d0
	subq.w	#1,d0
	move.w	d0,(22+l1233,a7)
	bra	l1228
l1227
	move.w	(44+l1233,a7),-(a7)
	moveq	#0,d0
	move.b	(45+l1233,a7),d0
	move.w	d0,-(a7)
	lea	(4+l1233,a7),a0
	move.w	(26+l1233,a7),d0
	ext.l	d0
	lsl.l	#1,d0
	move.w	(0,a0,d0.l),-(a7)
	move.w	(40+l1233,a7),-(a7)
	move.w	(40+l1233,a7),-(a7)
	move.l	(38+l1233,a7),-(a7)
	jsr	l1177
	move.w	(58+l1233,a7),d0
	lsl.w	#2,d0
	add.w	d0,(46+l1233,a7)
	add.w	#14,a7
l1230
	subq.w	#1,(22+l1233,a7)
l1228
	tst.w	(22+l1233,a7)
	bge	l1227
l1229
l1217
l1231	reg	d2
	movem.l	(a7)+,d2
l1233	equ	4
	add.w	#24,a7
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1234
	movem.l	l1237,-(a7)
	move.w	(22+l1239,a7),-(a7)
	moveq	#0,d0
	move.b	(23+l1239,a7),d0
	move.w	d0,-(a7)
	move.w	(20+l1239,a7),-(a7)
	move.l	(18+l1239,a7),-(a7)
	move.w	(20+l1239,a7),-(a7)
	move.w	(20+l1239,a7),d0
	subq.w	#1,d0
	move.w	d0,-(a7)
	move.l	(18+l1239,a7),-(a7)
	jsr	l1216
	move.w	(40+l1239,a7),-(a7)
	moveq	#0,d0
	move.b	(41+l1239,a7),d0
	move.w	d0,-(a7)
	move.w	(38+l1239,a7),-(a7)
	move.l	(36+l1239,a7),-(a7)
	move.w	(38+l1239,a7),-(a7)
	moveq	#1,d0
	add.w	(38+l1239,a7),d0
	move.w	d0,-(a7)
	move.l	(36+l1239,a7),-(a7)
	jsr	l1216
	move.w	(58+l1239,a7),-(a7)
	moveq	#0,d0
	move.b	(59+l1239,a7),d0
	move.w	d0,-(a7)
	move.w	(56+l1239,a7),-(a7)
	move.l	(54+l1239,a7),-(a7)
	move.w	(56+l1239,a7),d0
	subq.w	#1,d0
	move.w	d0,-(a7)
	move.w	(56+l1239,a7),-(a7)
	move.l	(54+l1239,a7),-(a7)
	jsr	l1216
	move.w	(76+l1239,a7),-(a7)
	moveq	#0,d0
	move.b	(77+l1239,a7),d0
	move.w	d0,-(a7)
	move.w	(74+l1239,a7),-(a7)
	move.l	(72+l1239,a7),-(a7)
	moveq	#1,d0
	add.w	(74+l1239,a7),d0
	move.w	d0,-(a7)
	move.w	(74+l1239,a7),-(a7)
	move.l	(72+l1239,a7),-(a7)
	jsr	l1216
	move.w	(94+l1239,a7),-(a7)
	moveq	#0,d0
	move.b	(93+l1239,a7),d0
	move.w	d0,-(a7)
	move.w	(92+l1239,a7),-(a7)
	move.l	(90+l1239,a7),-(a7)
	move.w	(92+l1239,a7),-(a7)
	move.w	(92+l1239,a7),-(a7)
	move.l	(90+l1239,a7),-(a7)
	jsr	l1216
	add.w	#90,a7
l1235
l1237	reg
l1239	equ	0
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1240
	subq.w	#4,a7
	movem.l	l1251,-(a7)
	move.w	#0,(2+l1253,a7)
	bra	l1244
l1243
	move.w	#0,(0+l1253,a7)
	bra	l1248
l1247
	move.w	(12+l1253,a7),d0
	ext.l	d0
	move.l	#240,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	move.l	(8+l1253,a7),a0
	add.l	d0,a0
	move.w	(2+l1253,a7),d0
	ext.l	d0
	moveq	#20,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	add.l	d0,a0
	add.w	(0+l1253,a7),a0
	move.b	#1,(a0)
l1250
	addq.w	#1,(0+l1253,a7)
l1248
	cmp.w	#20,(0+l1253,a7)
	blt	l1247
l1249
l1246
	addq.w	#1,(2+l1253,a7)
l1244
	cmp.w	#12,(2+l1253,a7)
	blt	l1243
l1245
l1241
l1251	reg	d2/d3
	movem.l	(a7)+,d2/d3
l1253	equ	8
	addq.w	#4,a7
	rts
; stacksize=12
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1254
	sub.w	#12,a7
	movem.l	l1273,-(a7)
	tst.w	(26+l1275,a7)
	ble	l1257
l1259
	tst.w	(28+l1275,a7)
	bgt	l1258
l1257
	bra	l1255
l1258
	cmp.w	#320,(22+l1275,a7)
	bge	l1260
l1264
	cmp.w	#192,(24+l1275,a7)
	bge	l1260
l1263
	move.w	(22+l1275,a7),d0
	add.w	(26+l1275,a7),d0
	ble	l1260
l1262
	move.w	(24+l1275,a7),d0
	add.w	(28+l1275,a7),d0
	bgt	l1261
l1260
	bra	l1255
l1261
	move.w	#319,-(a7)
	move.w	#0,-(a7)
	move.w	(26+l1275,a7),-(a7)
	jsr	l15
	tst.w	d0
	bge	l1276
	add.w	#15,d0
l1276:
	asr.w	#4,d0
	move.w	d0,(6+l1275,a7)
	move.w	#191,-(a7)
	move.w	#0,-(a7)
	move.w	(34+l1275,a7),-(a7)
	jsr	l15
	tst.w	d0
	bge	l1277
	add.w	#15,d0
l1277:
	asr.w	#4,d0
	move.w	d0,(14+l1275,a7)
	move.w	#319,-(a7)
	move.w	#0,-(a7)
	move.w	(38+l1275,a7),d0
	add.w	(42+l1275,a7),d0
	subq.w	#1,d0
	move.w	d0,-(a7)
	jsr	l15
	tst.w	d0
	bge	l1278
	add.w	#15,d0
l1278:
	asr.w	#4,d0
	move.w	d0,(22+l1275,a7)
	move.w	#191,-(a7)
	move.w	#0,-(a7)
	move.w	(46+l1275,a7),d0
	add.w	(50+l1275,a7),d0
	subq.w	#1,d0
	move.w	d0,-(a7)
	jsr	l15
	tst.w	d0
	bge	l1279
	add.w	#15,d0
l1279:
	asr.w	#4,d0
	move.w	d0,(30+l1275,a7)
	move.w	(26+l1275,a7),(34+l1275,a7)
	add.w	#24,a7
	bra	l1266
l1265
	move.w	(0+l1275,a7),(8+l1275,a7)
	bra	l1270
l1269
	move.w	(20+l1275,a7),d0
	ext.l	d0
	move.l	#240,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	move.l	(16+l1275,a7),a0
	add.l	d0,a0
	move.w	(10+l1275,a7),d0
	ext.l	d0
	moveq	#20,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	add.l	d0,a0
	add.w	(8+l1275,a7),a0
	move.b	#1,(a0)
l1272
	addq.w	#1,(8+l1275,a7)
l1270
	move.w	(8+l1275,a7),d0
	cmp.w	(4+l1275,a7),d0
	ble	l1269
l1271
l1268
	addq.w	#1,(10+l1275,a7)
l1266
	move.w	(10+l1275,a7),d0
	cmp.w	(6+l1275,a7),d0
	ble	l1265
l1267
l1255
l1273	reg	d2/d3
	movem.l	(a7)+,d2/d3
l1275	equ	8
	add.w	#12,a7
	rts
; stacksize=48
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1280
	movem.l	l1283,-(a7)
	move.w	#4,-(a7)
	move.w	#320,-(a7)
	move.w	#0,-(a7)
	move.w	#0,-(a7)
	move.w	(16+l1285,a7),-(a7)
	move.l	(14+l1285,a7),-(a7)
	jsr	l1254
	move.w	#4,-(a7)
	move.w	#320,-(a7)
	move.w	#188,-(a7)
	move.w	#0,-(a7)
	move.w	(30+l1285,a7),-(a7)
	move.l	(28+l1285,a7),-(a7)
	jsr	l1254
	move.w	#192,-(a7)
	move.w	#4,-(a7)
	move.w	#0,-(a7)
	move.w	#0,-(a7)
	move.w	(44+l1285,a7),-(a7)
	move.l	(42+l1285,a7),-(a7)
	jsr	l1254
	move.w	#192,-(a7)
	move.w	#4,-(a7)
	move.w	#0,-(a7)
	move.w	#316,-(a7)
	move.w	(58+l1285,a7),-(a7)
	move.l	(56+l1285,a7),-(a7)
	jsr	l1254
	add.w	#56,a7
l1281
l1283	reg
l1285	equ	0
	rts
; stacksize=108
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1286
	movem.l	l1293,-(a7)
	tst.l	(10+l1295,a7)
	beq	l1290
l1292
	move.l	(10+l1295,a7),a0
	tst.w	(4,a0)
	ble	l1290
l1291
	move.l	(10+l1295,a7),a0
	tst.w	(6,a0)
	ble	l1290
l1289
	move.l	(10+l1295,a7),a0
	move.w	(6,a0),-(a7)
	move.l	(12+l1295,a7),a0
	move.w	(4,a0),-(a7)
	move.l	(14+l1295,a7),a0
	move.w	(2,a0),-(a7)
	move.l	(16+l1295,a7),a0
	move.w	(a0),-(a7)
	move.w	(16+l1295,a7),-(a7)
	move.l	(14+l1295,a7),-(a7)
	jsr	l1254
	add.w	#14,a7
l1290
l1287
l1293	reg
l1295	equ	0
	rts
; stacksize=66
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1296
	movem.l	l1305,-(a7)
	move.l	(4+l1307,a7),a0
	move.l	(8+l1307,a7),a1
	move.w	(a0),d0
	cmp.w	(a1),d0
	bne	l1301
l1304
	move.l	(4+l1307,a7),a0
	move.l	(8+l1307,a7),a1
	move.w	(2,a0),d0
	cmp.w	(2,a1),d0
	bne	l1301
l1303
	move.l	(4+l1307,a7),a0
	move.l	(8+l1307,a7),a1
	move.w	(4,a0),d0
	cmp.w	(4,a1),d0
	bne	l1301
l1299
	move.l	(4+l1307,a7),a0
	move.l	(8+l1307,a7),a1
	move.w	(6,a0),d0
	cmp.w	(6,a1),d0
	bne	l1301
l1300
	moveq	#1,d0
	bra	l1302
l1301
	moveq	#0,d0
l1302
l1297
l1305	reg
l1307	equ	0
	rts
; stacksize=0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1308
	sub.w	#12,a7
	movem.l	l1325,-(a7)
	tst.l	(22+l1327,a7)
	beq	l1311
l1314
	move.l	(22+l1327,a7),a0
	tst.w	(4,a0)
	ble	l1311
l1313
	move.l	(22+l1327,a7),a0
	tst.w	(6,a0)
	bgt	l1312
l1311
	moveq	#0,d0
	bra	l1309
l1312
	move.w	#319,-(a7)
	move.w	#0,-(a7)
	move.l	(26+l1327,a7),a0
	move.w	(a0),-(a7)
	jsr	l15
	tst.w	d0
	bge	l1328
	add.w	#15,d0
l1328:
	asr.w	#4,d0
	move.w	d0,(6+l1327,a7)
	move.w	#191,-(a7)
	move.w	#0,-(a7)
	move.l	(32+l1327,a7),a0
	move.w	(2,a0),-(a7)
	jsr	l15
	tst.w	d0
	bge	l1329
	add.w	#15,d0
l1329:
	asr.w	#4,d0
	move.w	d0,(14+l1327,a7)
	move.w	#319,-(a7)
	move.w	#0,-(a7)
	move.l	(38+l1327,a7),a0
	move.l	(38+l1327,a7),a1
	move.w	(a1),d0
	add.w	(4,a0),d0
	subq.w	#1,d0
	move.w	d0,-(a7)
	jsr	l15
	tst.w	d0
	bge	l1330
	add.w	#15,d0
l1330:
	asr.w	#4,d0
	move.w	d0,(22+l1327,a7)
	move.w	#191,-(a7)
	move.w	#0,-(a7)
	move.l	(44+l1327,a7),a0
	move.l	(44+l1327,a7),a1
	move.w	(2,a0),d0
	add.w	(6,a1),d0
	subq.w	#1,d0
	move.w	d0,-(a7)
	jsr	l15
	tst.w	d0
	bge	l1331
	add.w	#15,d0
l1331:
	asr.w	#4,d0
	move.w	d0,(30+l1327,a7)
	move.w	(26+l1327,a7),(34+l1327,a7)
	add.w	#24,a7
	bra	l1316
l1315
	move.w	(0+l1327,a7),(8+l1327,a7)
	bra	l1320
l1319
	move.w	(20+l1327,a7),d0
	ext.l	d0
	move.l	#240,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	move.l	(16+l1327,a7),a0
	add.l	d0,a0
	move.w	(10+l1327,a7),d0
	ext.l	d0
	moveq	#20,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	add.l	d0,a0
	add.w	(8+l1327,a7),a0
	tst.b	(a0)
	beq	l1324
	moveq	#1,d0
	bra	l1309
l1324
l1322
	addq.w	#1,(8+l1327,a7)
l1320
	move.w	(8+l1327,a7),d0
	cmp.w	(4+l1327,a7),d0
	ble	l1319
l1321
l1318
	addq.w	#1,(10+l1327,a7)
l1316
	move.w	(10+l1327,a7),d0
	cmp.w	(6+l1327,a7),d0
	ble	l1315
l1317
	moveq	#0,d0
l1309
l1325	reg	d2/d3
	movem.l	(a7)+,d2/d3
l1327	equ	8
	add.w	#12,a7
	rts
; stacksize=48
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1332
	subq.w	#4,a7
	movem.l	l1349,-(a7)
	move.l	(8+l1351,a7),a0
	add.w	#2468,a0
	move.w	(12+l1351,a7),d0
	ext.l	d0
	moveq	#14,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	tst.b	(0,a0,d0.l)
	beq	l1336
	move.l	(8+l1351,a7),a0
	add.w	#2468,a0
	move.w	(12+l1351,a7),d0
	ext.l	d0
	moveq	#14,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	pea	(6,a0,d0.l)
	move.w	(16+l1351,a7),-(a7)
	move.l	(14+l1351,a7),-(a7)
	jsr	l1286
	add.w	#10,a7
l1336
	move.l	(8+l1351,a7),a0
	add.w	#2468,a0
	move.w	(12+l1351,a7),d0
	ext.l	d0
	moveq	#14,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	move.b	#0,(0,a0,d0.l)
	move.w	#0,(0+l1351,a7)
	bra	l1338
l1337
	move.l	(8+l1351,a7),a0
	add.w	#2496,a0
	move.w	(12+l1351,a7),d0
	ext.l	d0
	moveq	#112,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	add.l	d0,a0
	move.w	(0+l1351,a7),d0
	ext.l	d0
	moveq	#14,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	tst.b	(0,a0,d0.l)
	beq	l1342
	move.l	(8+l1351,a7),a0
	add.w	#2496,a0
	move.w	(12+l1351,a7),d0
	ext.l	d0
	moveq	#112,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	add.l	d0,a0
	move.w	(0+l1351,a7),d0
	ext.l	d0
	moveq	#14,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	pea	(6,a0,d0.l)
	move.w	(16+l1351,a7),-(a7)
	move.l	(14+l1351,a7),-(a7)
	jsr	l1286
	add.w	#10,a7
l1342
	move.l	(8+l1351,a7),a0
	add.w	#2496,a0
	move.w	(12+l1351,a7),d0
	ext.l	d0
	moveq	#112,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	add.l	d0,a0
	move.w	(0+l1351,a7),d0
	ext.l	d0
	moveq	#14,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	move.b	#0,(0,a0,d0.l)
l1340
	addq.w	#1,(0+l1351,a7)
l1338
	cmp.w	#8,(0+l1351,a7)
	blt	l1337
l1339
	move.w	#0,(0+l1351,a7)
	bra	l1344
l1343
	move.l	(8+l1351,a7),a0
	add.w	#2720,a0
	move.w	(12+l1351,a7),d0
	ext.l	d0
	move.l	#224,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	add.l	d0,a0
	move.w	(0+l1351,a7),d0
	ext.l	d0
	moveq	#14,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	tst.b	(0,a0,d0.l)
	beq	l1348
	move.l	(8+l1351,a7),a0
	add.w	#2720,a0
	move.w	(12+l1351,a7),d0
	ext.l	d0
	move.l	#224,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	add.l	d0,a0
	move.w	(0+l1351,a7),d0
	ext.l	d0
	moveq	#14,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	pea	(6,a0,d0.l)
	move.w	(16+l1351,a7),-(a7)
	move.l	(14+l1351,a7),-(a7)
	jsr	l1286
	add.w	#10,a7
l1348
	move.l	(8+l1351,a7),a0
	add.w	#2720,a0
	move.w	(12+l1351,a7),d0
	ext.l	d0
	move.l	#224,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	add.l	d0,a0
	move.w	(0+l1351,a7),d0
	ext.l	d0
	moveq	#14,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	move.b	#0,(0,a0,d0.l)
l1346
	addq.w	#1,(0+l1351,a7)
l1344
	cmp.w	#16,(0+l1351,a7)
	blt	l1343
l1345
l1333
l1349	reg	d2/d3
	movem.l	(a7)+,d2/d3
l1351	equ	8
	addq.w	#4,a7
	rts
; stacksize=92
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1352
	subq.w	#4,a7
	movem.l	l1368,-(a7)
	move.l	(14+l1370,a7),a0
	tst.b	(a0)
	bne	l1356
l1355
	bra	l1353
l1356
	tst.b	(19+l1370,a7)
	beq	l1359
l1362
	move.l	(14+l1370,a7),a0
	move.l	(2,a0),d0
	cmp.l	(20+l1370,a7),d0
	bne	l1359
l1361
	tst.l	(24+l1370,a7)
	beq	l1359
l1357
	move.l	(24+l1370,a7),-(a7)
	move.l	(18+l1370,a7),a0
	pea	(6,a0)
	jsr	l1296
	addq.w	#8,a7
	tst.b	d0
	beq	l1359
l1358
	moveq	#0,d0
	bra	l1360
l1359
	moveq	#1,d0
l1360
	move.b	d0,(0+l1370,a7)
	bne	l1364
l1365
	move.l	(14+l1370,a7),a0
	pea	(6,a0)
	move.w	(16+l1370,a7),-(a7)
	move.l	(14+l1370,a7),-(a7)
	jsr	l1308
	add.w	#10,a7
	tst.b	d0
	beq	l1364
l1363
	move.b	#1,(0+l1370,a7)
l1364
	tst.b	(0+l1370,a7)
	beq	l1367
	move.l	(14+l1370,a7),a0
	pea	(6,a0)
	move.w	(16+l1370,a7),-(a7)
	move.l	(14+l1370,a7),-(a7)
	jsr	l1286
	move.l	(24+l1370,a7),a0
	move.b	#0,(a0)
	add.w	#10,a7
l1367
l1353
l1368	reg
l1370	equ	0
	addq.w	#4,a7
	rts
; stacksize=84
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1371
	sub.w	#12,a7
	movem.l	l1386,-(a7)
	tst.w	(34+l1388,a7)
	ble	l1374
l1380
	tst.w	(36+l1388,a7)
	ble	l1374
l1379
	cmp.w	#320,(30+l1388,a7)
	bge	l1374
l1378
	cmp.w	#200,(32+l1388,a7)
	bge	l1374
l1377
	move.w	(30+l1388,a7),d0
	add.w	(34+l1388,a7),d0
	ble	l1374
l1376
	move.w	(32+l1388,a7),d0
	add.w	(36+l1388,a7),d0
	bgt	l1375
l1374
	bra	l1372
l1375
	move.w	#319,-(a7)
	move.w	#0,-(a7)
	move.w	(34+l1388,a7),-(a7)
	jsr	l15
	move.w	d0,(8+l1388,a7)
	move.w	#199,-(a7)
	move.w	#0,-(a7)
	move.w	(42+l1388,a7),-(a7)
	jsr	l15
	move.w	d0,(16+l1388,a7)
	move.w	#320,-(a7)
	move.w	#0,-(a7)
	move.w	(46+l1388,a7),d0
	add.w	(50+l1388,a7),d0
	move.w	d0,-(a7)
	jsr	l15
	sub.w	(20+l1388,a7),d0
	move.w	d0,(24+l1388,a7)
	move.w	#200,-(a7)
	move.w	#0,-(a7)
	move.w	(54+l1388,a7),d0
	add.w	(58+l1388,a7),d0
	move.w	d0,-(a7)
	jsr	l15
	sub.w	(28+l1388,a7),d0
	move.w	d0,(32+l1388,a7)
	add.w	#24,a7
	tst.w	(6+l1388,a7)
	ble	l1381
l1383
	tst.w	(8+l1388,a7)
	bgt	l1382
l1381
	bra	l1372
l1382
	move.l	(20+l1388,a7),a0
	add.w	#4224,a0
	move.w	(24+l1388,a7),d0
	ext.l	d0
	lsl.l	#1,d0
	move.w	(0,a0,d0.l),(0+l1388,a7)
	cmp.w	#64,(0+l1388,a7)
	blt	l1385
l1384
	bra	l1372
l1385
	move.l	(20+l1388,a7),a0
	add.w	#3200,a0
	move.w	(24+l1388,a7),d0
	ext.l	d0
	moveq	#9,d1
	lsl.l	d1,d0
	add.l	d0,a0
	move.w	(0+l1388,a7),d0
	ext.l	d0
	lsl.l	#3,d0
	lea	(2+l1388,a7),a1
	lea	(0,a0,d0.l),a2
	move.l	(a1)+,(a2)+
	move.l	(a1)+,(a2)+
	moveq	#1,d0
	add.w	(0+l1388,a7),d0
	move.l	(20+l1388,a7),a0
	add.w	#4224,a0
	move.w	(24+l1388,a7),d1
	ext.l	d1
	lsl.l	#1,d1
	move.w	d0,(0,a0,d1.l)
l1372
l1386	reg	a2
	movem.l	(a7)+,a2
l1388	equ	4
	add.w	#12,a7
	rts
; stacksize=44
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1389
	subq.w	#4,a7
	movem.l	l1396,-(a7)
	move.w	#0,(0+l1398,a7)
	bra	l1393
l1392
	move.l	(12+l1398,a7),a0
	add.w	#3200,a0
	move.w	(16+l1398,a7),d0
	ext.l	d0
	moveq	#9,d1
	lsl.l	d1,d0
	add.l	d0,a0
	move.w	(0+l1398,a7),d0
	ext.l	d0
	lsl.l	#3,d0
	pea	(0,a0,d0.l)
	move.w	(20+l1398,a7),-(a7)
	move.l	(18+l1398,a7),-(a7)
	jsr	l1286
	add.w	#10,a7
l1395
	addq.w	#1,(0+l1398,a7)
l1393
	move.l	(12+l1398,a7),a0
	add.w	#4224,a0
	move.w	(16+l1398,a7),d0
	ext.l	d0
	lsl.l	#1,d0
	move.w	(0+l1398,a7),d1
	cmp.w	(0,a0,d0.l),d1
	blt	l1392
l1394
	move.l	(12+l1398,a7),a0
	add.w	#4224,a0
	move.w	(16+l1398,a7),d0
	ext.l	d0
	lsl.l	#1,d0
	move.w	#0,(0,a0,d0.l)
l1390
l1396	reg
l1398	equ	0
	addq.w	#4,a7
	rts
; stacksize=84
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1399
	movem.l	l1402,-(a7)
	move.w	#511,d0
	and.w	(6+l1404,a7),d0
	ext.l	d0
	move.w	#255,d1
	and.w	(8+l1404,a7),d1
	ext.l	d1
	moveq	#9,d2
	lsl.l	d2,d1
	or.l	d1,d0
	move.w	(4+l1404,a7),d1
	ext.l	d1
	and.l	#63,d1
	moveq	#17,d2
	lsl.l	d2,d1
	or.l	d1,d0
	move.l	#511,d1
	and.l	(10+l1404,a7),d1
	moveq	#23,d2
	lsl.l	d2,d1
	or.l	d1,d0
l1400
l1402	reg	d2
	movem.l	(a7)+,d2
l1404	equ	4
	rts
; stacksize=4
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1405
	subq.w	#4,a7
	movem.l	l1414,-(a7)
	moveq	#15,d0
	and.w	(8+l1416,a7),d0
	bne	l1409
l1408
	move.w	(8+l1416,a7),d0
	bra	l1410
l1409
	moveq	#-16,d0
	and.w	(8+l1416,a7),d0
l1410
	move.w	d0,(0+l1416,a7)
	moveq	#15,d0
	and.w	(8+l1416,a7),d0
	bne	l1412
l1411
	moveq	#16,d0
	bra	l1413
l1412
	moveq	#32,d0
l1413
	add.w	(0+l1416,a7),d0
	move.w	d0,(2+l1416,a7)
	move.w	#319,-(a7)
	move.w	#0,-(a7)
	move.w	(4+l1416,a7),-(a7)
	jsr	l15
	move.l	(18+l1416,a7),a0
	move.w	d0,(a0)
	move.w	#199,-(a7)
	move.w	#0,-(a7)
	move.w	(20+l1416,a7),-(a7)
	jsr	l15
	move.l	(24+l1416,a7),a0
	move.w	d0,(2,a0)
	move.w	#320,-(a7)
	move.w	#0,-(a7)
	move.w	(18+l1416,a7),-(a7)
	jsr	l15
	move.l	(30+l1416,a7),a0
	sub.w	(a0),d0
	move.l	(30+l1416,a7),a0
	move.w	d0,(4,a0)
	move.w	#200,-(a7)
	move.w	#0,-(a7)
	moveq	#16,d0
	add.w	(32+l1416,a7),d0
	move.w	d0,-(a7)
	jsr	l15
	move.l	(36+l1416,a7),a0
	sub.w	(2,a0),d0
	move.l	(36+l1416,a7),a0
	move.w	d0,(6,a0)
	add.w	#24,a7
l1406
l1414	reg
l1416	equ	0
	addq.w	#4,a7
	rts
; stacksize=32
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1417
	subq.w	#8,a7
	movem.l	l1429,-(a7)
	move.l	(12+l1431,a7),a0
	tst.b	(24,a0)
	beq	l1420
l1422
	move.l	(12+l1431,a7),a0
	tst.w	(22,a0)
	bne	l1421
l1420
	moveq	#0,d0
	bra	l1418
l1421
	move.w	#288,-(a7)
	move.w	#0,-(a7)
	move.l	(16+l1431,a7),a0
	move.l	(4,a0),d0
	asr.l	#8,d0
	move.w	d0,-(a7)
	jsr	l15
	move.w	d0,(6+l1431,a7)
	move.w	#184,-(a7)
	move.w	#0,-(a7)
	move.l	(22+l1431,a7),a0
	move.l	(8,a0),d0
	asr.l	#8,d0
	move.w	d0,-(a7)
	jsr	l15
	move.w	d0,(14+l1431,a7)
	move.l	(24+l1431,a7),a0
	move.w	(22,a0),d0
	ext.l	d0
	lsl.l	#1,d0
	lea	l6,a0
	move.w	(0,a0,d0.l),(16+l1431,a7)
	move.l	(28+l1431,a7),-(a7)
	move.w	(18+l1431,a7),-(a7)
	move.w	(18+l1431,a7),-(a7)
	jsr	l1405
	move.l	(32+l1431,a7),a0
	move.w	(22,a0),d0
	ext.l	d0
	move.l	d0,-(a7)
	move.w	(26+l1431,a7),-(a7)
	move.w	(26+l1431,a7),-(a7)
	move.w	(32+l1431,a7),-(a7)
	jsr	l1399
	move.l	(50+l1431,a7),a0
	move.l	d0,(a0)
	add.w	#30,a7
	tst.l	(24+l1431,a7)
	beq	l1424
	move.l	(24+l1431,a7),a0
	move.w	(0+l1431,a7),(a0)
l1424
	tst.l	(28+l1431,a7)
	beq	l1426
	move.l	(28+l1431,a7),a0
	move.w	(2+l1431,a7),(a0)
l1426
	tst.l	(32+l1431,a7)
	beq	l1428
	move.l	(32+l1431,a7),a0
	move.w	(4+l1431,a7),(a0)
l1428
	moveq	#1,d0
l1418
l1429	reg
l1431	equ	0
	addq.w	#8,a7
	rts
; stacksize=64
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1432
	subq.w	#8,a7
	movem.l	l1444,-(a7)
	move.l	(20+l1446,a7),a0
	tst.b	(28,a0)
	beq	l1435
l1437
	move.l	(12+l1446,a7),a0
	add.w	#3168,a0
	move.w	(16+l1446,a7),d0
	ext.l	d0
	lsl.l	#4,d0
	add.l	d0,a0
	add.w	(18+l1446,a7),a0
	tst.b	(a0)
	beq	l1436
l1435
	moveq	#0,d0
	bra	l1433
l1436
	move.w	#288,-(a7)
	move.w	#0,-(a7)
	move.l	(24+l1446,a7),a0
	move.l	(4,a0),d0
	asr.l	#8,d0
	move.w	d0,-(a7)
	jsr	l15
	move.w	d0,(6+l1446,a7)
	move.w	#184,-(a7)
	move.w	#0,-(a7)
	move.l	(30+l1446,a7),a0
	move.l	(8,a0),d0
	asr.l	#8,d0
	move.w	d0,-(a7)
	jsr	l15
	move.w	d0,(14+l1446,a7)
	move.l	(32+l1446,a7),-(a7)
	jsr	l120
	move.w	d0,(20+l1446,a7)
	move.l	(40+l1446,a7),-(a7)
	move.w	(22+l1446,a7),-(a7)
	move.w	(22+l1446,a7),-(a7)
	jsr	l1405
	move.l	(44+l1446,a7),a0
	move.w	(14,a0),d0
	ext.l	d0
	and.l	#15,d0
	move.l	(44+l1446,a7),a0
	move.w	(22,a0),d1
	ext.l	d1
	and.l	#3,d1
	lsl.l	#4,d1
	or.l	d1,d0
	move.l	d0,-(a7)
	move.w	(30+l1446,a7),-(a7)
	move.w	(30+l1446,a7),-(a7)
	move.w	(36+l1446,a7),-(a7)
	jsr	l1399
	move.l	(62+l1446,a7),a0
	move.l	d0,(a0)
	add.w	#34,a7
	tst.l	(32+l1446,a7)
	beq	l1439
	move.l	(32+l1446,a7),a0
	move.w	(0+l1446,a7),(a0)
l1439
	tst.l	(36+l1446,a7)
	beq	l1441
	move.l	(36+l1446,a7),a0
	move.w	(2+l1446,a7),(a0)
l1441
	tst.l	(40+l1446,a7)
	beq	l1443
	move.l	(40+l1446,a7),a0
	move.w	(4+l1446,a7),(a0)
l1443
	moveq	#1,d0
l1433
l1444	reg
l1446	equ	0
	addq.w	#8,a7
	rts
; stacksize=68
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1447
	sub.w	#12,a7
	movem.l	l1472,-(a7)
	move.l	(16+l1474,a7),a0
	tst.b	(30,a0)
	bne	l1451
l1450
	moveq	#0,d0
	bra	l1448
l1451
	move.l	(16+l1474,a7),a0
	tst.w	(26,a0)
	ble	l1453
l1454
	move.l	(16+l1474,a7),a0
	move.w	(26,a0),d0
	bge	l1475
	addq.w	#3,d0
l1475:
	asr.w	#2,d0
	and.w	#1,d0
	beq	l1453
l1452
	moveq	#0,d0
	bra	l1448
l1453
	move.l	(16+l1474,a7),a0
	cmp.w	#1,(18,a0)
	beq	l1457
l1455
	move.l	(16+l1474,a7),a0
	cmp.w	#2,(18,a0)
	beq	l1457
l1456
	moveq	#0,d0
	bra	l1458
l1457
	moveq	#1,d0
l1458
	move.b	d0,(0+l1474,a7)
	move.l	(16+l1474,a7),a0
	move.l	(16+l1474,a7),a1
	move.l	(4,a0),d0
	add.l	(8,a1),d0
	moveq	#9,d1
	asr.l	d1,d0
	move.w	d0,(2+l1474,a7)
	tst.b	(0+l1474,a7)
	beq	l1461
l1459
	moveq	#1,d0
	and.w	(2+l1474,a7),d0
	beq	l1461
l1460
	moveq	#1,d0
	bra	l1462
l1461
	moveq	#0,d0
l1462
	move.b	d0,(4+l1474,a7)
	move.l	(16+l1474,a7),a0
	move.l	(4,a0),d0
	asr.l	#8,d0
	move.w	d0,(6+l1474,a7)
	move.l	(16+l1474,a7),a0
	move.l	(8,a0),d0
	asr.l	#8,d0
	move.w	d0,(8+l1474,a7)
	move.l	(16+l1474,a7),a0
	cmp.w	#2,(18,a0)
	bne	l1464
l1465
	move.l	(16+l1474,a7),a0
	tst.w	(22,a0)
	ble	l1464
l1463
	move.l	(16+l1474,a7),a0
	move.w	(12,a0),-(a7)
	jsr	l25
	add.w	d0,(8+l1474,a7)
	move.l	(18+l1474,a7),a0
	move.w	(12,a0),-(a7)
	jsr	l35
	add.w	d0,(12+l1474,a7)
	addq.w	#4,a7
l1464
	move.w	#288,-(a7)
	move.w	#0,-(a7)
	move.w	(10+l1474,a7),-(a7)
	jsr	l15
	move.w	d0,(12+l1474,a7)
	move.w	#184,-(a7)
	move.w	#0,-(a7)
	move.w	(18+l1474,a7),-(a7)
	jsr	l15
	move.w	d0,(20+l1474,a7)
	moveq	#0,d0
	move.b	(16+l1474,a7),d0
	move.w	d0,-(a7)
	move.l	(30+l1474,a7),a0
	move.w	(12,a0),-(a7)
	jsr	l96
	move.w	d0,(26+l1474,a7)
	move.l	(36+l1474,a7),-(a7)
	move.w	(28+l1474,a7),-(a7)
	move.w	(28+l1474,a7),-(a7)
	jsr	l1405
	move.l	(40+l1474,a7),a0
	move.w	(18,a0),d0
	ext.l	d0
	and.l	#15,d0
	move.l	(40+l1474,a7),a0
	move.w	(12,a0),d1
	ext.l	d1
	and.l	#7,d1
	lsl.l	#4,d1
	or.l	d1,d0
	move.l	d0,-(a7)
	move.w	(36+l1474,a7),-(a7)
	move.w	(36+l1474,a7),-(a7)
	move.w	(42+l1474,a7),-(a7)
	jsr	l1399
	move.l	(58+l1474,a7),a0
	move.l	d0,(a0)
	add.w	#34,a7
	tst.l	(28+l1474,a7)
	beq	l1467
	move.l	(28+l1474,a7),a0
	move.w	(6+l1474,a7),(a0)
l1467
	tst.l	(32+l1474,a7)
	beq	l1469
	move.l	(32+l1474,a7),a0
	move.w	(8+l1474,a7),(a0)
l1469
	tst.l	(36+l1474,a7)
	beq	l1471
	move.l	(36+l1474,a7),a0
	move.w	(10+l1474,a7),(a0)
l1471
	moveq	#1,d0
l1448
l1472	reg
l1474	equ	0
	add.w	#12,a7
	rts
; stacksize=72
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1476
	sub.w	#16,a7
	movem.l	l1500,-(a7)
	move.w	#0,(0+l1502,a7)
	bra	l1480
l1479
	move.l	#0,-(a7)
	move.l	#0,-(a7)
	move.l	#0,-(a7)
	lea	(22+l1502,a7),a0
	move.l	a0,-(a7)
	lea	(18+l1502,a7),a0
	move.l	a0,-(a7)
	move.l	(46+l1502,a7),a0
	add.w	#2486,a0
	move.w	(20+l1502,a7),d0
	ext.l	d0
	moveq	#26,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	pea	(0,a0,d0.l)
	jsr	l1417
	move.b	d0,(38+l1502,a7)
	add.w	#24,a7
	beq	l1484
l1483
	lea	(2+l1502,a7),a0
	bra	l1485
l1484
	move.l	#0,a0
l1485
	move.l	a0,-(a7)
	move.l	(14+l1502,a7),-(a7)
	moveq	#0,d0
	move.b	(22+l1502,a7),d0
	move.w	d0,-(a7)
	move.l	(30+l1502,a7),a0
	add.w	#2496,a0
	move.w	(34+l1502,a7),d0
	ext.l	d0
	moveq	#112,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	add.l	d0,a0
	move.w	(10+l1502,a7),d0
	ext.l	d0
	moveq	#14,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	pea	(0,a0,d0.l)
	move.w	(38+l1502,a7),-(a7)
	move.l	(36+l1502,a7),-(a7)
	jsr	l1352
	add.w	#20,a7
l1482
	addq.w	#1,(0+l1502,a7)
l1480
	cmp.w	#8,(0+l1502,a7)
	blt	l1479
l1481
	move.w	#0,(0+l1502,a7)
	bra	l1487
l1486
	move.l	(26+l1502,a7),a0
	move.w	(0+l1502,a7),d0
	cmp.w	(2484,a0),d0
	bge	l1492
l1490
	move.l	#0,-(a7)
	move.l	#0,-(a7)
	move.l	#0,-(a7)
	lea	(22+l1502,a7),a0
	move.l	a0,-(a7)
	lea	(18+l1502,a7),a0
	move.l	a0,-(a7)
	move.l	(46+l1502,a7),a0
	add.w	#2004,a0
	move.w	(20+l1502,a7),d0
	ext.l	d0
	moveq	#30,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	pea	(0,a0,d0.l)
	move.w	(24+l1502,a7),-(a7)
	move.w	(50+l1502,a7),-(a7)
	move.l	(48+l1502,a7),-(a7)
	jsr	l1432
	add.w	#32,a7
	tst.b	d0
	beq	l1492
l1491
	moveq	#1,d0
	bra	l1493
l1492
	moveq	#0,d0
l1493
	move.b	d0,(14+l1502,a7)
	beq	l1495
l1494
	lea	(2+l1502,a7),a0
	bra	l1496
l1495
	move.l	#0,a0
l1496
	move.l	a0,-(a7)
	move.l	(14+l1502,a7),-(a7)
	moveq	#0,d0
	move.b	(22+l1502,a7),d0
	move.w	d0,-(a7)
	move.l	(30+l1502,a7),a0
	add.w	#2720,a0
	move.w	(34+l1502,a7),d0
	ext.l	d0
	move.l	#224,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	add.l	d0,a0
	move.w	(10+l1502,a7),d0
	ext.l	d0
	moveq	#14,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	pea	(0,a0,d0.l)
	move.w	(38+l1502,a7),-(a7)
	move.l	(36+l1502,a7),-(a7)
	jsr	l1352
	add.w	#20,a7
l1489
	addq.w	#1,(0+l1502,a7)
l1487
	cmp.w	#16,(0+l1502,a7)
	blt	l1486
l1488
	move.l	#0,-(a7)
	move.l	#0,-(a7)
	move.l	#0,-(a7)
	lea	(22+l1502,a7),a0
	move.l	a0,-(a7)
	lea	(18+l1502,a7),a0
	move.l	a0,-(a7)
	move.l	(46+l1502,a7),a0
	pea	(1972,a0)
	jsr	l1447
	move.b	d0,(38+l1502,a7)
	add.w	#24,a7
	beq	l1498
l1497
	lea	(2+l1502,a7),a0
	bra	l1499
l1498
	move.l	#0,a0
l1499
	move.l	a0,-(a7)
	move.l	(14+l1502,a7),-(a7)
	moveq	#0,d0
	move.b	(22+l1502,a7),d0
	move.w	d0,-(a7)
	move.l	(30+l1502,a7),a0
	add.w	#2468,a0
	move.w	(34+l1502,a7),d0
	ext.l	d0
	moveq	#14,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	pea	(0,a0,d0.l)
	move.w	(38+l1502,a7),-(a7)
	move.l	(36+l1502,a7),-(a7)
	jsr	l1352
	add.w	#20,a7
l1477
l1500	reg	d2/d3
	movem.l	(a7)+,d2/d3
l1502	equ	8
	add.w	#16,a7
	rts
; stacksize=132
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1503
	subq.w	#8,a7
	movem.l	l1511,-(a7)
	move.l	(12+l1513,a7),a0
	move.w	(3090,a0),-(a7)
	jsr	l57
	move.b	d0,(6+l1513,a7)
	move.l	(14+l1513,a7),a0
	addq.w	#4,a0
	move.w	(20+l1513,a7),d0
	ext.l	d0
	moveq	#40,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	add.l	d0,a0
	move.w	(18+l1513,a7),d0
	ext.l	d0
	lsl.l	#1,d0
	add.l	d0,a0
	move.b	(1,a0),d0
	move.b	d0,(2+l1513,a7)
	move.l	(14+l1513,a7),a0
	add.w	#484,a0
	move.w	(20+l1513,a7),d0
	ext.l	d0
	moveq	#40,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	add.l	d0,a0
	move.w	(18+l1513,a7),d0
	ext.l	d0
	lsl.l	#1,d0
	add.l	d0,a0
	move.b	(1,a0),d0
	move.b	d0,(3+l1513,a7)
	addq.w	#2,a7
	tst.b	(4+l1513,a7)
	beq	l1507
l1506
	move.l	(12+l1513,a7),a0
	add.w	#964,a0
	move.w	(18+l1513,a7),d0
	ext.l	d0
	moveq	#40,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	add.l	d0,a0
	move.w	(16+l1513,a7),d0
	ext.l	d0
	lsl.l	#1,d0
	add.l	d0,a0
	move.w	(a0),d0
	bra	l1508
l1507
	moveq	#0,d0
l1508
	move.b	d0,(2+l1513,a7)
	move.b	#255,(3+l1513,a7)
	move.l	(12+l1513,a7),a0
	add.w	#1444,a0
	move.w	(18+l1513,a7),d0
	ext.l	d0
	moveq	#20,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	add.l	d0,a0
	add.w	(16+l1513,a7),a0
	tst.b	(a0)
	beq	l1510
	move.l	(12+l1513,a7),a0
	move.l	(12+l1513,a7),a1
	moveq	#3,d0
	muls.w	(3096,a1),d0
	add.w	(3082,a0),d0
	moveq	#3,d1
	muls.w	(16+l1513,a7),d1
	add.w	d1,d0
	moveq	#5,d1
	muls.w	(18+l1513,a7),d1
	add.w	d1,d0
	move.w	d0,(6+l1513,a7)
	move.w	(6+l1513,a7),-(a7)
	move.l	(14+l1513,a7),a0
	add.w	#1684,a0
	move.w	(20+l1513,a7),d0
	ext.l	d0
	moveq	#20,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	add.l	d0,a0
	add.w	(18+l1513,a7),a0
	moveq	#0,d0
	move.b	(a0),d0
	move.w	d0,-(a7)
	jsr	l77
	move.b	d0,(7+l1513,a7)
	addq.w	#4,a7
l1510
	movem.l	(0+l1513,a7),d0
l1504
l1511	reg	d2/d3
	movem.l	(a7)+,d2/d3
l1513	equ	8
	addq.w	#8,a7
	rts
; stacksize=24
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1514
	movem.l	l1522,-(a7)
	cmp.w	#1,(4+l1524,a7)
	beq	l1519
l1521
	cmp.w	#2,(4+l1524,a7)
	beq	l1519
l1517
	cmp.w	#3,(4+l1524,a7)
	beq	l1519
l1518
	moveq	#0,d0
	bra	l1520
l1519
	moveq	#1,d0
l1520
l1515
l1522	reg
l1524	equ	0
	rts
; stacksize=0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1525
	movem.l	l1534,-(a7)
	move.w	#31,-(a7)
	move.w	(12+l1536,a7),-(a7)
	move.w	(12+l1536,a7),-(a7)
	move.l	(10+l1536,a7),-(a7)
	jsr	l881
	add.w	#10,a7
	cmp.w	#1,(12+l1536,a7)
	blt	l1529
l1528
	move.w	#30,-(a7)
	move.w	(12+l1536,a7),-(a7)
	move.w	(12+l1536,a7),d0
	subq.w	#1,d0
	move.w	d0,-(a7)
	move.l	(10+l1536,a7),-(a7)
	jsr	l881
	move.w	#30,-(a7)
	move.w	(22+l1536,a7),-(a7)
	moveq	#1,d0
	add.w	(22+l1536,a7),d0
	move.w	d0,-(a7)
	move.l	(20+l1536,a7),-(a7)
	jsr	l881
	move.w	#30,-(a7)
	move.w	(32+l1536,a7),d0
	subq.w	#1,d0
	move.w	d0,-(a7)
	move.w	(32+l1536,a7),-(a7)
	move.l	(30+l1536,a7),-(a7)
	jsr	l881
	move.w	#30,-(a7)
	moveq	#1,d0
	add.w	(42+l1536,a7),d0
	move.w	d0,-(a7)
	move.w	(42+l1536,a7),-(a7)
	move.l	(40+l1536,a7),-(a7)
	jsr	l881
	add.w	#40,a7
l1529
	cmp.w	#2,(12+l1536,a7)
	blt	l1531
l1530
	move.w	#29,-(a7)
	move.w	(12+l1536,a7),-(a7)
	move.w	(12+l1536,a7),d0
	subq.w	#2,d0
	move.w	d0,-(a7)
	move.l	(10+l1536,a7),-(a7)
	jsr	l881
	move.w	#29,-(a7)
	move.w	(22+l1536,a7),-(a7)
	moveq	#2,d0
	add.w	(22+l1536,a7),d0
	move.w	d0,-(a7)
	move.l	(20+l1536,a7),-(a7)
	jsr	l881
	move.w	#29,-(a7)
	move.w	(32+l1536,a7),d0
	subq.w	#2,d0
	move.w	d0,-(a7)
	move.w	(32+l1536,a7),-(a7)
	move.l	(30+l1536,a7),-(a7)
	jsr	l881
	move.w	#29,-(a7)
	moveq	#2,d0
	add.w	(42+l1536,a7),d0
	move.w	d0,-(a7)
	move.w	(42+l1536,a7),-(a7)
	move.l	(40+l1536,a7),-(a7)
	jsr	l881
	move.w	#30,-(a7)
	move.w	(52+l1536,a7),d0
	subq.w	#1,d0
	move.w	d0,-(a7)
	move.w	(52+l1536,a7),d0
	subq.w	#1,d0
	move.w	d0,-(a7)
	move.l	(50+l1536,a7),-(a7)
	jsr	l881
	move.w	#30,-(a7)
	move.w	(62+l1536,a7),d0
	subq.w	#1,d0
	move.w	d0,-(a7)
	moveq	#1,d0
	add.w	(62+l1536,a7),d0
	move.w	d0,-(a7)
	move.l	(60+l1536,a7),-(a7)
	jsr	l881
	move.w	#30,-(a7)
	moveq	#1,d0
	add.w	(72+l1536,a7),d0
	move.w	d0,-(a7)
	move.w	(72+l1536,a7),d0
	subq.w	#1,d0
	move.w	d0,-(a7)
	move.l	(70+l1536,a7),-(a7)
	jsr	l881
	move.w	#30,-(a7)
	moveq	#1,d0
	add.w	(82+l1536,a7),d0
	move.w	d0,-(a7)
	moveq	#1,d0
	add.w	(82+l1536,a7),d0
	move.w	d0,-(a7)
	move.l	(80+l1536,a7),-(a7)
	jsr	l881
	add.w	#80,a7
l1531
	cmp.w	#3,(12+l1536,a7)
	blt	l1533
l1532
	move.w	#29,-(a7)
	move.w	(12+l1536,a7),-(a7)
	move.w	(12+l1536,a7),d0
	subq.w	#3,d0
	move.w	d0,-(a7)
	move.l	(10+l1536,a7),-(a7)
	jsr	l881
	move.w	#29,-(a7)
	move.w	(22+l1536,a7),-(a7)
	moveq	#3,d0
	add.w	(22+l1536,a7),d0
	move.w	d0,-(a7)
	move.l	(20+l1536,a7),-(a7)
	jsr	l881
	move.w	#29,-(a7)
	move.w	(32+l1536,a7),d0
	subq.w	#3,d0
	move.w	d0,-(a7)
	move.w	(32+l1536,a7),-(a7)
	move.l	(30+l1536,a7),-(a7)
	jsr	l881
	move.w	#29,-(a7)
	moveq	#3,d0
	add.w	(42+l1536,a7),d0
	move.w	d0,-(a7)
	move.w	(42+l1536,a7),-(a7)
	move.l	(40+l1536,a7),-(a7)
	jsr	l881
	move.w	#29,-(a7)
	move.w	(52+l1536,a7),d0
	subq.w	#2,d0
	move.w	d0,-(a7)
	move.w	(52+l1536,a7),d0
	subq.w	#2,d0
	move.w	d0,-(a7)
	move.l	(50+l1536,a7),-(a7)
	jsr	l881
	move.w	#29,-(a7)
	move.w	(62+l1536,a7),d0
	subq.w	#2,d0
	move.w	d0,-(a7)
	moveq	#2,d0
	add.w	(62+l1536,a7),d0
	move.w	d0,-(a7)
	move.l	(60+l1536,a7),-(a7)
	jsr	l881
	move.w	#29,-(a7)
	moveq	#2,d0
	add.w	(72+l1536,a7),d0
	move.w	d0,-(a7)
	move.w	(72+l1536,a7),d0
	subq.w	#2,d0
	move.w	d0,-(a7)
	move.l	(70+l1536,a7),-(a7)
	jsr	l881
	move.w	#29,-(a7)
	moveq	#2,d0
	add.w	(82+l1536,a7),d0
	move.w	d0,-(a7)
	moveq	#2,d0
	add.w	(82+l1536,a7),d0
	move.w	d0,-(a7)
	move.l	(80+l1536,a7),-(a7)
	jsr	l881
	add.w	#80,a7
l1533
l1526
l1534	reg
l1536	equ	0
	rts
; stacksize=118
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1537
	sub.w	#12,a7
	movem.l	l1554,-(a7)
	move.w	(24+l1556,a7),d0
	ext.l	d0
	divs.w	#3,d0
	move.w	d0,(0+l1556,a7)
	move.w	(0+l1556,a7),d0
	ext.l	d0
	divs.w	#192,d0
	swap	d0
	move.w	d0,(2+l1556,a7)
	move.w	#0,(4+l1556,a7)
	moveq	#6,d0
	add.w	(20+l1556,a7),d0
	move.w	d0,(6+l1556,a7)
	moveq	#6,d0
	add.w	(22+l1556,a7),d0
	move.w	d0,(8+l1556,a7)
	cmp.w	#4,(2+l1556,a7)
	bge	l1541
l1540
	cmp.w	#2,(2+l1556,a7)
	bge	l1543
l1542
	moveq	#1,d0
	add.w	(2+l1556,a7),d0
	bra	l1544
l1543
	moveq	#4,d0
	sub.w	(2+l1556,a7),d0
l1544
	move.w	d0,(4+l1556,a7)
	bra	l1545
l1541
	cmp.w	#96,(2+l1556,a7)
	blt	l1547
l1548
	cmp.w	#100,(2+l1556,a7)
	bge	l1547
l1546
	cmp.w	#98,(2+l1556,a7)
	bge	l1550
l1549
	move.w	(2+l1556,a7),d0
	sub.w	#95,d0
	bra	l1551
l1550
	moveq	#100,d0
	sub.w	(2+l1556,a7),d0
l1551
	move.w	d0,(4+l1556,a7)
	moveq	#10,d0
	add.w	(20+l1556,a7),d0
	move.w	d0,(6+l1556,a7)
	moveq	#9,d0
	add.w	(22+l1556,a7),d0
	move.w	d0,(8+l1556,a7)
l1547
l1545
	tst.w	(4+l1556,a7)
	ble	l1553
l1552
	move.w	(4+l1556,a7),-(a7)
	move.w	(10+l1556,a7),-(a7)
	move.w	(10+l1556,a7),-(a7)
	move.l	(22+l1556,a7),-(a7)
	jsr	l1525
	add.w	#10,a7
l1553
l1538
l1554	reg
l1556	equ	0
	add.w	#12,a7
	rts
; stacksize=144
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1557
	subq.w	#4,a7
	movem.l	l1568,-(a7)
	move.w	#0,(0+l1570,a7)
	bra	l1561
l1560
	move.w	#31,-(a7)
	move.w	(16+l1570,a7),-(a7)
	move.w	(16+l1570,a7),d0
	add.w	(4+l1570,a7),d0
	move.w	d0,-(a7)
	move.l	(14+l1570,a7),-(a7)
	jsr	l881
	move.w	#31,-(a7)
	move.w	(26+l1570,a7),d0
	add.w	(12+l1570,a7),d0
	move.w	d0,-(a7)
	move.w	(26+l1570,a7),-(a7)
	move.l	(24+l1570,a7),-(a7)
	jsr	l881
	move.w	#1,-(a7)
	moveq	#16,d0
	add.w	(36+l1570,a7),d0
	subq.w	#1,d0
	move.w	d0,-(a7)
	move.w	(36+l1570,a7),d0
	add.w	(24+l1570,a7),d0
	move.w	d0,-(a7)
	move.l	(34+l1570,a7),-(a7)
	jsr	l881
	move.w	#1,-(a7)
	move.w	(46+l1570,a7),d0
	add.w	(32+l1570,a7),d0
	move.w	d0,-(a7)
	moveq	#16,d0
	add.w	(46+l1570,a7),d0
	subq.w	#1,d0
	move.w	d0,-(a7)
	move.l	(44+l1570,a7),-(a7)
	jsr	l881
	add.w	#40,a7
l1563
	addq.w	#1,(0+l1570,a7)
l1561
	cmp.w	#16,(0+l1570,a7)
	blt	l1560
l1562
	move.w	#1,(0+l1570,a7)
	bra	l1565
l1564
	move.w	#9,-(a7)
	moveq	#1,d0
	add.w	(16+l1570,a7),d0
	move.w	d0,-(a7)
	move.w	(16+l1570,a7),d0
	add.w	(4+l1570,a7),d0
	move.w	d0,-(a7)
	move.l	(14+l1570,a7),-(a7)
	jsr	l881
	move.w	#9,-(a7)
	move.w	(26+l1570,a7),d0
	add.w	(12+l1570,a7),d0
	move.w	d0,-(a7)
	moveq	#1,d0
	add.w	(26+l1570,a7),d0
	move.w	d0,-(a7)
	move.l	(24+l1570,a7),-(a7)
	jsr	l881
	move.w	#3,-(a7)
	moveq	#16,d0
	add.w	(36+l1570,a7),d0
	subq.w	#2,d0
	move.w	d0,-(a7)
	move.w	(36+l1570,a7),d0
	add.w	(24+l1570,a7),d0
	move.w	d0,-(a7)
	move.l	(34+l1570,a7),-(a7)
	jsr	l881
	move.w	#3,-(a7)
	move.w	(46+l1570,a7),d0
	add.w	(32+l1570,a7),d0
	move.w	d0,-(a7)
	moveq	#16,d0
	add.w	(46+l1570,a7),d0
	subq.w	#2,d0
	move.w	d0,-(a7)
	move.l	(44+l1570,a7),-(a7)
	jsr	l881
	add.w	#40,a7
l1567
	addq.w	#1,(0+l1570,a7)
l1565
	cmp.w	#14,(0+l1570,a7)
	blt	l1564
l1566
l1558
l1568	reg
l1570	equ	0
	addq.w	#4,a7
	rts
; stacksize=82
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1571
	movem.l	l1579,-(a7)
	tst.w	(8+l1581,a7)
	bne	l1575
l1574
	bra	l1572
l1575
	move.w	#31,-(a7)
	moveq	#7,d0
	add.w	(14+l1581,a7),d0
	move.w	d0,-(a7)
	moveq	#7,d0
	add.w	(14+l1581,a7),d0
	move.w	d0,-(a7)
	move.l	(10+l1581,a7),-(a7)
	jsr	l881
	move.w	#31,-(a7)
	moveq	#8,d0
	add.w	(24+l1581,a7),d0
	move.w	d0,-(a7)
	moveq	#8,d0
	add.w	(24+l1581,a7),d0
	move.w	d0,-(a7)
	move.l	(20+l1581,a7),-(a7)
	jsr	l881
	move.w	#30,-(a7)
	moveq	#8,d0
	add.w	(34+l1581,a7),d0
	move.w	d0,-(a7)
	moveq	#6,d0
	add.w	(34+l1581,a7),d0
	move.w	d0,-(a7)
	move.l	(30+l1581,a7),-(a7)
	jsr	l881
	move.w	#30,-(a7)
	moveq	#7,d0
	add.w	(44+l1581,a7),d0
	move.w	d0,-(a7)
	moveq	#9,d0
	add.w	(44+l1581,a7),d0
	move.w	d0,-(a7)
	move.l	(40+l1581,a7),-(a7)
	jsr	l881
	add.w	#40,a7
	tst.b	(15+l1581,a7)
	beq	l1577
l1578
	cmp.w	#1,(8+l1581,a7)
	bne	l1577
l1576
	move.w	2+l2,d0
	moveq	#5,d1
	muls.w	(10+l1581,a7),d1
	add.w	d1,d0
	moveq	#3,d1
	muls.w	(12+l1581,a7),d1
	add.w	d1,d0
	move.w	d0,-(a7)
	move.w	(14+l1581,a7),-(a7)
	move.w	(14+l1581,a7),-(a7)
	move.l	(10+l1581,a7),-(a7)
	jsr	l1537
	add.w	#10,a7
l1577
l1572
l1579	reg
l1581	equ	0
	rts
; stacksize=158
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1582
	subq.w	#4,a7
	movem.l	l1596,-(a7)
	moveq	#16,d0
	add.w	(14+l1598,a7),d0
	subq.w	#2,d0
	move.w	d0,(2+l1598,a7)
	move.w	#3,(0+l1598,a7)
	bra	l1586
l1585
	moveq	#1,d0
	and.w	(0+l1598,a7),d0
	beq	l1590
l1589
	moveq	#2,d0
	bra	l1591
l1590
	moveq	#1,d0
l1591
	move.w	d0,-(a7)
	move.w	(4+l1598,a7),-(a7)
	move.w	(16+l1598,a7),d0
	add.w	(4+l1598,a7),d0
	move.w	d0,-(a7)
	move.l	(14+l1598,a7),-(a7)
	jsr	l881
	add.w	#10,a7
l1588
	addq.w	#1,(0+l1598,a7)
l1586
	cmp.w	#12,(0+l1598,a7)
	ble	l1585
l1587
	move.w	#5,(0+l1598,a7)
	bra	l1593
l1592
	move.w	#1,-(a7)
	moveq	#1,d0
	add.w	(4+l1598,a7),d0
	move.w	d0,-(a7)
	move.w	(16+l1598,a7),d0
	add.w	(4+l1598,a7),d0
	move.w	d0,-(a7)
	move.l	(14+l1598,a7),-(a7)
	jsr	l881
	add.w	#10,a7
l1595
	addq.w	#1,(0+l1598,a7)
l1593
	cmp.w	#10,(0+l1598,a7)
	ble	l1592
l1594
l1583
l1596	reg
l1598	equ	0
	addq.w	#4,a7
	rts
; stacksize=52
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1599
	subq.w	#4,a7
	movem.l	l1605,-(a7)
	moveq	#1,d0
	and.w	(16+l1607,a7),d0
	bne	l1603
l1602
	moveq	#23,d0
	bra	l1604
l1603
	moveq	#30,d0
l1604
	move.b	d0,(0+l1607,a7)
	moveq	#0,d0
	move.b	(0+l1607,a7),d0
	move.w	d0,-(a7)
	moveq	#1,d0
	add.w	(16+l1607,a7),d0
	move.w	d0,-(a7)
	moveq	#1,d0
	add.w	(16+l1607,a7),d0
	move.w	d0,-(a7)
	move.l	(14+l1607,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(10+l1607,a7),d0
	move.w	d0,-(a7)
	moveq	#1,d0
	add.w	(26+l1607,a7),d0
	move.w	d0,-(a7)
	moveq	#2,d0
	add.w	(26+l1607,a7),d0
	move.w	d0,-(a7)
	move.l	(24+l1607,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(20+l1607,a7),d0
	move.w	d0,-(a7)
	moveq	#2,d0
	add.w	(36+l1607,a7),d0
	move.w	d0,-(a7)
	moveq	#1,d0
	add.w	(36+l1607,a7),d0
	move.w	d0,-(a7)
	move.l	(34+l1607,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(30+l1607,a7),d0
	move.w	d0,-(a7)
	moveq	#1,d0
	add.w	(46+l1607,a7),d0
	move.w	d0,-(a7)
	moveq	#16,d0
	add.w	(46+l1607,a7),d0
	subq.w	#2,d0
	move.w	d0,-(a7)
	move.l	(44+l1607,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(40+l1607,a7),d0
	move.w	d0,-(a7)
	moveq	#1,d0
	add.w	(56+l1607,a7),d0
	move.w	d0,-(a7)
	moveq	#16,d0
	add.w	(56+l1607,a7),d0
	subq.w	#3,d0
	move.w	d0,-(a7)
	move.l	(54+l1607,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(50+l1607,a7),d0
	move.w	d0,-(a7)
	moveq	#2,d0
	add.w	(66+l1607,a7),d0
	move.w	d0,-(a7)
	moveq	#16,d0
	add.w	(66+l1607,a7),d0
	subq.w	#2,d0
	move.w	d0,-(a7)
	move.l	(64+l1607,a7),-(a7)
	jsr	l881
	add.w	#60,a7
l1600
l1605	reg
l1607	equ	0
	addq.w	#4,a7
	rts
; stacksize=102
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1608
	sub.w	#16,a7
	movem.l	l1625,-(a7)
	tst.w	(28+l1627,a7)
	bgt	l1612
l1611
	bra	l1609
l1612
	moveq	#4,d0
	and.w	(28+l1627,a7),d0
	beq	l1614
l1613
	moveq	#13,d0
	bra	l1615
l1614
	moveq	#30,d0
l1615
	move.b	d0,(2+l1627,a7)
	move.w	(24+l1627,a7),(4+l1627,a7)
	move.w	(26+l1627,a7),(6+l1627,a7)
	moveq	#16,d0
	add.w	(24+l1627,a7),d0
	subq.w	#1,d0
	move.w	d0,(8+l1627,a7)
	moveq	#16,d0
	add.w	(26+l1627,a7),d0
	subq.w	#1,d0
	move.w	d0,(10+l1627,a7)
	move.w	#0,(0+l1627,a7)
	bra	l1617
l1616
	moveq	#0,d0
	move.b	(2+l1627,a7),d0
	move.w	d0,-(a7)
	move.w	(8+l1627,a7),-(a7)
	move.w	(8+l1627,a7),d0
	add.w	(4+l1627,a7),d0
	move.w	d0,-(a7)
	move.l	(26+l1627,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(12+l1627,a7),d0
	move.w	d0,-(a7)
	move.w	(18+l1627,a7),d0
	add.w	(12+l1627,a7),d0
	move.w	d0,-(a7)
	move.w	(18+l1627,a7),-(a7)
	move.l	(36+l1627,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(22+l1627,a7),d0
	move.w	d0,-(a7)
	move.w	(28+l1627,a7),-(a7)
	move.w	(32+l1627,a7),d0
	sub.w	(24+l1627,a7),d0
	move.w	d0,-(a7)
	move.l	(46+l1627,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(32+l1627,a7),d0
	move.w	d0,-(a7)
	move.w	(38+l1627,a7),d0
	add.w	(32+l1627,a7),d0
	move.w	d0,-(a7)
	move.w	(42+l1627,a7),-(a7)
	move.l	(56+l1627,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(42+l1627,a7),d0
	move.w	d0,-(a7)
	move.w	(52+l1627,a7),-(a7)
	move.w	(48+l1627,a7),d0
	add.w	(44+l1627,a7),d0
	move.w	d0,-(a7)
	move.l	(66+l1627,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(52+l1627,a7),d0
	move.w	d0,-(a7)
	move.w	(62+l1627,a7),d0
	sub.w	(52+l1627,a7),d0
	move.w	d0,-(a7)
	move.w	(58+l1627,a7),-(a7)
	move.l	(76+l1627,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(62+l1627,a7),d0
	move.w	d0,-(a7)
	move.w	(72+l1627,a7),-(a7)
	move.w	(72+l1627,a7),d0
	sub.w	(64+l1627,a7),d0
	move.w	d0,-(a7)
	move.l	(86+l1627,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(72+l1627,a7),d0
	move.w	d0,-(a7)
	move.w	(82+l1627,a7),d0
	sub.w	(72+l1627,a7),d0
	move.w	d0,-(a7)
	move.w	(82+l1627,a7),-(a7)
	move.l	(96+l1627,a7),-(a7)
	jsr	l881
	add.w	#80,a7
l1619
	addq.w	#1,(0+l1627,a7)
l1617
	cmp.w	#4,(0+l1627,a7)
	blt	l1616
l1618
	cmp.w	#10,(28+l1627,a7)
	bge	l1621
l1620
	moveq	#2,d0
	and.w	(28+l1627,a7),d0
	beq	l1623
l1622
	moveq	#30,d0
	bra	l1624
l1623
	moveq	#8,d0
l1624
	move.b	d0,(12+l1627,a7)
	moveq	#0,d0
	move.b	(12+l1627,a7),d0
	move.w	d0,-(a7)
	moveq	#8,d0
	add.w	(28+l1627,a7),d0
	move.w	d0,-(a7)
	moveq	#8,d0
	add.w	(28+l1627,a7),d0
	move.w	d0,-(a7)
	move.l	(26+l1627,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(22+l1627,a7),d0
	move.w	d0,-(a7)
	moveq	#8,d0
	add.w	(38+l1627,a7),d0
	move.w	d0,-(a7)
	moveq	#7,d0
	add.w	(38+l1627,a7),d0
	move.w	d0,-(a7)
	move.l	(36+l1627,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(32+l1627,a7),d0
	move.w	d0,-(a7)
	moveq	#7,d0
	add.w	(48+l1627,a7),d0
	move.w	d0,-(a7)
	moveq	#8,d0
	add.w	(48+l1627,a7),d0
	move.w	d0,-(a7)
	move.l	(46+l1627,a7),-(a7)
	jsr	l881
	add.w	#30,a7
l1621
l1609
l1625	reg
l1627	equ	0
	add.w	#16,a7
	rts
; stacksize=134
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1628
	subq.w	#4,a7
	movem.l	l1648,-(a7)
	moveq	#1,d0
	and.w	(18+l1650,a7),d0
	bne	l1632
l1631
	moveq	#1,d0
	bra	l1633
l1632
	moveq	#0,d0
l1633
	move.w	d0,(0+l1650,a7)
	move.w	(16+l1650,a7),d0
	subq.w	#1,d0
	beq	l1641
	subq.w	#1,d0
	beq	l1644
	subq.w	#1,d0
	beq	l1638
	subq.w	#1,d0
	beq	l1635
	bra	l1647
l1635
	move.w	#2,-(a7)
	moveq	#12,d0
	add.w	(16+l1650,a7),d0
	move.w	d0,-(a7)
	moveq	#2,d0
	add.w	(16+l1650,a7),d0
	move.w	d0,-(a7)
	move.l	(14+l1650,a7),-(a7)
	jsr	l881
	move.w	#30,-(a7)
	moveq	#11,d0
	add.w	(26+l1650,a7),d0
	move.w	d0,-(a7)
	moveq	#1,d0
	add.w	(26+l1650,a7),d0
	move.w	d0,-(a7)
	move.l	(24+l1650,a7),-(a7)
	jsr	l881
	add.w	#20,a7
	tst.w	(0+l1650,a7)
	beq	l1637
	move.w	#2,-(a7)
	moveq	#12,d0
	add.w	(16+l1650,a7),d0
	move.w	d0,-(a7)
	move.w	(16+l1650,a7),-(a7)
	move.l	(14+l1650,a7),-(a7)
	jsr	l881
	add.w	#10,a7
l1637
	bra	l1634
l1638
	move.w	#2,-(a7)
	moveq	#12,d0
	add.w	(16+l1650,a7),d0
	move.w	d0,-(a7)
	moveq	#13,d0
	add.w	(16+l1650,a7),d0
	move.w	d0,-(a7)
	move.l	(14+l1650,a7),-(a7)
	jsr	l881
	move.w	#30,-(a7)
	moveq	#11,d0
	add.w	(26+l1650,a7),d0
	move.w	d0,-(a7)
	moveq	#14,d0
	add.w	(26+l1650,a7),d0
	move.w	d0,-(a7)
	move.l	(24+l1650,a7),-(a7)
	jsr	l881
	add.w	#20,a7
	tst.w	(0+l1650,a7)
	beq	l1640
	move.w	#2,-(a7)
	moveq	#12,d0
	add.w	(16+l1650,a7),d0
	move.w	d0,-(a7)
	moveq	#15,d0
	add.w	(16+l1650,a7),d0
	move.w	d0,-(a7)
	move.l	(14+l1650,a7),-(a7)
	jsr	l881
	add.w	#10,a7
l1640
	bra	l1634
l1641
	move.w	#2,-(a7)
	moveq	#14,d0
	add.w	(16+l1650,a7),d0
	move.w	d0,-(a7)
	moveq	#7,d0
	add.w	(16+l1650,a7),d0
	move.w	d0,-(a7)
	move.l	(14+l1650,a7),-(a7)
	jsr	l881
	move.w	#30,-(a7)
	moveq	#15,d0
	add.w	(26+l1650,a7),d0
	move.w	d0,-(a7)
	moveq	#8,d0
	add.w	(26+l1650,a7),d0
	move.w	d0,-(a7)
	move.l	(24+l1650,a7),-(a7)
	jsr	l881
	add.w	#20,a7
	tst.w	(0+l1650,a7)
	beq	l1643
	move.w	#2,-(a7)
	moveq	#14,d0
	add.w	(16+l1650,a7),d0
	move.w	d0,-(a7)
	moveq	#9,d0
	add.w	(16+l1650,a7),d0
	move.w	d0,-(a7)
	move.l	(14+l1650,a7),-(a7)
	jsr	l881
	add.w	#10,a7
l1643
	bra	l1634
l1644
	move.w	#2,-(a7)
	moveq	#2,d0
	add.w	(16+l1650,a7),d0
	move.w	d0,-(a7)
	moveq	#7,d0
	add.w	(16+l1650,a7),d0
	move.w	d0,-(a7)
	move.l	(14+l1650,a7),-(a7)
	jsr	l881
	move.w	#30,-(a7)
	moveq	#1,d0
	add.w	(26+l1650,a7),d0
	move.w	d0,-(a7)
	moveq	#8,d0
	add.w	(26+l1650,a7),d0
	move.w	d0,-(a7)
	move.l	(24+l1650,a7),-(a7)
	jsr	l881
	add.w	#20,a7
	tst.w	(0+l1650,a7)
	beq	l1646
	move.w	#2,-(a7)
	moveq	#2,d0
	add.w	(16+l1650,a7),d0
	move.w	d0,-(a7)
	moveq	#9,d0
	add.w	(16+l1650,a7),d0
	move.w	d0,-(a7)
	move.l	(14+l1650,a7),-(a7)
	jsr	l881
	add.w	#10,a7
l1646
	bra	l1634
l1647
l1634
l1629
l1648	reg
l1650	equ	0
	addq.w	#4,a7
	rts
; stacksize=62
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1651
	subq.w	#4,a7
	movem.l	l1660,-(a7)
	move.w	(20+l1662,a7),d0
	lsl.w	#4,d0
	move.w	d0,(0+l1662,a7)
	move.w	(22+l1662,a7),d0
	lsl.w	#4,d0
	move.w	d0,(2+l1662,a7)
	move.w	(2+l1662,a7),-(a7)
	move.w	(2+l1662,a7),-(a7)
	move.l	(20+l1662,a7),a0
	moveq	#0,d0
	move.b	(a0),d0
	lsl.l	#1,d0
	lea	l5,a0
	move.w	(0,a0,d0.l),-(a7)
	move.l	(18+l1662,a7),-(a7)
	move.l	(18+l1662,a7),-(a7)
	jsr	l374
	move.l	(30+l1662,a7),a0
	add.w	#14,a7
	tst.b	(1,a0)
	beq	l1655
l1654
	move.w	(2+l1662,a7),-(a7)
	move.w	(2+l1662,a7),-(a7)
	move.l	(20+l1662,a7),a0
	moveq	#0,d0
	move.b	(1,a0),d0
	lsl.l	#1,d0
	lea	l6,a0
	move.w	(0,a0,d0.l),-(a7)
	move.l	(18+l1662,a7),-(a7)
	move.l	(18+l1662,a7),-(a7)
	jsr	l374
	add.w	#14,a7
l1655
	move.l	(16+l1662,a7),a0
	tst.b	(2,a0)
	beq	l1657
l1656
	move.w	(2+l1662,a7),-(a7)
	move.w	(2+l1662,a7),-(a7)
	move.l	(20+l1662,a7),a0
	moveq	#0,d0
	move.b	(2,a0),d0
	lsl.l	#1,d0
	lea	l7,a0
	move.w	(0,a0,d0.l),-(a7)
	move.l	(18+l1662,a7),-(a7)
	move.l	(18+l1662,a7),-(a7)
	jsr	l380
	add.w	#14,a7
l1657
	move.l	(16+l1662,a7),a0
	cmp.b	#255,(3,a0)
	beq	l1659
l1658
	move.w	(2+l1662,a7),-(a7)
	move.w	(2+l1662,a7),-(a7)
	move.l	(20+l1662,a7),a0
	moveq	#0,d0
	move.b	(3,a0),d0
	move.w	d0,-(a7)
	move.l	(18+l1662,a7),-(a7)
	move.l	(18+l1662,a7),-(a7)
	jsr	l380
	add.w	#14,a7
l1659
l1652
l1660	reg
l1662	equ	0
	addq.w	#4,a7
	rts
; stacksize=68
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1663
	movem.l	l1668,-(a7)
	move.l	(4+l1670,a7),a0
	add.w	#2400,a0
	add.w	(12+l1670,a7),a0
	tst.b	(a0)
	bne	l1667
l1666
	move.w	(12+l1670,a7),-(a7)
	move.l	(6+l1670,a7),-(a7)
	jsr	l1240
	move.l	(10+l1670,a7),a0
	add.w	#2400,a0
	add.w	(18+l1670,a7),a0
	move.b	#1,(a0)
	addq.w	#6,a7
l1667
l1664
l1668	reg
l1670	equ	0
	rts
; stacksize=22
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1671
	sub.w	#16,a7
	movem.l	l1692,-(a7)
	move.l	(24+l1694,a7),-(a7)
	jsr	_game_dirty_static_rows
	move.l	d0,(4+l1694,a7)
	addq.w	#4,a7
	bne	l1675
l1674
	bra	l1672
l1675
	move.w	#0,(6+l1694,a7)
	bra	l1677
l1676
	move.w	(6+l1694,a7),d0
	ext.l	d0
	lsl.l	#2,d0
	move.l	(0+l1694,a7),a0
	move.l	(0,a0,d0.l),(8+l1694,a7)
	bne	l1681
l1680
	bra	l1679
l1681
	move.w	#0,(4+l1694,a7)
	bra	l1683
l1682
	move.w	#0,(12+l1694,a7)
	bra	l1687
l1686
	move.w	(12+l1694,a7),d0
	ext.l	d0
	moveq	#1,d1
	lsl.l	d0,d1
	move.l	d1,d0
	and.l	(8+l1694,a7),d0
	beq	l1691
l1690
	move.w	(4+l1694,a7),d0
	ext.l	d0
	move.l	#240,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	move.l	(20+l1694,a7),a0
	add.l	d0,a0
	move.w	(6+l1694,a7),d0
	ext.l	d0
	moveq	#20,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	add.l	d0,a0
	add.w	(12+l1694,a7),a0
	move.b	#1,(a0)
l1691
l1689
	addq.w	#1,(12+l1694,a7)
l1687
	cmp.w	#20,(12+l1694,a7)
	blt	l1686
l1688
l1685
	addq.w	#1,(4+l1694,a7)
l1683
	cmp.w	#2,(4+l1694,a7)
	blt	l1682
l1684
l1679
	addq.w	#1,(6+l1694,a7)
l1677
	cmp.w	#12,(6+l1694,a7)
	blt	l1676
l1678
l1672
l1692	reg	d2/d3
	movem.l	(a7)+,d2/d3
l1694	equ	8
	add.w	#16,a7
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1695
	sub.w	#12,a7
	movem.l	l1708,-(a7)
	move.l	(16+l1710,a7),a0
	add.w	#4946,a0
	move.l	a0,(4+l1710,a7)
	move.w	#0,(2+l1710,a7)
	bra	l1699
l1698
	move.w	#0,(0+l1710,a7)
	bra	l1703
l1702
	move.w	(24+l1710,a7),d0
	ext.l	d0
	move.l	#240,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	move.l	(4+l1710,a7),a0
	add.l	d0,a0
	move.w	(2+l1710,a7),d0
	ext.l	d0
	moveq	#20,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	add.l	d0,a0
	add.w	(0+l1710,a7),a0
	tst.b	(a0)
	bne	l1707
l1706
	bra	l1705
l1707
	move.w	(2+l1710,a7),-(a7)
	move.w	(2+l1710,a7),-(a7)
	move.l	(30+l1710,a7),-(a7)
	jsr	l1503
	movem.l	d0,(16+l1710,a7)
	move.l	(12+l1710,a7),a0
	add.w	#480,a0
	move.w	(32+l1710,a7),d0
	ext.l	d0
	move.l	#960,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	add.l	d0,a0
	move.w	(10+l1710,a7),d0
	ext.l	d0
	moveq	#80,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	add.l	d0,a0
	move.w	(8+l1710,a7),d0
	ext.l	d0
	lsl.l	#2,d0
	move.l	(16+l1710,a7),(0,a0,d0.l)
	move.w	(10+l1710,a7),-(a7)
	move.w	(10+l1710,a7),-(a7)
	move.l	(16+l1710,a7),a0
	add.w	#480,a0
	move.w	(36+l1710,a7),d0
	ext.l	d0
	move.l	#960,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	add.l	d0,a0
	move.w	(14+l1710,a7),d0
	ext.l	d0
	moveq	#80,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	add.l	d0,a0
	move.w	(12+l1710,a7),d0
	ext.l	d0
	lsl.l	#2,d0
	pea	(0,a0,d0.l)
	move.l	(36+l1710,a7),-(a7)
	move.l	(36+l1710,a7),-(a7)
	jsr	l1651
	move.w	(48+l1710,a7),d0
	ext.l	d0
	move.l	#240,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	move.l	(28+l1710,a7),a0
	add.l	d0,a0
	move.w	(26+l1710,a7),d0
	ext.l	d0
	moveq	#20,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	add.l	d0,a0
	add.w	(24+l1710,a7),a0
	move.b	#0,(a0)
	add.w	#24,a7
l1705
	addq.w	#1,(0+l1710,a7)
l1703
	cmp.w	#20,(0+l1710,a7)
	blt	l1702
l1704
l1701
	addq.w	#1,(2+l1710,a7)
l1699
	cmp.w	#12,(2+l1710,a7)
	blt	l1698
l1700
l1696
l1708	reg	d2/d3
	movem.l	(a7)+,d2/d3
l1710	equ	8
	add.w	#12,a7
	rts
; stacksize=116
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1711
	movem.l	l1714,-(a7)
	moveq	#16,d2
	moveq	#0,d0
	move.l	(8+l1716,a7),a0
	add.w	#3168,a0
	move.w	(12+l1716,a7),d1
	ext.l	d1
	lsl.l	#4,d1
	add.l	d1,a0
	inline
	move.l	a0,a1
	cmp.l	#16,d2
	blo	.l3
	move.l	a0,d1
	and.b	#1,d1
	beq	.l1
	move.b	d0,(a0)+
	subq.l	#1,d2
.l1
	move.b	d0,d1
	lsl.w	#8,d0
	move.b	d1,d0
	move.w	d0,d1
	swap	d0
	move.w	d1,d0
	moveq	#3,d1
	and.l	d2,d1
	sub.l	d1,d2
.l2
	move.l	d0,(a0)+
	subq.l	#4,d2
	bne	.l2
	move.w	d1,d2
.l3
	subq.w	#1,d2
	blo	.l5
.l4
	move.b	d0,(a0)+
	dbf	d2,.l4
.l5
	move.l	a1,d0
	einline
	move.l	d0,a1
l1712
l1714	reg	d2
	movem.l	(a7)+,d2
l1716	equ	4
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1717
	movem.l	l1720,-(a7)
	moveq	#0,d0
	move.b	(13+l1722,a7),d0
	move.w	d0,-(a7)
	move.w	(12+l1722,a7),-(a7)
	move.w	(12+l1722,a7),-(a7)
	move.l	(10+l1722,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(23+l1722,a7),d0
	move.w	d0,-(a7)
	move.w	(22+l1722,a7),-(a7)
	move.w	(22+l1722,a7),d0
	subq.w	#1,d0
	move.w	d0,-(a7)
	move.l	(20+l1722,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(33+l1722,a7),d0
	move.w	d0,-(a7)
	move.w	(32+l1722,a7),-(a7)
	moveq	#1,d0
	add.w	(32+l1722,a7),d0
	move.w	d0,-(a7)
	move.l	(30+l1722,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(43+l1722,a7),d0
	move.w	d0,-(a7)
	move.w	(42+l1722,a7),d0
	subq.w	#1,d0
	move.w	d0,-(a7)
	move.w	(42+l1722,a7),-(a7)
	move.l	(40+l1722,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(53+l1722,a7),d0
	move.w	d0,-(a7)
	moveq	#1,d0
	add.w	(52+l1722,a7),d0
	move.w	d0,-(a7)
	move.w	(52+l1722,a7),-(a7)
	move.l	(50+l1722,a7),-(a7)
	jsr	l881
	add.w	#50,a7
l1718
l1720	reg
l1722	equ	0
	rts
; stacksize=88
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1723
	sub.w	#20,a7
	movem.l	l1749,-(a7)
	jsr	l290
	move.w	#0,(0+l1751,a7)
	bra	l1727
l1726
	move.l	(38+l1751,a7),a0
	add.w	#2694,a0
	move.w	(0+l1751,a7),d0
	ext.l	d0
	moveq	#14,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	add.l	d0,a0
	move.l	a0,(2+l1751,a7)
	move.l	(2+l1751,a7),a0
	tst.b	(12,a0)
	beq	l1730
l1732
	move.l	(2+l1751,a7),a0
	tst.w	(10,a0)
	bgt	l1731
l1730
	bra	l1729
l1731
	move.l	(2+l1751,a7),a0
	cmp.w	#10,(10,a0)
	bge	l1734
l1735
	move.l	(2+l1751,a7),a0
	moveq	#1,d0
	and.w	(10,a0),d0
	beq	l1734
l1733
	bra	l1729
l1734
	move.l	(2+l1751,a7),a0
	move.w	(8,a0),(6+l1751,a7)
	bge	l1737
l1736
	move.w	#0,(6+l1751,a7)
l1737
	cmp.w	#9999,(6+l1751,a7)
	ble	l1739
l1738
	move.w	#9999,(6+l1751,a7)
l1739
	move.w	#0,-(a7)
	move.w	(8+l1751,a7),d0
	ext.l	d0
	move.l	d0,-(a7)
	jsr	l1154
	move.w	d0,(14+l1751,a7)
	moveq	#1,d0
	and.w	(6+l1751,a7),d0
	addq.w	#6,a7
	bne	l1741
l1740
	moveq	#-2,d0
	bra	l1742
l1741
	moveq	#2,d0
l1742
	move.w	d0,(10+l1751,a7)
	moveq	#1,d0
	and.w	(0+l1751,a7),d0
	move.l	(2+l1751,a7),a0
	move.w	(10,a0),d1
	ext.l	d1
	divs.w	#6,d1
	and.w	#1,d1
	add.w	d1,d0
	move.w	d0,(12+l1751,a7)
	move.w	#320,d0
	sub.w	(8+l1751,a7),d0
	subq.w	#2,d0
	move.w	d0,-(a7)
	move.w	#1,-(a7)
	move.l	(6+l1751,a7),a0
	move.l	(a0),d0
	asr.l	#8,d0
	move.w	(12+l1751,a7),d1
	bge	l1752
	addq.w	#1,d1
l1752:
	asr.w	#1,d1
	ext.l	d1
	sub.l	d1,d0
	move.w	(14+l1751,a7),d1
	ext.l	d1
	add.l	d1,d0
	move.w	d0,-(a7)
	jsr	l15
	move.w	d0,(20+l1751,a7)
	move.w	#184,-(a7)
	move.w	#1,-(a7)
	move.l	(12+l1751,a7),a0
	move.l	(4,a0),d0
	asr.l	#8,d0
	subq.l	#3,d0
	move.w	(22+l1751,a7),d1
	ext.l	d1
	sub.l	d1,d0
	move.w	d0,-(a7)
	jsr	l15
	move.w	d0,(28+l1751,a7)
	move.w	#10,-(a7)
	moveq	#4,d0
	add.w	(22+l1751,a7),d0
	move.w	d0,-(a7)
	move.w	(32+l1751,a7),d0
	subq.w	#2,d0
	move.w	d0,-(a7)
	move.w	(32+l1751,a7),d0
	subq.w	#2,d0
	move.w	d0,-(a7)
	move.l	(54+l1751,a7),-(a7)
	move.w	(56+l1751,a7),-(a7)
	move.l	(54+l1751,a7),-(a7)
	move.l	(54+l1751,a7),-(a7)
	jsr	l1371
	move.w	#1,-(a7)
	move.l	(38+l1751,a7),a0
	cmp.w	#800,(8,a0)
	blt	l1744
l1743
	moveq	#8,d0
	bra	l1745
l1744
	move.l	(38+l1751,a7),a0
	cmp.w	#400,(8,a0)
	blt	l1747
l1746
	moveq	#12,d1
	bra	l1748
l1747
	moveq	#30,d1
l1748
	move.w	d1,d0
l1745
	move.w	d0,-(a7)
	move.w	#0,-(a7)
	move.w	(46+l1751,a7),d0
	ext.l	d0
	move.l	d0,-(a7)
	move.w	(60+l1751,a7),-(a7)
	move.w	(60+l1751,a7),-(a7)
	move.l	(82+l1751,a7),-(a7)
	jsr	l1148
	add.w	#52,a7
l1729
	addq.w	#1,(0+l1751,a7)
l1727
	cmp.w	#8,(0+l1751,a7)
	blt	l1726
l1728
l1724
l1749	reg	d2/d3
	movem.l	(a7)+,d2/d3
l1751	equ	8
	add.w	#20,a7
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1753
	sub.w	#20,a7
	movem.l	l1776,-(a7)
	move.w	#0,(0+l1778,a7)
	bra	l1757
l1756
	move.l	(38+l1778,a7),a0
	add.w	#2806,a0
	move.w	(0+l1778,a7),d0
	ext.l	d0
	moveq	#22,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	add.l	d0,a0
	move.l	a0,(2+l1778,a7)
	move.l	(2+l1778,a7),a0
	tst.b	(21,a0)
	beq	l1760
l1762
	move.l	(2+l1778,a7),a0
	tst.w	(18,a0)
	bgt	l1761
l1760
	bra	l1759
l1761
	move.l	(2+l1778,a7),a0
	cmp.w	#4,(18,a0)
	bge	l1764
l1765
	move.l	(2+l1778,a7),a0
	moveq	#1,d0
	and.w	(18,a0),d0
	beq	l1764
l1763
	bra	l1759
l1764
	move.l	(2+l1778,a7),a0
	move.l	(a0),d0
	asr.l	#8,d0
	move.w	d0,(6+l1778,a7)
	move.l	(2+l1778,a7),a0
	move.l	(4,a0),d0
	asr.l	#8,d0
	move.w	d0,(8+l1778,a7)
	move.l	(2+l1778,a7),a0
	cmp.w	#7,(8,a0)
	beq	l1768
l1766
	move.l	(2+l1778,a7),a0
	cmp.w	#8,(8,a0)
	beq	l1768
l1767
	moveq	#0,d0
	bra	l1769
l1768
	moveq	#1,d0
l1769
	move.b	d0,(10+l1778,a7)
	beq	l1771
	move.l	(2+l1778,a7),a0
	move.l	(2+l1778,a7),a1
	move.w	(10,a0),d0
	sub.w	(18,a1),d0
	move.w	d0,(12+l1778,a7)
	move.w	#288,-(a7)
	move.w	#0,-(a7)
	move.w	(10+l1778,a7),d0
	subq.w	#8,d0
	move.w	d0,-(a7)
	jsr	l15
	move.w	d0,(20+l1778,a7)
	move.w	#184,-(a7)
	move.w	#0,-(a7)
	move.w	(18+l1778,a7),d0
	subq.w	#8,d0
	cmp.w	#2,(22+l1778,a7)
	ble	l1773
l1772
	move.w	(22+l1778,a7),d1
	subq.w	#2,d1
	tst.w	d1
	bge	l1779
	addq.w	#3,d1
l1779:
	asr.w	#2,d1
	bra	l1774
l1773
	moveq	#0,d1
l1774
	add.w	d1,d0
	move.w	d0,-(a7)
	jsr	l15
	move.w	d0,(28+l1778,a7)
	move.w	#16,-(a7)
	move.w	#32,-(a7)
	move.w	(32+l1778,a7),-(a7)
	move.w	(32+l1778,a7),-(a7)
	move.l	(54+l1778,a7),-(a7)
	move.w	(56+l1778,a7),-(a7)
	move.l	(54+l1778,a7),-(a7)
	move.l	(54+l1778,a7),-(a7)
	jsr	l1371
	move.w	(50+l1778,a7),-(a7)
	move.w	(50+l1778,a7),-(a7)
	move.l	(40+l1778,a7),-(a7)
	jsr	l137
	addq.w	#4,a7
	move.w	d0,-(a7)
	move.l	(74+l1778,a7),-(a7)
	move.l	(68+l1778,a7),-(a7)
	jsr	l380
	add.w	#48,a7
	bra	l1775
l1771
l1775
l1759
	addq.w	#1,(0+l1778,a7)
l1757
	cmp.w	#12,(0+l1778,a7)
	blt	l1756
l1758
l1754
l1776	reg	d2/d3
	movem.l	(a7)+,d2/d3
l1778	equ	8
	add.w	#20,a7
	rts
; stacksize=124
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1780
	sub.w	#28,a7
	movem.l	l1813,-(a7)
	move.w	#0,(0+l1815,a7)
	bra	l1784
l1783
	move.l	(46+l1815,a7),a0
	add.w	#2486,a0
	move.w	(0+l1815,a7),d0
	ext.l	d0
	moveq	#26,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	add.l	d0,a0
	move.l	a0,(2+l1815,a7)
	move.l	(36+l1815,a7),a0
	add.w	#2496,a0
	move.w	(40+l1815,a7),d0
	ext.l	d0
	moveq	#112,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	add.l	d0,a0
	move.w	(0+l1815,a7),d0
	ext.l	d0
	moveq	#14,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	add.l	d0,a0
	move.l	a0,(6+l1815,a7)
	lea	(22+l1815,a7),a0
	move.l	a0,-(a7)
	lea	(30+l1815,a7),a0
	move.l	a0,-(a7)
	lea	(32+l1815,a7),a0
	move.l	a0,-(a7)
	lea	(30+l1815,a7),a0
	move.l	a0,-(a7)
	lea	(26+l1815,a7),a0
	move.l	a0,-(a7)
	move.l	(22+l1815,a7),-(a7)
	jsr	l1417
	add.w	#24,a7
	tst.b	d0
	bne	l1788
l1787
	bra	l1786
l1788
	move.l	(6+l1815,a7),a0
	tst.b	(a0)
	beq	l1790
l1792
	move.l	(6+l1815,a7),a0
	move.l	(2,a0),d0
	cmp.l	(18+l1815,a7),d0
	bne	l1790
l1791
	lea	(10+l1815,a7),a0
	move.l	a0,-(a7)
	move.l	(10+l1815,a7),a0
	pea	(6,a0)
	jsr	l1296
	addq.w	#8,a7
	tst.b	d0
	beq	l1790
l1789
	bra	l1786
l1790
	move.w	(26+l1815,a7),-(a7)
	move.w	(26+l1815,a7),-(a7)
	move.w	(26+l1815,a7),-(a7)
	move.l	(48+l1815,a7),-(a7)
	move.l	(42+l1815,a7),-(a7)
	jsr	l380
	move.l	(20+l1815,a7),a0
	lea	(24+l1815,a7),a1
	lea	(6,a0),a2
	move.l	(a1)+,(a2)+
	move.l	(a1)+,(a2)+
	move.l	(20+l1815,a7),a0
	move.l	(32+l1815,a7),(2,a0)
	move.l	(20+l1815,a7),a0
	move.b	#1,(a0)
	add.w	#14,a7
l1786
	addq.w	#1,(0+l1815,a7)
l1784
	cmp.w	#8,(0+l1815,a7)
	blt	l1783
l1785
	move.w	#0,(0+l1815,a7)
	bra	l1794
l1793
	move.l	(46+l1815,a7),a0
	add.w	#2004,a0
	move.w	(0+l1815,a7),d0
	ext.l	d0
	moveq	#30,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	add.l	d0,a0
	move.l	a0,(2+l1815,a7)
	move.l	(36+l1815,a7),a0
	add.w	#2720,a0
	move.w	(40+l1815,a7),d0
	ext.l	d0
	move.l	#224,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	add.l	d0,a0
	move.w	(0+l1815,a7),d0
	ext.l	d0
	moveq	#14,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	add.l	d0,a0
	move.l	a0,(6+l1815,a7)
	lea	(18+l1815,a7),a0
	move.l	a0,-(a7)
	lea	(30+l1815,a7),a0
	move.l	a0,-(a7)
	lea	(32+l1815,a7),a0
	move.l	a0,-(a7)
	lea	(32+l1815,a7),a0
	move.l	a0,-(a7)
	lea	(26+l1815,a7),a0
	move.l	a0,-(a7)
	move.l	(22+l1815,a7),-(a7)
	move.w	(24+l1815,a7),-(a7)
	move.w	(66+l1815,a7),-(a7)
	move.l	(64+l1815,a7),-(a7)
	jsr	l1432
	add.w	#32,a7
	tst.b	d0
	bne	l1798
l1797
	bra	l1796
l1798
	move.l	(6+l1815,a7),a0
	tst.b	(a0)
	beq	l1800
l1802
	move.l	(6+l1815,a7),a0
	move.l	(2,a0),d0
	cmp.l	(20+l1815,a7),d0
	bne	l1800
l1801
	lea	(10+l1815,a7),a0
	move.l	a0,-(a7)
	move.l	(10+l1815,a7),a0
	pea	(6,a0)
	jsr	l1296
	addq.w	#8,a7
	tst.b	d0
	beq	l1800
l1799
	bra	l1796
l1800
	move.w	(26+l1815,a7),-(a7)
	move.w	(26+l1815,a7),-(a7)
	move.w	(22+l1815,a7),-(a7)
	move.l	(48+l1815,a7),-(a7)
	move.l	(42+l1815,a7),-(a7)
	jsr	l380
	move.l	(16+l1815,a7),a0
	add.w	#14,a7
	tst.w	(14,a0)
	bne	l1804
l1805
	move.l	(2+l1815,a7),a0
	tst.w	(24,a0)
	ble	l1804
l1803
l1804
l1806
	move.l	(6+l1815,a7),a0
	lea	(10+l1815,a7),a1
	lea	(6,a0),a2
	move.l	(a1)+,(a2)+
	move.l	(a1)+,(a2)+
	move.l	(6+l1815,a7),a0
	move.l	(20+l1815,a7),(2,a0)
	move.l	(6+l1815,a7),a0
	move.b	#1,(a0)
l1796
	addq.w	#1,(0+l1815,a7)
l1794
	move.l	(46+l1815,a7),a0
	move.w	(0+l1815,a7),d0
	cmp.w	(2484,a0),d0
	blt	l1793
l1795
	move.l	(36+l1815,a7),a0
	add.w	#2468,a0
	move.w	(40+l1815,a7),d0
	ext.l	d0
	moveq	#14,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	add.l	d0,a0
	move.l	a0,(2+l1815,a7)
	lea	(18+l1815,a7),a0
	move.l	a0,-(a7)
	lea	(26+l1815,a7),a0
	move.l	a0,-(a7)
	lea	(28+l1815,a7),a0
	move.l	a0,-(a7)
	lea	(26+l1815,a7),a0
	move.l	a0,-(a7)
	lea	(22+l1815,a7),a0
	move.l	a0,-(a7)
	move.l	(66+l1815,a7),a0
	pea	(1972,a0)
	jsr	l1447
	add.w	#24,a7
	tst.b	d0
	beq	l1808
	move.l	(2+l1815,a7),a0
	tst.b	(a0)
	beq	l1810
l1812
	move.l	(2+l1815,a7),a0
	move.l	(2,a0),d0
	cmp.l	(14+l1815,a7),d0
	bne	l1810
l1811
	lea	(6+l1815,a7),a0
	move.l	a0,-(a7)
	move.l	(6+l1815,a7),a0
	pea	(6,a0)
	jsr	l1296
	addq.w	#8,a7
	tst.b	d0
	beq	l1810
l1809
	bra	l1781
l1810
	move.w	(22+l1815,a7),-(a7)
	move.w	(22+l1815,a7),-(a7)
	move.w	(22+l1815,a7),-(a7)
	move.l	(48+l1815,a7),-(a7)
	move.l	(42+l1815,a7),-(a7)
	jsr	l380
	move.l	(16+l1815,a7),a0
	lea	(20+l1815,a7),a1
	lea	(6,a0),a2
	move.l	(a1)+,(a2)+
	move.l	(a1)+,(a2)+
	move.l	(16+l1815,a7),a0
	move.l	(28+l1815,a7),(2,a0)
	move.l	(16+l1815,a7),a0
	move.b	#1,(a0)
	add.w	#14,a7
l1808
l1781
l1813	reg	a2/d2/d3
	movem.l	(a7)+,a2/d2/d3
l1815	equ	12
	add.w	#28,a7
	rts
; stacksize=144
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1816
	subq.w	#4,a7
	movem.l	l1823,-(a7)
	move.w	#193,(0+l1825,a7)
	bra	l1820
l1819
	moveq	#0,d0
	move.b	(15+l1825,a7),d0
	move.w	d0,-(a7)
	move.w	(2+l1825,a7),-(a7)
	move.w	(16+l1825,a7),-(a7)
	move.l	(14+l1825,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(27+l1825,a7),d0
	move.w	d0,-(a7)
	move.w	(12+l1825,a7),-(a7)
	moveq	#1,d0
	add.w	(26+l1825,a7),d0
	move.w	d0,-(a7)
	move.l	(24+l1825,a7),-(a7)
	jsr	l881
	add.w	#20,a7
l1822
	addq.w	#1,(0+l1825,a7)
l1820
	cmp.w	#199,(0+l1825,a7)
	blt	l1819
l1821
l1817
l1823	reg
l1825	equ	0
	addq.w	#4,a7
	rts
; stacksize=62
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1826
	sub.w	#12,a7
	movem.l	l1840,-(a7)
	move.w	#8,-(a7)
	move.w	#0,-(a7)
	move.w	(28+l1842,a7),-(a7)
	jsr	l15
	move.w	d0,(8+l1842,a7)
	move.w	#8,-(a7)
	move.w	#0,-(a7)
	move.w	(36+l1842,a7),-(a7)
	jsr	l15
	move.w	d0,(16+l1842,a7)
	move.w	#0,(12+l1842,a7)
	add.w	#12,a7
	bra	l1830
l1829
	move.w	(0+l1842,a7),d0
	ext.l	d0
	divs.w	#4,d0
	swap	d0
	muls.w	#3,d0
	add.w	(20+l1842,a7),d0
	move.w	d0,(6+l1842,a7)
	move.w	(0+l1842,a7),d0
	bge	l1843
	addq.w	#3,d0
l1843:
	asr.w	#2,d0
	muls.w	#3,d0
	add.w	(22+l1842,a7),d0
	move.w	d0,(8+l1842,a7)
	move.b	#2,(10+l1842,a7)
	move.w	(0+l1842,a7),d0
	cmp.w	(4+l1842,a7),d0
	bge	l1834
l1833
	move.b	#3,(10+l1842,a7)
l1834
	move.w	(0+l1842,a7),d0
	cmp.w	(2+l1842,a7),d0
	bge	l1836
l1835
	move.w	(28+l1842,a7),d0
	bge	l1844
	addq.w	#3,d0
l1844:
	asr.w	#2,d0
	add.w	(0+l1842,a7),d0
	and.w	#1,d0
	bne	l1838
l1837
	moveq	#30,d0
	bra	l1839
l1838
	moveq	#22,d0
l1839
	move.b	d0,(10+l1842,a7)
l1836
	moveq	#0,d0
	move.b	(10+l1842,a7),d0
	move.w	d0,-(a7)
	move.w	(10+l1842,a7),-(a7)
	move.w	(10+l1842,a7),-(a7)
	move.l	(22+l1842,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(20+l1842,a7),d0
	move.w	d0,-(a7)
	move.w	(20+l1842,a7),-(a7)
	moveq	#1,d0
	add.w	(20+l1842,a7),d0
	move.w	d0,-(a7)
	move.l	(32+l1842,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(30+l1842,a7),d0
	move.w	d0,-(a7)
	moveq	#1,d0
	add.w	(30+l1842,a7),d0
	move.w	d0,-(a7)
	move.w	(30+l1842,a7),-(a7)
	move.l	(42+l1842,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(40+l1842,a7),d0
	move.w	d0,-(a7)
	moveq	#1,d0
	add.w	(40+l1842,a7),d0
	move.w	d0,-(a7)
	moveq	#1,d0
	add.w	(40+l1842,a7),d0
	move.w	d0,-(a7)
	move.l	(52+l1842,a7),-(a7)
	jsr	l881
	add.w	#40,a7
l1832
	addq.w	#1,(0+l1842,a7)
l1830
	cmp.w	#8,(0+l1842,a7)
	blt	l1829
l1831
l1827
l1840	reg
l1842	equ	0
	add.w	#12,a7
	rts
; stacksize=90
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1845
	movem.l	l1854,-(a7)
	move.l	(4+l1856,a7),a0
	tst.b	(3095,a0)
	beq	l1849
l1848
	move.l	(4+l1856,a7),a0
	move.w	(3082,a0),d0
	bge	l1857
	addq.w	#3,d0
l1857:
	asr.w	#2,d0
	and.w	#1,d0
	beq	l1852
l1851
	moveq	#23,d0
	bra	l1853
l1852
	moveq	#12,d0
l1853
	bra	l1850
l1849
	moveq	#14,d0
l1850
l1846
l1854	reg
l1856	equ	0
	rts
; stacksize=0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1858
	movem.l	l1868,-(a7)
	move.l	(4+l1870,a7),a0
	tst.w	(3108,a0)
	bgt	l1862
l1861
	moveq	#3,d0
	bra	l1859
l1862
	move.l	(4+l1870,a7),a0
	tst.w	(3110,a0)
	bgt	l1864
l1863
	moveq	#23,d0
	bra	l1859
l1864
	move.l	(4+l1870,a7),a0
	move.w	(3082,a0),d0
	ext.l	d0
	divs.w	#5,d0
	and.w	#1,d0
	bne	l1866
l1865
	moveq	#30,d0
	bra	l1867
l1866
	moveq	#22,d0
l1867
l1859
l1868	reg
l1870	equ	0
	rts
; stacksize=0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1871
	movem.l	l1874,-(a7)
	move.w	#1,-(a7)
	move.w	#198,-(a7)
	move.w	(14+l1876,a7),-(a7)
	move.w	#192,-(a7)
	move.w	(16+l1876,a7),-(a7)
	move.l	(14+l1876,a7),-(a7)
	jsr	l920
	add.w	#14,a7
l1872
l1874	reg
l1876	equ	0
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1877
	subq.w	#4,a7
	movem.l	l1887,-(a7)
	move.l	(12+l1889,a7),a0
	tst.w	(3082,a0)
	ble	l1881
l1880
	move.l	(12+l1889,a7),a0
	move.w	(3082,a0),d0
	ext.l	d0
	divs.w	#50,d0
	bra	l1882
l1881
	moveq	#0,d0
l1882
	move.w	d0,(0+l1889,a7)
	move.l	(12+l1889,a7),-(a7)
	jsr	l1845
	move.b	d0,(6+l1889,a7)
	move.l	(16+l1889,a7),-(a7)
	jsr	l1858
	move.b	d0,(11+l1889,a7)
	move.w	#1,-(a7)
	move.w	#29,-(a7)
	move.w	#6,-(a7)
	move.l	(26+l1889,a7),a0
	cmp.l	#99999999,(3074,a0)
	bls	l1884
l1883
	move.l	#99999999,d0
	bra	l1885
l1884
	move.l	(26+l1889,a7),a0
	move.l	(3074,a0),d0
l1885
	move.l	d0,-(a7)
	move.w	#193,-(a7)
	move.w	#34,-(a7)
	move.l	(30+l1889,a7),-(a7)
	jsr	l1148
	move.w	#1,-(a7)
	move.w	#18,-(a7)
	move.w	#2,-(a7)
	move.l	(44+l1889,a7),a0
	move.w	(3080,a0),d0
	ext.l	d0
	move.l	d0,-(a7)
	move.w	#193,-(a7)
	move.w	#161,-(a7)
	move.l	(48+l1889,a7),-(a7)
	jsr	l1148
	move.w	#1,-(a7)
	moveq	#0,d0
	move.b	(48+l1889,a7),d0
	move.w	d0,-(a7)
	move.w	#2,-(a7)
	move.w	(50+l1889,a7),d0
	ext.l	d0
	move.l	d0,-(a7)
	move.w	#193,-(a7)
	move.w	#201,-(a7)
	move.l	(66+l1889,a7),-(a7)
	jsr	l1148
	move.w	#1,-(a7)
	moveq	#0,d0
	move.b	(67+l1889,a7),d0
	move.w	d0,-(a7)
	move.w	#1,-(a7)
	move.w	#99,-(a7)
	move.w	#0,-(a7)
	move.l	(84+l1889,a7),a0
	move.w	(3110,a0),-(a7)
	jsr	l15
	addq.w	#6,a7
	ext.l	d0
	move.l	d0,-(a7)
	move.w	#193,-(a7)
	move.w	#244,-(a7)
	move.l	(84+l1889,a7),-(a7)
	jsr	l1148
	move.w	#1,-(a7)
	move.w	#7,-(a7)
	pea	l1886
	move.w	#193,-(a7)
	move.w	#248,-(a7)
	move.l	(100+l1889,a7),-(a7)
	jsr	l1048
	move.w	#1,-(a7)
	moveq	#0,d0
	move.b	(101+l1889,a7),d0
	move.w	d0,-(a7)
	move.w	#1,-(a7)
	move.w	#99,-(a7)
	move.w	#0,-(a7)
	move.l	(118+l1889,a7),a0
	move.w	(3108,a0),-(a7)
	jsr	l15
	addq.w	#6,a7
	ext.l	d0
	move.l	d0,-(a7)
	move.w	#193,-(a7)
	move.w	#252,-(a7)
	move.l	(118+l1889,a7),-(a7)
	jsr	l1148
	move.l	(126+l1889,a7),a0
	move.w	(3082,a0),-(a7)
	move.l	(128+l1889,a7),a0
	move.w	(3108,a0),-(a7)
	move.l	(130+l1889,a7),a0
	move.w	(3110,a0),-(a7)
	move.w	#193,-(a7)
	move.w	#255,-(a7)
	move.l	(132+l1889,a7),-(a7)
	jsr	l1826
	move.w	#1,-(a7)
	move.w	#23,-(a7)
	move.w	#2,-(a7)
	move.w	#99,-(a7)
	move.w	#0,-(a7)
	move.l	(150+l1889,a7),a0
	move.w	(3078,a0),-(a7)
	jsr	l15
	addq.w	#6,a7
	ext.l	d0
	move.l	d0,-(a7)
	move.w	#193,-(a7)
	move.w	#287,-(a7)
	move.l	(150+l1889,a7),-(a7)
	jsr	l1148
	add.w	#146,a7
l1878
l1887	reg
l1889	equ	0
	addq.w	#4,a7
	rts
	cnop	0,4
l1886
	dc.b	58
	dc.b	0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1890
	subq.w	#4,a7
	movem.l	l1908,-(a7)
	jsr	l290
	move.l	(16+l1910,a7),a0
	move.l	(20+l1910,a7),a1
	move.l	(a0),d0
	cmp.l	(a1),d0
	beq	l1894
l1893
	move.w	#140,-(a7)
	move.w	#33,-(a7)
	move.l	(12+l1910,a7),-(a7)
	jsr	l1871
	move.w	#1,-(a7)
	move.w	#29,-(a7)
	move.w	#6,-(a7)
	move.l	(34+l1910,a7),a0
	move.l	(a0),-(a7)
	move.w	#193,-(a7)
	move.w	#34,-(a7)
	move.l	(30+l1910,a7),-(a7)
	jsr	l1148
	add.w	#26,a7
l1894
	move.l	(16+l1910,a7),a0
	move.l	(20+l1910,a7),a1
	move.w	(4,a0),d0
	cmp.w	(4,a1),d0
	beq	l1896
l1895
	move.w	#173,-(a7)
	move.w	#160,-(a7)
	move.l	(12+l1910,a7),-(a7)
	jsr	l1871
	move.w	#1,-(a7)
	move.w	#18,-(a7)
	move.w	#2,-(a7)
	move.l	(34+l1910,a7),a0
	moveq	#0,d0
	move.w	(4,a0),d0
	move.l	d0,-(a7)
	move.w	#193,-(a7)
	move.w	#161,-(a7)
	move.l	(30+l1910,a7),-(a7)
	jsr	l1148
	add.w	#26,a7
l1896
	move.l	(16+l1910,a7),a0
	move.l	(20+l1910,a7),a1
	move.w	(6,a0),d0
	cmp.w	(6,a1),d0
	bne	l1897
l1900
	move.l	(16+l1910,a7),a0
	move.l	(20+l1910,a7),a1
	move.b	(14,a0),d0
	cmp.b	(14,a1),d0
	bne	l1897
l1899
	move.l	(16+l1910,a7),a0
	move.l	(20+l1910,a7),a1
	move.b	(15,a0),d0
	cmp.b	(15,a1),d0
	beq	l1898
l1897
	move.w	#216,-(a7)
	move.w	#200,-(a7)
	move.l	(12+l1910,a7),-(a7)
	jsr	l1871
	move.w	#1,-(a7)
	move.l	(22+l1910,a7),-(a7)
	jsr	l1845
	addq.w	#4,a7
	and.w	#255,d0
	move.w	d0,-(a7)
	move.w	#2,-(a7)
	move.l	(34+l1910,a7),a0
	moveq	#0,d0
	move.w	(6,a0),d0
	move.l	d0,-(a7)
	move.w	#193,-(a7)
	move.w	#201,-(a7)
	move.l	(30+l1910,a7),-(a7)
	jsr	l1148
	add.w	#26,a7
l1898
	move.l	(16+l1910,a7),a0
	move.l	(20+l1910,a7),a1
	move.w	(10,a0),d0
	cmp.w	(10,a1),d0
	bne	l1901
l1904
	move.l	(16+l1910,a7),a0
	move.l	(20+l1910,a7),a1
	move.w	(12,a0),d0
	cmp.w	(12,a1),d0
	bne	l1901
l1903
	move.l	(16+l1910,a7),a0
	move.l	(20+l1910,a7),a1
	move.b	(16,a0),d0
	cmp.b	(16,a1),d0
	beq	l1902
l1901
	move.l	(12+l1910,a7),-(a7)
	jsr	l1858
	move.b	d0,(4+l1910,a7)
	move.w	#265,-(a7)
	move.w	#243,-(a7)
	move.l	(16+l1910,a7),-(a7)
	jsr	l1871
	move.w	#1,-(a7)
	moveq	#0,d0
	move.b	(14+l1910,a7),d0
	move.w	d0,-(a7)
	move.w	#1,-(a7)
	move.l	(38+l1910,a7),a0
	moveq	#0,d0
	move.w	(10,a0),d0
	move.l	d0,-(a7)
	move.w	#193,-(a7)
	move.w	#244,-(a7)
	move.l	(34+l1910,a7),-(a7)
	jsr	l1148
	move.w	#1,-(a7)
	move.w	#7,-(a7)
	pea	l1905
	move.w	#193,-(a7)
	move.w	#248,-(a7)
	move.l	(50+l1910,a7),-(a7)
	jsr	l1048
	move.w	#1,-(a7)
	moveq	#0,d0
	move.b	(48+l1910,a7),d0
	move.w	d0,-(a7)
	move.w	#1,-(a7)
	move.l	(72+l1910,a7),a0
	moveq	#0,d0
	move.w	(12,a0),d0
	move.l	d0,-(a7)
	move.w	#193,-(a7)
	move.w	#252,-(a7)
	move.l	(68+l1910,a7),-(a7)
	jsr	l1148
	move.l	(76+l1910,a7),a0
	move.w	(3082,a0),-(a7)
	move.l	(78+l1910,a7),a0
	move.w	(3108,a0),-(a7)
	move.l	(80+l1910,a7),a0
	move.w	(3110,a0),-(a7)
	move.w	#193,-(a7)
	move.w	#255,-(a7)
	move.l	(82+l1910,a7),-(a7)
	jsr	l1826
	add.w	#78,a7
l1902
	move.l	(16+l1910,a7),a0
	move.l	(20+l1910,a7),a1
	move.w	(8,a0),d0
	cmp.w	(8,a1),d0
	beq	l1907
l1906
	move.w	#306,-(a7)
	move.w	#286,-(a7)
	move.l	(12+l1910,a7),-(a7)
	jsr	l1871
	move.w	#1,-(a7)
	move.w	#23,-(a7)
	move.w	#2,-(a7)
	move.l	(34+l1910,a7),a0
	moveq	#0,d0
	move.w	(8,a0),d0
	move.l	d0,-(a7)
	move.w	#193,-(a7)
	move.w	#287,-(a7)
	move.l	(30+l1910,a7),-(a7)
	jsr	l1148
	add.w	#26,a7
l1907
l1891
l1908	reg
l1910	equ	0
	addq.w	#4,a7
	rts
	cnop	0,4
l1905
	dc.b	58
	dc.b	0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1911
	subq.w	#4,a7
	movem.l	l1933,-(a7)
	jsr	l290
	move.w	#1,-(a7)
	move.w	#199,-(a7)
	move.w	#319,-(a7)
	move.w	#192,-(a7)
	move.w	#0,-(a7)
	move.l	(18+l1935,a7),-(a7)
	jsr	l920
	move.w	#0,(14+l1935,a7)
	add.w	#14,a7
	bra	l1915
l1914
	move.w	#3,-(a7)
	move.w	#192,-(a7)
	move.w	(4+l1935,a7),-(a7)
	move.l	(14+l1935,a7),-(a7)
	jsr	l881
	move.w	#2,-(a7)
	move.w	#199,-(a7)
	move.w	(14+l1935,a7),-(a7)
	move.l	(24+l1935,a7),-(a7)
	jsr	l881
	add.w	#20,a7
l1917
	addq.w	#1,(0+l1935,a7)
l1915
	cmp.w	#320,(0+l1935,a7)
	blt	l1914
l1916
	move.w	#193,(2+l1935,a7)
	bra	l1919
l1918
	moveq	#1,d0
	and.w	(2+l1935,a7),d0
	bne	l1923
l1922
	move.w	#0,(0+l1935,a7)
	bra	l1925
l1924
	move.w	#2,-(a7)
	move.w	(4+l1935,a7),-(a7)
	move.w	(4+l1935,a7),-(a7)
	move.l	(14+l1935,a7),-(a7)
	jsr	l881
	add.w	#10,a7
l1927
	add.w	#32,(0+l1935,a7)
l1925
	cmp.w	#320,(0+l1935,a7)
	blt	l1924
l1926
l1923
l1921
	addq.w	#1,(2+l1935,a7)
l1919
	cmp.w	#199,(2+l1935,a7)
	blt	l1918
l1920
	move.w	#3,-(a7)
	move.w	#2,-(a7)
	move.w	#142,-(a7)
	move.l	(14+l1935,a7),-(a7)
	jsr	l1816
	move.w	#3,-(a7)
	move.w	#2,-(a7)
	move.w	#174,-(a7)
	move.l	(24+l1935,a7),-(a7)
	jsr	l1816
	move.w	#3,-(a7)
	move.w	#2,-(a7)
	move.w	#218,-(a7)
	move.l	(34+l1935,a7),-(a7)
	jsr	l1816
	move.w	#3,-(a7)
	move.w	#2,-(a7)
	move.w	#266,-(a7)
	move.l	(44+l1935,a7),-(a7)
	jsr	l1816
	move.w	#1,-(a7)
	move.w	#7,-(a7)
	pea	l1928
	move.w	#193,-(a7)
	move.w	#4,-(a7)
	move.l	(60+l1935,a7),-(a7)
	jsr	l1048
	move.w	#1,-(a7)
	move.w	#7,-(a7)
	pea	l1929
	move.w	#193,-(a7)
	move.w	#148,-(a7)
	move.l	(76+l1935,a7),-(a7)
	jsr	l1048
	move.w	#1,-(a7)
	move.w	#7,-(a7)
	pea	l1930
	move.w	#193,-(a7)
	move.w	#180,-(a7)
	move.l	(92+l1935,a7),-(a7)
	jsr	l1048
	move.w	#1,-(a7)
	move.w	#7,-(a7)
	pea	l1931
	move.w	#193,-(a7)
	move.w	#224,-(a7)
	move.l	(108+l1935,a7),-(a7)
	jsr	l1048
	move.w	#1,-(a7)
	move.w	#7,-(a7)
	pea	l1932
	move.w	#193,-(a7)
	move.w	#271,-(a7)
	move.l	(124+l1935,a7),-(a7)
	jsr	l1048
	move.l	(132+l1935,a7),-(a7)
	move.l	(132+l1935,a7),-(a7)
	jsr	l1877
	add.w	#128,a7
l1912
l1933	reg
l1935	equ	0
	addq.w	#4,a7
	rts
	cnop	0,4
l1928
	dc.b	83
	dc.b	67
	dc.b	79
	dc.b	82
	dc.b	69
	dc.b	0
	cnop	0,4
l1929
	dc.b	82
	dc.b	68
	dc.b	0
	cnop	0,4
l1930
	dc.b	84
	dc.b	73
	dc.b	77
	dc.b	69
	dc.b	0
	cnop	0,4
l1931
	dc.b	66
	dc.b	79
	dc.b	77
	dc.b	66
	dc.b	0
	cnop	0,4
l1932
	dc.b	76
	dc.b	73
	dc.b	86
	dc.b	0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1936
	sub.w	#20,a7
	movem.l	l1955,-(a7)
	move.l	(28+l1957,a7),a0
	cmp.l	#99999999,(3074,a0)
	bls	l1940
l1939
	move.l	#99999999,d0
	bra	l1941
l1940
	move.l	(28+l1957,a7),a0
	move.l	(3074,a0),d0
l1941
	move.l	d0,(0+l1957,a7)
	move.w	#999,-(a7)
	move.w	#0,-(a7)
	move.l	(32+l1957,a7),a0
	move.w	(3080,a0),-(a7)
	jsr	l15
	move.w	d0,(10+l1957,a7)
	move.l	(34+l1957,a7),a0
	addq.w	#6,a7
	tst.w	(3082,a0)
	ble	l1943
l1942
	move.l	(28+l1957,a7),a0
	move.w	(3082,a0),d0
	ext.l	d0
	divs.w	#50,d0
	bra	l1944
l1943
	moveq	#0,d0
l1944
	move.w	d0,(6+l1957,a7)
	move.w	#99,-(a7)
	move.w	#0,-(a7)
	move.l	(32+l1957,a7),a0
	move.w	(3078,a0),-(a7)
	jsr	l15
	move.w	d0,(14+l1957,a7)
	move.w	#99,-(a7)
	move.w	#0,-(a7)
	move.l	(38+l1957,a7),a0
	move.w	(3110,a0),-(a7)
	jsr	l15
	move.w	d0,(22+l1957,a7)
	move.w	#99,-(a7)
	move.w	#0,-(a7)
	move.l	(44+l1957,a7),a0
	move.w	(3108,a0),-(a7)
	jsr	l15
	move.w	d0,(30+l1957,a7)
	move.l	(46+l1957,a7),a0
	add.w	#18,a7
	tst.b	(3095,a0)
	beq	l1946
l1945
	moveq	#1,d0
	bra	l1947
l1946
	moveq	#0,d0
l1947
	move.b	d0,(14+l1957,a7)
	beq	l1949
l1948
	move.l	(28+l1957,a7),a0
	move.w	(3082,a0),d0
	bge	l1958
	addq.w	#3,d0
l1958:
	asr.w	#2,d0
	and.w	#1,d0
	and.w	#255,d0
	bra	l1950
l1949
	moveq	#0,d0
l1950
	move.b	d0,(15+l1957,a7)
	move.l	(28+l1957,a7),a0
	tst.w	(3108,a0)
	ble	l1952
l1954
	move.l	(28+l1957,a7),a0
	tst.w	(3110,a0)
	ble	l1952
l1951
	move.l	(28+l1957,a7),a0
	move.w	(3082,a0),d0
	bge	l1959
	addq.w	#3,d0
l1959:
	asr.w	#2,d0
	and.w	#1,d0
	lsl.b	#1,d0
	move.l	(28+l1957,a7),a0
	move.w	(3082,a0),d1
	ext.l	d1
	divs.w	#5,d1
	and.w	#1,d1
	or.b	d1,d0
	and.w	#255,d0
	bra	l1953
l1952
	moveq	#0,d0
l1953
	move.b	d0,(16+l1957,a7)
	move.b	#0,(17+l1957,a7)
	move.l	(24+l1957,a7),a0
	lea	(0+l1957,a7),a1
	lea	(a0),a2
	moveq.l	#3,d0
l1960:
	move.b	(a1)+,(a2)+
	move.b	(a1)+,(a2)+
	move.b	(a1)+,(a2)+
	move.b	(a1)+,(a2)+
	dbra	d0,l1960
	move.b	(a1)+,(a2)+
	move.b	(a1)+,(a2)+
l1937
l1955	reg	a2
	movem.l	(a7)+,a2
l1957	equ	4
	add.w	#20,a7
	rts
; stacksize=46
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1961
	movem.l	l1976,-(a7)
	move.l	(4+l1978,a7),a0
	move.l	(8+l1978,a7),a1
	move.l	(a0),d0
	cmp.l	(a1),d0
	bne	l1966
l1975
	move.l	(4+l1978,a7),a0
	move.l	(8+l1978,a7),a1
	move.w	(4,a0),d0
	cmp.w	(4,a1),d0
	bne	l1966
l1974
	move.l	(4+l1978,a7),a0
	move.l	(8+l1978,a7),a1
	move.w	(6,a0),d0
	cmp.w	(6,a1),d0
	bne	l1966
l1973
	move.l	(4+l1978,a7),a0
	move.l	(8+l1978,a7),a1
	move.w	(8,a0),d0
	cmp.w	(8,a1),d0
	bne	l1966
l1972
	move.l	(4+l1978,a7),a0
	move.l	(8+l1978,a7),a1
	move.w	(10,a0),d0
	cmp.w	(10,a1),d0
	bne	l1966
l1971
	move.l	(4+l1978,a7),a0
	move.l	(8+l1978,a7),a1
	move.w	(12,a0),d0
	cmp.w	(12,a1),d0
	bne	l1966
l1970
	move.l	(4+l1978,a7),a0
	move.l	(8+l1978,a7),a1
	move.b	(14,a0),d0
	cmp.b	(14,a1),d0
	bne	l1966
l1969
	move.l	(4+l1978,a7),a0
	move.l	(8+l1978,a7),a1
	move.b	(15,a0),d0
	cmp.b	(15,a1),d0
	bne	l1966
l1968
	move.l	(4+l1978,a7),a0
	move.l	(8+l1978,a7),a1
	move.b	(16,a0),d0
	cmp.b	(16,a1),d0
	bne	l1966
l1964
	move.l	(4+l1978,a7),a0
	move.l	(8+l1978,a7),a1
	move.b	(17,a0),d0
	cmp.b	(17,a1),d0
	bne	l1966
l1965
	moveq	#1,d0
	bra	l1967
l1966
	moveq	#0,d0
l1967
l1962
l1976	reg
l1978	equ	0
	rts
; stacksize=0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l1979
	sub.w	#20,a7
	movem.l	l2009,-(a7)
	move.w	(32+l2011,a7),d0
	sub.w	(28+l2011,a7),d0
	moveq	#1,d1
	add.w	d0,d1
	move.w	d1,(0+l2011,a7)
	move.w	(34+l2011,a7),d0
	sub.w	(30+l2011,a7),d0
	moveq	#1,d1
	add.w	d0,d1
	move.w	d1,(2+l2011,a7)
	move.w	(0+l2011,a7),d0
	add.w	(2+l2011,a7),d0
	lsl.w	#1,d0
	subq.w	#4,d0
	move.w	d0,(4+l2011,a7)
	move.w	#1,-(a7)
	move.w	(36+l2011,a7),-(a7)
	move.w	(36+l2011,a7),-(a7)
	move.w	(36+l2011,a7),-(a7)
	move.w	(36+l2011,a7),-(a7)
	move.l	(34+l2011,a7),-(a7)
	jsr	l920
	move.w	#30,-(a7)
	move.w	(46+l2011,a7),-(a7)
	move.w	(50+l2011,a7),-(a7)
	move.w	(50+l2011,a7),-(a7)
	move.w	(50+l2011,a7),-(a7)
	move.l	(48+l2011,a7),-(a7)
	jsr	l920
	move.w	#30,-(a7)
	move.w	(64+l2011,a7),-(a7)
	move.w	(64+l2011,a7),-(a7)
	move.w	(68+l2011,a7),-(a7)
	move.w	(64+l2011,a7),-(a7)
	move.l	(62+l2011,a7),-(a7)
	jsr	l920
	move.w	#30,-(a7)
	move.w	(78+l2011,a7),-(a7)
	move.w	(74+l2011,a7),-(a7)
	move.w	(78+l2011,a7),-(a7)
	move.w	(78+l2011,a7),-(a7)
	move.l	(76+l2011,a7),-(a7)
	jsr	l920
	move.w	#30,-(a7)
	move.w	(92+l2011,a7),-(a7)
	move.w	(92+l2011,a7),-(a7)
	move.w	(92+l2011,a7),-(a7)
	move.w	(96+l2011,a7),-(a7)
	move.l	(90+l2011,a7),-(a7)
	jsr	l920
	moveq	#0,d0
	move.b	(107+l2011,a7),d0
	move.w	d0,-(a7)
	moveq	#2,d0
	add.w	(102+l2011,a7),d0
	move.w	d0,-(a7)
	move.w	(106+l2011,a7),d0
	subq.w	#2,d0
	move.w	d0,-(a7)
	moveq	#2,d0
	add.w	(106+l2011,a7),d0
	move.w	d0,-(a7)
	moveq	#2,d0
	add.w	(106+l2011,a7),d0
	move.w	d0,-(a7)
	move.l	(104+l2011,a7),-(a7)
	jsr	l920
	moveq	#0,d0
	move.b	(121+l2011,a7),d0
	move.w	d0,-(a7)
	move.w	(120+l2011,a7),d0
	subq.w	#2,d0
	move.w	d0,-(a7)
	move.w	(120+l2011,a7),d0
	subq.w	#2,d0
	move.w	d0,-(a7)
	move.w	(124+l2011,a7),d0
	subq.w	#2,d0
	move.w	d0,-(a7)
	moveq	#2,d0
	add.w	(120+l2011,a7),d0
	move.w	d0,-(a7)
	move.l	(118+l2011,a7),-(a7)
	jsr	l920
	add.w	#98,a7
	tst.w	(4+l2011,a7)
	ble	l1983
l1982
	move.w	(4+l2011,a7),d0
	ext.l	d0
	move.l	d0,d1
	move.l	l2,d0
	lsr.l	#1,d0
	jsr	__divu
	move.l	d1,d0
	move.l	d0,d2
	move.w	d2,(8+l2011,a7)
	move.w	#0,(6+l2011,a7)
	bra	l1985
l1984
	move.w	(8+l2011,a7),d0
	sub.w	(6+l2011,a7),d0
	move.w	d0,(10+l2011,a7)
	move.w	(28+l2011,a7),(12+l2011,a7)
	move.w	(30+l2011,a7),(14+l2011,a7)
	tst.w	(6+l2011,a7)
	bne	l1989
l1988
	moveq	#31,d0
	bra	l1990
l1989
	cmp.w	#1,(6+l2011,a7)
	bne	l1992
l1991
	moveq	#30,d1
	bra	l1993
l1992
	cmp.w	#4,(6+l2011,a7)
	bge	l1995
l1994
	moveq	#0,d2
	move.b	(37+l2011,a7),d2
	bra	l1996
l1995
	moveq	#2,d2
l1996
	move.w	d2,d1
l1993
	move.w	d1,d0
l1990
	move.b	d0,(16+l2011,a7)
	bra	l1998
l1997
	move.w	(4+l2011,a7),d0
	add.w	d0,(10+l2011,a7)
l1998
	tst.w	(10+l2011,a7)
	blt	l1997
l1999
	move.w	(10+l2011,a7),d0
	ext.l	d0
	divs.w	(4+l2011,a7),d0
	swap	d0
	move.w	d0,(10+l2011,a7)
	move.w	(10+l2011,a7),d0
	cmp.w	(0+l2011,a7),d0
	bge	l2001
l2000
	move.w	(28+l2011,a7),d0
	add.w	(10+l2011,a7),d0
	move.w	d0,(12+l2011,a7)
	move.w	(30+l2011,a7),(14+l2011,a7)
	bra	l2002
l2001
	move.w	(0+l2011,a7),d0
	add.w	(2+l2011,a7),d0
	subq.w	#1,d0
	cmp.w	(10+l2011,a7),d0
	ble	l2004
l2003
	move.w	(32+l2011,a7),(12+l2011,a7)
	move.w	(10+l2011,a7),d0
	sub.w	(0+l2011,a7),d0
	add.w	(30+l2011,a7),d0
	moveq	#1,d1
	add.w	d0,d1
	move.w	d1,(14+l2011,a7)
	bra	l2005
l2004
	move.w	(0+l2011,a7),d0
	add.w	(2+l2011,a7),d0
	add.w	(0+l2011,a7),d0
	subq.w	#2,d0
	cmp.w	(10+l2011,a7),d0
	ble	l2007
l2006
	move.w	(10+l2011,a7),d0
	sub.w	(0+l2011,a7),d0
	move.w	(2+l2011,a7),d1
	subq.w	#1,d1
	sub.w	d1,d0
	neg.w	d0
	add.w	(32+l2011,a7),d0
	subq.w	#1,d0
	move.w	d0,(12+l2011,a7)
	move.w	(34+l2011,a7),(14+l2011,a7)
	bra	l2008
l2007
	move.w	(28+l2011,a7),(12+l2011,a7)
	move.w	(10+l2011,a7),d0
	sub.w	(0+l2011,a7),d0
	move.w	(2+l2011,a7),d1
	subq.w	#1,d1
	sub.w	d1,d0
	move.w	(0+l2011,a7),d1
	subq.w	#1,d1
	sub.w	d1,d0
	neg.w	d0
	add.w	(34+l2011,a7),d0
	subq.w	#1,d0
	move.w	d0,(14+l2011,a7)
l2008
l2005
l2002
	moveq	#0,d0
	move.b	(16+l2011,a7),d0
	move.w	d0,-(a7)
	move.w	(16+l2011,a7),-(a7)
	move.w	(16+l2011,a7),-(a7)
	move.l	(30+l2011,a7),-(a7)
	jsr	l881
	add.w	#10,a7
l1987
	addq.w	#1,(6+l2011,a7)
l1985
	cmp.w	#5,(6+l2011,a7)
	blt	l1984
l1986
l1983
l1980
l2009	reg	d2
	movem.l	(a7)+,d2
l2011	equ	4
	add.w	#20,a7
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l2012
	sub.w	#12,a7
	movem.l	l2035,-(a7)
	move.w	(20+l2037,a7),d0
	add.w	(24+l2037,a7),d0
	subq.w	#1,d0
	move.w	d0,(4+l2037,a7)
	move.w	(22+l2037,a7),d0
	add.w	(26+l2037,a7),d0
	subq.w	#1,d0
	move.w	d0,(6+l2037,a7)
	tst.b	(31+l2037,a7)
	beq	l2016
l2015
	moveq	#30,d0
	bra	l2017
l2016
	moveq	#3,d0
l2017
	move.b	d0,(8+l2037,a7)
	tst.b	(31+l2037,a7)
	beq	l2019
l2018
	moveq	#0,d0
	move.b	(29+l2037,a7),d0
	bra	l2020
l2019
	moveq	#2,d0
l2020
	move.b	d0,(9+l2037,a7)
	move.w	#1,-(a7)
	move.w	(8+l2037,a7),-(a7)
	move.w	(8+l2037,a7),-(a7)
	move.w	(28+l2037,a7),-(a7)
	move.w	(28+l2037,a7),-(a7)
	move.l	(26+l2037,a7),-(a7)
	jsr	l920
	moveq	#0,d0
	move.b	(22+l2037,a7),d0
	move.w	d0,-(a7)
	move.w	(38+l2037,a7),-(a7)
	move.w	(22+l2037,a7),-(a7)
	move.w	(40+l2037,a7),-(a7)
	move.l	(38+l2037,a7),-(a7)
	jsr	l892
	moveq	#0,d0
	move.b	(34+l2037,a7),d0
	move.w	d0,-(a7)
	move.w	(34+l2037,a7),-(a7)
	move.w	(34+l2037,a7),-(a7)
	move.w	(52+l2037,a7),-(a7)
	move.l	(50+l2037,a7),-(a7)
	jsr	l892
	moveq	#0,d0
	move.b	(46+l2037,a7),d0
	move.w	d0,-(a7)
	move.w	(46+l2037,a7),-(a7)
	move.w	(62+l2037,a7),-(a7)
	move.w	(66+l2037,a7),-(a7)
	move.w	(66+l2037,a7),-(a7)
	move.l	(64+l2037,a7),-(a7)
	jsr	l920
	moveq	#0,d0
	move.b	(60+l2037,a7),d0
	move.w	d0,-(a7)
	move.w	(60+l2037,a7),-(a7)
	move.w	(60+l2037,a7),-(a7)
	move.w	(80+l2037,a7),-(a7)
	move.w	(64+l2037,a7),-(a7)
	move.l	(78+l2037,a7),-(a7)
	jsr	l920
	moveq	#0,d0
	move.b	(75+l2037,a7),d0
	move.w	d0,-(a7)
	moveq	#1,d0
	add.w	(90+l2037,a7),d0
	move.w	d0,-(a7)
	move.w	(74+l2037,a7),d0
	subq.w	#1,d0
	move.w	d0,-(a7)
	moveq	#1,d0
	add.w	(92+l2037,a7),d0
	move.w	d0,-(a7)
	move.l	(90+l2037,a7),-(a7)
	jsr	l892
	moveq	#0,d0
	move.b	(87+l2037,a7),d0
	move.w	d0,-(a7)
	move.w	(86+l2037,a7),d0
	subq.w	#1,d0
	move.w	d0,-(a7)
	move.w	(86+l2037,a7),d0
	subq.w	#1,d0
	move.w	d0,-(a7)
	moveq	#1,d0
	add.w	(104+l2037,a7),d0
	move.w	d0,-(a7)
	move.l	(102+l2037,a7),-(a7)
	jsr	l892
	moveq	#0,d0
	move.b	(99+l2037,a7),d0
	move.w	d0,-(a7)
	move.w	(98+l2037,a7),d0
	subq.w	#1,d0
	move.w	d0,-(a7)
	moveq	#1,d0
	add.w	(114+l2037,a7),d0
	move.w	d0,-(a7)
	moveq	#1,d0
	add.w	(118+l2037,a7),d0
	move.w	d0,-(a7)
	moveq	#1,d0
	add.w	(118+l2037,a7),d0
	move.w	d0,-(a7)
	move.l	(116+l2037,a7),-(a7)
	jsr	l920
	moveq	#0,d0
	move.b	(113+l2037,a7),d0
	move.w	d0,-(a7)
	move.w	(112+l2037,a7),d0
	subq.w	#1,d0
	move.w	d0,-(a7)
	move.w	(112+l2037,a7),d0
	subq.w	#1,d0
	move.w	d0,-(a7)
	moveq	#1,d0
	add.w	(132+l2037,a7),d0
	move.w	d0,-(a7)
	move.w	(116+l2037,a7),d0
	subq.w	#1,d0
	move.w	d0,-(a7)
	move.l	(130+l2037,a7),-(a7)
	jsr	l920
	moveq	#5,d0
	add.w	(140+l2037,a7),d0
	move.w	d0,(120+l2037,a7)
	add.w	#118,a7
	bra	l2022
l2021
	move.w	(2+l2037,a7),d0
	sub.w	(22+l2037,a7),d0
	and.w	#4,d0
	beq	l2026
l2025
	moveq	#6,d0
	bra	l2027
l2026
	moveq	#2,d0
l2027
	move.w	d0,(10+l2037,a7)
	move.w	(20+l2037,a7),d0
	add.w	(10+l2037,a7),d0
	move.w	d0,(0+l2037,a7)
	bra	l2029
l2028
	move.w	#2,-(a7)
	move.w	(4+l2037,a7),-(a7)
	move.w	(4+l2037,a7),-(a7)
	move.l	(22+l2037,a7),-(a7)
	jsr	l881
	add.w	#10,a7
l2031
	addq.w	#8,(0+l2037,a7)
l2029
	move.w	(4+l2037,a7),d0
	subq.w	#5,d0
	cmp.w	(0+l2037,a7),d0
	bge	l2028
l2030
l2024
	addq.w	#4,(2+l2037,a7)
l2022
	move.w	(6+l2037,a7),d0
	subq.w	#5,d0
	cmp.w	(2+l2037,a7),d0
	bge	l2021
l2023
	tst.b	(31+l2037,a7)
	beq	l2033
l2034
	cmp.w	#3,(22+l2037,a7)
	blt	l2033
l2032
	move.w	(24+l2037,a7),d0
	bge	l2038
	addq.w	#1,d0
l2038:
	asr.w	#1,d0
	add.w	(20+l2037,a7),d0
	move.w	d0,(10+l2037,a7)
	move.w	#30,-(a7)
	move.w	(24+l2037,a7),d0
	subq.w	#3,d0
	move.w	d0,-(a7)
	move.w	(14+l2037,a7),-(a7)
	move.l	(22+l2037,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(39+l2037,a7),d0
	move.w	d0,-(a7)
	move.w	(34+l2037,a7),d0
	subq.w	#2,d0
	move.w	d0,-(a7)
	move.w	(24+l2037,a7),d0
	subq.w	#1,d0
	move.w	d0,-(a7)
	move.l	(32+l2037,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(49+l2037,a7),d0
	move.w	d0,-(a7)
	move.w	(44+l2037,a7),d0
	subq.w	#2,d0
	move.w	d0,-(a7)
	move.w	(34+l2037,a7),-(a7)
	move.l	(42+l2037,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(59+l2037,a7),d0
	move.w	d0,-(a7)
	move.w	(54+l2037,a7),d0
	subq.w	#2,d0
	move.w	d0,-(a7)
	moveq	#1,d0
	add.w	(44+l2037,a7),d0
	move.w	d0,-(a7)
	move.l	(52+l2037,a7),-(a7)
	jsr	l881
	move.w	#30,-(a7)
	move.w	(64+l2037,a7),d0
	subq.w	#1,d0
	move.w	d0,-(a7)
	move.w	(54+l2037,a7),-(a7)
	move.l	(62+l2037,a7),-(a7)
	jsr	l881
	add.w	#50,a7
l2033
l2013
l2035	reg
l2037	equ	0
	add.w	#12,a7
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l2039
	sub.w	#16,a7
	movem.l	l2047,-(a7)
	move.w	#86,(2+l2049,a7)
	move.w	#233,(4+l2049,a7)
	move.w	#96,(6+l2049,a7)
	move.w	#144,(8+l2049,a7)
	moveq	#10,d0
	add.w	(2+l2049,a7),d0
	move.w	d0,(10+l2049,a7)
	move.w	(4+l2049,a7),d0
	sub.w	#10,d0
	move.w	d0,(12+l2049,a7)
	move.w	#16,-(a7)
	move.w	(10+l2049,a7),-(a7)
	move.w	(8+l2049,a7),-(a7)
	move.w	(12+l2049,a7),-(a7)
	move.w	(10+l2049,a7),-(a7)
	move.l	(30+l2049,a7),-(a7)
	jsr	l1979
	move.w	#30,-(a7)
	pea	l2042
	moveq	#6,d0
	add.w	(26+l2049,a7),d0
	move.w	d0,-(a7)
	move.w	(26+l2049,a7),-(a7)
	move.w	(26+l2049,a7),-(a7)
	move.l	(46+l2049,a7),-(a7)
	jsr	l1041
	move.w	#0,(30+l2049,a7)
	add.w	#30,a7
	bra	l2044
l2043
	moveq	#14,d0
	add.w	(6+l2049,a7),d0
	moveq	#6,d1
	muls.w	(0+l2049,a7),d1
	add.w	d1,d0
	move.w	d0,(14+l2049,a7)
	move.w	#29,-(a7)
	move.w	#1,-(a7)
	moveq	#1,d0
	add.w	(4+l2049,a7),d0
	ext.l	d0
	move.l	d0,-(a7)
	move.w	(22+l2049,a7),-(a7)
	move.w	(20+l2049,a7),-(a7)
	move.l	(32+l2049,a7),-(a7)
	jsr	l1130
	move.w	#29,-(a7)
	move.w	(18+l2049,a7),d0
	ext.l	d0
	lsl.l	#3,d0
	lea	l8,a0
	move.l	(0,a0,d0.l),-(a7)
	move.w	(36+l2049,a7),-(a7)
	moveq	#8,d0
	add.w	(34+l2049,a7),d0
	move.w	d0,-(a7)
	move.l	(46+l2049,a7),-(a7)
	jsr	l1020
	move.w	#30,-(a7)
	move.w	#5,-(a7)
	move.w	(34+l2049,a7),d0
	ext.l	d0
	lsl.l	#3,d0
	lea	l8,a0
	move.l	(4,a0,d0.l),-(a7)
	move.w	(52+l2049,a7),-(a7)
	move.w	(52+l2049,a7),-(a7)
	move.l	(62+l2049,a7),-(a7)
	jsr	l1171
	add.w	#46,a7
l2046
	addq.w	#1,(0+l2049,a7)
l2044
	cmp.w	#5,(0+l2049,a7)
	blt	l2043
l2045
l2040
l2047	reg
l2049	equ	0
	add.w	#16,a7
	rts
	cnop	0,4
l2042
	dc.b	72
	dc.b	73
	dc.b	32
	dc.b	83
	dc.b	67
	dc.b	79
	dc.b	82
	dc.b	69
	dc.b	83
	dc.b	0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l2050
	movem.l	l2059,-(a7)
	move.w	(12+l2061,a7),d0
	subq.w	#1,d0
	beq	l2056
	subq.w	#1,d0
	beq	l2057
	subq.w	#1,d0
	beq	l2054
	subq.w	#1,d0
	beq	l2055
	bra	l2058
l2054
	moveq	#0,d0
	move.b	(15+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#2,d0
	add.w	(12+l2061,a7),d0
	move.w	d0,-(a7)
	move.w	(12+l2061,a7),-(a7)
	move.l	(10+l2061,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(25+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#1,d0
	add.w	(22+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#1,d0
	add.w	(22+l2061,a7),d0
	move.w	d0,-(a7)
	move.l	(20+l2061,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(35+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#2,d0
	add.w	(32+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#1,d0
	add.w	(32+l2061,a7),d0
	move.w	d0,-(a7)
	move.l	(30+l2061,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(45+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#3,d0
	add.w	(42+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#1,d0
	add.w	(42+l2061,a7),d0
	move.w	d0,-(a7)
	move.l	(40+l2061,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(55+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#2,d0
	add.w	(52+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#2,d0
	add.w	(52+l2061,a7),d0
	move.w	d0,-(a7)
	move.l	(50+l2061,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(65+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#2,d0
	add.w	(62+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#3,d0
	add.w	(62+l2061,a7),d0
	move.w	d0,-(a7)
	move.l	(60+l2061,a7),-(a7)
	jsr	l881
	add.w	#60,a7
	bra	l2053
l2055
	moveq	#0,d0
	move.b	(15+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#2,d0
	add.w	(12+l2061,a7),d0
	move.w	d0,-(a7)
	move.w	(12+l2061,a7),-(a7)
	move.l	(10+l2061,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(25+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#2,d0
	add.w	(22+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#1,d0
	add.w	(22+l2061,a7),d0
	move.w	d0,-(a7)
	move.l	(20+l2061,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(35+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#1,d0
	add.w	(32+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#2,d0
	add.w	(32+l2061,a7),d0
	move.w	d0,-(a7)
	move.l	(30+l2061,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(45+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#2,d0
	add.w	(42+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#2,d0
	add.w	(42+l2061,a7),d0
	move.w	d0,-(a7)
	move.l	(40+l2061,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(55+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#3,d0
	add.w	(52+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#2,d0
	add.w	(52+l2061,a7),d0
	move.w	d0,-(a7)
	move.l	(50+l2061,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(65+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#2,d0
	add.w	(62+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#3,d0
	add.w	(62+l2061,a7),d0
	move.w	d0,-(a7)
	move.l	(60+l2061,a7),-(a7)
	jsr	l881
	add.w	#60,a7
	bra	l2053
l2056
	moveq	#0,d0
	move.b	(15+l2061,a7),d0
	move.w	d0,-(a7)
	move.w	(12+l2061,a7),-(a7)
	moveq	#1,d0
	add.w	(12+l2061,a7),d0
	move.w	d0,-(a7)
	move.l	(10+l2061,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(25+l2061,a7),d0
	move.w	d0,-(a7)
	move.w	(22+l2061,a7),-(a7)
	moveq	#2,d0
	add.w	(22+l2061,a7),d0
	move.w	d0,-(a7)
	move.l	(20+l2061,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(35+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#1,d0
	add.w	(32+l2061,a7),d0
	move.w	d0,-(a7)
	move.w	(32+l2061,a7),-(a7)
	move.l	(30+l2061,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(45+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#1,d0
	add.w	(42+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#1,d0
	add.w	(42+l2061,a7),d0
	move.w	d0,-(a7)
	move.l	(40+l2061,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(55+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#1,d0
	add.w	(52+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#2,d0
	add.w	(52+l2061,a7),d0
	move.w	d0,-(a7)
	move.l	(50+l2061,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(65+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#1,d0
	add.w	(62+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#3,d0
	add.w	(62+l2061,a7),d0
	move.w	d0,-(a7)
	move.l	(60+l2061,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(75+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#2,d0
	add.w	(72+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#1,d0
	add.w	(72+l2061,a7),d0
	move.w	d0,-(a7)
	move.l	(70+l2061,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(85+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#2,d0
	add.w	(82+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#2,d0
	add.w	(82+l2061,a7),d0
	move.w	d0,-(a7)
	move.l	(80+l2061,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(95+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#3,d0
	add.w	(92+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#1,d0
	add.w	(92+l2061,a7),d0
	move.w	d0,-(a7)
	move.l	(90+l2061,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(105+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#3,d0
	add.w	(102+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#2,d0
	add.w	(102+l2061,a7),d0
	move.w	d0,-(a7)
	move.l	(100+l2061,a7),-(a7)
	jsr	l881
	add.w	#100,a7
	bra	l2053
l2057
	moveq	#0,d0
	move.b	(15+l2061,a7),d0
	move.w	d0,-(a7)
	move.w	(12+l2061,a7),-(a7)
	moveq	#1,d0
	add.w	(12+l2061,a7),d0
	move.w	d0,-(a7)
	move.l	(10+l2061,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(25+l2061,a7),d0
	move.w	d0,-(a7)
	move.w	(22+l2061,a7),-(a7)
	moveq	#2,d0
	add.w	(22+l2061,a7),d0
	move.w	d0,-(a7)
	move.l	(20+l2061,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(35+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#1,d0
	add.w	(32+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#1,d0
	add.w	(32+l2061,a7),d0
	move.w	d0,-(a7)
	move.l	(30+l2061,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(45+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#1,d0
	add.w	(42+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#2,d0
	add.w	(42+l2061,a7),d0
	move.w	d0,-(a7)
	move.l	(40+l2061,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(55+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#2,d0
	add.w	(52+l2061,a7),d0
	move.w	d0,-(a7)
	move.w	(52+l2061,a7),-(a7)
	move.l	(50+l2061,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(65+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#2,d0
	add.w	(62+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#1,d0
	add.w	(62+l2061,a7),d0
	move.w	d0,-(a7)
	move.l	(60+l2061,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(75+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#2,d0
	add.w	(72+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#2,d0
	add.w	(72+l2061,a7),d0
	move.w	d0,-(a7)
	move.l	(70+l2061,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(85+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#2,d0
	add.w	(82+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#3,d0
	add.w	(82+l2061,a7),d0
	move.w	d0,-(a7)
	move.l	(80+l2061,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(95+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#3,d0
	add.w	(92+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#1,d0
	add.w	(92+l2061,a7),d0
	move.w	d0,-(a7)
	move.l	(90+l2061,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(105+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#3,d0
	add.w	(102+l2061,a7),d0
	move.w	d0,-(a7)
	moveq	#2,d0
	add.w	(102+l2061,a7),d0
	move.w	d0,-(a7)
	move.l	(100+l2061,a7),-(a7)
	jsr	l881
	add.w	#100,a7
	bra	l2053
l2058
l2053
l2051
l2059	reg
l2061	equ	0
	rts
; stacksize=138
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l2062
	subq.w	#4,a7
	movem.l	l2068,-(a7)
	moveq	#6,d1
	move.l	l2,d0
	jsr	__divu
	move.l	d0,d2
	and.l	#1,d2
	bne	l2066
l2065
	moveq	#0,d0
	move.b	(17+l2070,a7),d0
	bra	l2067
l2066
	moveq	#0,d0
	move.b	(19+l2070,a7),d0
l2067
	move.b	d0,(0+l2070,a7)
	moveq	#0,d0
	move.b	(0+l2070,a7),d0
	move.w	d0,-(a7)
	move.w	#4,-(a7)
	move.w	(18+l2070,a7),-(a7)
	move.w	(18+l2070,a7),-(a7)
	move.l	(16+l2070,a7),-(a7)
	jsr	l2050
	moveq	#0,d0
	move.b	(12+l2070,a7),d0
	move.w	d0,-(a7)
	move.w	#4,-(a7)
	move.w	(30+l2070,a7),-(a7)
	moveq	#5,d0
	add.w	(30+l2070,a7),d0
	move.w	d0,-(a7)
	move.l	(28+l2070,a7),-(a7)
	jsr	l2050
	moveq	#0,d0
	move.b	(24+l2070,a7),d0
	move.w	d0,-(a7)
	move.w	#4,-(a7)
	move.w	(42+l2070,a7),-(a7)
	moveq	#10,d0
	add.w	(42+l2070,a7),d0
	move.w	d0,-(a7)
	move.l	(40+l2070,a7),-(a7)
	jsr	l2050
	add.w	#36,a7
l2063
l2068	reg	d2
	movem.l	(a7)+,d2
l2070	equ	4
	addq.w	#4,a7
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l2071
	sub.w	#20,a7
	movem.l	l2088,-(a7)
	move.w	(42+l2090,a7),d0
	sub.w	#16,d0
	tst.w	d0
	bge	l2091
	addq.w	#1,d0
l2091:
	asr.w	#1,d0
	add.w	(38+l2090,a7),d0
	move.w	d0,(0+l2090,a7)
	moveq	#8,d0
	add.w	(36+l2090,a7),d0
	move.w	d0,(2+l2090,a7)
	move.w	(36+l2090,a7),d0
	add.w	(40+l2090,a7),d0
	sub.w	#16,d0
	subq.w	#8,d0
	move.w	d0,(4+l2090,a7)
	move.w	(4+l2090,a7),d0
	sub.w	(2+l2090,a7),d0
	move.w	d0,(6+l2090,a7)
	move.w	(6+l2090,a7),d0
	ext.l	d0
	divs.w	#3,d0
	add.w	(2+l2090,a7),d0
	move.w	d0,(8+l2090,a7)
	move.w	(6+l2090,a7),d0
	lsl.w	#1,d0
	ext.l	d0
	divs.w	#3,d0
	add.w	(2+l2090,a7),d0
	move.w	d0,(10+l2090,a7)
	move.w	(4+l2090,a7),(12+l2090,a7)
	moveq	#5,d1
	move.l	l2,d0
	jsr	__divu
	move.l	d0,d2
	and.l	#1,d2
	bne	l2075
l2074
	moveq	#30,d0
	bra	l2076
l2075
	moveq	#14,d0
l2076
	move.b	d0,(14+l2090,a7)
	move.w	(0+l2090,a7),-(a7)
	move.w	(4+l2090,a7),-(a7)
	move.w	#0,-(a7)
	move.w	#4,-(a7)
	jsr	l96
	addq.w	#4,a7
	move.w	d0,-(a7)
	move.l	(34+l2090,a7),-(a7)
	move.l	(34+l2090,a7),-(a7)
	jsr	l380
	jsr	l290
	moveq	#0,d0
	move.b	(28+l2090,a7),d0
	move.w	d0,-(a7)
	move.w	#1,-(a7)
	move.w	(18+l2090,a7),d0
	subq.w	#4,d0
	move.w	d0,-(a7)
	moveq	#6,d0
	add.w	(22+l2090,a7),d0
	move.w	d0,-(a7)
	move.l	(50+l2090,a7),-(a7)
	jsr	l2050
	moveq	#0,d0
	move.b	(40+l2090,a7),d0
	move.w	d0,-(a7)
	move.w	#2,-(a7)
	moveq	#16,d0
	add.w	(30+l2090,a7),d0
	move.w	d0,-(a7)
	moveq	#6,d0
	add.w	(34+l2090,a7),d0
	move.w	d0,-(a7)
	move.l	(62+l2090,a7),-(a7)
	jsr	l2050
	moveq	#0,d0
	move.b	(52+l2090,a7),d0
	move.w	d0,-(a7)
	move.w	#3,-(a7)
	moveq	#6,d0
	add.w	(42+l2090,a7),d0
	move.w	d0,-(a7)
	move.w	(46+l2090,a7),d0
	subq.w	#5,d0
	move.w	d0,-(a7)
	move.l	(74+l2090,a7),-(a7)
	jsr	l2050
	moveq	#0,d0
	move.b	(64+l2090,a7),d0
	move.w	d0,-(a7)
	move.w	#4,-(a7)
	moveq	#6,d0
	add.w	(54+l2090,a7),d0
	move.w	d0,-(a7)
	moveq	#17,d0
	add.w	(58+l2090,a7),d0
	move.w	d0,-(a7)
	move.l	(86+l2090,a7),-(a7)
	jsr	l2050
	move.w	(62+l2090,a7),-(a7)
	move.w	(72+l2090,a7),-(a7)
	move.w	#3,-(a7)
	move.l	(96+l2090,a7),-(a7)
	move.l	(96+l2090,a7),-(a7)
	jsr	l380
	jsr	l290
	move.w	(76+l2090,a7),-(a7)
	move.w	(86+l2090,a7),-(a7)
	move.l	(108+l2090,a7),-(a7)
	jsr	l1557
	move.w	2+l2,d0
	moveq	#5,d1
	muls.w	(92+l2090,a7),d1
	add.w	d1,d0
	moveq	#3,d1
	muls.w	(84+l2090,a7),d1
	add.w	d1,d0
	move.w	d0,-(a7)
	move.w	(86+l2090,a7),-(a7)
	move.w	(96+l2090,a7),-(a7)
	move.l	(118+l2090,a7),-(a7)
	jsr	l1537
	moveq	#0,d0
	move.b	(108+l2090,a7),d0
	move.w	d0,-(a7)
	move.w	#4,-(a7)
	moveq	#6,d0
	add.w	(98+l2090,a7),d0
	move.w	d0,-(a7)
	move.w	(108+l2090,a7),d0
	add.w	(110+l2090,a7),d0
	tst.w	d0
	bge	l2092
	addq.w	#1,d0
l2092:
	asr.w	#1,d0
	subq.w	#2,d0
	move.w	d0,-(a7)
	move.l	(130+l2090,a7),-(a7)
	jsr	l2050
	move.w	(106+l2090,a7),-(a7)
	move.w	(118+l2090,a7),-(a7)
	move.l	l2,d0
	lsr.l	#3,d0
	and.l	#1,d0
	beq	l2078
l2077
	moveq	#16,d0
	bra	l2079
l2078
	moveq	#15,d0
l2079
	move.w	d0,-(a7)
	move.l	(140+l2090,a7),-(a7)
	move.l	(140+l2090,a7),-(a7)
	jsr	l380
	move.l	l2,d0
	lsr.l	#2,d0
	and.l	#1,d0
	add.w	#120,a7
	bne	l2081
l2080
	move.w	(0+l2090,a7),d0
	subq.w	#2,d0
	move.w	d0,-(a7)
	moveq	#2,d0
	add.w	(12+l2090,a7),d0
	move.w	d0,-(a7)
	move.w	#29,-(a7)
	move.l	(34+l2090,a7),-(a7)
	move.l	(34+l2090,a7),-(a7)
	jsr	l380
	add.w	#14,a7
l2081
	move.l	(32+l2090,a7),a0
	tst.w	(3108,a0)
	ble	l2083
l2082
	moveq	#6,d1
	move.l	l2,d0
	jsr	__divu
	move.l	d0,d2
	and.l	#1,d2
	bne	l2085
l2084
	moveq	#31,d0
	bra	l2086
l2085
	moveq	#32,d0
l2086
	move.w	d0,(16+l2090,a7)
	move.w	(0+l2090,a7),-(a7)
	move.w	(14+l2090,a7),-(a7)
	move.w	(20+l2090,a7),-(a7)
	move.l	(34+l2090,a7),-(a7)
	move.l	(34+l2090,a7),-(a7)
	jsr	l380
	jsr	l290
	moveq	#0,d0
	move.b	(28+l2090,a7),d0
	move.w	d0,-(a7)
	move.w	#3,-(a7)
	moveq	#6,d0
	add.w	(18+l2090,a7),d0
	move.w	d0,-(a7)
	move.w	(32+l2090,a7),d0
	subq.w	#7,d0
	move.w	d0,-(a7)
	move.l	(50+l2090,a7),-(a7)
	jsr	l2050
	add.w	#26,a7
	bra	l2087
l2083
	move.w	(0+l2090,a7),-(a7)
	move.w	(14+l2090,a7),-(a7)
	move.w	#30,-(a7)
	move.l	(34+l2090,a7),-(a7)
	move.l	(34+l2090,a7),-(a7)
	jsr	l380
	add.w	#14,a7
l2087
	jsr	l290
l2072
l2088	reg	d2
	movem.l	(a7)+,d2
l2090	equ	4
	add.w	#20,a7
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l2093
	sub.w	#36,a7
	movem.l	l2112,-(a7)
	move.w	#48,(2+l2114,a7)
	move.w	#271,(4+l2114,a7)
	move.w	#58,(6+l2114,a7)
	move.w	#143,(8+l2114,a7)
	move.w	#58,(10+l2114,a7)
	move.w	#44,(12+l2114,a7)
	move.w	#8,(14+l2114,a7)
	move.l	(48+l2114,a7),a0
	move.w	(3180,a0),(16+l2114,a7)
	cmp.w	#1,(16+l2114,a7)
	bge	l2097
l2096
	move.w	#1,(16+l2114,a7)
l2097
	cmp.w	#3,(16+l2114,a7)
	ble	l2099
l2098
	move.w	#3,(16+l2114,a7)
l2099
	move.w	(16+l2114,a7),d0
	muls.w	(10+l2114,a7),d0
	move.w	(16+l2114,a7),d1
	subq.w	#1,d1
	muls.w	(14+l2114,a7),d1
	add.w	d1,d0
	move.w	d0,(18+l2114,a7)
	move.w	(4+l2114,a7),d0
	sub.w	(2+l2114,a7),d0
	addq.w	#1,d0
	sub.w	(18+l2114,a7),d0
	tst.w	d0
	bge	l2115
	addq.w	#1,d0
l2115:
	asr.w	#1,d0
	add.w	(2+l2114,a7),d0
	move.w	d0,(20+l2114,a7)
	move.w	#18,-(a7)
	move.w	(10+l2114,a7),-(a7)
	move.w	(8+l2114,a7),-(a7)
	move.w	(12+l2114,a7),-(a7)
	move.w	(10+l2114,a7),-(a7)
	move.l	(54+l2114,a7),-(a7)
	jsr	l1979
	move.w	#1,-(a7)
	move.w	#31,-(a7)
	pea	l2100
	move.w	#64,-(a7)
	move.w	(28+l2114,a7),-(a7)
	move.w	(28+l2114,a7),-(a7)
	move.l	(72+l2114,a7),-(a7)
	jsr	l1054
	move.w	#0,(32+l2114,a7)
	add.w	#32,a7
	bra	l2102
l2101
	move.l	(48+l2114,a7),a0
	add.w	#3174,a0
	move.w	(0+l2114,a7),d0
	ext.l	d0
	lsl.l	#1,d0
	move.w	(0,a0,d0.l),(28+l2114,a7)
	move.l	(48+l2114,a7),a0
	move.w	(0+l2114,a7),d0
	cmp.w	(3182,a0),d0
	beq	l2105
l2107
	moveq	#0,d0
	bra	l2106
l2105
	moveq	#1,d0
l2106
	move.b	d0,(30+l2114,a7)
	move.w	(28+l2114,a7),-(a7)
	jsr	l193
	move.b	d0,(33+l2114,a7)
	move.w	(12+l2114,a7),d0
	add.w	(16+l2114,a7),d0
	muls.w	(2+l2114,a7),d0
	add.w	(22+l2114,a7),d0
	move.w	d0,(34+l2114,a7)
	move.w	#75,(36+l2114,a7)
	moveq	#0,d0
	move.b	(32+l2114,a7),d0
	move.w	d0,-(a7)
	moveq	#0,d0
	move.b	(35+l2114,a7),d0
	move.w	d0,-(a7)
	move.w	(18+l2114,a7),-(a7)
	move.w	(18+l2114,a7),-(a7)
	move.w	(44+l2114,a7),-(a7)
	move.w	(44+l2114,a7),-(a7)
	move.l	(58+l2114,a7),-(a7)
	jsr	l2012
	moveq	#5,d0
	add.w	(52+l2114,a7),d0
	move.w	d0,-(a7)
	move.w	(30+l2114,a7),d0
	sub.w	#16,d0
	tst.w	d0
	bge	l2116
	addq.w	#1,d0
l2116:
	asr.w	#1,d0
	add.w	(52+l2114,a7),d0
	move.w	d0,-(a7)
	move.w	(50+l2114,a7),-(a7)
	jsr	l206
	addq.w	#2,a7
	move.w	d0,-(a7)
	move.l	(68+l2114,a7),-(a7)
	move.l	(68+l2114,a7),-(a7)
	jsr	l380
	jsr	l290
	move.w	#1,-(a7)
	moveq	#0,d0
	move.b	(65+l2114,a7),d0
	move.w	d0,-(a7)
	move.w	(64+l2114,a7),-(a7)
	jsr	l155
	addq.w	#2,a7
	move.l	d0,-(a7)
	moveq	#24,d0
	add.w	(74+l2114,a7),d0
	move.w	d0,-(a7)
	move.w	(74+l2114,a7),d0
	add.w	(52+l2114,a7),d0
	subq.w	#3,d0
	move.w	d0,-(a7)
	moveq	#2,d0
	add.w	(76+l2114,a7),d0
	move.w	d0,-(a7)
	move.l	(90+l2114,a7),-(a7)
	jsr	l1054
	move.w	#1,-(a7)
	move.w	#30,-(a7)
	pea	l2108
	moveq	#31,d0
	add.w	(92+l2114,a7),d0
	move.w	d0,-(a7)
	moveq	#18,d0
	add.w	(92+l2114,a7),d0
	move.w	d0,-(a7)
	move.l	(106+l2114,a7),-(a7)
	jsr	l1048
	move.w	#1,-(a7)
	move.w	#30,-(a7)
	move.w	#1,-(a7)
	move.l	(120+l2114,a7),a0
	add.w	#3140,a0
	move.w	(100+l2114,a7),d0
	ext.l	d0
	lsl.l	#1,d0
	add.l	d0,a0
	moveq	#1,d0
	add.w	(a0),d0
	ext.l	d0
	move.l	d0,-(a7)
	moveq	#31,d0
	add.w	(110+l2114,a7),d0
	move.w	d0,-(a7)
	moveq	#30,d0
	add.w	(110+l2114,a7),d0
	move.w	d0,-(a7)
	move.l	(124+l2114,a7),-(a7)
	jsr	l1148
	add.w	#84,a7
l2104
	addq.w	#1,(0+l2114,a7)
l2102
	move.w	(0+l2114,a7),d0
	cmp.w	(16+l2114,a7),d0
	blt	l2101
l2103
	move.l	(48+l2114,a7),a0
	move.w	(3182,a0),(22+l2114,a7)
	blt	l2109
l2111
	move.w	(22+l2114,a7),d0
	cmp.w	(16+l2114,a7),d0
	blt	l2110
l2109
	move.w	#0,(22+l2114,a7)
l2110
	move.l	(48+l2114,a7),a0
	add.w	#3174,a0
	move.w	(22+l2114,a7),d0
	ext.l	d0
	lsl.l	#1,d0
	move.w	(0,a0,d0.l),(24+l2114,a7)
	move.w	(24+l2114,a7),-(a7)
	jsr	l193
	move.b	d0,(28+l2114,a7)
	move.w	#1,-(a7)
	moveq	#0,d0
	move.b	(30+l2114,a7),d0
	move.w	d0,-(a7)
	move.w	(30+l2114,a7),-(a7)
	jsr	l155
	addq.w	#2,a7
	move.l	d0,-(a7)
	move.w	#124,-(a7)
	move.w	(16+l2114,a7),-(a7)
	move.w	(16+l2114,a7),-(a7)
	move.l	(60+l2114,a7),-(a7)
	jsr	l1054
	move.w	#1,-(a7)
	move.w	#30,-(a7)
	move.w	(48+l2114,a7),-(a7)
	jsr	l174
	addq.w	#2,a7
	move.l	d0,-(a7)
	move.w	#130,-(a7)
	move.w	(34+l2114,a7),-(a7)
	move.w	(34+l2114,a7),-(a7)
	move.l	(78+l2114,a7),-(a7)
	jsr	l1054
	move.w	#30,-(a7)
	move.w	#3,-(a7)
	move.w	#135,-(a7)
	moveq	#8,d0
	add.w	(46+l2114,a7),d0
	move.w	d0,-(a7)
	move.l	(90+l2114,a7),-(a7)
	jsr	l2050
	move.w	#30,-(a7)
	move.w	#4,-(a7)
	move.w	#135,-(a7)
	move.w	(60+l2114,a7),d0
	sub.w	#12,d0
	move.w	d0,-(a7)
	move.l	(102+l2114,a7),-(a7)
	jsr	l2050
	move.w	#30,-(a7)
	moveq	#0,d0
	move.b	(90+l2114,a7),d0
	move.w	d0,-(a7)
	move.w	#135,-(a7)
	move.w	(72+l2114,a7),d0
	sub.w	(70+l2114,a7),d0
	tst.w	d0
	bge	l2117
	addq.w	#1,d0
l2117:
	asr.w	#1,d0
	add.w	(70+l2114,a7),d0
	subq.w	#7,d0
	move.w	d0,-(a7)
	move.l	(114+l2114,a7),-(a7)
	jsr	l2062
	add.w	#74,a7
l2094
l2112	reg
l2114	equ	0
	add.w	#36,a7
	rts
	cnop	0,4
l2108
	dc.b	76
	dc.b	86
	dc.b	0
	cnop	0,4
l2100
	dc.b	80
	dc.b	73
	dc.b	67
	dc.b	75
	dc.b	32
	dc.b	80
	dc.b	69
	dc.b	82
	dc.b	75
	dc.b	0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l2118
	sub.w	#44,a7
	movem.l	l2168,-(a7)
	move.w	#36,(2+l2170,a7)
	move.w	#283,(4+l2170,a7)
	move.w	#58,(6+l2170,a7)
	move.w	#143,(8+l2170,a7)
	move.w	#48,(10+l2170,a7)
	move.w	#44,(12+l2170,a7)
	move.w	#8,(14+l2170,a7)
	move.l	(56+l2170,a7),a0
	move.w	(3200,a0),(16+l2170,a7)
	cmp.w	#1,(16+l2170,a7)
	bge	l2122
l2121
	move.w	#1,(16+l2170,a7)
l2122
	cmp.w	#4,(16+l2170,a7)
	ble	l2124
l2123
	move.w	#4,(16+l2170,a7)
l2124
	move.w	(16+l2170,a7),d0
	muls.w	(10+l2170,a7),d0
	move.w	(16+l2170,a7),d1
	subq.w	#1,d1
	muls.w	(14+l2170,a7),d1
	add.w	d1,d0
	move.w	d0,(18+l2170,a7)
	move.w	(4+l2170,a7),d0
	sub.w	(2+l2170,a7),d0
	addq.w	#1,d0
	sub.w	(18+l2170,a7),d0
	tst.w	d0
	bge	l2171
	addq.w	#1,d0
l2171:
	asr.w	#1,d0
	add.w	(2+l2170,a7),d0
	move.w	d0,(20+l2170,a7)
	move.w	#16,-(a7)
	move.w	(10+l2170,a7),-(a7)
	move.w	(8+l2170,a7),-(a7)
	move.w	(12+l2170,a7),-(a7)
	move.w	(10+l2170,a7),-(a7)
	move.l	(62+l2170,a7),-(a7)
	jsr	l1979
	move.w	#1,-(a7)
	move.w	#31,-(a7)
	pea	l2125
	move.w	#64,-(a7)
	move.w	(28+l2170,a7),-(a7)
	move.w	(28+l2170,a7),-(a7)
	move.l	(80+l2170,a7),-(a7)
	jsr	l1054
	move.w	#1,-(a7)
	move.w	#30,-(a7)
	pea	l2126
	move.w	#70,-(a7)
	move.w	(46+l2170,a7),-(a7)
	move.w	(46+l2170,a7),-(a7)
	move.l	(98+l2170,a7),-(a7)
	jsr	l1054
	move.w	#8,-(a7)
	move.w	#1,-(a7)
	move.l	(110+l2170,a7),a0
	move.l	(3114,a0),-(a7)
	move.w	#70,-(a7)
	move.w	(64+l2170,a7),d0
	sub.w	#10,d0
	move.w	d0,-(a7)
	move.l	(114+l2170,a7),-(a7)
	jsr	l1171
	move.w	#0,(66+l2170,a7)
	add.w	#66,a7
	bra	l2128
l2127
	move.l	(56+l2170,a7),a0
	add.w	#3184,a0
	move.w	(0+l2170,a7),d0
	ext.l	d0
	lsl.l	#1,d0
	move.w	(0,a0,d0.l),(30+l2170,a7)
	move.l	(56+l2170,a7),a0
	add.w	#3192,a0
	move.w	(0+l2170,a7),d0
	ext.l	d0
	lsl.l	#1,d0
	move.w	(0,a0,d0.l),(32+l2170,a7)
	move.l	(56+l2170,a7),a0
	move.w	(0+l2170,a7),d0
	cmp.w	(3202,a0),d0
	beq	l2131
l2133
	moveq	#0,d0
	bra	l2132
l2131
	moveq	#1,d0
l2132
	move.b	d0,(34+l2170,a7)
	move.l	(56+l2170,a7),a0
	add.w	#3114,a0
	tst.w	(32+l2170,a7)
	bge	l2138
l2137
	moveq	#0,d0
	bra	l2139
l2138
	move.w	(32+l2170,a7),d0
l2139
	ext.l	d0
	cmp.l	(a0),d0
	bls	l2134
l2136
	moveq	#0,d0
	bra	l2135
l2134
	moveq	#1,d0
l2135
	move.b	d0,(35+l2170,a7)
	beq	l2141
l2140
	move.w	(30+l2170,a7),-(a7)
	jsr	l253
	and.w	#255,d0
	addq.w	#2,a7
	bra	l2142
l2141
	moveq	#23,d0
l2142
	move.b	d0,(36+l2170,a7)
	move.w	(10+l2170,a7),d0
	add.w	(14+l2170,a7),d0
	muls.w	(0+l2170,a7),d0
	add.w	(20+l2170,a7),d0
	move.w	d0,(38+l2170,a7)
	move.w	#75,(40+l2170,a7)
	moveq	#0,d0
	move.b	(34+l2170,a7),d0
	move.w	d0,-(a7)
	moveq	#0,d0
	move.b	(38+l2170,a7),d0
	move.w	d0,-(a7)
	move.w	(16+l2170,a7),-(a7)
	move.w	(16+l2170,a7),-(a7)
	move.w	(48+l2170,a7),-(a7)
	move.w	(48+l2170,a7),-(a7)
	move.l	(64+l2170,a7),-(a7)
	jsr	l2012
	moveq	#5,d0
	add.w	(56+l2170,a7),d0
	move.w	d0,-(a7)
	move.w	(28+l2170,a7),d0
	sub.w	#16,d0
	tst.w	d0
	bge	l2172
	addq.w	#1,d0
l2172:
	asr.w	#1,d0
	add.w	(56+l2170,a7),d0
	move.w	d0,-(a7)
	move.w	(50+l2170,a7),-(a7)
	jsr	l265
	addq.w	#2,a7
	move.w	d0,-(a7)
	move.l	(74+l2170,a7),-(a7)
	move.l	(74+l2170,a7),-(a7)
	jsr	l380
	jsr	l290
	move.w	#1,-(a7)
	moveq	#0,d0
	move.b	(68+l2170,a7),d0
	move.w	d0,-(a7)
	move.w	(64+l2170,a7),-(a7)
	jsr	l219
	addq.w	#2,a7
	move.l	d0,-(a7)
	moveq	#24,d0
	add.w	(78+l2170,a7),d0
	move.w	d0,-(a7)
	move.w	(78+l2170,a7),d0
	add.w	(50+l2170,a7),d0
	subq.w	#3,d0
	move.w	d0,-(a7)
	moveq	#2,d0
	add.w	(80+l2170,a7),d0
	move.w	d0,-(a7)
	move.l	(96+l2170,a7),-(a7)
	jsr	l1054
	move.w	#1,-(a7)
	tst.b	(85+l2170,a7)
	beq	l2144
l2143
	moveq	#30,d0
	bra	l2145
l2144
	moveq	#23,d0
l2145
	move.w	d0,-(a7)
	pea	l2146
	moveq	#31,d0
	add.w	(96+l2170,a7),d0
	move.w	d0,-(a7)
	moveq	#14,d0
	add.w	(96+l2170,a7),d0
	move.w	d0,-(a7)
	move.l	(112+l2170,a7),-(a7)
	jsr	l1048
	move.w	#1,-(a7)
	tst.b	(101+l2170,a7)
	beq	l2148
l2147
	moveq	#30,d0
	bra	l2149
l2148
	moveq	#23,d0
l2149
	move.w	d0,-(a7)
	move.w	#1,-(a7)
	move.w	#999,-(a7)
	move.w	#0,-(a7)
	move.w	(106+l2170,a7),-(a7)
	jsr	l15
	addq.w	#6,a7
	ext.l	d0
	move.l	d0,-(a7)
	moveq	#31,d0
	add.w	(114+l2170,a7),d0
	move.w	d0,-(a7)
	moveq	#22,d0
	add.w	(114+l2170,a7),d0
	move.w	d0,-(a7)
	move.l	(130+l2170,a7),-(a7)
	jsr	l1148
	add.w	#82,a7
l2130
	addq.w	#1,(0+l2170,a7)
l2128
	move.w	(0+l2170,a7),d0
	cmp.w	(16+l2170,a7),d0
	blt	l2127
l2129
	move.l	(56+l2170,a7),a0
	move.w	(3202,a0),(22+l2170,a7)
	blt	l2150
l2152
	move.w	(22+l2170,a7),d0
	cmp.w	(16+l2170,a7),d0
	blt	l2151
l2150
	move.w	#0,(22+l2170,a7)
l2151
	move.l	(56+l2170,a7),a0
	add.w	#3184,a0
	move.w	(22+l2170,a7),d0
	ext.l	d0
	lsl.l	#1,d0
	move.w	(0,a0,d0.l),(24+l2170,a7)
	move.l	(56+l2170,a7),a0
	add.w	#3192,a0
	move.w	(22+l2170,a7),d0
	ext.l	d0
	lsl.l	#1,d0
	move.w	(0,a0,d0.l),(26+l2170,a7)
	move.l	(56+l2170,a7),a0
	add.w	#3114,a0
	tst.w	(26+l2170,a7)
	bge	l2157
l2156
	moveq	#0,d0
	bra	l2158
l2157
	move.w	(26+l2170,a7),d0
l2158
	ext.l	d0
	cmp.l	(a0),d0
	bls	l2153
l2155
	moveq	#0,d0
	bra	l2154
l2153
	moveq	#1,d0
l2154
	move.b	d0,(28+l2170,a7)
	beq	l2160
l2159
	moveq	#30,d0
	bra	l2161
l2160
	moveq	#23,d0
l2161
	move.b	d0,(29+l2170,a7)
	move.w	#1,-(a7)
	tst.b	(30+l2170,a7)
	beq	l2163
l2162
	move.w	(26+l2170,a7),-(a7)
	jsr	l253
	addq.w	#2,a7
	and.w	#255,d0
	bra	l2164
l2163
	moveq	#23,d0
l2164
	move.w	d0,-(a7)
	move.w	(28+l2170,a7),-(a7)
	jsr	l219
	addq.w	#2,a7
	move.l	d0,-(a7)
	move.w	#124,-(a7)
	move.w	(14+l2170,a7),-(a7)
	move.w	(14+l2170,a7),-(a7)
	move.l	(66+l2170,a7),-(a7)
	jsr	l1054
	move.w	#1,-(a7)
	moveq	#0,d0
	move.b	(49+l2170,a7),d0
	move.w	d0,-(a7)
	move.w	(46+l2170,a7),-(a7)
	jsr	l236
	addq.w	#2,a7
	move.l	d0,-(a7)
	move.w	#130,-(a7)
	move.w	(32+l2170,a7),-(a7)
	move.w	(32+l2170,a7),-(a7)
	move.l	(84+l2170,a7),-(a7)
	jsr	l1054
	move.w	#30,-(a7)
	move.w	#3,-(a7)
	move.w	#135,-(a7)
	moveq	#8,d0
	add.w	(44+l2170,a7),d0
	move.w	d0,-(a7)
	move.l	(96+l2170,a7),-(a7)
	jsr	l2050
	move.w	#30,-(a7)
	move.w	#4,-(a7)
	move.w	#135,-(a7)
	move.w	(58+l2170,a7),d0
	sub.w	#12,d0
	move.w	d0,-(a7)
	move.l	(108+l2170,a7),-(a7)
	jsr	l2050
	add.w	#60,a7
	tst.b	(28+l2170,a7)
	beq	l2166
l2165
	moveq	#16,d0
	bra	l2167
l2166
	moveq	#2,d0
l2167
	move.w	d0,-(a7)
	moveq	#0,d0
	move.b	(31+l2170,a7),d0
	move.w	d0,-(a7)
	move.w	#135,-(a7)
	move.w	(10+l2170,a7),d0
	sub.w	(8+l2170,a7),d0
	tst.w	d0
	bge	l2173
	addq.w	#1,d0
l2173:
	asr.w	#1,d0
	add.w	(8+l2170,a7),d0
	subq.w	#7,d0
	move.w	d0,-(a7)
	move.l	(60+l2170,a7),-(a7)
	jsr	l2062
	add.w	#12,a7
l2119
l2168	reg
l2170	equ	0
	add.w	#44,a7
	rts
	cnop	0,4
l2146
	dc.b	67
	dc.b	0
	cnop	0,4
l2125
	dc.b	77
	dc.b	69
	dc.b	84
	dc.b	65
	dc.b	32
	dc.b	85
	dc.b	80
	dc.b	71
	dc.b	82
	dc.b	65
	dc.b	68
	dc.b	69
	dc.b	0
	cnop	0,4
l2126
	dc.b	83
	dc.b	72
	dc.b	65
	dc.b	82
	dc.b	68
	dc.b	83
	dc.b	0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l2174
	sub.w	#16,a7
	movem.l	l2195,-(a7)
	moveq	#6,d1
	move.l	l2,d0
	jsr	__divu
	move.l	d0,d2
	and.l	#1,d2
	bne	l2179
l2178
	moveq	#0,d0
	bra	l2180
l2179
	moveq	#1,d0
l2180
	move.w	d0,(0+l2197,a7)
	move.w	(28+l2197,a7),d0
	sub.w	#62,d0
	move.w	d0,(2+l2197,a7)
	move.w	(30+l2197,a7),d0
	add.w	(0+l2197,a7),d0
	move.w	d0,(4+l2197,a7)
	move.w	(4+l2197,a7),-(a7)
	move.w	(4+l2197,a7),-(a7)
	move.l	(28+l2197,a7),-(a7)
	move.l	(28+l2197,a7),-(a7)
	jsr	l386
	jsr	l290
	move.w	#0,(18+l2197,a7)
	add.w	#12,a7
	bra	l2182
l2181
	move.w	(6+l2197,a7),d0
	ext.l	d0
	lsl.l	#2,d0
	lea	l2177,a0
	add.l	d0,a0
	moveq	#125,d0
	muls.w	(a0),d0
	ext.l	d0
	divs.w	#220,d0
	add.w	(2+l2197,a7),d0
	move.w	d0,(8+l2197,a7)
	move.w	(6+l2197,a7),d0
	ext.l	d0
	lsl.l	#2,d0
	lea	l2177,a0
	add.l	d0,a0
	moveq	#83,d0
	muls.w	(2,a0),d0
	ext.l	d0
	divs.w	#48,d0
	add.w	(4+l2197,a7),d0
	move.w	d0,(10+l2197,a7)
	move.l	l2,d0
	lsr.l	#1,d0
	moveq	#5,d1
	muls.w	(6+l2197,a7),d1
	ext.l	d1
	add.l	d1,d0
	moveq	#15,d1
	and.w	d0,d1
	move.w	d1,(12+l2197,a7)
	move.w	#0,(14+l2197,a7)
	cmp.w	#3,(12+l2197,a7)
	bge	l2186
l2185
	moveq	#1,d0
	add.w	(12+l2197,a7),d0
	move.w	d0,(14+l2197,a7)
	bra	l2187
l2186
	cmp.w	#3,(12+l2197,a7)
	bne	l2189
l2188
	move.w	#2,(14+l2197,a7)
	bra	l2190
l2189
	cmp.w	#4,(12+l2197,a7)
	bne	l2192
l2191
	move.w	#1,(14+l2197,a7)
l2192
l2190
l2187
	tst.w	(14+l2197,a7)
	ble	l2194
l2193
	move.w	(14+l2197,a7),-(a7)
	move.w	(12+l2197,a7),-(a7)
	move.w	(12+l2197,a7),-(a7)
	move.l	(30+l2197,a7),-(a7)
	jsr	l1525
	add.w	#10,a7
l2194
l2184
	addq.w	#1,(6+l2197,a7)
l2182
	cmp.w	#5,(6+l2197,a7)
	blt	l2181
l2183
l2175
l2195	reg	d2
	movem.l	(a7)+,d2
l2197	equ	4
	add.w	#16,a7
	rts
	cnop	0,4
l2177
	dc.w	88
	dc.w	16
	dc.w	170
	dc.w	14
	dc.w	36
	dc.w	30
	dc.w	124
	dc.w	31
	dc.w	198
	dc.w	32
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l2198
	movem.l	l2201,-(a7)
	move.l	#16777619,d0
	move.l	(4+l2203,a7),d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	move.l	(8+l2203,a7),d1
	eor.l	d1,d0
l2199
l2201	reg	d2/d3
	movem.l	(a7)+,d2/d3
l2203	equ	8
	rts
; stacksize=8
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l2204
	movem.l	l2212,-(a7)
	move.l	(4+l2214,a7),a0
	cmp.w	#2,(3226,a0)
	bne	l2208
l2207
	moveq	#0,d0
	bra	l2205
l2208
	move.l	(4+l2214,a7),a0
	cmp.w	#1,(3226,a0)
	bne	l2210
l2211
	move.l	(4+l2214,a7),a0
	tst.b	(3221,a0)
	bne	l2210
l2209
	moveq	#0,d0
	bra	l2205
l2210
	moveq	#6,d1
	move.l	l2,d0
	jsr	__divu
	move.l	d0,d2
	and.l	#15,d2
	move.l	d2,d0
l2205
l2212	reg	d2
	movem.l	(a7)+,d2
l2214	equ	4
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l2215
	subq.w	#8,a7
	movem.l	l2229,-(a7)
	move.w	#0,(0+l2231,a7)
	move.w	#0,(2+l2231,a7)
	move.w	#0,(4+l2231,a7)
	move.w	#0,(6+l2231,a7)
	move.l	(12+l2231,a7),a0
	tst.w	(3226,a0)
	bne	l2219
l2220
	move.l	(12+l2231,a7),a0
	tst.b	(3221,a0)
	beq	l2219
l2218
	move.w	#60,(0+l2231,a7)
	move.w	#0,(2+l2231,a7)
	move.w	#228,(4+l2231,a7)
	move.w	#184,(6+l2231,a7)
	movem.l	(0+l2231,a7),d0/d1
	bra	l2216
l2219
	move.l	(12+l2231,a7),a0
	move.w	(3226,a0),d0
	sub.w	#0,d0
	beq	l2222
	subq.w	#2,d0
	beq	l2223
	subq.w	#1,d0
	beq	l2224
	subq.w	#1,d0
	beq	l2225
	subq.w	#1,d0
	beq	l2226
	subq.w	#1,d0
	beq	l2227
	bra	l2228
l2222
	move.w	#44,(0+l2231,a7)
	move.w	#54,(2+l2231,a7)
	move.w	#232,(4+l2231,a7)
	move.w	#94,(6+l2231,a7)
	bra	l2221
l2223
	move.w	#76,(0+l2231,a7)
	move.w	#78,(2+l2231,a7)
	move.w	#168,(4+l2231,a7)
	move.w	#40,(6+l2231,a7)
	bra	l2221
l2224
	move.w	#56,(0+l2231,a7)
	move.w	#70,(2+l2231,a7)
	move.w	#208,(4+l2231,a7)
	move.w	#74,(6+l2231,a7)
	bra	l2221
l2225
	move.w	#52,(0+l2231,a7)
	move.w	#66,(2+l2231,a7)
	move.w	#216,(4+l2231,a7)
	move.w	#84,(6+l2231,a7)
	bra	l2221
l2226
	move.w	#44,(0+l2231,a7)
	move.w	#54,(2+l2231,a7)
	move.w	#232,(4+l2231,a7)
	move.w	#94,(6+l2231,a7)
	bra	l2221
l2227
	move.w	#32,(0+l2231,a7)
	move.w	#54,(2+l2231,a7)
	move.w	#256,(4+l2231,a7)
	move.w	#94,(6+l2231,a7)
	bra	l2221
l2228
l2221
	movem.l	(0+l2231,a7),d0/d1
l2216
l2229	reg
l2231	equ	0
	addq.w	#8,a7
	rts
; stacksize=8
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l2232
	subq.w	#8,a7
	movem.l	l2266,-(a7)
	move.l	#2166136261,(0+l2268,a7)
	move.l	(12+l2268,a7),a0
	move.w	(3226,a0),d0
	ext.l	d0
	move.l	d0,-(a7)
	move.l	(4+l2268,a7),-(a7)
	jsr	l2198
	move.l	d0,(8+l2268,a7)
	move.l	(20+l2268,a7),a0
	addq.w	#8,a7
	tst.b	(3221,a0)
	beq	l2236
l2235
	moveq	#1,d0
	bra	l2237
l2236
	moveq	#0,d0
l2237
	move.l	d0,-(a7)
	move.l	(4+l2268,a7),-(a7)
	jsr	l2198
	move.l	d0,(8+l2268,a7)
	move.l	(20+l2268,a7),a0
	move.w	(3080,a0),d0
	ext.l	d0
	move.l	d0,-(a7)
	move.l	(12+l2268,a7),-(a7)
	jsr	l2198
	move.l	d0,(16+l2268,a7)
	move.l	(28+l2268,a7),-(a7)
	jsr	l2204
	move.l	d0,-(a7)
	move.l	(24+l2268,a7),-(a7)
	jsr	l2198
	move.l	d0,(28+l2268,a7)
	move.l	(40+l2268,a7),a0
	add.w	#28,a7
	tst.w	(3226,a0)
	bne	l2239
l2240
	move.l	(12+l2268,a7),a0
	tst.b	(3221,a0)
	beq	l2239
l2238
	move.l	(0+l2268,a7),d0
	bra	l2233
l2239
	move.l	(12+l2268,a7),a0
	move.w	(3226,a0),d0
	subq.w	#3,d0
	beq	l2242
	subq.w	#1,d0
	beq	l2246
	subq.w	#1,d0
	beq	l2253
	subq.w	#1,d0
	beq	l2259
	bra	l2265
l2242
	move.l	(12+l2268,a7),a0
	tst.w	(3084,a0)
	ble	l2244
l2243
	move.l	(12+l2268,a7),a0
	move.w	(3084,a0),d0
	ext.l	d0
	bra	l2245
l2244
	moveq	#0,d0
l2245
	move.l	d0,-(a7)
	move.l	(4+l2268,a7),-(a7)
	jsr	l2198
	move.l	d0,(8+l2268,a7)
	addq.w	#8,a7
	bra	l2241
l2246
	move.l	(12+l2268,a7),a0
	cmp.l	#99999999,(3074,a0)
	bls	l2248
l2247
	move.l	#99999999,d0
	bra	l2249
l2248
	move.l	(12+l2268,a7),a0
	move.l	(3074,a0),d0
l2249
	move.l	d0,-(a7)
	move.l	(4+l2268,a7),-(a7)
	jsr	l2198
	move.l	d0,(8+l2268,a7)
	move.l	(20+l2268,a7),a0
	move.w	(3112,a0),d0
	ext.l	d0
	move.l	d0,-(a7)
	move.l	(12+l2268,a7),-(a7)
	jsr	l2198
	move.l	d0,(16+l2268,a7)
	move.l	(28+l2268,a7),a0
	add.w	#16,a7
	tst.w	(3228,a0)
	ble	l2251
l2250
	moveq	#1,d0
	bra	l2252
l2251
	moveq	#0,d0
l2252
	move.l	d0,-(a7)
	move.l	(4+l2268,a7),-(a7)
	jsr	l2198
	move.l	d0,(8+l2268,a7)
	addq.w	#8,a7
	bra	l2241
l2253
	move.l	(12+l2268,a7),a0
	move.w	(3180,a0),d0
	ext.l	d0
	move.l	d0,-(a7)
	move.l	(4+l2268,a7),-(a7)
	jsr	l2198
	move.l	d0,(8+l2268,a7)
	move.l	(20+l2268,a7),a0
	move.w	(3182,a0),d0
	ext.l	d0
	move.l	d0,-(a7)
	move.l	(12+l2268,a7),-(a7)
	jsr	l2198
	move.l	d0,(16+l2268,a7)
	move.w	#0,(20+l2268,a7)
	add.w	#16,a7
	bra	l2255
l2254
	move.l	(12+l2268,a7),a0
	add.w	#3174,a0
	move.w	(4+l2268,a7),d0
	ext.l	d0
	lsl.l	#1,d0
	move.w	(0,a0,d0.l),(6+l2268,a7)
	move.w	(6+l2268,a7),d0
	ext.l	d0
	move.l	d0,-(a7)
	move.l	(4+l2268,a7),-(a7)
	jsr	l2198
	move.l	d0,(8+l2268,a7)
	move.l	(20+l2268,a7),a0
	add.w	#3140,a0
	move.w	(14+l2268,a7),d0
	ext.l	d0
	lsl.l	#1,d0
	add.l	d0,a0
	move.w	(a0),d0
	ext.l	d0
	move.l	d0,-(a7)
	move.l	(12+l2268,a7),-(a7)
	jsr	l2198
	move.l	d0,(16+l2268,a7)
	add.w	#16,a7
l2257
	addq.w	#1,(4+l2268,a7)
l2255
	move.l	(12+l2268,a7),a0
	move.w	(4+l2268,a7),d0
	cmp.w	(3180,a0),d0
	bge	l2256
l2258
	cmp.w	#3,(4+l2268,a7)
	blt	l2254
l2256
	bra	l2241
l2259
	move.l	(12+l2268,a7),a0
	move.w	(3200,a0),d0
	ext.l	d0
	move.l	d0,-(a7)
	move.l	(4+l2268,a7),-(a7)
	jsr	l2198
	move.l	d0,(8+l2268,a7)
	move.l	(20+l2268,a7),a0
	move.w	(3202,a0),d0
	ext.l	d0
	move.l	d0,-(a7)
	move.l	(12+l2268,a7),-(a7)
	jsr	l2198
	move.l	d0,(16+l2268,a7)
	move.l	(28+l2268,a7),a0
	move.l	(3114,a0),-(a7)
	move.l	(20+l2268,a7),-(a7)
	jsr	l2198
	move.l	d0,(24+l2268,a7)
	move.w	#0,(28+l2268,a7)
	add.w	#24,a7
	bra	l2261
l2260
	move.l	(12+l2268,a7),a0
	add.w	#3184,a0
	move.w	(4+l2268,a7),d0
	ext.l	d0
	lsl.l	#1,d0
	add.l	d0,a0
	move.w	(a0),d0
	ext.l	d0
	move.l	d0,-(a7)
	move.l	(4+l2268,a7),-(a7)
	jsr	l2198
	move.l	d0,(8+l2268,a7)
	move.l	(20+l2268,a7),a0
	add.w	#3192,a0
	move.w	(12+l2268,a7),d0
	ext.l	d0
	lsl.l	#1,d0
	add.l	d0,a0
	move.w	(a0),d0
	ext.l	d0
	move.l	d0,-(a7)
	move.l	(12+l2268,a7),-(a7)
	jsr	l2198
	move.l	d0,(16+l2268,a7)
	add.w	#16,a7
l2263
	addq.w	#1,(4+l2268,a7)
l2261
	move.l	(12+l2268,a7),a0
	move.w	(4+l2268,a7),d0
	cmp.w	(3200,a0),d0
	bge	l2262
l2264
	cmp.w	#4,(4+l2268,a7)
	blt	l2260
l2262
	bra	l2241
l2265
l2241
	move.l	(0+l2268,a7),d0
l2233
l2266	reg
l2268	equ	0
	addq.w	#8,a7
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l2269
	subq.w	#8,a7
	movem.l	l2302,-(a7)
	move.b	#0,(0+l2304,a7)
	move.l	(16+l2304,a7),a0
	tst.w	(3124,a0)
	beq	l2273
l2275
	move.l	(16+l2304,a7),a0
	tst.w	(3126,a0)
	ble	l2273
l2274
	move.l	(16+l2304,a7),a0
	move.w	(3126,a0),d0
	ext.l	d0
	divs.w	#3,d0
	and.w	#1,d0
	bne	l2273
l2272
	move.l	(16+l2304,a7),a0
	move.w	(3124,a0),-(a7)
	jsr	l277
	move.b	d0,(8+l2304,a7)
	move.w	#2,(4+l2304,a7)
	addq.w	#2,a7
	bra	l2277
l2276
	moveq	#0,d0
	move.b	(6+l2304,a7),d0
	move.w	d0,-(a7)
	move.w	#2,-(a7)
	move.w	(6+l2304,a7),-(a7)
	move.l	(18+l2304,a7),-(a7)
	jsr	l881
	moveq	#0,d0
	move.b	(16+l2304,a7),d0
	move.w	d0,-(a7)
	move.w	#189,-(a7)
	move.w	(16+l2304,a7),-(a7)
	move.l	(28+l2304,a7),-(a7)
	jsr	l881
	add.w	#20,a7
l2279
	addq.w	#1,(2+l2304,a7)
l2277
	cmp.w	#318,(2+l2304,a7)
	blt	l2276
l2278
	move.b	#1,(0+l2304,a7)
l2273
	move.l	(16+l2304,a7),a0
	tst.w	(3092,a0)
	ble	l2281
l2282
	move.l	(16+l2304,a7),a0
	move.w	(3092,a0),d0
	ext.l	d0
	divs.w	#3,d0
	and.w	#1,d0
	bne	l2281
l2280
	move.w	#0,(2+l2304,a7)
	bra	l2284
l2283
	move.w	#8,-(a7)
	move.w	#0,-(a7)
	move.w	(6+l2304,a7),-(a7)
	move.l	(18+l2304,a7),-(a7)
	jsr	l881
	move.w	#8,-(a7)
	move.w	#199,-(a7)
	move.w	(16+l2304,a7),-(a7)
	move.l	(28+l2304,a7),-(a7)
	jsr	l881
	add.w	#20,a7
l2286
	addq.w	#1,(2+l2304,a7)
l2284
	cmp.w	#320,(2+l2304,a7)
	blt	l2283
l2285
	move.w	#0,(4+l2304,a7)
	bra	l2288
l2287
	move.w	#8,-(a7)
	move.w	(6+l2304,a7),-(a7)
	move.w	#0,-(a7)
	move.l	(18+l2304,a7),-(a7)
	jsr	l881
	move.w	#8,-(a7)
	move.w	(16+l2304,a7),-(a7)
	move.w	#319,-(a7)
	move.l	(28+l2304,a7),-(a7)
	jsr	l881
	add.w	#20,a7
l2290
	addq.w	#1,(4+l2304,a7)
l2288
	cmp.w	#200,(4+l2304,a7)
	blt	l2287
l2289
	move.b	#1,(0+l2304,a7)
l2281
	move.l	(16+l2304,a7),a0
	cmp.w	#1,(3226,a0)
	bne	l2292
l2293
	move.l	(16+l2304,a7),a0
	tst.w	(3096,a0)
	ble	l2292
l2291
	move.w	#0,(2+l2304,a7)
	bra	l2295
l2294
	move.w	#23,-(a7)
	move.w	#0,-(a7)
	move.w	(6+l2304,a7),-(a7)
	move.l	(18+l2304,a7),-(a7)
	jsr	l881
	move.w	#23,-(a7)
	move.w	#191,-(a7)
	move.w	(16+l2304,a7),-(a7)
	move.l	(28+l2304,a7),-(a7)
	jsr	l881
	add.w	#20,a7
l2297
	addq.w	#1,(2+l2304,a7)
l2295
	cmp.w	#320,(2+l2304,a7)
	blt	l2294
l2296
	move.w	#0,(4+l2304,a7)
	bra	l2299
l2298
	move.w	#23,-(a7)
	move.w	(6+l2304,a7),-(a7)
	move.w	#0,-(a7)
	move.l	(18+l2304,a7),-(a7)
	jsr	l881
	move.w	#23,-(a7)
	move.w	(16+l2304,a7),-(a7)
	move.w	#319,-(a7)
	move.l	(28+l2304,a7),-(a7)
	jsr	l881
	add.w	#20,a7
l2301
	addq.w	#1,(4+l2304,a7)
l2299
	cmp.w	#192,(4+l2304,a7)
	blt	l2298
l2300
	move.b	#1,(0+l2304,a7)
l2292
	move.b	(0+l2304,a7),d0
l2270
l2302	reg
l2304	equ	0
	addq.w	#8,a7
	rts
; stacksize=66
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l2305
	movem.l	l2317,-(a7)
	move.l	(4+l2319,a7),a0
	tst.w	(3124,a0)
	beq	l2312
l2314
	move.l	(4+l2319,a7),a0
	tst.w	(3126,a0)
	ble	l2312
l2313
	move.l	(4+l2319,a7),a0
	move.w	(3126,a0),d0
	ext.l	d0
	divs.w	#3,d0
	and.w	#1,d0
	beq	l2310
l2312
	move.l	(4+l2319,a7),a0
	tst.w	(3092,a0)
	ble	l2308
l2315
	move.l	(4+l2319,a7),a0
	move.w	(3092,a0),d0
	ext.l	d0
	divs.w	#3,d0
	and.w	#1,d0
	beq	l2310
l2308
	move.l	(4+l2319,a7),a0
	cmp.w	#1,(3226,a0)
	bne	l2309
l2316
	move.l	(4+l2319,a7),a0
	tst.w	(3096,a0)
	bgt	l2310
l2309
	moveq	#0,d0
	bra	l2311
l2310
	moveq	#1,d0
l2311
l2306
l2317	reg
l2319	equ	0
	rts
; stacksize=0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l2320
	movem.l	l2327,-(a7)
	move.l	(4+l2329,a7),a0
	cmp.w	#1,(3226,a0)
	bne	l2325
l2323
	move.l	(4+l2329,a7),a0
	tst.b	(3221,a0)
	bne	l2325
l2324
	moveq	#0,d0
	bra	l2326
l2325
	moveq	#1,d0
l2326
l2321
l2327	reg
l2329	equ	0
	rts
; stacksize=0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l2330
	movem.l	l2339,-(a7)
	move.l	(4+l2341,a7),a0
	move.l	(8+l2341,a7),a1
	move.w	(a0),d0
	cmp.w	(a1),d0
	bne	l2335
l2338
	move.l	(4+l2341,a7),a0
	move.l	(8+l2341,a7),a1
	move.w	(2,a0),d0
	cmp.w	(2,a1),d0
	bne	l2335
l2337
	move.l	(4+l2341,a7),a0
	move.l	(8+l2341,a7),a1
	move.w	(4,a0),d0
	cmp.w	(4,a1),d0
	bne	l2335
l2333
	move.l	(4+l2341,a7),a0
	move.l	(8+l2341,a7),a1
	move.w	(6,a0),d0
	cmp.w	(6,a1),d0
	bne	l2335
l2334
	moveq	#1,d0
	bra	l2336
l2335
	moveq	#0,d0
l2336
l2331
l2339	reg
l2341	equ	0
	rts
; stacksize=0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l2342
	sub.w	#60,a7
	movem.l	l2402,-(a7)
	move.l	(72+l2404,a7),-(a7)
	jsr	l2320
	addq.w	#4,a7
	tst.b	d0
	bne	l2346
l2345
	move.l	(72+l2404,a7),-(a7)
	jsr	l2305
	addq.w	#4,a7
	tst.b	d0
	bne	l2348
l2347
	moveq	#0,d0
	bra	l2343
l2348
	jsr	l290
	move.l	(72+l2404,a7),-(a7)
	move.l	(72+l2404,a7),-(a7)
	jsr	l2269
	move.b	d0,(8+l2404,a7)
	move.b	(8+l2404,a7),d0
	addq.w	#8,a7
	bra	l2343
l2346
	jsr	l290
	move.l	(72+l2404,a7),a0
	tst.w	(3226,a0)
	bne	l2350
l2351
	move.l	(72+l2404,a7),a0
	tst.b	(3221,a0)
	beq	l2350
l2349
	move.w	#10,-(a7)
	move.w	#160,-(a7)
	move.l	(72+l2404,a7),-(a7)
	move.l	(72+l2404,a7),-(a7)
	jsr	l2174
	move.w	#1,-(a7)
	moveq	#14,d1
	move.l	l2,d0
	jsr	__divu
	move.l	d0,d2
	and.l	#1,d2
	bne	l2353
l2352
	moveq	#29,d0
	bra	l2354
l2353
	moveq	#30,d0
l2354
	move.w	d0,-(a7)
	pea	l2355
	move.w	#84,-(a7)
	move.w	#319,-(a7)
	move.w	#0,-(a7)
	move.l	(94+l2404,a7),-(a7)
	jsr	l1054
	move.l	(98+l2404,a7),-(a7)
	jsr	l2039
	move.w	#14,-(a7)
	move.w	#178,-(a7)
	move.w	#249,-(a7)
	move.w	#148,-(a7)
	move.w	#70,-(a7)
	move.l	(112+l2404,a7),-(a7)
	jsr	l1979
	move.w	#1,-(a7)
	move.w	#30,-(a7)
	pea	l2356
	move.w	#155,-(a7)
	move.w	#249,-(a7)
	move.w	#70,-(a7)
	move.l	(130+l2404,a7),-(a7)
	jsr	l1054
	move.w	#1,-(a7)
	move.w	#31,-(a7)
	move.w	#2,-(a7)
	move.l	(144+l2404,a7),a0
	move.w	(3080,a0),d0
	ext.l	d0
	move.l	d0,-(a7)
	move.w	#155,-(a7)
	move.w	#164,-(a7)
	move.l	(148+l2404,a7),-(a7)
	jsr	l1148
	move.w	#1,-(a7)
	move.w	#31,-(a7)
	pea	l2357
	move.w	#163,-(a7)
	move.w	#249,-(a7)
	move.w	#70,-(a7)
	move.l	(166+l2404,a7),-(a7)
	jsr	l1054
	move.w	#14,-(a7)
	move.w	#30,-(a7)
	move.w	#171,-(a7)
	move.w	#152,-(a7)
	move.l	(178+l2404,a7),-(a7)
	jsr	l2062
	moveq	#1,d0
	add.w	#114,a7
	bra	l2343
l2350
	move.l	(72+l2404,a7),a0
	tst.w	(3226,a0)
	bne	l2359
l2358
	move.w	#48,(2+l2404,a7)
	move.w	#271,(4+l2404,a7)
	move.w	#58,(6+l2404,a7)
	move.w	#143,(8+l2404,a7)
	move.w	(4+l2404,a7),d0
	sub.w	(2+l2404,a7),d0
	moveq	#1,d1
	add.w	d0,d1
	move.w	d1,(10+l2404,a7)
	move.w	#2,-(a7)
	move.l	(74+l2404,a7),a0
	move.w	(3080,a0),d0
	ext.l	d0
	move.l	d0,-(a7)
	jsr	l1154
	move.w	d0,(18+l2404,a7)
	move.w	(16+l2404,a7),d0
	sub.w	(18+l2404,a7),d0
	tst.w	d0
	bge	l2405
	addq.w	#1,d0
l2405:
	asr.w	#1,d0
	add.w	(8+l2404,a7),d0
	move.w	d0,(20+l2404,a7)
	moveq	#14,d0
	add.w	(8+l2404,a7),d0
	move.w	d0,(22+l2404,a7)
	moveq	#24,d0
	add.w	(12+l2404,a7),d0
	move.w	d0,(24+l2404,a7)
	move.w	(16+l2404,a7),d0
	sub.w	#28,d0
	move.w	d0,(26+l2404,a7)
	move.w	#44,(28+l2404,a7)
	move.w	#14,-(a7)
	move.w	(16+l2404,a7),-(a7)
	move.w	(14+l2404,a7),-(a7)
	move.w	(18+l2404,a7),-(a7)
	move.w	(16+l2404,a7),-(a7)
	move.l	(84+l2404,a7),-(a7)
	jsr	l1979
	move.w	#1,-(a7)
	move.w	#31,-(a7)
	pea	l2360
	moveq	#8,d0
	add.w	(34+l2404,a7),d0
	move.w	d0,-(a7)
	move.w	(34+l2404,a7),-(a7)
	move.w	(34+l2404,a7),-(a7)
	move.l	(102+l2404,a7),-(a7)
	jsr	l1054
	move.w	#1,-(a7)
	move.w	#31,-(a7)
	move.w	#2,-(a7)
	move.l	(116+l2404,a7),a0
	move.w	(3080,a0),d0
	ext.l	d0
	move.l	d0,-(a7)
	moveq	#16,d0
	add.w	(54+l2404,a7),d0
	move.w	d0,-(a7)
	move.w	(64+l2404,a7),-(a7)
	move.l	(120+l2404,a7),-(a7)
	jsr	l1148
	move.w	#1,-(a7)
	move.w	#14,-(a7)
	move.w	(82+l2404,a7),-(a7)
	move.w	(82+l2404,a7),-(a7)
	move.w	(82+l2404,a7),-(a7)
	move.w	(82+l2404,a7),-(a7)
	move.l	(136+l2404,a7),-(a7)
	jsr	l2012
	move.w	(94+l2404,a7),d0
	subq.w	#8,d0
	move.w	d0,-(a7)
	move.w	(94+l2404,a7),d0
	sub.w	#12,d0
	move.w	d0,-(a7)
	moveq	#4,d0
	add.w	(94+l2404,a7),d0
	move.w	d0,-(a7)
	moveq	#6,d0
	add.w	(94+l2404,a7),d0
	move.w	d0,-(a7)
	move.l	(152+l2404,a7),-(a7)
	move.l	(152+l2404,a7),-(a7)
	move.l	(152+l2404,a7),-(a7)
	jsr	l2071
	move.w	#14,-(a7)
	move.w	#30,-(a7)
	move.w	(104+l2404,a7),d0
	sub.w	#11,d0
	move.w	d0,-(a7)
	move.w	(108+l2404,a7),d0
	bge	l2406
	addq.w	#1,d0
l2406:
	asr.w	#1,d0
	add.w	(100+l2404,a7),d0
	subq.w	#7,d0
	move.w	d0,-(a7)
	move.l	(168+l2404,a7),-(a7)
	jsr	l2062
	moveq	#1,d0
	add.w	#104,a7
	bra	l2343
l2359
	move.l	(72+l2404,a7),a0
	cmp.w	#3,(3226,a0)
	bne	l2362
l2361
	move.w	#60,(2+l2404,a7)
	move.w	#259,(4+l2404,a7)
	move.w	#74,(6+l2404,a7)
	move.w	#137,(8+l2404,a7)
	move.l	(72+l2404,a7),a0
	tst.w	(3084,a0)
	ble	l2364
l2363
	move.l	(72+l2404,a7),a0
	move.w	(3084,a0),d0
	ext.l	d0
	bra	l2365
l2364
	moveq	#0,d0
l2365
	move.l	d0,(10+l2404,a7)
	move.w	#0,(50+l2404,a7)
	move.l	(10+l2404,a7),(54+l2404,a7)
l2366
	moveq	#10,d1
	move.l	(54+l2404,a7),d0
	jsr	__divu
	move.l	d1,d0
	move.l	d0,d2
	lea	(30+l2404,a7),a0
	move.w	(50+l2404,a7),d0
	addq.w	#1,(50+l2404,a7)
	ext.l	d0
	lsl.l	#1,d0
	move.w	d2,(0,a0,d0.l)
	moveq	#10,d1
	move.l	(54+l2404,a7),d0
	jsr	__divu
	move.l	d0,d2
	move.l	d2,(54+l2404,a7)
l2368
	tst.l	(54+l2404,a7)
	bls	l2367
l2369
	cmp.w	#10,(50+l2404,a7)
	blt	l2366
l2367
	move.b	#43,(14+l2404,a7)
	move.w	(50+l2404,a7),d0
	subq.w	#1,d0
	move.w	d0,(52+l2404,a7)
	bra	l2371
l2370
	lea	(30+l2404,a7),a0
	move.w	(52+l2404,a7),d0
	ext.l	d0
	lsl.l	#1,d0
	add.l	d0,a0
	moveq	#48,d0
	add.w	(a0),d0
	lea	(14+l2404,a7),a0
	move.w	(50+l2404,a7),d1
	sub.w	(52+l2404,a7),d1
	move.b	d0,(0,a0,d1.w)
l2373
	subq.w	#1,(52+l2404,a7)
l2371
	tst.w	(52+l2404,a7)
	bge	l2370
l2372
	lea	(14+l2404,a7),a0
	moveq	#1,d0
	add.w	(50+l2404,a7),d0
	move.b	#0,(0,a0,d0.w)
	move.w	#11,-(a7)
	move.w	(10+l2404,a7),-(a7)
	move.w	(8+l2404,a7),-(a7)
	move.w	(12+l2404,a7),-(a7)
	move.w	(10+l2404,a7),-(a7)
	move.l	(78+l2404,a7),-(a7)
	jsr	l1979
	move.w	#1,-(a7)
	moveq	#20,d0
	add.w	(22+l2404,a7),d0
	move.w	d0,-(a7)
	move.w	(22+l2404,a7),d0
	subq.w	#5,d0
	move.w	d0,-(a7)
	moveq	#5,d0
	add.w	(26+l2404,a7),d0
	move.w	d0,-(a7)
	moveq	#5,d0
	add.w	(24+l2404,a7),d0
	move.w	d0,-(a7)
	move.l	(92+l2404,a7),-(a7)
	jsr	l920
	move.w	#2,-(a7)
	move.w	#1,-(a7)
	move.w	#31,-(a7)
	pea	l2374
	moveq	#8,d0
	add.w	(44+l2404,a7),d0
	move.w	d0,-(a7)
	move.w	(44+l2404,a7),-(a7)
	move.w	(44+l2404,a7),-(a7)
	move.l	(112+l2404,a7),-(a7)
	jsr	l1123
	move.w	#1,-(a7)
	move.w	#30,-(a7)
	pea	l2375
	moveq	#28,d0
	add.w	(62+l2404,a7),d0
	move.w	d0,-(a7)
	move.w	(62+l2404,a7),-(a7)
	move.w	(62+l2404,a7),-(a7)
	move.l	(130+l2404,a7),-(a7)
	jsr	l1054
	move.w	#2,-(a7)
	move.w	#1,-(a7)
	move.w	#31,-(a7)
	lea	(86+l2404,a7),a0
	move.l	a0,-(a7)
	moveq	#39,d0
	add.w	(82+l2404,a7),d0
	move.w	d0,-(a7)
	move.w	(82+l2404,a7),-(a7)
	move.w	(82+l2404,a7),-(a7)
	move.l	(150+l2404,a7),-(a7)
	jsr	l1123
	move.w	#11,-(a7)
	move.w	#31,-(a7)
	move.w	(98+l2404,a7),d0
	sub.w	#10,d0
	move.w	d0,-(a7)
	move.w	(96+l2404,a7),d0
	sub.w	(94+l2404,a7),d0
	tst.w	d0
	bge	l2407
	addq.w	#1,d0
l2407:
	asr.w	#1,d0
	add.w	(94+l2404,a7),d0
	subq.w	#7,d0
	move.w	d0,-(a7)
	move.l	(162+l2404,a7),-(a7)
	jsr	l2062
	moveq	#1,d0
	add.w	#98,a7
	bra	l2343
l2362
	move.l	(72+l2404,a7),a0
	cmp.w	#4,(3226,a0)
	bne	l2377
l2376
	move.w	#56,(2+l2404,a7)
	move.w	#263,(4+l2404,a7)
	move.w	#70,(6+l2404,a7)
	move.w	#145,(8+l2404,a7)
	move.l	(72+l2404,a7),a0
	cmp.l	#99999999,(3074,a0)
	bls	l2379
l2378
	move.l	#99999999,d0
	bra	l2380
l2379
	move.l	(72+l2404,a7),a0
	move.l	(3074,a0),d0
l2380
	move.l	d0,(10+l2404,a7)
	move.w	#3,(14+l2404,a7)
	move.w	(14+l2404,a7),-(a7)
	move.w	#5,-(a7)
	move.l	(14+l2404,a7),-(a7)
	jsr	l1204
	move.w	d0,(24+l2404,a7)
	move.w	(12+l2404,a7),d0
	sub.w	(10+l2404,a7),d0
	addq.w	#1,d0
	sub.w	(24+l2404,a7),d0
	tst.w	d0
	bge	l2408
	addq.w	#1,d0
l2408:
	asr.w	#1,d0
	add.w	(10+l2404,a7),d0
	move.w	d0,(26+l2404,a7)
	move.l	l2,d0
	lsr.l	#2,d0
	and.l	#1,d0
	addq.w	#8,a7
	bne	l2382
l2381
	moveq	#31,d0
	bra	l2383
l2382
	moveq	#30,d0
l2383
	move.b	d0,(20+l2404,a7)
	move.w	#21,-(a7)
	move.w	(10+l2404,a7),-(a7)
	move.w	(8+l2404,a7),-(a7)
	move.w	(12+l2404,a7),-(a7)
	move.w	(10+l2404,a7),-(a7)
	move.l	(78+l2404,a7),-(a7)
	jsr	l1979
	move.w	#1,-(a7)
	move.w	#31,-(a7)
	pea	l2384
	moveq	#7,d0
	add.w	(28+l2404,a7),d0
	move.w	d0,-(a7)
	move.w	(28+l2404,a7),-(a7)
	move.w	(28+l2404,a7),-(a7)
	move.l	(96+l2404,a7),-(a7)
	jsr	l1054
	move.w	#1,-(a7)
	move.w	#30,-(a7)
	pea	l2385
	moveq	#16,d0
	add.w	(46+l2404,a7),d0
	move.w	d0,-(a7)
	move.w	(46+l2404,a7),-(a7)
	move.w	(46+l2404,a7),-(a7)
	move.l	(114+l2404,a7),-(a7)
	jsr	l1054
	move.w	(64+l2404,a7),-(a7)
	move.w	#1,-(a7)
	moveq	#0,d0
	move.b	(74+l2404,a7),d0
	move.w	d0,-(a7)
	move.w	#5,-(a7)
	move.l	(68+l2404,a7),-(a7)
	moveq	#25,d0
	add.w	(68+l2404,a7),d0
	move.w	d0,-(a7)
	move.w	(82+l2404,a7),-(a7)
	move.l	(134+l2404,a7),-(a7)
	jsr	l1234
	move.l	(142+l2404,a7),a0
	add.w	#70,a7
	tst.w	(3112,a0)
	ble	l2387
l2386
	move.w	#1,-(a7)
	move.w	#8,-(a7)
	pea	l2388
	move.w	(16+l2404,a7),d0
	sub.w	#16,d0
	move.w	d0,-(a7)
	move.w	(14+l2404,a7),-(a7)
	move.w	(14+l2404,a7),-(a7)
	move.l	(82+l2404,a7),-(a7)
	jsr	l1054
	move.w	#8,-(a7)
	move.w	#1,-(a7)
	move.l	(94+l2404,a7),a0
	move.w	(3112,a0),d0
	ext.l	d0
	move.l	d0,-(a7)
	move.w	(34+l2404,a7),d0
	sub.w	#16,d0
	move.w	d0,-(a7)
	move.w	(32+l2404,a7),d0
	sub.w	#20,d0
	move.w	d0,-(a7)
	move.l	(98+l2404,a7),-(a7)
	jsr	l1171
	add.w	#34,a7
l2387
	move.l	(72+l2404,a7),a0
	tst.w	(3228,a0)
	ble	l2390
l2389
	move.w	#1,-(a7)
	move.w	#30,-(a7)
	pea	l2391
	move.w	(16+l2404,a7),d0
	sub.w	#9,d0
	move.w	d0,-(a7)
	move.w	(14+l2404,a7),-(a7)
	move.w	(14+l2404,a7),-(a7)
	move.l	(82+l2404,a7),-(a7)
	jsr	l1054
	add.w	#18,a7
	bra	l2392
l2390
	move.w	#1,-(a7)
	move.w	#31,-(a7)
	pea	l2393
	move.w	(16+l2404,a7),d0
	sub.w	#9,d0
	move.w	d0,-(a7)
	move.w	(14+l2404,a7),-(a7)
	move.w	(14+l2404,a7),-(a7)
	move.l	(82+l2404,a7),-(a7)
	jsr	l1054
	add.w	#18,a7
l2392
	moveq	#1,d0
	bra	l2343
l2377
	move.l	(72+l2404,a7),a0
	cmp.w	#2,(3226,a0)
	bne	l2395
l2394
	move.w	#23,-(a7)
	move.w	#112,-(a7)
	move.w	#239,-(a7)
	move.w	#82,-(a7)
	move.w	#80,-(a7)
	move.l	(78+l2404,a7),-(a7)
	jsr	l1979
	move.w	#1,-(a7)
	move.w	#31,-(a7)
	pea	l2396
	move.w	#92,-(a7)
	move.w	#239,-(a7)
	move.w	#80,-(a7)
	move.l	(96+l2404,a7),-(a7)
	jsr	l1054
	move.w	#1,-(a7)
	move.w	#30,-(a7)
	pea	l2397
	move.w	#100,-(a7)
	move.w	#239,-(a7)
	move.w	#80,-(a7)
	move.l	(114+l2404,a7),-(a7)
	jsr	l1054
	moveq	#1,d0
	add.w	#50,a7
	bra	l2343
l2395
	move.l	(72+l2404,a7),a0
	cmp.w	#5,(3226,a0)
	bne	l2399
l2398
	move.l	(72+l2404,a7),-(a7)
	move.l	(72+l2404,a7),-(a7)
	move.l	(72+l2404,a7),-(a7)
	jsr	l2093
	moveq	#1,d0
	add.w	#12,a7
	bra	l2343
l2399
	move.l	(72+l2404,a7),a0
	cmp.w	#6,(3226,a0)
	bne	l2401
l2400
	move.l	(72+l2404,a7),-(a7)
	move.l	(72+l2404,a7),-(a7)
	move.l	(72+l2404,a7),-(a7)
	jsr	l2118
	moveq	#1,d0
	add.w	#12,a7
	bra	l2343
l2401
	moveq	#1,d0
l2343
l2402	reg	d2
	movem.l	(a7)+,d2
l2404	equ	4
	add.w	#60,a7
	rts
	cnop	0,4
l2355
	dc.b	82
	dc.b	69
	dc.b	84
	dc.b	82
	dc.b	79
	dc.b	70
	dc.b	79
	dc.b	85
	dc.b	78
	dc.b	68
	dc.b	82
	dc.b	89
	dc.b	0
	cnop	0,4
l2356
	dc.b	82
	dc.b	79
	dc.b	85
	dc.b	78
	dc.b	68
	dc.b	0
	cnop	0,4
l2357
	dc.b	70
	dc.b	73
	dc.b	82
	dc.b	69
	dc.b	32
	dc.b	83
	dc.b	84
	dc.b	65
	dc.b	82
	dc.b	84
	dc.b	0
	cnop	0,4
l2360
	dc.b	82
	dc.b	79
	dc.b	85
	dc.b	78
	dc.b	68
	dc.b	0
	cnop	0,4
l2374
	dc.b	82
	dc.b	79
	dc.b	85
	dc.b	78
	dc.b	68
	dc.b	32
	dc.b	67
	dc.b	76
	dc.b	69
	dc.b	65
	dc.b	82
	dc.b	0
	cnop	0,4
l2375
	dc.b	84
	dc.b	73
	dc.b	77
	dc.b	69
	dc.b	32
	dc.b	66
	dc.b	79
	dc.b	78
	dc.b	85
	dc.b	83
	dc.b	0
	cnop	0,4
l2388
	dc.b	66
	dc.b	65
	dc.b	78
	dc.b	75
	dc.b	32
	dc.b	83
	dc.b	72
	dc.b	65
	dc.b	82
	dc.b	68
	dc.b	83
	dc.b	0
	cnop	0,4
l2391
	dc.b	80
	dc.b	76
	dc.b	69
	dc.b	65
	dc.b	83
	dc.b	69
	dc.b	32
	dc.b	87
	dc.b	65
	dc.b	73
	dc.b	84
	dc.b	0
	cnop	0,4
l2393
	dc.b	70
	dc.b	73
	dc.b	82
	dc.b	69
	dc.b	32
	dc.b	82
	dc.b	69
	dc.b	83
	dc.b	84
	dc.b	65
	dc.b	82
	dc.b	84
	dc.b	0
	cnop	0,4
l2384
	dc.b	71
	dc.b	65
	dc.b	77
	dc.b	69
	dc.b	32
	dc.b	79
	dc.b	86
	dc.b	69
	dc.b	82
	dc.b	0
	cnop	0,4
l2385
	dc.b	70
	dc.b	73
	dc.b	78
	dc.b	65
	dc.b	76
	dc.b	32
	dc.b	83
	dc.b	67
	dc.b	79
	dc.b	82
	dc.b	69
	dc.b	0
	cnop	0,4
l2396
	dc.b	68
	dc.b	73
	dc.b	69
	dc.b	68
	dc.b	0
	cnop	0,4
l2397
	dc.b	82
	dc.b	69
	dc.b	83
	dc.b	80
	dc.b	65
	dc.b	87
	dc.b	78
	dc.b	73
	dc.b	78
	dc.b	71
	dc.b	0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l2409
	movem.l	l2414,-(a7)
	jsr	l290
	move.w	#1,-(a7)
	move.w	#199,-(a7)
	move.w	#319,-(a7)
	move.w	#0,-(a7)
	move.w	#0,-(a7)
	move.l	(14+l2416,a7),-(a7)
	jsr	l920
	move.w	#21,-(a7)
	move.w	#122,-(a7)
	move.w	#247,-(a7)
	move.w	#72,-(a7)
	move.w	#72,-(a7)
	move.l	(28+l2416,a7),-(a7)
	jsr	l1979
	move.w	#31,-(a7)
	pea	l2412
	move.w	#88,-(a7)
	move.w	#247,-(a7)
	move.w	#72,-(a7)
	move.l	(44+l2416,a7),-(a7)
	jsr	l1041
	move.w	#30,-(a7)
	pea	l2413
	move.w	#104,-(a7)
	move.w	#247,-(a7)
	move.w	#72,-(a7)
	move.l	(60+l2416,a7),-(a7)
	jsr	l1041
	add.w	#60,a7
l2410
l2414	reg
l2416	equ	0
	rts
	cnop	0,4
l2412
	dc.b	73
	dc.b	67
	dc.b	69
	dc.b	80
	dc.b	65
	dc.b	78
	dc.b	73
	dc.b	67
	dc.b	0
	cnop	0,4
l2413
	dc.b	76
	dc.b	79
	dc.b	65
	dc.b	68
	dc.b	73
	dc.b	78
	dc.b	71
	dc.b	0
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l2417
	sub.w	#64,a7
	movem.l	l2495,-(a7)
	move.l	(68+l2497,a7),a0
	add.w	#4946,a0
	move.l	a0,(0+l2497,a7)
	move.l	(78+l2497,a7),-(a7)
	jsr	l2320
	move.b	d0,(8+l2497,a7)
	move.l	(4+l2497,a7),a0
	add.w	#2440,a0
	move.w	(76+l2497,a7),d0
	ext.l	d0
	moveq	#14,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	add.l	d0,a0
	move.l	a0,(10+l2497,a7)
	move.l	#0,(22+l2497,a7)
	move.b	#1,(26+l2497,a7)
	addq.w	#4,a7
	move.l	(6+l2497,a7),a0
	tst.b	(a0)
	beq	l2421
l2423
	move.l	(6+l2497,a7),a0
	tst.b	(1,a0)
	beq	l2421
l2420
	moveq	#1,d0
	bra	l2422
l2421
	moveq	#0,d0
l2422
	move.b	d0,(24+l2497,a7)
	move.l	(78+l2497,a7),a0
	tst.w	(3226,a0)
	bne	l2425
l2427
	move.l	(78+l2497,a7),a0
	tst.b	(3221,a0)
	beq	l2425
l2424
	moveq	#1,d0
	bra	l2426
l2425
	moveq	#0,d0
l2426
	move.b	d0,(25+l2497,a7)
	moveq	#0,d0
	move.b	(4+l2497,a7),d0
	move.w	d0,-(a7)
	move.l	(80+l2497,a7),-(a7)
	move.w	(78+l2497,a7),-(a7)
	move.l	(8+l2497,a7),-(a7)
	move.l	(80+l2497,a7),-(a7)
	jsr	l1711
	add.w	#16,a7
	tst.b	(4+l2497,a7)
	beq	l2429
	move.l	(78+l2497,a7),-(a7)
	jsr	l2215
	movem.l	d0/d1,(48+l2497,a7)
	lea	(48+l2497,a7),a0
	lea	(14+l2497,a7),a1
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	move.l	(82+l2497,a7),-(a7)
	jsr	l2232
	move.l	d0,(26+l2497,a7)
	addq.w	#8,a7
	move.l	(6+l2497,a7),a0
	tst.b	(a0)
	beq	l2431
l2433
	move.l	(6+l2497,a7),a0
	tst.b	(1,a0)
	beq	l2431
l2432
	move.l	(6+l2497,a7),a0
	move.l	(2,a0),d0
	cmp.l	(18+l2497,a7),d0
	bne	l2431
l2430
	bra	l2418
l2431
	tst.b	(25+l2497,a7)
	bne	l2435
l2438
	move.l	(6+l2497,a7),a0
	tst.b	(a0)
	beq	l2435
l2437
	move.l	(6+l2497,a7),a0
	tst.b	(1,a0)
	beq	l2435
l2436
	lea	(10+l2497,a7),a0
	move.l	a0,-(a7)
	move.l	(10+l2497,a7),a0
	pea	(6,a0)
	jsr	l2330
	addq.w	#8,a7
	tst.b	d0
	beq	l2435
l2434
	move.b	#0,(22+l2497,a7)
	bra	l2439
l2435
	move.l	(6+l2497,a7),a0
	tst.b	(a0)
	beq	l2441
l2442
	move.l	(6+l2497,a7),a0
	tst.b	(1,a0)
	beq	l2441
l2440
	move.l	(6+l2497,a7),a0
	pea	(6,a0)
	move.w	(76+l2497,a7),-(a7)
	move.l	(6+l2497,a7),-(a7)
	jsr	l1286
	add.w	#10,a7
l2441
l2439
	bra	l2443
l2429
	move.l	(6+l2497,a7),a0
	tst.b	(a0)
	beq	l2445
l2446
	move.l	(6+l2497,a7),a0
	tst.b	(1,a0)
	beq	l2445
l2444
	move.l	(6+l2497,a7),a0
	pea	(6,a0)
	move.w	(76+l2497,a7),-(a7)
	move.l	(6+l2497,a7),-(a7)
	jsr	l1286
	move.l	(16+l2497,a7),a0
	move.b	#0,(1,a0)
	move.l	(16+l2497,a7),a0
	move.b	#0,(a0)
	add.w	#10,a7
l2445
l2443
	tst.b	(4+l2497,a7)
	beq	l2447
l2449
	tst.b	(22+l2497,a7)
	beq	l2448
l2447
	tst.b	(4+l2497,a7)
	bne	l2450
l2453
	tst.b	(24+l2497,a7)
	bne	l2450
l2452
	move.l	(0+l2497,a7),a0
	add.w	#2400,a0
	add.w	(72+l2497,a7),a0
	tst.b	(a0)
	bne	l2451
l2450
	move.w	(72+l2497,a7),-(a7)
	move.l	(2+l2497,a7),-(a7)
	jsr	l1332
	addq.w	#6,a7
l2451
l2454
	move.l	#14675968,a0
	move.w	#544,(384,a0)
l2456
l2455
	move.l	(74+l2497,a7),-(a7)
	move.w	(76+l2497,a7),-(a7)
	move.l	(6+l2497,a7),-(a7)
	move.l	(78+l2497,a7),-(a7)
	jsr	l1389
	move.w	(86+l2497,a7),-(a7)
	move.l	(94+l2497,a7),-(a7)
	move.l	(20+l2497,a7),-(a7)
	jsr	l1663
	move.l	(102+l2497,a7),-(a7)
	move.w	(100+l2497,a7),-(a7)
	move.l	(30+l2497,a7),-(a7)
	jsr	l1476
	move.l	(112+l2497,a7),-(a7)
	move.w	(110+l2497,a7),-(a7)
	move.l	(114+l2497,a7),-(a7)
	move.l	(112+l2497,a7),-(a7)
	jsr	l1695
	add.w	#48,a7
l2457
	move.l	#14675968,a0
	move.w	#0,(384,a0)
l2459
l2458
l2448
	move.l	(78+l2497,a7),-(a7)
	lea	(48+l2497,a7),a0
	move.l	a0,-(a7)
	jsr	l1936
	lea	(52+l2497,a7),a0
	lea	(34+l2497,a7),a1
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	move.w	(a0)+,(a1)+
	addq.w	#8,a7
	tst.b	(4+l2497,a7)
	beq	l2461
l2462
	move.l	#14675968,a0
	move.w	#136,(384,a0)
l2464
l2463
	move.l	(0+l2497,a7),a0
	add.w	#2402,a0
	add.w	(72+l2497,a7),a0
	tst.b	(a0)
	bne	l2466
l2465
	move.l	(78+l2497,a7),-(a7)
	move.l	(78+l2497,a7),-(a7)
	jsr	l1911
	move.l	(8+l2497,a7),a0
	add.w	#2404,a0
	move.w	(80+l2497,a7),d0
	ext.l	d0
	moveq	#18,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	lea	(34+l2497,a7),a1
	lea	(0,a0,d0.l),a2
	move.l	(a1)+,(a2)+
	move.l	(a1)+,(a2)+
	move.l	(a1)+,(a2)+
	move.l	(a1)+,(a2)+
	move.w	(a1)+,(a2)+
	move.l	(8+l2497,a7),a0
	add.w	#2402,a0
	add.w	(80+l2497,a7),a0
	move.b	#1,(a0)
	addq.w	#8,a7
	bra	l2467
l2466
	lea	(26+l2497,a7),a0
	move.l	a0,-(a7)
	move.l	(4+l2497,a7),a0
	add.w	#2404,a0
	move.w	(76+l2497,a7),d0
	ext.l	d0
	moveq	#18,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	pea	(0,a0,d0.l)
	jsr	l1961
	addq.w	#8,a7
	tst.b	d0
	bne	l2469
l2468
	lea	(26+l2497,a7),a0
	move.l	a0,-(a7)
	move.l	(4+l2497,a7),a0
	add.w	#2404,a0
	move.w	(76+l2497,a7),d0
	ext.l	d0
	moveq	#18,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	pea	(0,a0,d0.l)
	move.l	(86+l2497,a7),-(a7)
	move.l	(86+l2497,a7),-(a7)
	jsr	l1890
	move.l	(16+l2497,a7),a0
	add.w	#2404,a0
	move.w	(88+l2497,a7),d0
	ext.l	d0
	moveq	#18,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	lea	(42+l2497,a7),a1
	lea	(0,a0,d0.l),a2
	move.l	(a1)+,(a2)+
	move.l	(a1)+,(a2)+
	move.l	(a1)+,(a2)+
	move.l	(a1)+,(a2)+
	move.w	(a1)+,(a2)+
	add.w	#16,a7
l2469
l2467
l2470
	move.l	#14675968,a0
	move.w	#2056,(384,a0)
l2472
l2471
	move.l	(78+l2497,a7),-(a7)
	move.l	(78+l2497,a7),-(a7)
	move.l	(76+l2497,a7),-(a7)
	jsr	l2342
	add.w	#12,a7
l2473
	move.l	#14675968,a0
	move.w	#0,(384,a0)
l2475
l2474
	move.l	(6+l2497,a7),a0
	lea	(10+l2497,a7),a1
	lea	(6,a0),a2
	move.l	(a1)+,(a2)+
	move.l	(a1)+,(a2)+
	move.l	(6+l2497,a7),a0
	move.l	(18+l2497,a7),(2,a0)
	move.l	(6+l2497,a7),a0
	move.b	#1,(1,a0)
	move.l	(6+l2497,a7),a0
	move.b	#1,(a0)
	bra	l2418
l2461
l2476
	move.l	#14675968,a0
	move.w	#128,(384,a0)
l2478
l2477
	move.l	(78+l2497,a7),-(a7)
	move.l	(78+l2497,a7),-(a7)
	move.w	(80+l2497,a7),-(a7)
	move.l	(10+l2497,a7),-(a7)
	move.l	(82+l2497,a7),-(a7)
	jsr	l1780
	move.l	(96+l2497,a7),-(a7)
	move.l	(96+l2497,a7),-(a7)
	move.w	(98+l2497,a7),-(a7)
	move.l	(28+l2497,a7),-(a7)
	move.l	(100+l2497,a7),-(a7)
	jsr	l1753
	add.w	#36,a7
l2479
	move.l	#14675968,a0
	move.w	#136,(384,a0)
l2481
l2480
	move.l	(0+l2497,a7),a0
	add.w	#2402,a0
	add.w	(72+l2497,a7),a0
	tst.b	(a0)
	bne	l2483
l2482
	move.l	(78+l2497,a7),-(a7)
	move.l	(78+l2497,a7),-(a7)
	jsr	l1911
	move.l	(8+l2497,a7),a0
	add.w	#2404,a0
	move.w	(80+l2497,a7),d0
	ext.l	d0
	moveq	#18,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	lea	(34+l2497,a7),a1
	lea	(0,a0,d0.l),a2
	move.l	(a1)+,(a2)+
	move.l	(a1)+,(a2)+
	move.l	(a1)+,(a2)+
	move.l	(a1)+,(a2)+
	move.w	(a1)+,(a2)+
	move.l	(8+l2497,a7),a0
	add.w	#2402,a0
	add.w	(80+l2497,a7),a0
	move.b	#1,(a0)
	addq.w	#8,a7
	bra	l2484
l2483
	lea	(26+l2497,a7),a0
	move.l	a0,-(a7)
	move.l	(4+l2497,a7),a0
	add.w	#2404,a0
	move.w	(76+l2497,a7),d0
	ext.l	d0
	moveq	#18,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	pea	(0,a0,d0.l)
	jsr	l1961
	addq.w	#8,a7
	tst.b	d0
	bne	l2486
l2485
	lea	(26+l2497,a7),a0
	move.l	a0,-(a7)
	move.l	(4+l2497,a7),a0
	add.w	#2404,a0
	move.w	(76+l2497,a7),d0
	ext.l	d0
	moveq	#18,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	pea	(0,a0,d0.l)
	move.l	(86+l2497,a7),-(a7)
	move.l	(86+l2497,a7),-(a7)
	jsr	l1890
	move.l	(16+l2497,a7),a0
	add.w	#2404,a0
	move.w	(88+l2497,a7),d0
	ext.l	d0
	moveq	#18,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	lea	(42+l2497,a7),a1
	lea	(0,a0,d0.l),a2
	move.l	(a1)+,(a2)+
	move.l	(a1)+,(a2)+
	move.l	(a1)+,(a2)+
	move.l	(a1)+,(a2)+
	move.w	(a1)+,(a2)+
	add.w	#16,a7
l2486
l2484
l2487
	move.l	#14675968,a0
	move.w	#2056,(384,a0)
l2489
l2488
	move.l	(78+l2497,a7),-(a7)
	move.l	(78+l2497,a7),-(a7)
	move.l	(76+l2497,a7),-(a7)
	jsr	l2342
	move.b	d0,(35+l2497,a7)
	add.w	#12,a7
l2490
	move.l	#14675968,a0
	move.w	#0,(384,a0)
l2492
l2491
	tst.b	(23+l2497,a7)
	beq	l2494
	move.w	(72+l2497,a7),-(a7)
	move.l	(2+l2497,a7),-(a7)
	jsr	l1280
	addq.w	#6,a7
l2494
l2418
l2495	reg	a2/d2/d3
	movem.l	(a7)+,a2/d2/d3
l2497	equ	12
	add.w	#64,a7
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l2498
	subq.w	#8,a7
	movem.l	l2504,-(a7)
	move.l	#12574721,(0+l2506,a7)
	move.l	(0+l2506,a7),a0
	move.b	(a0),(4+l2506,a7)
	moveq	#0,d0
	move.b	(4+l2506,a7),d0
	and.w	#192,d0
	beq	l2501
l2503
	moveq	#0,d0
	bra	l2502
l2501
	moveq	#1,d0
l2502
l2499
l2504	reg
l2506	equ	0
	addq.w	#8,a7
	rts
; stacksize=8
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l2507
	subq.w	#8,a7
	movem.l	l2513,-(a7)
	move.l	#12574721,(0+l2515,a7)
	move.l	(0+l2515,a7),a0
	move.b	(a0),(4+l2515,a7)
	moveq	#0,d0
	move.b	(4+l2515,a7),d0
	and.w	#128,d0
	beq	l2510
l2512
	moveq	#0,d0
	bra	l2511
l2510
	moveq	#1,d0
l2511
l2508
l2513	reg
l2515	equ	0
	addq.w	#8,a7
	rts
; stacksize=8
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l2516
	subq.w	#8,a7
	movem.l	l2539,-(a7)
	move.b	#0,(0+l2541,a7)
	move.w	#512,d0
	and.w	(12+l2541,a7),d0
	bne	l2519
l2521
	moveq	#0,d0
	bra	l2520
l2519
	moveq	#1,d0
l2520
	move.w	d0,(2+l2541,a7)
	moveq	#2,d0
	and.w	(12+l2541,a7),d0
	bne	l2522
l2524
	moveq	#0,d0
	bra	l2523
l2522
	moveq	#1,d0
l2523
	move.w	d0,(4+l2541,a7)
	tst.w	(2+l2541,a7)
	beq	l2526
	or.b	#1,(0+l2541,a7)
l2526
	tst.w	(4+l2541,a7)
	beq	l2528
	or.b	#2,(0+l2541,a7)
l2528
	move.w	#256,d0
	and.w	(12+l2541,a7),d0
	bne	l2531
l2533
	moveq	#0,d0
	bra	l2532
l2531
	moveq	#1,d0
l2532
	move.w	(2+l2541,a7),d1
	eor.w	d1,d0
	beq	l2530
l2529
	or.b	#4,(0+l2541,a7)
l2530
	moveq	#1,d0
	and.w	(12+l2541,a7),d0
	bne	l2536
l2538
	moveq	#0,d0
	bra	l2537
l2536
	moveq	#1,d0
l2537
	move.w	(4+l2541,a7),d1
	eor.w	d1,d0
	beq	l2535
l2534
	or.b	#8,(0+l2541,a7)
l2535
	move.b	(0+l2541,a7),d0
l2517
l2539	reg
l2541	equ	0
	addq.w	#8,a7
	rts
; stacksize=8
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l2542
	sub.w	#12,a7
	movem.l	l2576,-(a7)
	move.b	#0,(10+l2578,a7)
	moveq	#10,d2
	moveq	#0,d0
	lea	(0+l2578,a7),a0
	inline
	move.l	a0,a1
	cmp.l	#16,d2
	blo	.l3
	move.l	a0,d1
	and.b	#1,d1
	beq	.l1
	move.b	d0,(a0)+
	subq.l	#1,d2
.l1
	move.b	d0,d1
	lsl.w	#8,d0
	move.b	d1,d0
	move.w	d0,d1
	swap	d0
	move.w	d1,d0
	moveq	#3,d1
	and.l	d2,d1
	sub.l	d1,d2
.l2
	move.l	d0,(a0)+
	subq.l	#4,d2
	bne	.l2
	move.w	d1,d2
.l3
	subq.w	#1,d2
	blo	.l5
.l4
	move.b	d0,(a0)+
	dbf	d2,.l4
.l5
	move.l	a1,d0
	einline
	move.l	d0,a1
	move.l	#14675968,a0
	move.w	(12,a0),-(a7)
	jsr	l2516
	move.b	d0,(12+l2578,a7)
	moveq	#0,d0
	move.b	(12+l2578,a7),d0
	and.w	#1,d0
	addq.w	#2,a7
	beq	l2546
l2545
	move.b	#1,(2+l2578,a7)
l2546
	moveq	#0,d0
	move.b	(10+l2578,a7),d0
	and.w	#2,d0
	beq	l2548
l2547
	move.b	#1,(3+l2578,a7)
l2548
	moveq	#0,d0
	move.b	(10+l2578,a7),d0
	and.w	#4,d0
	beq	l2550
l2549
	move.b	#1,(0+l2578,a7)
l2550
	moveq	#0,d0
	move.b	(10+l2578,a7),d0
	and.w	#8,d0
	beq	l2552
l2551
	move.b	#1,(1+l2578,a7)
l2552
	move.w	#0,(8+l2578,a7)
	moveq	#0,d0
	move.b	(10+l2578,a7),d0
	and.w	#1,d0
	beq	l2554
l2555
	move.l	(20+l2578,a7),a0
	moveq	#0,d0
	move.b	(a0),d0
	and.w	#1,d0
	bne	l2554
l2553
	move.w	#3,(8+l2578,a7)
l2554
	moveq	#0,d0
	move.b	(10+l2578,a7),d0
	and.w	#2,d0
	beq	l2557
l2558
	move.l	(20+l2578,a7),a0
	moveq	#0,d0
	move.b	(a0),d0
	and.w	#2,d0
	bne	l2557
l2556
	move.w	#4,(8+l2578,a7)
l2557
	moveq	#0,d0
	move.b	(10+l2578,a7),d0
	and.w	#4,d0
	beq	l2560
l2561
	move.l	(20+l2578,a7),a0
	moveq	#0,d0
	move.b	(a0),d0
	and.w	#4,d0
	bne	l2560
l2559
	move.w	#1,(8+l2578,a7)
l2560
	moveq	#0,d0
	move.b	(10+l2578,a7),d0
	and.w	#8,d0
	beq	l2563
l2564
	move.l	(20+l2578,a7),a0
	moveq	#0,d0
	move.b	(a0),d0
	and.w	#8,d0
	bne	l2563
l2562
	move.w	#2,(8+l2578,a7)
l2563
	move.l	(20+l2578,a7),a0
	move.b	(10+l2578,a7),(a0)
	jsr	l2507
	tst.w	d0
	beq	l2566
l2565
	moveq	#1,d0
	bra	l2567
l2566
	moveq	#0,d0
l2567
	move.b	d0,(11+l2578,a7)
	move.b	#0,(4+l2578,a7)
	tst.b	(11+l2578,a7)
	beq	l2570
l2568
	move.l	(24+l2578,a7),a0
	tst.b	(a0)
	bne	l2570
l2569
	moveq	#1,d0
	bra	l2571
l2570
	moveq	#0,d0
l2571
	move.b	d0,(5+l2578,a7)
	tst.b	(11+l2578,a7)
	bne	l2574
l2572
	move.l	(24+l2578,a7),a0
	tst.b	(a0)
	beq	l2574
l2573
	moveq	#1,d0
	bra	l2575
l2574
	moveq	#0,d0
l2575
	move.b	d0,(6+l2578,a7)
	move.l	(24+l2578,a7),a0
	move.b	(11+l2578,a7),(a0)
	move.l	(16+l2578,a7),a0
	lea	(0+l2578,a7),a1
	lea	(a0),a2
	moveq.l	#1,d0
l2579:
	move.b	(a1)+,(a2)+
	move.b	(a1)+,(a2)+
	move.b	(a1)+,(a2)+
	move.b	(a1)+,(a2)+
	dbra	d0,l2579
	move.b	(a1)+,(a2)+
	move.b	(a1)+,(a2)+
l2543
l2576	reg	a2/d2
	movem.l	(a7)+,a2/d2
l2578	equ	8
	add.w	#12,a7
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l2580
	sub.w	#16,a7
	movem.l	l2597,-(a7)
	move.l	#192,(10+l2599,a7)
	move.l	#65538,d1
	move.l	(10+l2599,a7),d0
	lsl.l	#1,d0
	move.l	_SysBase,a6
	jsr	-198(a6)
	move.l	d0,a0
	move.l	a0,(4+l2599,a7)
	bne	l2584
l2583
	moveq	#0,d0
	bra	l2581
l2584
	move.l	(20+l2599,a7),a0
	move.l	(4+l2599,a7),(a0)
	move.l	(10+l2599,a7),d0
	lsl.l	#1,d0
	move.l	(20+l2599,a7),a0
	move.l	d0,(4,a0)
	move.l	(4+l2599,a7),(0+l2599,a7)
	move.l	(0+l2599,a7),a0
	addq.l	#2,(0+l2599,a7)
	move.w	#142,(a0)
	move.l	(0+l2599,a7),a0
	addq.l	#2,(0+l2599,a7)
	move.w	#11393,(a0)
	move.l	(0+l2599,a7),a0
	addq.l	#2,(0+l2599,a7)
	move.w	#144,(a0)
	move.l	(0+l2599,a7),a0
	addq.l	#2,(0+l2599,a7)
	move.w	#62657,(a0)
	move.l	(0+l2599,a7),a0
	addq.l	#2,(0+l2599,a7)
	move.w	#146,(a0)
	move.l	(0+l2599,a7),a0
	addq.l	#2,(0+l2599,a7)
	move.w	#56,(a0)
	move.l	(0+l2599,a7),a0
	addq.l	#2,(0+l2599,a7)
	move.w	#148,(a0)
	move.l	(0+l2599,a7),a0
	addq.l	#2,(0+l2599,a7)
	move.w	#208,(a0)
	move.l	(0+l2599,a7),a0
	addq.l	#2,(0+l2599,a7)
	move.w	#256,(a0)
	move.l	(0+l2599,a7),a0
	addq.l	#2,(0+l2599,a7)
	move.w	#20992,(a0)
	move.l	(0+l2599,a7),a0
	addq.l	#2,(0+l2599,a7)
	move.w	#258,(a0)
	move.l	(0+l2599,a7),a0
	addq.l	#2,(0+l2599,a7)
	move.w	#0,(a0)
	move.l	(0+l2599,a7),a0
	addq.l	#2,(0+l2599,a7)
	move.w	#260,(a0)
	move.l	(0+l2599,a7),a0
	addq.l	#2,(0+l2599,a7)
	move.w	#0,(a0)
	move.l	(0+l2599,a7),a0
	addq.l	#2,(0+l2599,a7)
	move.w	#264,(a0)
	move.l	(0+l2599,a7),a0
	addq.l	#2,(0+l2599,a7)
	move.w	#160,(a0)
	move.l	(0+l2599,a7),a0
	addq.l	#2,(0+l2599,a7)
	move.w	#266,(a0)
	move.l	(0+l2599,a7),a0
	addq.l	#2,(0+l2599,a7)
	move.w	#160,(a0)
	move.w	#0,(8+l2599,a7)
	bra	l2586
l2585
	move.w	(8+l2599,a7),d0
	lsl.w	#2,d0
	add.w	#224,d0
	move.l	(0+l2599,a7),a0
	addq.l	#2,(0+l2599,a7)
	move.w	d0,(a0)
	move.l	(20+l2599,a7),a0
	move.w	(8+l2599,a7),d0
	ext.l	d0
	lsl.l	#2,d0
	move.l	(0+l2599,a7),(8,a0,d0.l)
	move.l	(0+l2599,a7),a0
	addq.l	#2,(0+l2599,a7)
	move.w	#0,(a0)
	move.w	(8+l2599,a7),d0
	lsl.w	#2,d0
	add.w	#226,d0
	move.l	(0+l2599,a7),a0
	addq.l	#2,(0+l2599,a7)
	move.w	d0,(a0)
	move.l	(20+l2599,a7),a0
	move.w	(8+l2599,a7),d0
	ext.l	d0
	lsl.l	#2,d0
	move.l	(0+l2599,a7),(28,a0,d0.l)
	move.l	(0+l2599,a7),a0
	addq.l	#2,(0+l2599,a7)
	move.w	#0,(a0)
l2588
	addq.w	#1,(8+l2599,a7)
l2586
	cmp.w	#5,(8+l2599,a7)
	blt	l2585
l2587
	move.w	#0,(8+l2599,a7)
	bra	l2590
l2589
	move.w	(8+l2599,a7),d0
	lsl.w	#2,d0
	add.w	#288,d0
	move.l	(0+l2599,a7),a0
	addq.l	#2,(0+l2599,a7)
	move.w	d0,(a0)
	move.l	(20+l2599,a7),a0
	move.w	(8+l2599,a7),d0
	ext.l	d0
	lsl.l	#2,d0
	move.l	(0+l2599,a7),(48,a0,d0.l)
	move.l	(0+l2599,a7),a0
	addq.l	#2,(0+l2599,a7)
	move.w	#0,(a0)
	move.w	(8+l2599,a7),d0
	lsl.w	#2,d0
	add.w	#290,d0
	move.l	(0+l2599,a7),a0
	addq.l	#2,(0+l2599,a7)
	move.w	d0,(a0)
	move.l	(20+l2599,a7),a0
	move.w	(8+l2599,a7),d0
	ext.l	d0
	lsl.l	#2,d0
	move.l	(0+l2599,a7),(80,a0,d0.l)
	move.l	(0+l2599,a7),a0
	addq.l	#2,(0+l2599,a7)
	move.w	#0,(a0)
l2592
	addq.w	#1,(8+l2599,a7)
l2590
	cmp.w	#8,(8+l2599,a7)
	blt	l2589
l2591
	move.w	#0,(8+l2599,a7)
	bra	l2594
l2593
	move.w	(8+l2599,a7),d0
	lsl.w	#1,d0
	add.w	#384,d0
	move.l	(0+l2599,a7),a0
	addq.l	#2,(0+l2599,a7)
	move.w	d0,(a0)
	move.w	(8+l2599,a7),d0
	ext.l	d0
	lsl.l	#1,d0
	lea	_g_amiga_palette,a0
	move.l	(0+l2599,a7),a1
	addq.l	#2,(0+l2599,a7)
	move.w	(0,a0,d0.l),(a1)
l2596
	addq.w	#1,(8+l2599,a7)
l2594
	cmp.w	#32,(8+l2599,a7)
	blt	l2593
l2595
	move.l	(0+l2599,a7),a0
	addq.l	#2,(0+l2599,a7)
	move.w	#62471,(a0)
	move.l	(0+l2599,a7),a0
	addq.l	#2,(0+l2599,a7)
	move.w	#65534,(a0)
	move.l	(0+l2599,a7),a0
	addq.l	#2,(0+l2599,a7)
	move.w	#256,(a0)
	move.l	(0+l2599,a7),a0
	addq.l	#2,(0+l2599,a7)
	move.w	#0,(a0)
	move.l	(0+l2599,a7),a0
	addq.l	#2,(0+l2599,a7)
	move.w	#384,(a0)
	move.l	(0+l2599,a7),a0
	addq.l	#2,(0+l2599,a7)
	move.w	#0,(a0)
	move.l	(0+l2599,a7),a0
	addq.l	#2,(0+l2599,a7)
	move.w	#65535,(a0)
	move.l	(0+l2599,a7),a0
	addq.l	#2,(0+l2599,a7)
	move.w	#65534,(a0)
	moveq	#1,d0
l2581
l2597	reg	a6
	movem.l	(a7)+,a6
l2599	equ	4
	add.w	#16,a7
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l2600
	subq.w	#8,a7
	movem.l	l2607,-(a7)
	move.w	#0,(0+l2609,a7)
	bra	l2604
l2603
	move.l	(16+l2609,a7),d0
	move.w	(0+l2609,a7),d1
	ext.l	d1
	moveq	#40,d2
	move.l	d1,d3
	move.l	d2,d4
	swap	d3
	swap	d4
	mulu.w	d2,d3
	mulu.w	d1,d4
	mulu.w	d2,d1
	add.w	d4,d3
	swap	d3
	clr.w	d3
	add.l	d3,d1
	add.l	d1,d0
	move.l	d0,(2+l2609,a7)
	moveq	#16,d1
	move.l	(2+l2609,a7),d0
	lsr.l	d1,d0
	move.l	(12+l2609,a7),a0
	move.w	(0+l2609,a7),d1
	ext.l	d1
	lsl.l	#2,d1
	move.l	(8,a0,d1.l),a0
	move.w	d0,(a0)
	move.w	(4+l2609,a7),d0
	and.w	#65535,d0
	move.l	(12+l2609,a7),a0
	move.w	(0+l2609,a7),d1
	ext.l	d1
	lsl.l	#2,d1
	move.l	(28,a0,d1.l),a0
	move.w	d0,(a0)
l2606
	addq.w	#1,(0+l2609,a7)
l2604
	cmp.w	#5,(0+l2609,a7)
	blt	l2603
l2605
l2601
l2607	reg	d2/d3/d4
	movem.l	(a7)+,d2/d3/d4
l2609	equ	12
	addq.w	#8,a7
	rts
; stacksize=20
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l2610
	subq.w	#8,a7
	movem.l	l2617,-(a7)
	move.w	#0,(0+l2619,a7)
	bra	l2614
l2613
	move.l	(16+l2619,a7),d0
	move.w	(0+l2619,a7),d1
	ext.l	d1
	moveq	#72,d2
	move.l	d1,d3
	move.l	d2,d4
	swap	d3
	swap	d4
	mulu.w	d2,d3
	mulu.w	d1,d4
	mulu.w	d2,d1
	add.w	d4,d3
	swap	d3
	clr.w	d3
	add.l	d3,d1
	add.l	d1,d0
	move.l	d0,(2+l2619,a7)
	moveq	#16,d1
	move.l	(2+l2619,a7),d0
	lsr.l	d1,d0
	move.l	(12+l2619,a7),a0
	move.w	(0+l2619,a7),d1
	ext.l	d1
	lsl.l	#2,d1
	move.l	(48,a0,d1.l),a0
	move.w	d0,(a0)
	move.w	(4+l2619,a7),d0
	and.w	#65535,d0
	move.l	(12+l2619,a7),a0
	move.w	(0+l2619,a7),d1
	ext.l	d1
	lsl.l	#2,d1
	move.l	(80,a0,d1.l),a0
	move.w	d0,(a0)
l2616
	addq.w	#1,(0+l2619,a7)
l2614
	cmp.w	#8,(0+l2619,a7)
	blt	l2613
l2615
l2611
l2617	reg	d2/d3/d4
	movem.l	(a7)+,d2/d3/d4
l2619	equ	12
	addq.w	#8,a7
	rts
; stacksize=20
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l2620
	movem.l	l2626,-(a7)
	move.l	_GfxBase,a0
	move.l	(34,a0),l1
	move.l	#0,a1
	move.l	_GfxBase,a6
	jsr	-222(a6)
	move.l	_GfxBase,a6
	jsr	-270(a6)
	move.l	_GfxBase,a6
	jsr	-270(a6)
	move.l	#14675968,a0
	move.w	#511,(150,a0)
	move.l	_GfxBase,a0
	moveq	#4,d0
	and.w	(206,a0),d0
	cmp.w	#4,d0
	beq	l2623
l2625
	moveq	#0,d0
	bra	l2624
l2623
	moveq	#1,d0
l2624
l2621
l2626	reg	a6
	movem.l	(a7)+,a6
l2628	equ	4
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l2629
	movem.l	l2632,-(a7)
	jsr	l290
	move.l	l1,a1
	move.l	_GfxBase,a6
	jsr	-222(a6)
	move.l	_GfxBase,a6
	jsr	-270(a6)
	move.l	_GfxBase,a6
	jsr	-270(a6)
	move.l	_GfxBase,a0
	move.l	(38,a0),d0
	move.l	#14675968,a0
	move.l	d0,(128,a0)
	move.l	#14675968,a0
	move.w	#0,(136,a0)
	move.l	_IntuitionBase,a6
	jsr	-390(a6)
	move.l	#0,l1
l2630
l2632	reg	a6
	movem.l	(a7)+,a6
l2634	equ	4
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l2635
	movem.l	l2651,-(a7)
	move.l	#9178,d2
	moveq	#0,d0
	move.l	(4+l2653,a7),a0
	inline
	move.l	a0,a1
	cmp.l	#16,d2
	blo	.l3
	move.l	a0,d1
	and.b	#1,d1
	beq	.l1
	move.b	d0,(a0)+
	subq.l	#1,d2
.l1
	move.b	d0,d1
	lsl.w	#8,d0
	move.b	d1,d0
	move.w	d0,d1
	swap	d0
	move.w	d1,d0
	moveq	#3,d1
	and.l	d2,d1
	sub.l	d1,d2
.l2
	move.l	d0,(a0)+
	subq.l	#4,d2
	bne	.l2
	move.w	d1,d2
.l3
	subq.w	#1,d2
	blo	.l5
.l4
	move.b	d0,(a0)+
	dbf	d2,.l4
.l5
	move.l	a1,d0
	einline
	move.l	d0,a1
	move.l	#65538,d1
	move.l	#40000,d0
	move.l	_SysBase,a6
	jsr	-198(a6)
	move.l	d0,a0
	move.l	(4+l2653,a7),a1
	move.l	a0,(a1)
	move.l	#65538,d1
	move.l	#40000,d0
	move.l	_SysBase,a6
	jsr	-198(a6)
	move.l	d0,a0
	move.l	(4+l2653,a7),a1
	move.l	a0,(4,a1)
	moveq	#2,d1
	move.l	#5280,d0
	move.l	_SysBase,a6
	jsr	-198(a6)
	move.l	d0,a0
	move.l	(4+l2653,a7),a1
	move.l	a0,(8,a1)
	moveq	#2,d1
	move.l	#5280,d0
	move.l	_SysBase,a6
	jsr	-198(a6)
	move.l	d0,a0
	move.l	(4+l2653,a7),a1
	move.l	a0,(12,a1)
	moveq	#2,d1
	move.l	#10560,d0
	move.l	_SysBase,a6
	jsr	-198(a6)
	move.l	d0,a0
	move.l	(4+l2653,a7),a1
	move.l	a0,(16,a1)
	moveq	#2,d1
	move.l	#10560,d0
	move.l	_SysBase,a6
	jsr	-198(a6)
	move.l	d0,a0
	move.l	(4+l2653,a7),a1
	move.l	a0,(20,a1)
	moveq	#2,d1
	move.l	#7470,d0
	move.l	_SysBase,a6
	jsr	-198(a6)
	move.l	d0,a0
	move.l	(4+l2653,a7),a1
	move.l	a0,(24,a1)
	moveq	#2,d1
	move.l	#7470,d0
	move.l	_SysBase,a6
	jsr	-198(a6)
	move.l	d0,a0
	move.l	(4+l2653,a7),a1
	move.l	a0,(28,a1)
	move.l	#65538,d1
	move.l	#576,d0
	move.l	_SysBase,a6
	jsr	-198(a6)
	move.l	d0,a0
	move.l	(4+l2653,a7),a1
	move.l	a0,(32,a1)
	move.l	#65538,d1
	move.l	#576,d0
	move.l	_SysBase,a6
	jsr	-198(a6)
	move.l	d0,a0
	move.l	(4+l2653,a7),a1
	add.w	#32,a1
	move.l	a0,(4,a1)
	move.l	(4+l2653,a7),a0
	tst.l	(a0)
	beq	l2638
l2648
	move.l	(4+l2653,a7),a0
	tst.l	(4,a0)
	beq	l2638
l2647
	move.l	(4+l2653,a7),a0
	tst.l	(8,a0)
	beq	l2638
l2646
	move.l	(4+l2653,a7),a0
	tst.l	(12,a0)
	beq	l2638
l2645
	move.l	(4+l2653,a7),a0
	tst.l	(16,a0)
	beq	l2638
l2644
	move.l	(4+l2653,a7),a0
	tst.l	(20,a0)
	beq	l2638
l2643
	move.l	(4+l2653,a7),a0
	tst.l	(24,a0)
	beq	l2638
l2642
	move.l	(4+l2653,a7),a0
	tst.l	(28,a0)
	beq	l2638
l2641
	move.l	(4+l2653,a7),a0
	tst.l	(32,a0)
	beq	l2638
l2640
	move.l	(4+l2653,a7),a0
	add.w	#32,a0
	tst.l	(4,a0)
	bne	l2639
l2638
	moveq	#0,d0
	bra	l2636
l2639
	move.l	#5280,d0
	move.l	(4+l2653,a7),a0
	move.l	(8,a0),a1
	lea	_g_amiga_tile_data,a0
	move.l	_SysBase,a6
	jsr	-624(a6)
	move.l	#5280,d0
	move.l	(4+l2653,a7),a0
	move.l	(12,a0),a1
	lea	_g_amiga_tile_mask,a0
	move.l	_SysBase,a6
	jsr	-624(a6)
	move.l	#10560,d0
	move.l	(4+l2653,a7),a0
	move.l	(16,a0),a1
	lea	_g_amiga_bob_data,a0
	move.l	_SysBase,a6
	jsr	-624(a6)
	move.l	#10560,d0
	move.l	(4+l2653,a7),a0
	move.l	(20,a0),a1
	lea	_g_amiga_bob_mask,a0
	move.l	_SysBase,a6
	jsr	-624(a6)
	move.l	#7470,d0
	move.l	(4+l2653,a7),a0
	move.l	(24,a0),a1
	lea	_g_amiga_title_overlay_data,a0
	move.l	_SysBase,a6
	jsr	-624(a6)
	move.l	#7470,d0
	move.l	(4+l2653,a7),a0
	move.l	(28,a0),a1
	lea	_g_amiga_title_overlay_mask,a0
	move.l	_SysBase,a6
	jsr	-624(a6)
	move.l	(4+l2653,a7),-(a7)
	jsr	l435
	move.l	(8+l2653,a7),a0
	move.l	(32,a0),-(a7)
	jsr	l477
	move.l	(12+l2653,a7),a0
	add.w	#32,a0
	move.l	(4,a0),-(a7)
	jsr	l477
	move.l	(16+l2653,a7),a0
	pea	(4264,a0)
	jsr	l842
	move.l	(20+l2653,a7),a0
	pea	(4834,a0)
	jsr	l2580
	add.w	#20,a7
	tst.w	d0
	bne	l2650
l2649
	moveq	#0,d0
	bra	l2636
l2650
	move.w	#0,-(a7)
	move.l	(6+l2653,a7),a0
	pea	(4946,a0)
	jsr	l1240
	move.w	#1,-(a7)
	move.l	(12+l2653,a7),a0
	pea	(4946,a0)
	jsr	l1240
	moveq	#1,d0
	add.w	#12,a7
l2636
l2651	reg	a6/d2
	movem.l	(a7)+,a6/d2
l2653	equ	8
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l2654
	movem.l	l2679,-(a7)
	move.l	(4+l2681,a7),a0
	pea	(4264,a0)
	jsr	l850
	move.l	(8+l2681,a7),a0
	addq.w	#4,a7
	tst.l	(4834,a0)
	beq	l2658
	move.l	(4+l2681,a7),a0
	add.w	#4834,a0
	move.l	(4,a0),d0
	move.l	(4+l2681,a7),a0
	move.l	(4834,a0),a1
	move.l	_SysBase,a6
	jsr	-210(a6)
	move.l	(4+l2681,a7),a0
	move.l	#0,(4834,a0)
l2658
	move.l	(4+l2681,a7),a0
	tst.l	(20,a0)
	beq	l2660
	move.l	#10560,d0
	move.l	(4+l2681,a7),a0
	move.l	(20,a0),a1
	move.l	_SysBase,a6
	jsr	-210(a6)
	move.l	(4+l2681,a7),a0
	move.l	#0,(20,a0)
l2660
	move.l	(4+l2681,a7),a0
	tst.l	(28,a0)
	beq	l2662
	move.l	#7470,d0
	move.l	(4+l2681,a7),a0
	move.l	(28,a0),a1
	move.l	_SysBase,a6
	jsr	-210(a6)
	move.l	(4+l2681,a7),a0
	move.l	#0,(28,a0)
l2662
	move.l	(4+l2681,a7),a0
	tst.l	(24,a0)
	beq	l2664
	move.l	#7470,d0
	move.l	(4+l2681,a7),a0
	move.l	(24,a0),a1
	move.l	_SysBase,a6
	jsr	-210(a6)
	move.l	(4+l2681,a7),a0
	move.l	#0,(24,a0)
l2664
	move.l	(4+l2681,a7),a0
	tst.l	(12,a0)
	beq	l2666
	move.l	#5280,d0
	move.l	(4+l2681,a7),a0
	move.l	(12,a0),a1
	move.l	_SysBase,a6
	jsr	-210(a6)
	move.l	(4+l2681,a7),a0
	move.l	#0,(12,a0)
l2666
	move.l	(4+l2681,a7),a0
	tst.l	(32,a0)
	beq	l2668
	move.l	#576,d0
	move.l	(4+l2681,a7),a0
	move.l	(32,a0),a1
	move.l	_SysBase,a6
	jsr	-210(a6)
	move.l	(4+l2681,a7),a0
	move.l	#0,(32,a0)
l2668
	move.l	(4+l2681,a7),a0
	add.w	#32,a0
	tst.l	(4,a0)
	beq	l2670
	move.l	#576,d0
	move.l	(4+l2681,a7),a0
	add.w	#32,a0
	move.l	(4,a0),a1
	move.l	_SysBase,a6
	jsr	-210(a6)
	move.l	(4+l2681,a7),a0
	add.w	#32,a0
	move.l	#0,(4,a0)
l2670
	move.l	(4+l2681,a7),a0
	tst.l	(16,a0)
	beq	l2672
	move.l	#10560,d0
	move.l	(4+l2681,a7),a0
	move.l	(16,a0),a1
	move.l	_SysBase,a6
	jsr	-210(a6)
	move.l	(4+l2681,a7),a0
	move.l	#0,(16,a0)
l2672
	move.l	(4+l2681,a7),a0
	tst.l	(8,a0)
	beq	l2674
	move.l	#5280,d0
	move.l	(4+l2681,a7),a0
	move.l	(8,a0),a1
	move.l	_SysBase,a6
	jsr	-210(a6)
	move.l	(4+l2681,a7),a0
	move.l	#0,(8,a0)
l2674
	move.l	(4+l2681,a7),a0
	tst.l	(a0)
	beq	l2676
	move.l	#40000,d0
	move.l	(4+l2681,a7),a0
	move.l	(a0),a1
	move.l	_SysBase,a6
	jsr	-210(a6)
	move.l	(4+l2681,a7),a0
	move.l	#0,(a0)
l2676
	move.l	(4+l2681,a7),a0
	tst.l	(4,a0)
	beq	l2678
	move.l	#40000,d0
	move.l	(4+l2681,a7),a0
	move.l	(4,a0),a1
	move.l	_SysBase,a6
	jsr	-210(a6)
	move.l	(4+l2681,a7),a0
	move.l	#0,(4,a0)
l2678
l2655
l2679	reg	a6
	movem.l	(a7)+,a6
l2681	equ	4
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l2682
	movem.l	l2689,-(a7)
	jsr	l290
	move.l	(4+l2691,a7),a0
	pea	(4264,a0)
	jsr	l850
	move.l	(8+l2691,a7),a0
	addq.w	#4,a7
	tst.w	(9176,a0)
	beq	l2686
	move.l	_GfxBase,a6
	jsr	-462(a6)
	move.l	(4+l2691,a7),a0
	move.w	#0,(9176,a0)
l2686
	jsr	l2629
	move.l	(4+l2691,a7),a0
	tst.w	(9174,a0)
	beq	l2688
	move.l	_IntuitionBase,a6
	jsr	-210(a6)
	move.l	(4+l2691,a7),a0
	move.w	#0,(9174,a0)
l2688
	move.l	(4+l2691,a7),-(a7)
	jsr	l2654
	addq.w	#4,a7
l2683
l2689	reg	a6
	movem.l	(a7)+,a6
l2691	equ	4
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l2692
	movem.l	l2695,-(a7)
	move.l	#2654435769,d0
	add.l	(8+l2697,a7),d0
	move.l	(4+l2697,a7),d1
	lsl.l	#6,d1
	add.l	d1,d0
	move.l	(4+l2697,a7),d1
	lsr.l	#2,d1
	add.l	d1,d0
	eor.l	d0,(4+l2697,a7)
	moveq	#16,d1
	move.l	(4+l2697,a7),d0
	lsr.l	d1,d0
	eor.l	d0,(4+l2697,a7)
	move.l	(4+l2697,a7),d0
	move.l	#2146121005,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	move.l	d0,(4+l2697,a7)
	moveq	#15,d1
	move.l	(4+l2697,a7),d0
	lsr.l	d1,d0
	eor.l	d0,(4+l2697,a7)
	move.l	(4+l2697,a7),d0
	move.l	#2221713035,d1
	move.l	d0,d2
	move.l	d1,d3
	swap	d2
	swap	d3
	mulu.w	d1,d2
	mulu.w	d0,d3
	mulu.w	d1,d0
	add.w	d3,d2
	swap	d2
	clr.w	d2
	add.l	d2,d0
	move.l	d0,(4+l2697,a7)
	moveq	#16,d1
	move.l	(4+l2697,a7),d0
	lsr.l	d1,d0
	eor.l	d0,(4+l2697,a7)
	move.l	(4+l2697,a7),d0
l2693
l2695	reg	d2/d3
	movem.l	(a7)+,d2/d3
l2697	equ	8
	rts
; stacksize=8
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	cnop	0,4
l2698
	sub.w	#36,a7
	movem.l	l2703,-(a7)
	move.l	#2772211285,(12+l2705,a7)
	move.l	#12574721,(16+l2705,a7)
	move.l	#12575745,(20+l2705,a7)
	move.l	#12576001,(24+l2705,a7)
	move.l	#12572160,(28+l2705,a7)
	move.l	#12572416,(32+l2705,a7)
	lea	(0+l2705,a7),a0
	move.l	a0,d1
	move.l	_DOSBase,a6
	jsr	-192(a6)
	move.l	(0+l2705,a7),-(a7)
	move.l	(16+l2705,a7),-(a7)
	jsr	l2692
	move.l	d0,(20+l2705,a7)
	move.l	(12+l2705,a7),-(a7)
	move.l	(24+l2705,a7),-(a7)
	jsr	l2692
	move.l	d0,(28+l2705,a7)
	move.l	(24+l2705,a7),-(a7)
	move.l	(32+l2705,a7),-(a7)
	jsr	l2692
	move.l	d0,(36+l2705,a7)
	move.l	#14675968,a0
	moveq	#0,d0
	move.w	(4,a0),d0
	moveq	#16,d1
	lsl.l	d1,d0
	move.l	#14675968,a0
	moveq	#0,d1
	move.w	(6,a0),d1
	or.l	d1,d0
	move.l	d0,-(a7)
	move.l	(40+l2705,a7),-(a7)
	jsr	l2692
	move.l	d0,(44+l2705,a7)
	move.l	#14675968,a0
	moveq	#0,d0
	move.w	(12,a0),d0
	moveq	#16,d1
	lsl.l	d1,d0
	move.l	#14675968,a0
	moveq	#0,d1
	move.w	(18,a0),d1
	or.l	d1,d0
	move.l	d0,-(a7)
	move.l	(48+l2705,a7),-(a7)
	jsr	l2692
	move.l	d0,(52+l2705,a7)
	move.l	#14675968,a0
	moveq	#0,d0
	move.w	(20,a0),d0
	moveq	#16,d1
	lsl.l	d1,d0
	move.l	(56+l2705,a7),a0
	moveq	#0,d1
	move.b	(a0),d1
	or.l	d1,d0
	move.l	d0,-(a7)
	move.l	(56+l2705,a7),-(a7)
	jsr	l2692
	move.l	d0,(60+l2705,a7)
	move.l	(72+l2705,a7),a0
	moveq	#0,d0
	move.b	(a0),d0
	moveq	#24,d1
	lsl.l	d1,d0
	move.l	(68+l2705,a7),a0
	moveq	#0,d1
	move.b	(a0),d1
	moveq	#16,d2
	lsl.l	d2,d1
	or.l	d1,d0
	move.l	(80+l2705,a7),a0
	moveq	#0,d1
	move.b	(a0),d1
	lsl.l	#8,d1
	or.l	d1,d0
	move.l	(76+l2705,a7),a0
	moveq	#0,d1
	move.b	(a0),d1
	or.l	d1,d0
	move.l	d0,-(a7)
	move.l	(64+l2705,a7),-(a7)
	jsr	l2692
	move.l	d0,(68+l2705,a7)
	add.w	#56,a7
	bne	l2702
l2701
	move.l	#305419896,(12+l2705,a7)
l2702
	move.l	(12+l2705,a7),d0
l2699
l2703	reg	a6/d2
	movem.l	(a7)+,a6/d2
l2705	equ	8
	add.w	#36,a7
	rts
	opt o+,ol+,op+,oc+,ot+,oj+,ob+,om+
	public	_main
	cnop	0,4
_main
	sub.w	#64,a7
	movem.l	l2770,-(a7)
	move.l	#l3,(0+l2772,a7)
	move.l	#l4,(4+l2772,a7)
	move.l	(4+l2772,a7),(8+l2772,a7)
	move.b	#0,(12+l2772,a7)
	move.b	#0,(13+l2772,a7)
	move.w	#0,(14+l2772,a7)
	move.w	#1,(16+l2772,a7)
	move.w	#0,(18+l2772,a7)
	move.w	#0,(20+l2772,a7)
	move.w	#0,(22+l2772,a7)
	move.w	#0,(24+l2772,a7)
	move.l	#9178,d2
	moveq	#0,d0
	move.l	(0+l2772,a7),a0
	inline
	move.l	a0,a1
	cmp.l	#16,d2
	blo	.l3
	move.l	a0,d1
	and.b	#1,d1
	beq	.l1
	move.b	d0,(a0)+
	subq.l	#1,d2
.l1
	move.b	d0,d1
	lsl.w	#8,d0
	move.b	d1,d0
	move.w	d0,d1
	swap	d0
	move.w	d1,d0
	moveq	#3,d1
	and.l	d2,d1
	sub.l	d1,d2
.l2
	move.l	d0,(a0)+
	subq.l	#4,d2
	bne	.l2
	move.w	d1,d2
.l3
	subq.w	#1,d2
	blo	.l5
.l4
	move.b	d0,(a0)+
	dbf	d2,.l4
.l5
	move.l	a1,d0
	einline
	move.l	d0,a1
	moveq	#0,d0
	lea	l2708,a1
	move.l	_SysBase,a6
	jsr	-552(a6)
	move.l	d0,a0
	move.l	a0,_GfxBase
	bne	l2710
l2709
	moveq	#20,d0
	bra	l2706
l2710
	moveq	#0,d0
	lea	l2711,a1
	move.l	_SysBase,a6
	jsr	-552(a6)
	move.l	d0,a0
	move.l	a0,_IntuitionBase
	bne	l2713
l2712
	move.l	_GfxBase,a1
	move.l	_SysBase,a6
	jsr	-414(a6)
	moveq	#20,d0
	bra	l2706
l2713
	moveq	#20,d0
	move.l	#0,a1
	move.l	_SysBase,a6
	move.l	d0,(30+l2772,a7)
	jsr	-294(a6)
	move.l	d0,a0
	move.l	(30+l2772,a7),d0
	move.l	a0,a1
	move.l	_SysBase,a6
	jsr	-300(a6)
	move.b	d0,d1
	move.l	_IntuitionBase,a6
	jsr	-78(a6)
	tst.l	d0
	beq	l2715
l2714
	moveq	#1,d0
	bra	l2716
l2715
	moveq	#0,d0
l2716
	move.l	(0+l2772,a7),a0
	move.w	d0,(9174,a0)
	jsr	l2620
	tst.w	d0
	bne	l2718
l2717
	jsr	l2629
	move.l	(0+l2772,a7),a0
	tst.w	(9174,a0)
	beq	l2720
	move.l	_IntuitionBase,a6
	jsr	-210(a6)
l2720
	move.l	_IntuitionBase,a1
	move.l	_SysBase,a6
	jsr	-414(a6)
	move.l	_GfxBase,a1
	move.l	_SysBase,a6
	jsr	-414(a6)
	moveq	#20,d0
	bra	l2706
l2718
	move.l	_GfxBase,a6
	jsr	-456(a6)
	move.l	(0+l2772,a7),a0
	move.w	#1,(9176,a0)
	move.l	(0+l2772,a7),-(a7)
	jsr	l2635
	addq.w	#4,a7
	tst.w	d0
	bne	l2722
l2721
	move.l	(0+l2772,a7),-(a7)
	jsr	l2682
	move.l	_IntuitionBase,a1
	move.l	_SysBase,a6
	jsr	-414(a6)
	move.l	_GfxBase,a1
	move.l	_SysBase,a6
	jsr	-414(a6)
	moveq	#20,d0
	addq.w	#4,a7
	bra	l2706
l2722
	move.l	(0+l2772,a7),a0
	move.l	(32,a0),-(a7)
	move.l	(4+l2772,a7),a0
	pea	(4834,a0)
	jsr	l2610
	move.l	(8+l2772,a7),a0
	move.l	(a0),-(a7)
	jsr	l2409
	jsr	l290
	move.l	(12+l2772,a7),a0
	move.l	(a0),-(a7)
	move.l	(16+l2772,a7),a0
	pea	(4834,a0)
	jsr	l2600
	move.l	(20+l2772,a7),a0
	move.l	(32,a0),-(a7)
	move.l	(24+l2772,a7),a0
	pea	(4834,a0)
	jsr	l2610
	move.l	(28+l2772,a7),a0
	move.l	(4834,a0),d0
	move.l	#14675968,a0
	move.l	d0,(128,a0)
	move.l	#14675968,a0
	move.w	#0,(136,a0)
	move.l	#14675968,a0
	move.w	#33728,(150,a0)
	jsr	l2698
	move.l	d0,-(a7)
	move.l	(36+l2772,a7),-(a7)
	jsr	_game_init
	move.l	(40+l2772,a7),-(a7)
	jsr	_game_consume_events
	move.l	d0,(74+l2772,a7)
	move.l	(44+l2772,a7),a0
	add.w	#40,a7
	tst.w	(3226,a0)
	bne	l2724
l2725
	move.l	(4+l2772,a7),a0
	tst.b	(3221,a0)
	beq	l2724
l2723
	moveq	#64,d0
	and.l	(34+l2772,a7),d0
	beq	l2727
l2726
	move.l	(0+l2772,a7),a0
	pea	(4264,a0)
	jsr	l716
	move.w	#1,(26+l2772,a7)
	addq.w	#4,a7
l2727
	and.l	#4294967231,(34+l2772,a7)
l2724
	move.l	(34+l2772,a7),-(a7)
	move.l	(4+l2772,a7),a0
	pea	(4264,a0)
	jsr	l730
	move.l	(8+l2772,a7),a0
	pea	(4264,a0)
	jsr	l820
	move.l	(16+l2772,a7),-(a7)
	move.l	(16+l2772,a7),a0
	pea	(4946,a0)
	jsr	l1671
	move.l	(24+l2772,a7),-(a7)
	jsr	_game_clear_dirty_static
	move.l	(32+l2772,a7),-(a7)
	move.l	(28+l2772,a7),a0
	move.l	(a0),-(a7)
	move.w	#0,-(a7)
	move.l	(34+l2772,a7),-(a7)
	jsr	l2417
	move.l	(46+l2772,a7),-(a7)
	move.l	(42+l2772,a7),a0
	move.l	(4,a0),-(a7)
	move.w	#1,-(a7)
	move.l	(48+l2772,a7),-(a7)
	jsr	l2417
	move.l	(60+l2772,a7),a0
	move.w	(3226,a0),(72+l2772,a7)
	jsr	l290
	move.l	(52+l2772,a7),a0
	move.l	(a0),-(a7)
	move.l	(56+l2772,a7),a0
	pea	(4834,a0)
	jsr	l2600
	move.l	(60+l2772,a7),a0
	move.l	(32,a0),-(a7)
	move.l	(64+l2772,a7),a0
	pea	(4834,a0)
	jsr	l2610
	jsr	l308
	add.w	#68,a7
l2728
	lea	(26+l2772,a7),a0
	move.l	a0,-(a7)
	jsr	l320
	lea	(17+l2772,a7),a0
	move.l	a0,-(a7)
	lea	(20+l2772,a7),a0
	move.l	a0,-(a7)
	lea	(56+l2772,a7),a0
	move.l	a0,-(a7)
	jsr	l2542
	lea	(60+l2772,a7),a0
	lea	(50+l2772,a7),a1
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	move.w	(a0)+,(a1)+
	jsr	l2498
	add.w	#16,a7
	tst.w	d0
	beq	l2732
	addq.w	#1,(14+l2772,a7)
	cmp.w	#50,(14+l2772,a7)
	blt	l2734
l2733
	bra	l2730
l2734
	bra	l2735
l2732
	move.w	#0,(14+l2772,a7)
l2735
	tst.b	(40+l2772,a7)
	beq	l2737
l2738
	move.l	(4+l2772,a7),a0
	cmp.w	#1,(3226,a0)
	beq	l2737
l2736
	move.l	(0+l2772,a7),a0
	pea	(4264,a0)
	jsr	l723
	addq.w	#4,a7
l2737
l2739
	move.l	#14675968,a0
	move.w	#1024,(384,a0)
l2741
l2740
	lea	(34+l2772,a7),a0
	move.l	a0,-(a7)
	move.l	(8+l2772,a7),-(a7)
	jsr	_game_step
	addq.w	#8,a7
l2742
	move.l	#14675968,a0
	move.w	#0,(384,a0)
l2744
l2743
	lea	(26+l2772,a7),a0
	move.l	a0,-(a7)
	jsr	l326
	addq.w	#4,a7
l2745
	move.l	#14675968,a0
	move.w	#2112,(384,a0)
l2747
l2746
	move.l	(4+l2772,a7),-(a7)
	jsr	_game_consume_events
	move.l	d0,(58+l2772,a7)
	move.l	(58+l2772,a7),(62+l2772,a7)
	move.l	(8+l2772,a7),a0
	addq.w	#4,a7
	tst.w	(3226,a0)
	bne	l2749
l2750
	move.l	(4+l2772,a7),a0
	tst.b	(3221,a0)
	beq	l2749
l2748
	moveq	#64,d0
	and.l	(58+l2772,a7),d0
	beq	l2752
l2753
	tst.w	(22+l2772,a7)
	bne	l2752
l2751
	move.l	(0+l2772,a7),a0
	pea	(4264,a0)
	jsr	l716
	move.w	#1,(26+l2772,a7)
	addq.w	#4,a7
l2752
	and.l	#4294967231,(58+l2772,a7)
	bra	l2754
l2749
	move.w	#0,(22+l2772,a7)
l2754
	move.l	(58+l2772,a7),-(a7)
	move.l	(4+l2772,a7),a0
	pea	(4264,a0)
	jsr	l730
	move.l	(8+l2772,a7),a0
	pea	(4264,a0)
	jsr	l820
	add.w	#12,a7
l2755
	move.l	#14675968,a0
	move.w	#0,(384,a0)
l2757
l2756
	lea	(26+l2772,a7),a0
	move.l	a0,-(a7)
	jsr	l326
	move.l	(8+l2772,a7),-(a7)
	move.l	(8+l2772,a7),a0
	pea	(4946,a0)
	jsr	l1671
	move.l	(16+l2772,a7),-(a7)
	jsr	_game_clear_dirty_static
	addq.w	#1,(34+l2772,a7)
	add.w	#16,a7
	cmp.w	#1,(18+l2772,a7)
	bge	l2758
l2760
	move.l	(8+l2772,a7),a0
	move.w	(3226,a0),d0
	cmp.w	(20+l2772,a7),d0
	beq	l2759
l2758
	move.w	#0,(18+l2772,a7)
l2761
	move.l	#14675968,a0
	move.w	#64,(384,a0)
l2763
l2762
	move.l	(8+l2772,a7),-(a7)
	move.w	(20+l2772,a7),d0
	ext.l	d0
	lsl.l	#2,d0
	move.l	(4+l2772,a7),a0
	move.l	(0,a0,d0.l),-(a7)
	move.w	(24+l2772,a7),-(a7)
	move.l	(10+l2772,a7),-(a7)
	jsr	l2417
	move.l	(22+l2772,a7),a0
	move.w	(3226,a0),(34+l2772,a7)
	add.w	#14,a7
l2764
	move.l	#14675968,a0
	move.w	#4,(384,a0)
l2766
l2765
	jsr	l290
	move.w	(16+l2772,a7),d0
	ext.l	d0
	lsl.l	#2,d0
	move.l	(0+l2772,a7),a0
	move.l	(0,a0,d0.l),-(a7)
	move.l	(4+l2772,a7),a0
	pea	(4834,a0)
	jsr	l2600
	move.l	(8+l2772,a7),a0
	move.w	(24+l2772,a7),d0
	ext.l	d0
	lsl.l	#2,d0
	move.l	(32,a0,d0.l),-(a7)
	move.l	(12+l2772,a7),a0
	pea	(4834,a0)
	jsr	l2610
	eor.w	#1,(32+l2772,a7)
	lea	(42+l2772,a7),a0
	move.l	a0,-(a7)
	jsr	l326
	add.w	#20,a7
l2759
l2767
	move.l	#14675968,a0
	move.w	#0,(384,a0)
l2769
l2768
	lea	(26+l2772,a7),a0
	move.l	a0,-(a7)
	jsr	l334
	moveq	#1,d0
	add.l	l2,d0
	move.l	#2147483647,d1
	and.l	d0,d1
	move.l	d1,l2
	addq.w	#4,a7
	bra	l2728
l2730
	move.l	(0+l2772,a7),-(a7)
	jsr	l2682
	move.l	_IntuitionBase,a1
	move.l	_SysBase,a6
	jsr	-414(a6)
	move.l	_GfxBase,a1
	move.l	_SysBase,a6
	jsr	-414(a6)
	move.w	(28+l2772,a7),d0
	addq.w	#4,a7
l2706
l2770	reg	a6/d2
	movem.l	(a7)+,a6/d2
l2772	equ	8
	add.w	#64,a7
	rts
	cnop	0,4
l2708
	dc.b	103
	dc.b	114
	dc.b	97
	dc.b	112
	dc.b	104
	dc.b	105
	dc.b	99
	dc.b	115
	dc.b	46
	dc.b	108
	dc.b	105
	dc.b	98
	dc.b	114
	dc.b	97
	dc.b	114
	dc.b	121
	dc.b	0
	cnop	0,4
l2711
	dc.b	105
	dc.b	110
	dc.b	116
	dc.b	117
	dc.b	105
	dc.b	116
	dc.b	105
	dc.b	111
	dc.b	110
	dc.b	46
	dc.b	108
	dc.b	105
	dc.b	98
	dc.b	114
	dc.b	97
	dc.b	114
	dc.b	121
	dc.b	0
	public	_GfxBase
	section	"DATA",data
	cnop	0,4
_GfxBase
	dc.l	0
	public	_IntuitionBase
	cnop	0,4
_IntuitionBase
	dc.l	0
	public	__divu
	public	_game_init
	public	_game_step
	public	_game_dirty_static_rows
	public	_game_clear_dirty_static
	public	_game_consume_events
	public	_g_amiga_palette
	public	_g_amiga_tile_data
	public	_g_amiga_tile_mask
	public	_g_amiga_bob_data
	public	_g_amiga_bob_mask
	public	_g_amiga_title_overlay_data
	public	_g_amiga_title_overlay_mask
	public	_SysBase
	public	_DOSBase
	section	"CODE",code
	cnop	0,4
l5
	dc.w	0
	dc.w	1
	dc.w	2
	cnop	0,4
l6
	dc.w	0
	dc.w	3
	dc.w	3
	dc.w	3
	dc.w	3
	cnop	0,4
l7
	dc.w	0
	dc.w	23
	dc.w	24
	dc.w	25
	dc.w	26
	dc.w	27
	cnop	0,4
l8
	dc.l	l9
	dc.l	50000
	dc.l	l10
	dc.l	35000
	dc.l	l11
	dc.l	24000
	dc.l	l12
	dc.l	16000
	dc.l	l13
	dc.l	9000
	cnop	0,4
l9
	dc.b	65
	dc.b	67
	dc.b	69
	dc.b	0
	cnop	0,4
l10
	dc.b	73
	dc.b	67
	dc.b	69
	dc.b	0
	cnop	0,4
l11
	dc.b	80
	dc.b	65
	dc.b	78
	dc.b	0
	cnop	0,4
l12
	dc.b	70
	dc.b	79
	dc.b	88
	dc.b	0
	cnop	0,4
l13
	dc.b	74
	dc.b	69
	dc.b	84
	dc.b	0
	cnop	0,4
l14
	dc.b	7
	dc.b	5
	dc.b	5
	dc.b	5
	dc.b	7
	dc.b	2
	dc.b	6
	dc.b	2
	dc.b	2
	dc.b	7
	dc.b	7
	dc.b	1
	dc.b	7
	dc.b	4
	dc.b	7
	dc.b	7
	dc.b	1
	dc.b	7
	dc.b	1
	dc.b	7
	dc.b	5
	dc.b	5
	dc.b	7
	dc.b	1
	dc.b	1
	dc.b	7
	dc.b	4
	dc.b	7
	dc.b	1
	dc.b	7
	dc.b	7
	dc.b	4
	dc.b	7
	dc.b	5
	dc.b	7
	dc.b	7
	dc.b	1
	dc.b	1
	dc.b	1
	dc.b	1
	dc.b	7
	dc.b	5
	dc.b	7
	dc.b	5
	dc.b	7
	dc.b	7
	dc.b	5
	dc.b	7
	dc.b	1
	dc.b	7
	section	"DATA",data
	cnop	0,4
l1
	dc.l	0
	cnop	0,4
l2
	dc.l	0
	section	"BSS",bss
	cnop	0,4
l3
	ds.b	9178
	cnop	0,4
l4
	ds.b	3240
