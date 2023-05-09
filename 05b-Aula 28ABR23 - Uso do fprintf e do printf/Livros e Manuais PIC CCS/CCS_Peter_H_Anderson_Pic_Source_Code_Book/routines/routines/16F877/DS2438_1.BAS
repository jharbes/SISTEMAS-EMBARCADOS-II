' DS2438_1.Bas (BX24)
'
' Illustrates an interface with the Dallas DS2438 Temperature and Voltage
' Monitor IC.
'
' Continually loops performing measurements of T_C, VDD and VAD.  This might 
' be used in any application where it is desireable to measure both temperature 
' and a voltage. In this example, the DS2438 measures the output of a Honeywell 
' HIH3610 Relative Humidity Sensor.
'
' Note that a module consisting of the DS2438 and an HIH3610 on a small
' PCB is available from Dallas Semi as "DSHS01K Humidity Sensor Experimenter's
' Kit".  For the moment, we also sell this module.  The price is a bit more than directly
' from Dallas.  However, you might save on the shipping from us.
'
'
' BX24					DS2438
'			+5
'			|
'			* 4.7K
' 			|
' RA.7 (Term 13) ---------------------- DQ (term 8)
' 
'			HIH-3610
'			 V_out ------ VAD (term 4)
'
' 4.7K pullup to +5 VDC on DQ.
'

'
' copyright, Peter H. Anderson, Baltimore, MD, May, '01

Sub Main()

  Dim VDD as Single, VAD as Single, T_C as Single, RH as Single
  
  Call OpenSerialPort(1, 19200)
  
  Do 
  
     T_C = MeasTemperature()
     VDD = MeasVDD()
     VAD = MeasADC()

     RH = CalcRH(VDD, VAD, T_C)
     
     Debug.Print "T_C = ";
     Call PutS(T_C)
     Debug.Print "  ";
     
     Debug.Print "VDD = ";
     Call PutS(VDD)
     Debug.Print "  ";	
     
     Debug.Print "VAD = ";
     Call PutS(VAD)
     Debug.Print "   ";
     
     Debug.Print "RH = ";
     Call PutS(RH)
     Debug.Print	' new line

     Sleep(5.0)                    
       
  Loop
  
End Sub  

Function CalcRH(ByVal VDD as Single, ByVal VAD as Single, ByVal T_C as Single) as Single
  Dim RH as Single, RHCorrected as Single

  RH = ((VAD / VDD) - 0.16) / 0.0062
  RHCorrected = RH * (1.0546 - 0.00216 * T_C)

  CalcRH = RHCorrected

End Function
 
  
Function MeasADC() as Single
  
  Dim A(1 to 9) as Byte 
  Dim V as Integer, N as Integer
    
  Call Init_1W(13)
  Call OutByte_1W(13, &HCC)	' skip ROM
  Call OutByte_1W(13, &H4E)
  Call OutByte_1W(13, &H00)
  Call OutByte_1W(13, &H00)	' setup for A/D input
  
  Call Init_1W(13)
  Call OutByte_1W(13, &HCC)	' skip ROM
  Call OutByte_1W(13, &HB4)	' A/D conversion   
  Call Sleep(1.0)
       
  Call Init_1W(13)
  Call OutByte_1W(13, &HCC)	' skip ROM
  Call OutByte_1W(13, &HB8)	' recall memory
  Call OutByte_1W(13, &H00)    
  
  Call Init_1W(13)
  Call OutByte_1W(13, &HCC)	' skip ROM
  Call OutByte_1W(13, &HBE)	    
  Call OutByte_1W(13, &H00)	' read scratchpad beginning at location 0

  For N = 1 to 9
       
     A(N) = InByte_1W(13)

  Next
 
  V = CInt(A(5)) * 256 + CInt(A(4)) 
  MeasADC = 0.01 * CSng(V)      
  
End Function
              
