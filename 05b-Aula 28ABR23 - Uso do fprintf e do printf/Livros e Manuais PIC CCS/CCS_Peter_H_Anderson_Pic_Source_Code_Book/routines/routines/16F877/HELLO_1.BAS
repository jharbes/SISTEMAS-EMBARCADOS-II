' Program HELLO_1.Bas
'
' Continually sends the string "Hello World" to PIC16F877 with a delay
' of 3.0 seconds.
'
' 9600 baud, noninverted.
'
'  BX24			 PIC16F877
'
'   12 -------------------> RC7/RX (term 26)
'
' Compile with SerCom3.Bas and SerialCom1.Bas
'
' copyright, Peter H. Anderson, Georgetown, SC, Mar, '01

Sub Main()

   Dim Str as String *15
   
   Call OpenSerialPort(1, 19200) ' for debugging

   Call DefineCom3(0, 12, &H08) ' noninverted, no parity, 8 data bits
                  ' input, output
   
   Call OpenSerialPort_3(9600)
    
   Str = "Hello World"
   Do
      Call PutStr_3(Str)
      Call PutByte_3(13)
      Call Sleep(3.0)
   Loop
   
End Sub   
   
   