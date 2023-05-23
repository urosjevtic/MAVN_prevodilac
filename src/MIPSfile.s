.globl main

.data
m1: 	.word 6
m2: 	.word 5

.text
main:
	la		 $t2, m1
	lw		 $t1, 2($t2)
	la		 $t0, m2
	lw		 $t0, 0($t0)
	sub		 $t2, $t1, $t0
	or		 $t0, $t1, $t2
	sw		 $t1, 17($t0)
