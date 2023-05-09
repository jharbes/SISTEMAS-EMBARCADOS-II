' SerCom3.BAS  
Option Explicit
' 
'
' This module is used to transfer data to and from one serial port in 
' addition to Com 1.  Note that the queue size must be 9 bytes larger 
' than the largest data element to provide space for the queue pointers.
'
' Note that Com1 is on pins 1 (TxD) and 2 (RxD).
' Com3: User defined pins.
'
' Public Sub OpenSerialPort_3(ByVal BaudRate as Long)
'
' Public Sub PutByte_3(ByVal Value as Byte)
' Public Sub PutB_3(ByVal Value as Byte)
' Public Sub PutI_3(ByVal Value as Integer)
' Public Sub PutUI_3(ByVal Value as UnsignedInteger)
' Public Sub PutL_3(ByVal Operand as Long)
' Public Sub PutUL_3(ByVal Value as UnsignedLong)
' Public Sub PutS_3(ByVal Value as Single)
' Public Sub PutSci_3(ByVal Value as Single)
' Public Sub NewLine_3()
' Public Sub PutStr_3(ByRef Tx as String)
' Public Sub PutLine_3(ByRef Tx as String)
' Public Sub GetByte_3(ByRef Value as Byte, ByRef Success as Boolean)
'
' Peter H. Anderson, Baltimore, MD, Apr, '00.  This is adapted from 
' Nick Taylor's Serial Port Routines.

Private Const InBufSize_3 as INTEGER = 40   ' 4-byte buffer.
Private Const OutBufSize_3 as INTEGER = 40  ' 1-byte buffer.

Private InBuf_3(1 To InBufSize_3) as BYTE
Private OutBuf_3(1 To OutBufSize_3) as BYTE

Public Sub OpenSerialPort_3(ByVal BaudRate as Long)

   CALL OpenQueue(InBuf_3, InBufSize_3)
   CALL OpenQueue(OutBuf_3, OutBufSize_3)
   CALL OpenCom(3, BaudRate, InBuf_3, OutBuf_3)

End Sub

Public Sub PutByte_3(ByVal Value as Byte)
   CALL PutQueue(OutBuf_3, Value, 1)
   Call Sleep(0.01)
End Sub

Public Sub PutB_3(ByVal Value as Byte)
   Dim L as Long
   L = CLng(Value)
   Call PutL_3(L)
End Sub

Public Sub PutI_3(ByVal Value as Integer)
   Dim L as Long
   L = CLng(Value)
   Call PutL_3(L)
End Sub

Public Sub PutUI_3(ByVal Value as UnsignedInteger)
   Dim L as Long
   Dim V as New UnsignedInteger

   V = Value
   ' Clear L.
   L = 0
   ' Copy Value into the lower two bytes of L.
   Call BlockMove(2, MemAddress(V), MemAddress(L))
   Call PutL_3(L)
End Sub

Public Sub PutL_3(ByVal Operand as Long)

   const NegativeLimit as LONG = -2147483648
   const Base as LONG = 10
   ' Reserve space for "2147483648"
   dim Digit(1 To 10) as BYTE
   dim Tmp as LONG
   dim NDigits as INTEGER
   dim I as INTEGER

   ' Negative limit must be handled as a special case.
   IF (Operand = NegativeLimit) THEN
      Digit(10) = 2 + 48
      Digit(9) = 1 + 48
      Digit(8) = 4 + 48
      Digit(7) = 7 + 48
      Digit(6) = 4 + 48
      Digit(5) = 8 + 48
      Digit(4) = 3 + 48
      Digit(3) = 6 + 48
      Digit(2) = 4 + 48
      Digit(1) = 8 + 48
      NDigits = 10
   ELSE
      NDigits = 0
      Tmp = Abs(Operand)
      DO
         NDigits = NDigits + 1
         Digit(NDigits) = CByte(Tmp mod Base) + 48
         Tmp = Tmp \ Base
         IF Tmp = 0 THEN
            EXIT DO
         END IF
      LOOP
   END IF
   IF (Operand < 0) THEN
      CALL PutByte_3(45)		' "-"
   END IF
   ' Digits are stored in reverse order of display.
   For I = NDigits To 1 Step -1
      CALL PutByte_3(Digit(I))
   NEXT
