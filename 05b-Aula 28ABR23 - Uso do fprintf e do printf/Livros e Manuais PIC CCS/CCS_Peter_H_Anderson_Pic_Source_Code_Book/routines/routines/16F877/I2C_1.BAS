' I2C_1.Bas (BX24)
'
' Compile with I2C_BX24.Bas and SerCom3.Bas and LCDCntrl.Bas
'
' Used to test I2C_SLV1.C (PIC16F877).
'
' Address I2C device with address 0x40 and commands it to perform a temperature measurement
' on Ch 0 followed by a one second delay.  The nine bytes are then read and displayed on a
' serial LCD.
'
' BX24			Serial LCD+		PIC16F877
'
' Term 13 -------------> (term 2)
'
' Term 15 <------------------------------------> RC4/SDA (term 23)
' Term 14 -------------------------------------> RC3/SCL (term 18)
'
' 4.7K pull-up resistors to +5 VDC on both SDA and SCL
'
' copyright, Peter H. Anderson, Georgetown, SC, Mar, '01

Public Const SDA_PIN as Byte = 15
Public Const SCL_PIN as Byte = 16

Sub Main()

   Dim Buff(1 to 9) as Byte
   Dim N as Integer
	
   Call DefineCom3(0, 13, &H08) ' noninverted, no parity, 8 data bits
		                  ' input, output
		  
   Call OpenSerialPort_3(9600)
   Call LCDInit()	

   Do
     ' perform a temperature measurement
      Call I2C_start()
      Call I2C_out_byte(&H40)	
	   
      Call Sleep(0.005)
      Call I2C_out_byte(&H80)	' temperature measurement on Ch 0
      Call I2C_stop()
		
      Call Sleep(2.0)
		
      Call I2C_start()
      Call I2C_out_byte(&H41)	'  Read
      Call Sleep(0.005)			
      For N = 1 to 8
	   
         Buff(N) = I2C_in_byte(TRUE)	' ack after each byte
	
      Next
		
      Buff(9) = I2C_in_byte(FALSE)	' no ack
      Call I2C_stop()
		
      Call DisplayResult(Buff)
				 
   Loop
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



