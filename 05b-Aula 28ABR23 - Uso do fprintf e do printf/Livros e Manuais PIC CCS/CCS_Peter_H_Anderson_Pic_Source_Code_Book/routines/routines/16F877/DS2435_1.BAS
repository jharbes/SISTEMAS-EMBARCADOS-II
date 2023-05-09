
' DA2435_1.Bas (BX24)
'
' Illustrates an interface with a Dallas DS2435 Temperature / Histogram IC
' The DS2435 is a member of the Dallas 1-W Family.
'
' Program reads and displays the two byte ID.  Reads and displays Temperature.
' Resets Cycle Counter.  Increments Cycle Counter five times and then reads the 
' counter and displays the value.
'
' Sets elapsed time to 0 and continually loops, reading and displaying the elapsed time
' in 1/2 minute intervals.
'
'
' BX24					DS2435
'			+5
'			|
'			* 4.7K
' 			|
' RA.7 (Term 13) ---------------------- DQ (term 2)
'					Term 1 - GRD
' 					Term 3 - +5V
' 
'
'
' copyright, Ernest Wells, Peter H. Anderson, Baltimore, MD, May, '01

Sub Main()

  Dim A(1 to 6) as Byte, N as Byte
  Dim Count as Integer, ET as Integer
  Dim T_C_Float as Single

  Call OpenSerialPort(1, 19200)
  
  Call Read2435_ID(A)	' fetch and display ID
  Debug.Print "ID = ";
  Call PutHexB(A(2))
  Debug.Print " ";
  Call PutHexB(A(1))
  Debug.Print		' new line
  
  Call Read2435Temperature(A)	
  T_C_Float = CSng(A(1)) / 2.0
  
  Debug.Print "T_C = "; 
  Call PutS(T_C_Float)
  Debug.Print  " "; CStr(A(2))
  
  Call Reset2435CycleCounter()
  
  For N = 1 to 5
     Call Increment2435CycleCounter()
  Next   
  
  Call Read2435CycleCounter(A)
  Count = CInt(A(2)) * 256 + CInt(A(1)) 
  	' only two least sig bytes displayed
  Debug.Print "Count = "; CStr(Count)
  
  Call Set2435SampleRate(0)	' 1/2 minute
  
  A(1) = 0
  A(2) = 0
  A(3) = 0
  
  Call Set2435ElapsedTime(A)	' zero the elapsed time
  
  Do
       
     Call Read2435ElapsedTime(A)
     ET = CInt(A(2)) * 256 + CInt(A(1))  
     Debug.Print "Elapsed Time = "; CStr(ET)
     Call Init_1W(13)
     Sleep(5.0)
  
  Loop
  
End Sub  

Sub Read2435_ID(ByRef A() as Byte)

   Call Init_1W(13)           ' reset 
   Call OutByte_1W(13, &HB2)   ' read registers on page 5 (ID bytes) 
   Call OutByte_1W(13,&H80)    ' ID address 80h and 81h 
   A(1) = InByte_1W(13)
   A(2) = InByte_1W(13)
   
End Sub

Sub Read2435Temperature(ByRef A() as Byte)

   Call Init_1W(13)
   Call OutByte_1W(0,&HD2)     'initiate temperature conversion cycle
   Call Sleep(1.0)

   Call Init_1W(13)       
   Call OutByte_1W(13, &HB2)    ' read register  
   Call OutByte_1W(13,&H60) 	' temperature is at locations 0x60, 0x61
  
   A(1) = InByte_1W(13)
   A(2) = InByte_1W(13)
   
End Sub   

Sub Reset2435CycleCounter()

   Call Init_1W(13)
   Call OutByte_1W(13,&HB8)     

End Sub

Sub Increment2435CycleCounter()

   Call Init_1W(13)
   Call OutByte_1W(13,&HB5)     

End Sub


Sub Read2435CycleCounter(ByRef A() as Byte)

   Call Init_1W(13)     
   Call OutByte_1W(13, &HB2)   ' read registers 
   Call OutByte_1W(13,&H82) 
  
   A(1) = InByte_1W(13)
   A(2) = InByte_1W(13)
   
End Sub   

Sub Set2435SampleRate(ByVal V as Byte)

   Call Init_1W(13)       
   Call OutByte_1W(13, &HEF)   
   Call OutByte_1W(13,&H8B) 
   Call OutByte_1W(13,V) 

End Sub  

Sub Set2435ElapsedTime(ByRef A() as Byte)

   Call Init_1W(13)       
   Call OutByte_1W(13, &HE6)   
   Call OutByte_1W(13, A(1)) 
   Call OutByte_1W(13, A(2)) 
   Call OutByte_1W(13, A(3)) 
   
End Sub   

Sub Read2435ElapsedTime(ByRef A() as Byte)

   Call Init_1W(13)       
   Call OutByte_1W(13, &HB2) 
   Call OutByte_1W(13, &H74) 
   
   A(1) = InByte_1W(13)	' low byte
   A(2) = InByte_1W(13)	' middle byte
   A(3) = InByte_1W(13)	' high byte	
   
End Sub   


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
