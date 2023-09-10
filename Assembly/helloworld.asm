; Where to load the program
* = $1000

start
	ldx $00			; set x to 0
loop
	jsr pushchar	; Copy the first char
;	beq end			; NULL value reached, exit
	inx				; increment index
	JMP loop

; Job done, loop forever (no way to exit yet)
end
	JMP end

;currently copies text in data to zeropage
pushchar			; Expects to find the char index in register X
	lda	data,x		;load character into Reg A
	sta $00,x		; put the character into the zp
	rts;			; return from subroutine

* = $1100
; Data scetion
data
.byte 72, 101, 108, 108, 111, 32, 119, 111, 114, 108, 100