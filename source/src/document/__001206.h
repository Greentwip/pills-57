_blEnvmixerPull:
80064A00 27BDFFB0    ADDIU   sp,sp,-50

80064B0C 92220012    LBU     v0,12(s1)
80064B10 A6020018    SH      v0,18(s0)

80064CEC 8C82000C    LW      v0,C(a0)
80064CF0 A6020018    SH      v0,18(s0)

_blEnvmixerPull:
80064A00 27BDFFB0    ADDIU   sp,sp,-50
80064A04 AFB20030    SW      s2,30(sp)
80064A08 8FB20060    LW      s2,60(sp)
80064A0C AFB00028    SW      s0,28(sp)
80064A10 00808021    MOVE    s0,a0
80064A14 AFBF004C    SW      ra,4C(sp)
80064A18 AFBE0048    SW      fp,48(sp)
80064A1C AFB70044    SW      s7,44(sp)
80064A20 AFB60040    SW      s6,40(sp)
80064A24 AFB5003C    SW      s5,3C(sp)
80064A28 AFB40038    SW      s4,38(sp)
80064A2C AFB30034    SW      s3,34(sp)
80064A30 AFB1002C    SW      s1,2C(sp)
80064A34 8E02003C    LW      v0,3C(s0)
80064A38 00C0B821    MOVE    s7,a2
80064A3C 00E0B021    MOVE    s6,a3
80064A40 AFB60024    SW      s6,24(sp)
80064A44 A7A0001A    SH      r0,1A(sp)
80064A48 10400113    BEQ     v0,r0,80064E98
80064A4C A7A00018    SH      r0,18(sp)
80064A50 241E0001    ADDIU   fp,r0,1
80064A54 3C14800B    LUI     s4,800B
80064A58 26949EC0    ADDIU   s4,s4,-6140
80064A5C 8E03003C    LW      v1,3C(s0)
80064A60 8FA20024    LW      v0,24(sp)
80064A64 8C680004    LW      t0,4(v1)
80064A68 01029823    SUBU    s3,t0,v0
80064A6C 02F3102A    SLT     v0,s7,s3
80064A70 14400109    BNE     v0,r0,80064E98
80064A74 AFA80024    SW      t0,24(sp)
80064A78 84630008    LH      v1,8(v1)
80064A7C 2C620011    SLTIU   v0,v1,11
80064A80 104000E8    BEQ     v0,r0,80064E24
80064A84 00031080    SLL     v0,v1,2
80064A88 3C01800B    LUI     r1,800B
80064A8C 00220821    ADDU    r1,r1,v0
80064A90 8C224C10    LW      v0,4C10(r1)
80064A94 00400008    JR      v0
80064A98 00000000    NOP
80064A9C 8E11003C    LW      s1,3C(s0)
80064AA0 8622000A    LH      v0,A(s1)
80064AA4 10400006    BEQ     v0,r0,80064AC0
80064AA8 0200A821    MOVE    s5,s0
80064AAC 02002021    MOVE    a0,s0
80064AB0 8E020008    LW      v0,8(s0)
80064AB4 24050008    ADDIU   a1,r0,8
80064AB8 0040F809    JALR    ra,v0
80064ABC 00003021    MOVE    a2,r0
80064AC0 8E260018    LW      a2,18(s1)
80064AC4 8E020008    LW      v0,8(s0)
80064AC8 02002021    MOVE    a0,s0
80064ACC 0040F809    JALR    ra,v0
80064AD0 24050005    ADDIU   a1,r0,5
80064AD4 02002021    MOVE    a0,s0
80064AD8 8E020008    LW      v0,8(s0)
80064ADC 24050009    ADDIU   a1,r0,9
80064AE0 0040F809    JALR    ra,v0
80064AE4 00003021    MOVE    a2,r0
80064AE8 AE1E0038    SW      fp,38(s0)
80064AEC AE000030    SW      r0,30(s0)
80064AF0 8E220014    LW      v0,14(s1)
80064AF4 AE020034    SW      v0,34(s0)
80064AF8 86220010    LH      v0,10(s1)
80064AFC 00420018    MULT    v0,v0
80064B00 00001012    MFLO    v0
80064B04 000213C3    SRA     v0,v0,F
80064B08 A602001A    SH      v0,1A(s0)
80064B0C 92220012    LBU     v0,12(s1)
80064B10 A6020018    SH      v0,18(s0)
80064B14 92220013    LBU     v0,13(s1)
80064B18 00021040    SLL     v0,v0,1
80064B1C 00541021    ADDU    v0,v0,s4
80064B20 94420000    LHU     v0,0(v0)
80064B24 A6020020    SH      v0,20(s0)
80064B28 92220013    LBU     v0,13(s1)
80064B2C 2408007F    ADDIU   t0,r0,7F
80064B30 01021023    SUBU    v0,t0,v0
80064B34 00021040    SLL     v0,v0,1
80064B38 00541021    ADDU    v0,v0,s4
80064B3C 94420000    LHU     v0,0(v0)
80064B40 A6020022    SH      v0,22(s0)
80064B44 8E220014    LW      v0,14(s1)
80064B48 10400004    BEQ     v0,r0,80064B5C
80064B4C 00000000    NOP
80064B50 A61E001C    SH      fp,1C(s0)
80064B54 080192EA    J       80064BA8
80064B58 A61E001E    SH      fp,1E(s0)
80064B5C 86020018    LH      v0,18(s0)
80064B60 8603001A    LH      v1,1A(s0)
80064B64 00021040    SLL     v0,v0,1
80064B68 00541021    ADDU    v0,v0,s4
80064B6C 84420000    LH      v0,0(v0)
80064B70 00620018    MULT    v1,v0
80064B74 00001812    MFLO    v1
80064B78 86020018    LH      v0,18(s0)
80064B7C 01021023    SUBU    v0,t0,v0
80064B80 00021040    SLL     v0,v0,1
80064B84 00541021    ADDU    v0,v0,s4
80064B88 00031BC3    SRA     v1,v1,F
80064B8C A603001C    SH      v1,1C(s0)
80064B90 8603001A    LH      v1,1A(s0)
80064B94 84420000    LH      v0,0(v0)
80064B98 00620018    MULT    v1,v0
80064B9C 00001812    MFLO    v1
80064BA0 00031BC3    SRA     v1,v1,F
80064BA4 A603001E    SH      v1,1E(s0)
80064BA8 8EA40000    LW      a0,0(s5)
80064BAC 108000AB    BEQ     a0,r0,80064E5C
80064BB0 00000000    NOP
80064BB4 8E26000C    LW      a2,C(s1)
80064BB8 8C820008    LW      v0,8(a0)
80064BBC 0040F809    JALR    ra,v0
80064BC0 24050007    ADDIU   a1,r0,7
80064BC4 08019397    J       80064E5C
80064BC8 00000000    NOP
80064BCC 02002021    MOVE    a0,s0
80064BD0 27A50018    ADDIU   a1,sp,18
80064BD4 27A6001A    ADDIU   a2,sp,1A
80064BD8 02603821    MOVE    a3,s3
80064BDC AFB60010    SW      s6,10(sp)
80064BE0 0C0193F5    JAL     80064FD4
80064BE4 AFB20014    SW      s2,14(sp)
80064BE8 8E050030    LW      a1,30(s0)
80064BEC 8E030034    LW      v1,34(s0)
80064BF0 00A3182A    SLT     v1,a1,v1
80064BF4 1460001C    BNE     v1,r0,80064C68
80064BF8 00409021    MOVE    s2,v0
80064BFC 86020018    LH      v0,18(s0)
80064C00 8603001A    LH      v1,1A(s0)
80064C04 00021040    SLL     v0,v0,1
80064C08 00541021    ADDU    v0,v0,s4
80064C0C 84420000    LH      v0,0(v0)
80064C10 00620018    MULT    v1,v0
80064C14 00001812    MFLO    v1
80064C18 86020018    LH      v0,18(s0)
80064C1C 2408007F    ADDIU   t0,r0,7F
80064C20 01021023    SUBU    v0,t0,v0
80064C24 00021040    SLL     v0,v0,1
80064C28 00541021    ADDU    v0,v0,s4
80064C2C 00031BC3    SRA     v1,v1,F
80064C30 A6030028    SH      v1,28(s0)
80064C34 8603001A    LH      v1,1A(s0)
80064C38 84420000    LH      v0,0(v0)
80064C3C 00620018    MULT    v1,v0
80064C40 00001812    MFLO    v1
80064C44 8E020034    LW      v0,34(s0)
80064C48 00031BC3    SRA     v1,v1,F
80064C4C A603002E    SH      v1,2E(s0)
80064C50 96030028    LHU     v1,28(s0)
80064C54 9604002E    LHU     a0,2E(s0)
80064C58 AE020030    SW      v0,30(s0)
80064C5C A603001C    SH      v1,1C(s0)
80064C60 08019330    J       80064CC0
80064C64 A604001E    SH      a0,1E(s0)
80064C68 86060026    LH      a2,26(s0)
80064C6C 8602001C    LH      v0,1C(s0)
80064C70 96070024    LHU     a3,24(s0)
80064C74 44826000    MTC1    v0,$12
80064C78 00000000    NOP
80064C7C 0C01958B    JAL     8006562C
80064C80 46806320    CVT.S.W f12,f12
80064C84 8E050030    LW      a1,30(s0)
80064C88 8606002C    LH      a2,2C(s0)
80064C8C 9607002A    LHU     a3,2A(s0)
80064C90 8603001E    LH      v1,1E(s0)
80064C94 4600008D    TRUNC.W.S       f2,f0
80064C98 44021000    MFC1    v0,$2
80064C9C 44836000    MTC1    v1,$12
80064CA0 00000000    NOP
80064CA4 46806320    CVT.S.W f12,f12
80064CA8 0C01958B    JAL     8006562C
80064CAC A602001C    SH      v0,1C(s0)
80064CB0 4600008D    TRUNC.W.S       f2,f0
80064CB4 44021000    MFC1    v0,$2
80064CB8 00000000    NOP
80064CBC A602001E    SH      v0,1E(s0)
80064CC0 8602001C    LH      v0,1C(s0)
80064CC4 50400001    BEQL    v0,r0,80064CCC
80064CC8 A61E001C    SH      fp,1C(s0)
80064CCC 8602001E    LH      v0,1E(s0)
80064CD0 50400001    BEQL    v0,r0,80064CD8
80064CD4 A61E001E    SH      fp,1E(s0)
80064CD8 8E04003C    LW      a0,3C(s0)
80064CDC 84830008    LH      v1,8(a0)
80064CE0 2402000C    ADDIU   v0,r0,C
80064CE4 14620003    BNE     v1,v0,80064CF4
80064CE8 00000000    NOP
80064CEC 8C82000C    LW      v0,C(a0)
80064CF0 A6020018    SH      v0,18(s0)
80064CF4 8E02003C    LW      v0,3C(s0)
80064CF8 84430008    LH      v1,8(v0)
80064CFC 2402000B    ADDIU   v0,r0,B
80064D00 1462000B    BNE     v1,v0,80064D30
80064D04 00000000    NOP
80064D08 8E02003C    LW      v0,3C(s0)
80064D0C AE000030    SW      r0,30(s0)
80064D10 8C42000C    LW      v0,C(v0)
80064D14 00420018    MULT    v0,v0
80064D18 00001012    MFLO    v0
80064D1C 8E03003C    LW      v1,3C(s0)
80064D20 000213C3    SRA     v0,v0,F
80064D24 A602001A    SH      v0,1A(s0)
80064D28 8C620010    LW      v0,10(v1)
80064D2C AE020034    SW      v0,34(s0)
80064D30 8E04003C    LW      a0,3C(s0)
80064D34 84830008    LH      v1,8(a0)
80064D38 24020010    ADDIU   v0,r0,10
80064D3C 54620047    BNEL    v1,v0,80064E5C
80064D40 AE1E0038    SW      fp,38(s0)
80064D44 8C82000C    LW      v0,C(a0)
80064D48 8E03003C    LW      v1,3C(s0)
80064D4C 00021040    SLL     v0,v0,1
80064D50 00541021    ADDU    v0,v0,s4
80064D54 94420000    LHU     v0,0(v0)
80064D58 A6020020    SH      v0,20(s0)
80064D5C 8C62000C    LW      v0,C(v1)
80064D60 2408007F    ADDIU   t0,r0,7F
80064D64 01021023    SUBU    v0,t0,v0
80064D68 00021040    SLL     v0,v0,1
80064D6C 00541021    ADDU    v0,v0,s4
80064D70 94420000    LHU     v0,0(v0)
80064D74 A6020022    SH      v0,22(s0)
80064D78 08019397    J       80064E5C
80064D7C AE1E0038    SW      fp,38(s0)
80064D80 8E11003C    LW      s1,3C(s0)
80064D84 8622000A    LH      v0,A(s1)
80064D88 10400005    BEQ     v0,r0,80064DA0
80064D8C 02002021    MOVE    a0,s0
80064D90 8E020008    LW      v0,8(s0)
80064D94 24050008    ADDIU   a1,r0,8
80064D98 0040F809    JALR    ra,v0
80064D9C 00003021    MOVE    a2,r0
80064DA0 8E26000C    LW      a2,C(s1)
80064DA4 8E020008    LW      v0,8(s0)
80064DA8 02002021    MOVE    a0,s0
80064DAC 0040F809    JALR    ra,v0
80064DB0 24050005    ADDIU   a1,r0,5
80064DB4 02002021    MOVE    a0,s0
80064DB8 8E020008    LW      v0,8(s0)
80064DBC 0801937C    J       80064DF0
80064DC0 24050009    ADDIU   a1,r0,9
80064DC4 02002021    MOVE    a0,s0
80064DC8 27A50018    ADDIU   a1,sp,18
80064DCC 27A6001A    ADDIU   a2,sp,1A
80064DD0 02603821    MOVE    a3,s3
80064DD4 AFB60010    SW      s6,10(sp)
80064DD8 0C0193F5    JAL     80064FD4
80064DDC AFB20014    SW      s2,14(sp)
80064DE0 00409021    MOVE    s2,v0
80064DE4 02002021    MOVE    a0,s0
80064DE8 8E020008    LW      v0,8(s0)
80064DEC 24050004    ADDIU   a1,r0,4
80064DF0 0040F809    JALR    ra,v0
80064DF4 00003021    MOVE    a2,r0
80064DF8 08019397    J       80064E5C
80064DFC 00000000    NOP
80064E00 8E02003C    LW      v0,3C(s0)
80064E04 3C04800B    LUI     a0,800B
80064E08 8C84AEF0    LW      a0,-5110(a0)
80064E0C 8C43000C    LW      v1,C(v0)
80064E10 AC6000D8    SW      r0,D8(v1)
80064E14 0C01DC43    JAL     8007710C __freePVoice
80064E18 8C45000C    LW      a1,C(v0)
80064E1C 08019397    J       80064E5C
80064E20 00000000    NOP
80064E24 02002021    MOVE    a0,s0
80064E28 27A50018    ADDIU   a1,sp,18
80064E2C 27A6001A    ADDIU   a2,sp,1A
80064E30 02603821    MOVE    a3,s3
80064E34 AFB60010    SW      s6,10(sp)
80064E38 0C0193F5    JAL     80064FD4
80064E3C AFB20014    SW      s2,14(sp)
80064E40 8E03003C    LW      v1,3C(s0)
80064E44 84650008    LH      a1,8(v1)
80064E48 8C66000C    LW      a2,C(v1)
80064E4C 8E030008    LW      v1,8(s0)
80064E50 00409021    MOVE    s2,v0
80064E54 0060F809    JALR    ra,v1
80064E58 02002021    MOVE    a0,s0
80064E5C 8E05003C    LW      a1,3C(s0)
80064E60 02F3B823    SUBU    s7,s7,s3
80064E64 97A2001A    LHU     v0,1A(sp)
80064E68 8CA40000    LW      a0,0(a1)
80064E6C 00131840    SLL     v1,s3,1
80064E70 00431021    ADDU    v0,v0,v1
80064E74 A7A2001A    SH      v0,1A(sp)
80064E78 14800002    BNE     a0,r0,80064E84
80064E7C AE04003C    SW      a0,3C(s0)
80064E80 AE000040    SW      r0,40(s0)
80064E84 0C01DC16    JAL     80077058 ___freeParam
80064E88 00A02021    MOVE    a0,a1
80064E8C 8E02003C    LW      v0,3C(s0)
80064E90 1440FEF2    BNE     v0,r0,80064A5C
80064E94 00000000    NOP
80064E98 02002021    MOVE    a0,s0
80064E9C 27A50018    ADDIU   a1,sp,18
80064EA0 27A6001A    ADDIU   a2,sp,1A
80064EA4 02E03821    MOVE    a3,s7
80064EA8 AFB60010    SW      s6,10(sp)
80064EAC 0C0193F5    JAL     80064FD4
80064EB0 AFB20014    SW      s2,14(sp)
80064EB4 8E030030    LW      v1,30(s0)
80064EB8 8E040034    LW      a0,34(s0)
80064EBC 0083182A    SLT     v1,a0,v1
80064EC0 10600002    BEQ     v1,r0,80064ECC
80064EC4 00409021    MOVE    s2,v0
80064EC8 AE040030    SW      a0,30(s0)
80064ECC 02401021    MOVE    v0,s2
80064ED0 8FBF004C    LW      ra,4C(sp)
80064ED4 8FBE0048    LW      fp,48(sp)
80064ED8 8FB70044    LW      s7,44(sp)
80064EDC 8FB60040    LW      s6,40(sp)
80064EE0 8FB5003C    LW      s5,3C(sp)
80064EE4 8FB40038    LW      s4,38(sp)
80064EE8 8FB30034    LW      s3,34(sp)
80064EEC 8FB20030    LW      s2,30(sp)
80064EF0 8FB1002C    LW      s1,2C(sp)
80064EF4 8FB00028    LW      s0,28(sp)
80064EF8 03E00008    JR      ra
80064EFC 27BD0050    ADDIU   sp,sp,50











_blEnvmixerPull:
00000000 27BDFFB0    ADDIU   sp,sp,-50

0000010C 92220012    LBU     v0,12(s1)
0000010C 24020040    ADDIU   v0,r0,40

000002EC 8C82000C    LW      v0,C(a0)
000002EC 24020040    ADDIU   v0,r0,40
