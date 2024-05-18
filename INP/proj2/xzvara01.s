; Vernamova sifra na architekture DLX
; Adam Zvara xzvara01

        .data 0x04          ; zacatek data segmentu v pameti
login:  .asciiz "xzvara01"  ; <-- nahradte vasim loginem
cipher: .space 9 	    ; sem ukladejte sifrovane znaky (za posledni nezapomente dat 0)

        .align 2            ; dale zarovnavej na ctverice (2^2) bajtu
laddr:  .word login         ; 4B adresa vstupniho textu (pro vypis)
caddr:  .word cipher        ; 4B adresa sifrovaneho retezce (pro vypis)

        .text 0x40          ; adresa zacatku programu v pameti
        .global main        ; 

main:   ; sem doplnte reseni Vernamovy sifry dle specifikace v zadani
	xor r17, r17, r17	;r17 = index pismena na ktorom aktualne som

prve_pismeno:
	xor r27, r27, r27
	addi r27, r27, 26	;poradie prveho sifrovacieho kluca v abecede ('z')

	xor r28, r28, r28
	addi r28, r28, 22	;poradie druheho sifrovacieho kluca v abecede ('v')

	lb r5, login(r17)
	
	;kontrola ci je nacitany znak pismeno
	slti r20, r5, 97
	bnez r20, end
	nop
	nop

	;pripocitanie prveho znaku
	add r5, r5, r27	;r5 += poradie pismena 'z' v abecede
	sgti r20, r5, 122	;nove_pismeno presahuje pismeno 'z'
	bnez r20, prekrocenie1	;prepocitanie noveho indexu ak to je potrebne
	nop 
	nop
	sb cipher(r17), r5	;nove_pismeno nepresahuje pismeno 'z'
	
	j druhe_pismeno
	nop
	nop

prekrocenie1:
	subi r5, r5, 123	;odpocitanie ascii(z)+1 -> offset od pismena 'a'
	addi r5, r5, 97		;r5 = nove_pismeno
	sb cipher(r17), r5

druhe_pismeno:
	addi r17, r17, 1
	lb r5, login(r17)

	;kontrola ci je nacitany znak pismeno
	slti r20, r5, 97
	bnez r20, end
	nop
	nop

	;odpocitavanie druheho pismena
	sub r5, r5, r28	;r5 -= poradie pismena 'v' v abecede
	slti r20, r5, 97	;nove_pismeno je mensie nez pismeno 'a'
	bnez r20, prekrocenie2
	nop
	nop
	sb cipher(r17), r5	;nove_pismeno nepresahuje pismeno 'z'
	addi r17, r17, 1	;r17++
	
	j prve_pismeno
	nop
	nop
	

prekrocenie2:
	xor r27, r27, r27
	addi r27, r0, 97	;r27 = 'a'
	sub r5, r27, r5		;r5 = offset od pismena z + 1
	subi r5, r5, 1		
	xor r27, r27, r27
	addi r27, r0, 122	;r27 = 'z'
	sub r5, r27, r5		;r5 = nove_pismeno
	sb cipher(r17), r5	;nove_pismeno nepresahuje pismeno 'z'
	
	addi r17, r17, 1
	j prve_pismeno
	nop
	nop
	

end:    
	sb cipher(r17), r0
	addi r14, r0, caddr ; <-- pro vypis sifry nahradte laddr adresou caddr
        trap 5  ; vypis textoveho retezce (jeho adresa se ocekava v r14)
        trap 0  ; ukonceni simulace