End Sub


Public Sub PutUL_3(ByVal Value as UnsignedLong)

   Dim UL as New UnsignedLong
   Dim L as Long
   Dim Digit as New UnsignedLong
   Dim I as Integer
   Dim Temp as New UnsignedLong

   ' If the top bit is clear, the number is ready to go.
   If ((Value AND &H80000000) = 0) Then
      Call PutL_3(CLng(Value))
      Exit Sub
   End If
   ' Divide by 10 is done by a right shift followed by a divide by 5.
   ' First clear top bit so we can do a signed divide.
   UL = Value
   UL = UL AND &H7FFFFFFF
   ' Shift to the right 1 bit.
   L = CLng(UL)
   L = L \ 2
   ' Put the top bit back, except shifted to the right 1 bit.
   UL = CuLng(L)
   UL = UL OR &H40000000
   ' The number now fits in a signed LONG.
   L = CLng(UL)
   ' Divide by 5.
   L = L \ 5
   CALL PutL_3(L)
   ' Multiply by 10. Since multiply doesn't work yet for UNSIGNEDLONG, we
   ' have to do the equivalent addition.
   Temp = CuLng(L)
   UL = 0
   For I = 1 To 10
      UL = UL + Temp
   NEXT
   ' Find the rightmost digit.
   Digit = Value - UL
   CALL PutL_3(CLng(Digit))
END sub

