' Sub LCDInit()
' Sub LCDClearAll()
' Sub LCDClearColumn()
' Sub LCDClearRow(ByVal Row as Byte) ' row is 0 to 3
' Sub LCDClearBlocks(ByVal Num as Byte) ' number of blocks to clear
' Sub LCDSetCursorPosition(ByVal Position as Byte) ' 0 - 79
' Sub LCDSetCursorStyle(ByVal Style as Byte) ' 0 = Hide, 1 = UL, 2 = Block
' Sub LCDSetBeepFreq(ByVal X as Byte) ' 0 - 255
' Sub LCDBacklightOn()
' Sub LCDBacklightOff()
' Sub LCDSetBeepFreq(ByVal X as Byte) ' 0 - 255
' Sub LCDBeep(ByVal NumBeeps as Byte, ByVal Tm as Single)
' Sub LCDSetBackLight(ByVal X as Byte) ' 0 - 255
' Sub LCDSetContrast(ByVal X as Byte) ' 0 - 255
' Sub LCDDisplayCustomChar(ByVal SCharNum as Byte)
'
' copyright, Peter H. Anderson, Baltimore, MD, May, '00


Sub LCDInit()
   Call LCDBacklightOff()
   Call LCDClearAll()   
   Call LCDSetCursorPosition(0)	' home cursor

   Call LCDSetCursorStyle(1)	' underline type cursor
   
   Call LCDSetBackLight(&H60)	' in range of &H00 to &Hff
   Call LCDSetContrast(&H80)	' in range of &H00 to &Hff
   Call LCDBacklightOn()   
End Sub

Sub LCDClearAll()
   Dim Column as Byte
   For Column = 0 to 19
      Call LCDSetCursorPosition(Column)
      Call LCDClearColumn()
   Next
   Call LCDSetCursorPosition(0)	' leave cursor in home position
   ' Call Sleep(5.0)
End Sub

Sub LCDClearColumn()
   Call PutByte_3(&H11)
End Sub

Sub LCDClearRow(ByVal Row as Byte)
   Dim N as Byte
   Call LCDSetCursorPosition(Row * 20)
   For N = 1 to 20
      Call PutByte_3(Asc(" "))
   Next
   LCDSetCursorPosition(Row * 20)	' leave the cursor at the beginning 					' of the row
End Sub

Sub LCDClearBlocks(ByVal Num as Byte)
   Dim N as Byte
   For N = 1 to Num
      Call PutByte_3(Asc(" "))
   Next   
   For N = 1 to Num
      Call PutByte_3(&H08)	' then back space
   Next
End Sub

Sub LCDSetCursorPosition(ByVal Position as Byte)
   Call PutByte_3(&H10)
   Call PutByte_3(Position)
End Sub

Sub LCDSetCursorStyle(ByVal Style as Byte)
   Call PutByte_3(&H04 + Style)
End Sub

Sub LCDSetBeepFreq(ByVal X as Byte)
   Call PutByte_3(&H17)
   Call PutByte_3(X)
End Sub

Sub LCDBeep(ByVal NumBeeps as Byte, ByVal Tm as Single)
   Dim N as Byte
   For N = 1 to NumBeeps
      Call PutByte_3(&H07)
      Call Sleep(Tm)
   Next
End Sub

Sub LCDBacklightOn()
   Call PutByte_3(&H0e)
End Sub

Sub LCDBacklightOff()
   Call PutByte_3(&H0f)	
End Sub

Sub LCDSetBackLight(ByVal X as Byte)
   Call PutByte_3(&H02)
   Call PutByte_3(X)
End Sub

Sub LCDSetContrast(ByVal X as Byte)
   Call PutByte_3(&H03)
   Call PutByte_3(X)
End Sub

Sub LCDDisplayCustomChar(ByVal SCharNum as Byte)
   Call PutByte_3(&H80 + SCharNum)
End Sub



