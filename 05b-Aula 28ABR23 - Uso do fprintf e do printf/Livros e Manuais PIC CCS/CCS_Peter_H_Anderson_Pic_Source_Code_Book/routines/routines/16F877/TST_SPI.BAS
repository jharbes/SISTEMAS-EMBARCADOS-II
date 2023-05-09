' TST_SPI.Bas (BX24)
'
' Used to test PIC16F877 in SPI slave mode.
'
' Brings CS output low and sends &H80 plus the DS1820 to be used in
' making a temperature measurement.  Receives the nine byte result of
' the previous measurement and displays on serial LCD.
'
' Compile with SerCom3.Bas, SerCom1.Bas and if using the BasicX Serial
' LCD with LCDCntrl.Bas.
'
'	BX24			 PIC16F877
'
' MOSI (term 15) -----------------> RC4/SDI (term 23
' MISO (term 16) <----------------- RC5/SDO (term 24)
' SCK (term 17) ------------------> RC3/SCK (term 18)
' CS (term 18) -------------------> RA5/AN4/SS (term 7)
'
' (Term 13) ---------------------------------------------> LCD (term 2)
'
' copyright, Peter H. Anderson, Georgetown, SC, Mar, '01

Const SO as Byte = 15
Const SI as Byte = 16
Const SCK as Byte = 17
Const CS as Byte = 18

Sub Main()

   Dim PreviousResult(1 to 9) as Byte, Channel as Byte
   Dim First as Boolean
   
   Call DefineCom3(0, 13, &H08) ' noninverted, no parity, 8 data bits
	                  ' input, output
	  
   Call OpenSerialPort_3(9600)
   Call LCDInit()	
   First = TRUE
	
   Call SPISetUp()
   Do
      Call SPIMakeTemperatureReading(0, PreviousResult)
      If (First) Then
	   First = FALSE
      Else 
	   Call DisplayResult(PreviousResult)
      End If
      Call Sleep(1.5)
   Loop
End Sub

Sub SPIMakeTemperatureReading(ByVal Ch as Byte, ByRef PreviousResult() as Byte)
    Dim Dummy as Byte
    Dim N as Integer

    Call PutPin(CS, 0)
    Dummy = SPI_IO(&H80+Ch)
	
    For N = 1 to 9
	   PreviousResult(N) = SPI_IO(&H00)	' fetch the 9 bytes
    Next	
    Call PutPin(CS, 1)
End Sub	
	
Sub SPISetUp()
    Call PutPin(CS, 1)
    Call PutPin(SO, 0)	' could be either a zero or one
    Call PutPin(SI, 2)	' make it an input
    Call PutPin(SCK, 0)
End Sub  
	
Function SPI_IO(ByVal X as Byte) as Byte
' assumes SO has been configured as an output, SCK as an output 0, SI as an input
' CS output low

   Dim N as Byte
	
   For N = 1 to 8
      If (GetBit(X, 7) = 1) Then
         Call PutPin(SO, 1)
      Else
         Call PutPin(SO, 0)
      End If
		
      Call PutPin(SCK, 1)
      Call ShortDelay()
      Call PutPin(SCK, 0)
      Call ShortDelay()  
      	
      If (GetPin(SI) = 1) Then
         X = X * 2 + 1
      Else
         X = X * 2
      End If
   Next	
   SPI_IO = X
End Function   

Sub ShortDelay()
   Sleep(0.001)
End Sub		

Sub DisplayResult(ByRef Vals() as Byte)
   Dim N as Integer
   Call LCDClearAll()
	
   For N = 1 to 4
      Call PutHexB_3(Vals(N))
      Call PutByte_3(Asc(" "))
   Next
	
   Call LCDSetCursorPosition(20)	' beginning of second line
	
   For N = 5 to 8
      Call PutHexB_3(Vals(N))
      Call PutByte_3(Asc(" "))
   Next

   Call LCDSetCursorPosition(40)	' beginning of third line
   Call PutHexB_3(Vals(9))
End Sub		
	   
Sub PutHexB_3(ByVal X as Byte)
   Dim Y as Byte

   Y = X\16
   Y = ToHexChar(Y)
   Call PutByte_3(Y)
   Y = X MOD 16
   Y = ToHexChar(Y)
   Call PutByte_3(Y)
End Sub

Function ToHexChar(ByVal X as Byte) as Byte
   Dim ReturnVal as Byte
   If (X < 10) Then
      ReturnVal = X + Asc("0")
   Else
      ReturnVal = X - 10 + Asc("A")
   End If
   ToHexChar = ReturnVal
End Function

