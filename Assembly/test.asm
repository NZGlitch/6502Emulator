; Where to load the program
* = $1000

start
	ldx $00			; set x to 0
loop
	jsr pushchar	; Copy the first char
;	beq end			; NULL value reached, exit
;	inx				; increment index
	JMP loop

; Job done, loop forever (no way to exit yet)
end
	JMP end

;currently copies text in data to zeropage
pushchar			; Expects to find the char index in register X
	lda	$8000,x		;load character into Reg A
	sta $00,x		; put the character into the zp
	rts;			; return from subroutine


; Data scetion
* = $8000
data
.text "Hello World"
.byte $0				; Null terminated string