.org $DA8000

.equ %controller, $A00000
.equ %btn_A, 0x10
.equ %btn_B, 0x20

:main
	ld hl,%controller
	ld [hl], %btn_A | %btn_B
	ld de,:end-:main
	ld b,7 ; whatever bla 7 nein
::loop
	push af
	mul [hl]
	dec b
	jnz ::loop
	ret
:end

