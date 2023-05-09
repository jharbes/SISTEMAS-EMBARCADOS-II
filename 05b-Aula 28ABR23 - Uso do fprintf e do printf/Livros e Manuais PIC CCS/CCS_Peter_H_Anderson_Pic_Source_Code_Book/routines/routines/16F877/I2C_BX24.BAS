' I2C_BX24.Bas
Attribute VB_Name = "I2C_BX24"
'-------------------
Option Explicit

' This module is a collection of low level I2C routines intended to be
' included in a project using such I2C devices as the Microchip 24 series 
' EEPROMs, MAX518 D/A, PCF8574 I/O Expander, DS1803 Digital Pot, PCF8591
' A/D plus D/A and many other devices.
'
' Public Sub I2C_out_byte(ByVal O_byte As Byte) - sends O_byte to I2C
' slave, most significant bit first.
'
'
' Public Function I2C_in_byte(ByVal Ack as Boolean) - receives a byte from I2C slave.
'
' Public Sub I2C_start() - initiates sequence by bringing SDA low while 
' SCL is high.  (Could be Private rather than Public).
'
' Public Sub I2C_stop() - terminates sequence by bringing SDA high 
' while SCL is high. (Could be Private rather than Public).
'
' Public Sub I2C_high_sda() - bring SDA high (high Z)
'
' Public Sub I2C_high_scl() - bring SCL high (high Z)  
'
' Public Sub I2C_low_sda() - bring SDA low, hard logic zero
'
' Public Sub I2C_low_scl() - bring SCL low, hard logic zero
'
' Note that SDA_PIN and SCL_PIN should be publicly defined in the calling module
'
' For example;
'    Const SDA_PIN as Byte = 13	
'    Const SCL_PIN as Byte = 14
'
' Copyright, Peter H. Anderson, Baltimore, MD, Sept, '99
' Nov, '99 - Revised for BX24.
' Mar, '00 - I2C_In_Byte was made a function.  Prior to this time it was a Sub
' where the result was passed by reference.
'
' Mar, '01 - I2C_out_byte revised to incorporate the sending of a NACK
'          - I2C_in_byte revised to include ACK or NACK
'
' ----------
Public Sub I2C_out_byte(ByVal O_byte As Byte) 

   DIM N as Byte
   Call Sleep(0.005)
   For N = 1 TO 8 Step 1
      If (O_byte >= 128) then	' most sig bit is a one
         Call I2C_high_sda()
         'Call PutB(1)		' used for debugging
      Else
         Call I2C_low_sda()	' set SDA and then clock
         'Call PutB(0)		' used for debugging
      End If

      Call I2C_high_scl()
      Call I2C_low_scl()

      O_byte = O_byte * 2	' shift left
      
   Next 
   
   Call I2C_high_sda()		' send NACK
   Call I2C_high_scl()
   Call I2C_low_scl()  
   ' Call NewLine()		' used for debugging
End Sub

Public Function I2C_in_byte(ByVal Ack as Boolean) as Byte

   DIM N as Byte, Y as Byte, I_Byte as Byte
   Call Sleep(0.005)
   I_Byte = 0

   For N = 1 to 8 Step 1
      Call I2C_high_scl()	' bring clock high
      Y =  GetPin(SDA_PIN)	' read SDA
      ' Call PutB(Y)
      Call I2C_low_scl()
      I_byte = I_byte * 2 + Y	' shift left and insert Y
   Next 
   ' Call NewLine()
   
   If (Ack) Then		' ack or nack
      Call I2C_low_sda()
   Else
      Call I2C_high_sda()
   End If
   
   Call I2C_high_scl()
   Call I2C_low_scl()  
   
   Call I2C_high_sda()
   
   I2C_In_Byte = I_Byte
End Function

Public Sub I2C_start()		' bring SDA low while SCL is high

   Call I2C_low_scl()
   Call I2C_high_sda()
   Call I2C_high_scl()
   Call I2C_low_sda()
   Call I2C_low_scl()

End Sub

Public Sub I2C_stop()		' bring SDA high while SCL is high

   Call I2C_low_scl()
   Call I2C_low_sda()
   Call I2C_high_scl()
   Call I2C_high_sda()

End Sub

Public Sub I2C_high_sda()

   Call PutPin(SDA_PIN, 2)	' tristate

End Sub

Public Sub I2C_high_scl()	' tristate

   Call PutPin(SCL_PIN, 2)		

End Sub

Public Sub I2C_low_sda()

   Call PutPin(SDA_PIN, 0)	' hard logic zero

End Sub

Public Sub I2C_low_scl()

   Call PutPin(SCL_PIN, 0)	' hard logic zero

End Sub