Public Sub PutS_3(ByVal Value as Single)

   dim X as SINGLE
   dim DecimalPlace as INTEGER
   dim Mantissa as SINGLE
   dim Exponent as INTEGER
   dim DigitPosition as INTEGER
   dim Factor as LONG
   dim D as INTEGER
   dim LMant as LONG
   dim DecimalHasDisplayed as BOOLEAN

   ' Special case for zero.
   IF (Value = 0!) THEN
      CALL PutByte_3(48)		' "0"
      CALL PutByte_3(46)		' "."
      CALL PutByte_3(48)		' "0"
      EXIT sub
   END IF
   X = Abs(Value)
   ' Use scientific notation for values too big or too small.
   IF (X < 0.1) OR (X > 999999.9) THEN
      CALL PutSci_3(Value)
      EXIT sub
   END IF
   ' What follows is non-exponent displays for 0.1000000 < Value < 999999.9
   ' Sign.
   IF (Value < 0!) THEN
      CALL PutByte_3(45)		' "-"
   END IF
   IF (X < 1!) THEN
      CALL PutByte_3(48)		' "0"
      CALL PutByte_3(46)		' "."
      DecimalPlace = 0
      ' Convert number to a 7-digit INTEGER.
      LMant = FixL((X * 10000000#) + 0.5)
   ELSE
      CALL SplitFloat(X, Mantissa, Exponent)
      DecimalPlace = Exponent + 2
      ' Convert mantissa to a 7-digit INTEGER.
      LMant = FixL((Abs(Mantissa) * 1000000!) + 0.5)
      ' Correct for roundoff error. Mantissa can't be > 9.999999
      IF (LMant > 9999999) THEN
         LMant = 9999999
      END IF
   END IF
   DecimalHasDisplayed = False
   Factor = 1000000
   For DigitPosition = 1 To 7
      IF (DigitPosition = DecimalPlace) THEN
         CALL PutByte_3(46)	' "."
         DecimalHasDisplayed = True
      END IF
      D = CInt(LMant \ Factor)
      CALL PutByte_3(CByte(D + 48))
      LMant = LMant mod Factor
      ' Stop trailing zeros, except for one immediately following the
      ' decimal place.
      IF (LMant = 0) THEN
         IF (DecimalHasDisplayed) THEN
            EXIT sub
         END IF
      END IF 
         Factor = Factor \ 10
   NEXT
End Sub

Public Sub PutSci_3(ByVal Value as Single)

   dim Mantissa as SINGLE
   dim Exponent as INTEGER
   dim LMant as LONG
   dim D as INTEGER

   CALL SplitFloat(Value, Mantissa, Exponent)
   ' Sign.
   IF (Mantissa < 0!) THEN
      CALL PutByte_3(45)		' "-"
   END IF
   ' Convert mantissa to a 7-digit INTEGER.
   LMant = FixL((Abs(Mantissa) * 1000000!) + 0.5)
   ' Correct for roundoff error. Mantissa can't be > 9.999999
   IF (LMant > 9999999) THEN
      LMant = 9999999
   END IF
   ' First digit of mantissa.
   D = CInt(LMant \ 1000000)
   CALL PutByte_3(CByte(D + 48))
   ' Decimal point.
   CALL PutByte_3(46)		' "."
   ' Remaining digits of mantissa.
   LMant = LMant mod 1000000
   CALL PutL_3(LMant)
   ' Exponent.
   CALL PutByte_3(69)		' "E"
   IF (Exponent < 0) THEN
      CALL PutByte_3(45)		' "-"
   ELSE
      CALL PutByte_3(43)		' "+"
   END IF
   CALL PutI_3(Abs(Exponent))
End Sub

Private Sub SplitFloat(ByVal Value as SINGLE, _
		    ByRef Mantissa as SINGLE, _
		    ByRef Exponent as INTEGER)

' Splits a floating point number into mantissa and exponent. The mantissa
' range is such that 1.0 <= Abs(Mantissa) < 10.0 for nonzero numbers, and
' zero otherwise.

   dim X as SINGLE
   dim Factor as SINGLE

   ' Zero is a special case.
   IF (Value = 0!) THEN
      Mantissa = 0!
      Exponent = 0
      EXIT sub
   END IF
   X = Abs(Value)
   Exponent = 0
   Factor = 1!
   ' Multiply or divide by ten to transform number to value between 1 and 10.
   DO
      IF (X >= 10!) THEN
         X = X / 10!
         Factor = Factor * 10!
         Exponent = Exponent + 1
      ELSEIF (X < 1!) THEN
         X = X * 10!
         Factor = Factor * 10!
         Exponent = Exponent - 1
      ELSE
         ' IF we reach this point, then 1.0 <= mantissa < 10.0.
         EXIT DO
      END IF
   LOOP
   ' Determine mantissa.
   IF (Exponent = 0) THEN
      Mantissa = Value
   ELSEIF (Exponent > 0) THEN
      Mantissa = Value / Factor
   ELSE
      Mantissa = Value * Factor
   END IF
End Sub

Public Sub NewLine_3()
   Call PutByte_3(&h0D)
   Call Sleep(5)
   Call PutByte_3(&h0A)
End Sub

Public Sub PutStr_3(ByRef Tx as String)
   Dim Length as Integer
   Dim Ch as String * 1
   Dim bCh as Byte
   Dim I as Integer

   Length = Len(Tx)
   For I = 1 To Length
      Ch = Mid(Tx, I, 1)
      bCh = Asc(Ch)
      Call PutByte_3(bCh)
   Next
End Sub

Public Sub PutLine_3(ByRef Tx as STRING)
   CALL PutStr_3(Tx)
   CALL NewLine_3
End sub

Public Sub GetByte_3(ByRef Value as Byte, _
   ByRef Success as Boolean)

   ' Find out if anything is in the queue.
   Success = StatusQueue(InBuf_3)

   ' If data is in the queue, extract it.
   If (Success) Then
      Call GetQueue(InBuf_3, Value, 1)
   Else
      Value = 0
   End If
End Sub
