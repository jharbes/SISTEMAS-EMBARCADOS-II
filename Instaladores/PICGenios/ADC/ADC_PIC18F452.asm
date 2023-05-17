
_main:

;ADC_PIC18F452.c,38 :: 		void main() {
;ADC_PIC18F452.c,40 :: 		trisb = 0; //define portb como saida
	CLRF        TRISB+0 
;ADC_PIC18F452.c,41 :: 		trisd = 0; //define portd como saida
	CLRF        TRISD+0 
;ADC_PIC18F452.c,43 :: 		ADCON1 = 0b00001101; //habilita canal A/D0 e A/D1 do PIC
	MOVLW       13
	MOVWF       ADCON1+0 
;ADC_PIC18F452.c,44 :: 		trisa=0b00001111; //define pinos como entrada
	MOVLW       15
	MOVWF       TRISA+0 
;ADC_PIC18F452.c,46 :: 		Lcd_Init();                        // Initialize Lcd
	CALL        _Lcd_Init+0, 0
;ADC_PIC18F452.c,47 :: 		Lcd_Cmd(_LCD_CLEAR); //apaga lcd
	MOVLW       1
	MOVWF       FARG_Lcd_Cmd_out_char+0 
	CALL        _Lcd_Cmd+0, 0
;ADC_PIC18F452.c,48 :: 		Lcd_Cmd(_LCD_CURSOR_OFF); //desliga cursor do lcd
	MOVLW       12
	MOVWF       FARG_Lcd_Cmd_out_char+0 
	CALL        _Lcd_Cmd+0, 0
;ADC_PIC18F452.c,49 :: 		Lcd_Out(1, 1, "AN0:"); //escreve mansagem na linha 1, coluna 1 do lcd
	MOVLW       1
	MOVWF       FARG_Lcd_Out_row+0 
	MOVLW       1
	MOVWF       FARG_Lcd_Out_column+0 
	MOVLW       ?lstr1_ADC_PIC18F452+0
	MOVWF       FARG_Lcd_Out_text+0 
	MOVLW       hi_addr(?lstr1_ADC_PIC18F452+0)
	MOVWF       FARG_Lcd_Out_text+1 
	CALL        _Lcd_Out+0, 0
;ADC_PIC18F452.c,51 :: 		Lcd_Out(2, 1, "AN1:"); //escreve mensagem na linha 2, coluna 1 do lcd
	MOVLW       2
	MOVWF       FARG_Lcd_Out_row+0 
	MOVLW       1
	MOVWF       FARG_Lcd_Out_column+0 
	MOVLW       ?lstr2_ADC_PIC18F452+0
	MOVWF       FARG_Lcd_Out_text+0 
	MOVLW       hi_addr(?lstr2_ADC_PIC18F452+0)
	MOVWF       FARG_Lcd_Out_text+1 
	CALL        _Lcd_Out+0, 0
;ADC_PIC18F452.c,53 :: 		ADCON1 = 0b00001011; //habilita canal A/D0 e A/D1 do PIC; VREF+= +5V; VREF- =GND
	MOVLW       11
	MOVWF       ADCON1+0 
;ADC_PIC18F452.c,54 :: 		trisa=0b00001111; //define pinos como entrada
	MOVLW       15
	MOVWF       TRISA+0 
;ADC_PIC18F452.c,55 :: 		do
L_main0:
;ADC_PIC18F452.c,57 :: 		temp_res = Adc_Read(0); //le canal ad0 do PIC e salva valor na variável temp_res
	CLRF        FARG_ADC_Read_channel+0 
	CALL        _ADC_Read+0, 0
	MOVF        R0, 0 
	MOVWF       _temp_res+0 
	MOVF        R1, 0 
	MOVWF       _temp_res+1 
	MOVLW       0
	MOVWF       _temp_res+2 
	MOVWF       _temp_res+3 
;ADC_PIC18F452.c,58 :: 		delay_us(10); //delay de 10 microsegundos
	MOVLW       6
	MOVWF       R13, 0
L_main3:
	DECFSZ      R13, 1, 1
	BRA         L_main3
	NOP
;ADC_PIC18F452.c,60 :: 		pot1= (temp_res)*5000/1024;
	MOVF        _temp_res+0, 0 
	MOVWF       R0 
	MOVF        _temp_res+1, 0 
	MOVWF       R1 
	MOVF        _temp_res+2, 0 
	MOVWF       R2 
	MOVF        _temp_res+3, 0 
	MOVWF       R3 
	MOVLW       136
	MOVWF       R4 
	MOVLW       19
	MOVWF       R5 
	MOVLW       0
	MOVWF       R6 
	MOVWF       R7 
	CALL        _Mul_32x32_U+0, 0
	MOVLW       10
	MOVWF       R4 
	MOVF        R0, 0 
	MOVWF       _pot1+0 
	MOVF        R1, 0 
	MOVWF       _pot1+1 
	MOVF        R2, 0 
	MOVWF       _pot1+2 
	MOVF        R3, 0 
	MOVWF       _pot1+3 
	MOVF        R4, 0 
L__main6:
	BZ          L__main7
	RRCF        _pot1+3, 1 
	RRCF        _pot1+2, 1 
	RRCF        _pot1+1, 1 
	RRCF        _pot1+0, 1 
	BCF         _pot1+3, 7 
	ADDLW       255
	GOTO        L__main6
L__main7:
;ADC_PIC18F452.c,62 :: 		temp_res2 = Adc_Read(1); //lê canal ad1 do PIC e salva valor na variável temp_res2
	MOVLW       1
	MOVWF       FARG_ADC_Read_channel+0 
	CALL        _ADC_Read+0, 0
	MOVF        R0, 0 
	MOVWF       _temp_res2+0 
	MOVF        R1, 0 
	MOVWF       _temp_res2+1 
	MOVLW       0
	MOVWF       _temp_res2+2 
	MOVWF       _temp_res2+3 
;ADC_PIC18F452.c,63 :: 		delay_us(10); //delay de 10 microsegundos
	MOVLW       6
	MOVWF       R13, 0
L_main4:
	DECFSZ      R13, 1, 1
	BRA         L_main4
	NOP
;ADC_PIC18F452.c,65 :: 		pot2= (temp_res2)*5000/1024;;
	MOVF        _temp_res2+0, 0 
	MOVWF       R0 
	MOVF        _temp_res2+1, 0 
	MOVWF       R1 
	MOVF        _temp_res2+2, 0 
	MOVWF       R2 
	MOVF        _temp_res2+3, 0 
	MOVWF       R3 
	MOVLW       136
	MOVWF       R4 
	MOVLW       19
	MOVWF       R5 
	MOVLW       0
	MOVWF       R6 
	MOVWF       R7 
	CALL        _Mul_32x32_U+0, 0
	MOVLW       10
	MOVWF       R4 
	MOVF        R0, 0 
	MOVWF       _pot2+0 
	MOVF        R1, 0 
	MOVWF       _pot2+1 
	MOVF        R2, 0 
	MOVWF       _pot2+2 
	MOVF        R3, 0 
	MOVWF       _pot2+3 
	MOVF        R4, 0 
L__main8:
	BZ          L__main9
	RRCF        _pot2+3, 1 
	RRCF        _pot2+2, 1 
	RRCF        _pot2+1, 1 
	RRCF        _pot2+0, 1 
	BCF         _pot2+3, 7 
	ADDLW       255
	GOTO        L__main8
L__main9:
;ADC_PIC18F452.c,67 :: 		LongToStr(pot1, texto);
	MOVF        _pot1+0, 0 
	MOVWF       FARG_LongToStr_input+0 
	MOVF        _pot1+1, 0 
	MOVWF       FARG_LongToStr_input+1 
	MOVF        _pot1+2, 0 
	MOVWF       FARG_LongToStr_input+2 
	MOVF        _pot1+3, 0 
	MOVWF       FARG_LongToStr_input+3 
	MOVLW       _texto+0
	MOVWF       FARG_LongToStr_output+0 
	MOVLW       hi_addr(_texto+0)
	MOVWF       FARG_LongToStr_output+1 
	CALL        _LongToStr+0, 0
;ADC_PIC18F452.c,68 :: 		Lcd_out(1,6,texto); //escreve no lcd o valor da conversão do ad0
	MOVLW       1
	MOVWF       FARG_Lcd_Out_row+0 
	MOVLW       6
	MOVWF       FARG_Lcd_Out_column+0 
	MOVLW       _texto+0
	MOVWF       FARG_Lcd_Out_text+0 
	MOVLW       hi_addr(_texto+0)
	MOVWF       FARG_Lcd_Out_text+1 
	CALL        _Lcd_Out+0, 0
;ADC_PIC18F452.c,70 :: 		LongToStr(pot2, texto);
	MOVF        _pot2+0, 0 
	MOVWF       FARG_LongToStr_input+0 
	MOVF        _pot2+1, 0 
	MOVWF       FARG_LongToStr_input+1 
	MOVF        _pot2+2, 0 
	MOVWF       FARG_LongToStr_input+2 
	MOVF        _pot2+3, 0 
	MOVWF       FARG_LongToStr_input+3 
	MOVLW       _texto+0
	MOVWF       FARG_LongToStr_output+0 
	MOVLW       hi_addr(_texto+0)
	MOVWF       FARG_LongToStr_output+1 
	CALL        _LongToStr+0, 0
;ADC_PIC18F452.c,71 :: 		Lcd_out(2,6,texto); //escreve no lcd o valor da conversão do ad1
	MOVLW       2
	MOVWF       FARG_Lcd_Out_row+0 
	MOVLW       6
	MOVWF       FARG_Lcd_Out_column+0 
	MOVLW       _texto+0
	MOVWF       FARG_Lcd_Out_text+0 
	MOVLW       hi_addr(_texto+0)
	MOVWF       FARG_Lcd_Out_text+1 
	CALL        _Lcd_Out+0, 0
;ADC_PIC18F452.c,76 :: 		while (1);
	GOTO        L_main0
;ADC_PIC18F452.c,77 :: 		}
L_end_main:
	GOTO        $+0
; end of _main