Function MeasVDD() as Single
  
  Dim A(1 to 9) as Byte
  Dim V as Integer, N as Integer
   
  Call Init_1W(13)
  Call OutByte_1W(13, &HCC)	' skip ROM
  Call OutByte_1W(13, &H4E)
  Call OutByte_1W(13, &H00)
  Call OutByte_1W(13, &H08)	' setup for VDD input
  
  Call Init_1W(13)
  Call OutByte_1W(13, &HCC)	' skip ROM
  Call OutByte_1W(13, &HB4)	' A/D conversion   
  Call Sleep(1.0)
       
  Call Init_1W(13)
  Call OutByte_1W(13, &HCC)	' skip ROM
  Call OutByte_1W(13, &HB8)	' recall memory
  Call OutByte_1W(13, &H00)
       
  Call Init_1W(13)
  Call OutByte_1W(13, &HCC)	' skip ROM
  Call OutByte_1W(13, &HBE)	    
  Call OutByte_1W(13, &H00)	' read scratchpad beginning at location 0

  For N = 1 to 9
       
     A(N) = InByte_1W(13)

  Next
 
  V = CInt(A(5)) * 256 + CInt(A(4)) 
  MeasVDD = 0.01 * CSng(V)      
  
End Function       
  
Function MeasTemperature() as Single
    
  Dim A(1 to 9) as Byte
  Dim V as Integer, N as Integer
      
  Call Init_1W(13)
  Call OutByte_1W(13, &HCC)	' skip ROM
  Call OutByte_1W(13, &H4E)
  Call OutByte_1W(13, &H00)
  Call OutByte_1W(13, &H00)	' setup for VDD input
    
  Call Init_1W(13)
  Call OutByte_1W(13, &HCC)	' skip ROM
  Call OutByte_1W(13, &H44)	' temperature   
  Call Sleep(1.0)
         
  Call Init_1W(13)
  Call OutByte_1W(13, &HCC)	' skip ROM
  Call OutByte_1W(13, &HB8)	' recall memory
  Call OutByte_1W(13, &H00)
         
  Call Init_1W(13)
  Call OutByte_1W(13, &HCC)	' skip ROM
  Call OutByte_1W(13, &HBE)	    
  Call OutByte_1W(13, &H00)	' read scratchpad beginning at location 0
         
  For N = 1 to 9
       
     A(N) = InByte_1W(13)

  Next
 
  V = CInt(A(3)) * 256 + CInt(A(2))     
  V = V \ 8
  MeasTemperature = 0.03125 * CSng(V)      
    
End Function     
  
Sub Init_1W(ByVal Pin as Byte) ' bring Pin low for 500 usecs and then back 
				' high

   Dim N as Integer
   Call PutPin(Pin, 2)	' be sure DQ is an input

   Call PutPin(Pin, 0)	
    
    For N = 1 to 3	' adjust for 500 usec delay
    Next

   Call PutPin(Pin, 2)
  
    For N = 1 to 3
    Next

End Sub

Function InByte_1W(ByVal Pin as Byte) as Byte

   Dim N as Integer, IByte as Byte, B as Byte
   For N =1 to 8
      B = Get1Wire(Pin)
      If (B=1) then
         IByte = (IByte\2) OR bx10000000
      Else
         IByte = IByte\2         
      End If
   Next

   InByte_1W = IByte

End Function
           
Sub OutByte_1W(ByVal Pin as Byte, ByVal OByte as Byte)

   Dim N as Integer, B as Byte
   For N = 1 to 8
     B = OByte AND bx00000001
     If (B=1) Then
        Call Put1Wire(Pin, 1)
     Else
        Call Put1Wire(Pin, 0)
     End If
     OByte = OByte \ 2
   Next
End Sub      

Sub StrongPullUp_1W(ByVal Pin as Byte)
	' Provide a hard logic one for 0.5 secs
   Call PutPin(Pin, 1)
   Call Sleep(0.5)
   Call PutPin(Pin, 2)
End Sub

Sub PutHexB(ByVal X as Byte)	' display a byte in hex format
    Dim Y as Byte
 
    Y= X \ 16 			' convert high nibble to character
    If (Y < 10) then
      Y = Y + Asc("0")
    Else
      Y = Y - 10 + Asc ("A")
    End If
    Call PutByte(Y)

    Y= X And bx00001111 	' same for low nibble
    If (Y < 10) then
      Y = Y + Asc("0")
    Else
      Y = Y - 10 + Asc ("A")
    End If
    Call PutByte(Y)
End Sub
