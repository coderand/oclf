;; This file is distributed under a MIT license. See LICENSE.txt for details.

;; Reverse engineered version of PKWARE Data Compression Library.
;; for Netwide Assembler
;;
;; PKWARE Data Compression Library (R)
;; Copyright 1989-1996 PKWARE Inc.  All Rights Reserved
;; Patent No. 5,051,745
;; PKWARE Data Compression Library Reg. U.S. Pat. and Tm. Off.
;; Version 1.11
;;
;; Source:
;; PKZIP v2.50 by PKWARE
;;
;; Reverse engineering by Dmitry "And" Andreev. 24.11.2002.
;; web : https://www.and.ntercon.ru
;;

                global  _dcl_explode

section code dword public use32

_dcl_explode :

                mov     eax, [esp+ 4]
                push    ebx
                mov     ecx, [esp+4+ 8]
                push    esi
                mov     esi, [esp+8+ 0Ch]
                push    edi
                mov     edx, [esp+0Ch+ 10h]
                lea     edi, [esi+1Ch]
                lea     ebx, [esi+2234h]
                mov     [esi+28h], eax
                mov     [esi+2Ch], ecx
                mov     [esi+24h], edx
                mov     dword [edi], 800h
                mov     edx, [esi+24h]
                push    edx
                push    edi
                push    ebx
                call    dword [esi+28h]
                add     esp, 0Ch
                mov     [esi+20h], eax
                cmp     eax, 4
                ja      short loc_403FC8
                mov     eax, 3
                pop     edi
                pop     esi
                pop     ebx
                retn

loc_403FC8 :
                xor     eax, eax
                xor     ecx, ecx
                mov     al, [ebx]
                xor     edx, edx
                mov     [esi+4], eax
                mov     cl, [esi+2235h]
                mov     [esi+0Ch], ecx
                mov     dl, [esi+2236h]
                mov     [esi+14h], edx
                mov     dword [esi+18h], 0
                mov     dword [edi], 3
                mov     edx, [esi+0Ch]
                cmp     edx, 4
                jb      loc_4040EC
                cmp     edx, 6
                ja      loc_4040EC
                mov     eax, 0FFFFh
                mov     cl, 10h
                sub     cl, dl
                sar     eax, cl
                mov     [esi+10h], eax
                mov     eax, [esi+4]
                test    eax, eax
                jz      loc_40404C
                cmp     eax, 1
                jz      loc_40402A
                mov     eax, 2
                pop     edi
                pop     esi
                pop     ebx
                retn

loc_4040EC :
                mov     eax, 1
                pop     edi
                pop     esi
                pop     ebx
                retn

loc_40402A :
                push    dword 100h
                lea     eax, [esi+2FB4h]
                push    dword unk_40E830
                push    eax
                call    sub_404600
                add     esp, 0Ch
                push    esi
                call    sub_404500
                add     esp, 4

loc_40404C :
                lea     edi, [esi+30F4h]
                push    dword 10h
                push    dword unk_40E810
                push    edi
                call    sub_404600
                add     esp, 0Ch
                lea     eax, [esi+2B34h]
                push    eax
                push    dword unk_40E820
                push    edi
                push    dword 10h
                call    sub_4044C0
                add     esp, 10h
                lea     eax, [esi+3104h]
                push    dword 10h
                push    dword unk_40E7E0
                push    eax
                call    sub_404600
                add     esp, 0Ch
                lea     eax, [esi+3114h]
                lea     edi, [esi+30B4h]
                push    dword 20h
                push    dword unk_40E7F0
                push    eax
                call    sub_404600
                add     esp, 0Ch
                push    dword 40h
                push    dword unk_40E760
                push    edi
                call    sub_404600
                add     esp, 0Ch
                lea     eax, [esi+2A34h]
                push    eax
                push    dword unk_40E7A0
                push    edi
                push    dword 40h
                call    sub_4044C0
                add     esp, 10h
                push    esi
                call    sub_404100
                add     esp, 4
                sub     eax, 306h
                cmp     eax, 1
                pop     edi
                sbb     eax, eax
                pop     esi
                and     eax, 4
                pop     ebx
                retn

sub_404100 :

                sub     esp, 8
                push    ebx
                push    esi
                mov     esi, [esp+10h +4]
                push    edi
                push    ebp
                mov     edi, 305h
                mov     dword [esi+8], 1000h
                mov     ebx, 100h

loc_40411C:                             ; CODE XREF: sub_404100+82
                                        ; sub_404100+BF
                push    esi
                call    sub_404200
                mov     [esp+1Ch -8], eax
                add     esp, 4
                cmp     eax, edi
                jnb     loc_4041CC_long
                jmp     loc_4041CC_next
loc_4041CC_long : jmp loc_4041CC
loc_4041CC_next :
                cmp     eax, ebx
                jnb     short loc_404141
                mov     ecx, [esi+8]
                mov     [ecx+esi+30h], al
                inc     dword [esi+8]
                jmp     short loc_40417B
; ---------------------------------------------------------------------------

loc_404141:                             ; CODE XREF: sub_404100+33
                sub     dword [esp+18h -8], 0FEh
                mov     eax, [esp+18h -8]
                push    eax
                push    esi
                call    sub_4043B0
                add     esp, 8
                test    eax, eax
                jz      short loc_4041C4
                mov     ebp, [esi+8]
                lea     edx, [ebp+esi+30h]
                mov     ecx, edx
                sub     ecx, eax
                mov     eax, [esp+18h -8]
                add     eax, ebp
                mov     [esi+8], eax

loc_40416F:                             ; CODE XREF: sub_404100+79
                mov     al, [ecx]
                inc     ecx
                mov     [edx], al
                inc     edx
                dec     dword [esp+18h -8]
                jnz     short loc_40416F

loc_40417B:                             ; CODE XREF: sub_404100+3F
                cmp     dword [esi+8], 2000h
                jb      short loc_40411C
                lea     ebp, [esi+1030h]
                mov     eax, [esi+24h]
                lea     ecx, [esp+18h -8]
                push    eax
                mov     dword [esp+1Ch -8], 1000h
                push    ecx
                push    ebp
                call    dword [esi+2Ch]
                add     esp, 0Ch
                mov     eax, [esi+8]
                sub     eax, 1000h
                lea     ecx, [esi+30h]
                push    eax
                push    ebp
                push    ecx
                call    sub_404600
                sub     dword [esi+8], 1000h
                add     esp, 0Ch
                jmp     loc_40411C
; ---------------------------------------------------------------------------

loc_4041C4:                             ; CODE XREF: sub_404100+59
                mov     dword [esp+18h -8], 306h

loc_4041CC:                             ; CODE XREF: sub_404100+2B
                mov     eax, [esi+8]
                mov     ecx, [esi+24h]
                sub     eax, 1000h
                push    ecx
                lea     edx, [esi+1030h]
                mov     [esp+1Ch -4], eax
                lea     eax, [esp+1Ch -4]
                push    eax
                push    edx
                call    dword [esi+2Ch]
                mov     eax, [esp+24h -8]
                add     esp, 0Ch
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                add     esp, 8
                retn

sub_404200 :

                push    ebx
                push    esi
                mov     esi, [esp+ 0Ch]
                push    edi
                push    dword 1
                push    esi
                test    byte [esi+14h], 1
                jz      loc_4042AF_long
                jmp     loc_4042AF_next
loc_4042AF_long : jmp loc_4042AF
loc_4042AF_next :
                call    sub_404430
                add     esp, 8
                test    eax, eax
                jz      short loc_404229
                mov     eax, 306h
                pop     edi
                pop     esi
                pop     ebx
                retn
; ---------------------------------------------------------------------------

loc_404229:                             ; CODE XREF: sub_404200+1E
                mov     eax, [esi+14h]
                and     eax, 0FFh
                movzx   edi, byte [eax+esi+2B34h]
                xor     eax, eax
                lea     ebx, [esi+edi]
                mov     al, [esi+edi+30F4h]
                push    eax
                push    esi
                call    sub_404430
                add     esp, 8
                test    eax, eax
                jz      short loc_40425C
                mov     eax, 306h
                pop     edi
                pop     esi
                pop     ebx
                retn
; ---------------------------------------------------------------------------

loc_40425C:                             ; CODE XREF: sub_404200+51
                mov     cl, [ebx+3104h]
                test    cl, cl
                jz      short loc_4042A3
                mov     ebx, 1
                xor     eax, eax
                shl     ebx, cl
                dec     ebx
                mov     al, cl
                and     ebx, [esi+14h]
                push    eax
                push    esi
                call    sub_404430
                add     esp, 8
                test    eax, eax
                jz      short loc_404296
                lea     eax, [edi+ebx]
                cmp     eax, 10Eh
                jz      short loc_404296
                mov     eax, 306h
                pop     edi
                pop     esi
                pop     ebx
                retn
; ---------------------------------------------------------------------------

loc_404296:                             ; CODE XREF: sub_404200+81
                                        ; sub_404200+8B
                xor     eax, eax
                mov     ax, [esi+edi*2+3114h]
                lea     edi, [eax+ebx]

loc_4042A3:                             ; CODE XREF: sub_404200+64
                add     edi, 100h

loc_4042A9:                             ; CODE XREF: sub_404200+E0
                                        ; sub_404200+19B
                mov     eax, edi
                pop     edi
                pop     esi
                pop     ebx
                retn
; ---------------------------------------------------------------------------

loc_4042AF:                             ; CODE XREF: sub_404200+E
                call    sub_404430
                add     esp, 8
                test    eax, eax
                jz      short loc_4042C4
                mov     eax, 306h
                pop     edi
                pop     esi
                pop     ebx
                retn
; ---------------------------------------------------------------------------

loc_4042C4:                             ; CODE XREF: sub_404200+B9
                cmp     dword [esi+4], 0
                jnz     short loc_4042EB
                mov     edi, [esi+14h]
                push    dword 8
                and     edi, 0FFh
                push    esi
                call    sub_404430
                add     esp, 8
                test    eax, eax
                jz      short loc_4042A9
                mov     eax, 306h
                pop     edi
                pop     esi
                pop     ebx
                retn
; ---------------------------------------------------------------------------

loc_4042EB:                             ; CODE XREF: sub_404200+C8
                mov     ecx, [esi+14h]
                mov     eax, ecx
                and     eax, 0FFh
                jz      short loc_40435E
                movzx   edi, byte [esi+eax+2C34h]
                cmp     edi, 0FFh
                jnz     short loc_404386_long
                jmp     loc_404386_next
loc_404386_long : jmp loc_404386
loc_404386_next :
                test    cl, 3Fh
                jz      short loc_404336
                push    dword 4
                push    esi
                call    sub_404430
                add     esp, 8
                test    eax, eax
                jz      short loc_404324
                mov     eax, 306h
                pop     edi
                pop     esi
                pop     ebx
                retn
; ---------------------------------------------------------------------------

loc_404324:                             ; CODE XREF: sub_404200+119
                mov     eax, [esi+14h]
                and     eax, 0FFh
                movzx   edi, byte [eax+esi+2D34h]
                jmp     short loc_404386
; ---------------------------------------------------------------------------

loc_404336:                             ; CODE XREF: sub_404200+10A
                push    dword 6
                push    esi
                call    sub_404430
                add     esp, 8
                test    eax, eax
                jz      short loc_40434E
                mov     eax, 306h
                pop     edi
                pop     esi
                pop     ebx
                retn
; ---------------------------------------------------------------------------

loc_40434E:                             ; CODE XREF: sub_404200+143
                mov     eax, [esi+14h]
                and     eax, 7Fh
                movzx   edi, byte [eax+esi+2E34h]
                jmp     short loc_404386
; ---------------------------------------------------------------------------

loc_40435E:                             ; CODE XREF: sub_404200+F5
                push    dword 8
                push    esi
                call    sub_404430
                add     esp, 8
                test    eax, eax
                jz      short loc_404376
                mov     eax, 306h
                pop     edi
                pop     esi
                pop     ebx
                retn

loc_404376:                             ; CODE XREF: sub_404200+16B
                mov     eax, [esi+14h]
                and     eax, 0FFh
                movzx   edi, byte [eax+esi+2EB4h]

loc_404386:                             ; CODE XREF: sub_404200+105
                                        ; sub_404200+134 ...
                xor     eax, eax
                mov     al, [esi+edi+2FB4h]
                push    eax
                push    esi
                call    sub_404430
                add     esp, 8
                test    eax, eax
                jz      loc_4042A9_long
                jmp     loc_4042A9_next
loc_4042A9_long : jmp   loc_4042A9
loc_4042A9_next :
                mov     eax, 306h
                pop     edi
                pop     esi
                pop     ebx
                retn

sub_4043B0 :

                push    esi
                xor     ecx, ecx
                mov     esi, [esp+ 8]
                push    edi
                mov     eax, [esi+14h]
                and     eax, 0FFh
                movzx   edi, byte [eax+esi+2A34h]
                mov     cl, [esi+edi+30B4h]
                push    ecx
                push    esi
                call    sub_404430
                add     esp, 8
                test    eax, eax
                jz      short loc_4043E2
                xor     eax, eax
                pop     edi
                pop     esi
                retn

; ---------------------------------------------------------------------------

loc_4043E2:                             ; CODE XREF: sub_4043B0+2B
                cmp     dword [esp+4+ 0Ch], 2
                jnz     short loc_404408
                shl     edi, 2
                mov     eax, [esi+14h]
                and     eax, 3
                push    dword 2
                or      edi, eax
                push    esi
                call    sub_404430
                add     esp, 8
                test    eax, eax
                jz      short loc_404428
                xor     eax, eax
                pop     edi
                pop     esi
                retn
; ---------------------------------------------------------------------------

loc_404408:                             ; CODE XREF: sub_4043B0+37
                mov     ecx, [esi+0Ch]
                mov     eax, [esi+10h]
                and     eax, [esi+14h]
                push    ecx
                shl     edi, cl
                or      edi, eax
                push    esi
                call    sub_404430
                add     esp, 8
                test    eax, eax
                jz      short loc_404428
                xor     eax, eax
                pop     edi
                pop     esi
                retn
; ---------------------------------------------------------------------------

loc_404428:                             ; CODE XREF: sub_4043B0+51
                                        ; sub_4043B0+71
                lea     eax, [edi+1]
                pop     edi
                pop     esi
                retn

sub_404430 :

                push    ebx
                push    esi
                mov     esi, [esp+ 0Ch]
                push    edi
                mov     ebx, [esp+4+ 10h]
                mov     eax, [esi+18h]
                cmp     eax, ebx
                jb      short loc_404452
                mov     cl, bl
                sub     eax, ebx
                pop     edi
                mov     [esi+18h], eax
                shr     dword [esi+14h], cl
                xor     eax, eax
                pop     esi
                pop     ebx
                retn
; ---------------------------------------------------------------------------

loc_404452:                             ; CODE XREF: sub_404430+10
                mov     cl, al
                lea     edi, [esi+1Ch]
                shr     dword [esi+14h], cl
                mov     eax, [esi+20h]
                cmp     [edi], eax
                jnz     short loc_40448F
                mov     eax, [esi+24h]
                lea     ecx, [esi+2234h]
                push    eax
                mov     dword [edi], 800h
                push    edi
                push    ecx
                call    dword [esi+28h]
                add     esp, 0Ch
                mov     [esi+20h], eax
                test    eax, eax
                jnz     short loc_404489
                mov     eax, 1
                pop     edi
                pop     esi
                pop     ebx
                retn
; ---------------------------------------------------------------------------

loc_404489:                             ; CODE XREF: sub_404430+4E
                mov     dword [edi], 0

loc_40448F:                             ; CODE XREF: sub_404430+2F
                mov     eax, [edi]
                xor     edx, edx
                mov     dl, [esi+eax+2234h]
                inc     eax
                mov     ecx, ebx
                shl     edx, 8
                mov     [edi], eax
                or      edx, [esi+14h]
                mov     eax, [esi+18h]
                sub     cl, al
                mov     [esi+14h], edx
                sub     eax, ebx
                pop     edi
                shr     edx, cl
                add     eax, 8
                mov     [esi+14h], edx
                mov     [esi+18h], eax
                xor     eax, eax
                pop     esi
                pop     ebx
                retn

sub_4044C0 :

                push    ebx
                push    esi
                mov     ebx, [esp+ 0Ch]
                push    edi
                push    ebp
                dec     ebx
                js      short loc_4044F5
                mov     edx, [esp+8+ 10h]
                mov     esi, [esp+8+ 14h]
                mov     edi, [esp+8+ 18h]

loc_4044D7:                             ; CODE XREF: sub_4044C0+33
                mov     ebp, 1
                mov     cl, [edx+ebx]
                shl     ebp, cl
                xor     eax, eax
                mov     al, [esi+ebx]

loc_4044E6:                             ; CODE XREF: sub_4044C0+30
                mov     [edi+eax], bl
                add     eax, ebp
                cmp     eax, 100h
                jb      short loc_4044E6
                dec     ebx
                jns     short loc_4044D7

loc_4044F5:                             ; CODE XREF: sub_4044C0+9
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                retn

sub_404500 :

                push    ebx
                push    esi
                push    edi
                mov     ebx, 0FFh
                push    ebp
                mov     edx, ebx
                mov     esi, unk_40EB2E
                mov     edi, [esp+4+ 10h]

loc_404514:                             ; CODE XREF: sub_404500+E9
                lea     ebp, [edi+edx+2FB4h]
                mov     cl, [ebp+0]
                cmp     cl, 8
                ja      short loc_404544
                mov     ebp, 1
                xor     eax, eax
                shl     ebp, cl
                mov     ax, [esi]

loc_40452F:                             ; CODE XREF: sub_404500+3D
                mov     [edi+eax+2C34h], dl
                add     eax, ebp
                cmp     eax, 100h
                jb      short loc_40452F
                jmp     loc_4045DF
; ---------------------------------------------------------------------------

loc_404544:                             ; CODE XREF: sub_404500+21
                mov     ax, [esi]
                and     ax, bx
                jz      short loc_4045B6
                and     eax, 0FFFFh
                mov     [eax+edi+2C34h], bl
                test    byte [esi],     3Fh
                mov     cl, [ebp+0]
                jz      short loc_40458B
                sub     cl, 4
                mov     [ebp+0], cl
                mov     ebp, 1
                shl     ebp, cl
                mov     ax, [esi]
                shr     ax, 4
                and     eax, 0FFFFh

loc_404579:                             ; CODE XREF: sub_404500+87
                mov     [edi+eax+2D34h], dl
                add     eax, ebp
                cmp     eax, 100h
                jb      short loc_404579
                jmp     short loc_4045DF
; ---------------------------------------------------------------------------

loc_40458B:                             ; CODE XREF: sub_404500+5E
                sub     cl, 6
                mov     [ebp+0], cl
                mov     ebp, 1
                shl     ebp, cl
                mov     ax, [esi]
                shr     ax, 6
                and     eax, 0FFFFh

loc_4045A4:                             ; CODE XREF: sub_404500+B2
                mov     [edi+eax+2E34h], dl
                add     eax, ebp
                cmp     eax, 80h
                jb      short loc_4045A4
                jmp     short loc_4045DF
; ---------------------------------------------------------------------------

loc_4045B6:                             ; CODE XREF: sub_404500+4A
                sub     cl, 8
                mov     [ebp+0], cl
                mov     ebp, 1
                shl     ebp, cl
                mov     ax, [esi]
                shr     ax, 8
                and     eax, 0FFFFh

loc_4045CF:                             ; CODE XREF: sub_404500+DD
                mov     [edi+eax+2EB4h], dl
                add     eax, ebp
                cmp     eax, 100h
                jb      short loc_4045CF

loc_4045DF:                             ; CODE XREF: sub_404500+3F
                                        ; sub_404500+89 ...
                sub     esi, 2
                dec     edx
                cmp     esi, unk_40E930
                jnb     loc_404514_long
                jmp     loc_404514_next
loc_404514_long : jmp   loc_404514
loc_404514_next :
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                retn

sub_404600 :
                mov     ecx, [esp+ 8]
                push    ebx
                mov     edx, [esp+4+ 4]
                push    esi
                mov     eax, [esp+8+ 0Ch]
                push    edi
                mov     esi, eax
                shr     esi, 2
                test    esi, esi
                jz      short loc_404625

loc_404618:                             ; CODE XREF: sub_404600+23
                mov     edi, [ecx]
                add     ecx, 4
                mov     [edx], edi
                add     edx, 4
                dec     esi
                jnz     short loc_404618

loc_404625:                             ; CODE XREF: sub_404600+16
                and     eax, 3
                jz      short loc_404633

loc_40462A:                             ; CODE XREF: sub_404600+31
                mov     bl, [ecx]
                inc     ecx
                mov     [edx], bl
                inc     edx
                dec     eax
                jnz     short loc_40462A

loc_404633:                             ; CODE XREF: sub_404600+28
                pop     edi
                pop     esi
                pop     ebx
                retn

section data dword public use32

unk_40E760 :    db    2                 ; DATA XREF: sub_403F80+12D
                db    4
                db    4
                db    5
                db    5
                db    5
                db    5
                db    6
                db    6
                db    6
                db    6
                db    6
                db    6
                db    6
                db    6
                db    6
                db    6
                db    6
                db    6
                db    6
                db    6
                db    6
                db    7
                db    7
                db    7
                db    7
                db    7
                db    7
                db    7
                db    7
                db    7
                db    7
                db    7
                db    7
                db    7
                db    7
                db    7
                db    7
                db    7
                db    7
                db    7
                db    7
                db    7
                db    7
                db    7
                db    7
                db    7
                db    7
                db    8
                db    8
                db    8
                db    8
                db    8
                db    8
                db    8
                db    8
                db    8
                db    8
                db    8
                db    8
                db    8
                db    8
                db    8
                db    8
unk_40E7A0 :    db    3                 ; DATA XREF: sub_403F80+142
                db  0Dh
                db    5
                db  19h
                db    9
                db  11h
                db    1
                db  3Eh
                db  1Eh
                db  2Eh
                db  0Eh
                db  36h
                db  16h
                db  26h
                db    6
                db  3Ah
                db  1Ah
                db  2Ah
                db  0Ah
                db  32h
                db  12h
                db  22h
                db  42h
                db    2
                db  7Ch
                db  3Ch
                db  5Ch
                db  1Ch
                db  6Ch
                db  2Ch
                db  4Ch
                db  0Ch
                db  74h
                db  34h
                db  54h
                db  14h
                db  64h
                db  24h
                db  44h
                db    4
                db  78h
                db  38h
                db  58h
                db  18h
                db  68h
                db  28h
                db  48h
                db    8
                db 0F0h
                db  70h
                db 0B0h
                db  30h
                db 0D0h
                db  50h
                db  90h
                db  10h
                db 0E0h
                db  60h
                db 0A0h
                db  20h
                db 0C0h
                db  40h
                db  80h
                db    0
unk_40E7E0 :    db    0                 ; DATA XREF: sub_403F80+101
                db    0
                db    0
                db    0
                db    0
                db    0
                db    0
                db    0
                db    1
                db    2
                db    3
                db    4
                db    5
                db    6
                db    7
                db    8
unk_40E7F0 :    db    0                 ; DATA XREF: sub_403F80+11D
                db    0
                db    1
                db    0
                db    2
                db    0
                db    3
                db    0
                db    4
                db    0
                db    5
                db    0
                db    6
                db    0
                db    7
                db    0
                db    8
                db    0
                db  0Ah
                db    0
                db  0Eh
                db    0
                db  16h
                db    0
                db  26h
                db    0
                db  46h
                db    0
                db  86h
                db    0
                db    6
                db    1
unk_40E810 :    db    3                 ; DATA XREF: sub_403F80+D4
                db    2
                db    3
                db    3
                db    4
                db    4
                db    4
                db    5
                db    5
                db    5
                db    5
                db    6
                db    6
                db    6
                db    7
                db    7
unk_40E820 :    db    5                 ; DATA XREF: sub_403F80+E9
                db    3
                db    1
                db    6
                db  0Ah
                db    2
                db  0Ch
                db  14h
                db    4
                db  18h
                db    8
                db  30h
                db  10h
                db  20h
                db  40h
                db    0
unk_40E830 :    db  0Bh                 ; DATA XREF: sub_403F80+B5
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db    8
                db    7
                db  0Ch
                db  0Ch
                db    7
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Dh
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db    4
                db  0Ah
                db    8
                db  0Ch
                db  0Ah
                db  0Ch
                db  0Ah
                db    8
                db    7
                db    7
                db    8
                db    9
                db    7
                db    6
                db    7
                db    8
                db    7
                db    6
                db    7
                db    7
                db    7
                db    7
                db    8
                db    7
                db    7
                db    8
                db    8
                db  0Ch
                db  0Bh
                db    7
                db    9
                db  0Bh
                db  0Ch
                db    6
                db    7
                db    6
                db    6
                db    5
                db    7
                db    8
                db    8
                db    6
                db  0Bh
                db    9
                db    6
                db    7
                db    6
                db    6
                db    7
                db  0Bh
                db    6
                db    6
                db    6
                db    7
                db    9
                db    8
                db    9
                db    9
                db  0Bh
                db    8
                db  0Bh
                db    9
                db  0Ch
                db    8
                db  0Ch
                db    5
                db    6
                db    6
                db    6
                db    5
                db    6
                db    6
                db    6
                db    5
                db  0Bh
                db    7
                db    5
                db    6
                db    5
                db    5
                db    6
                db  0Ah
                db    5
                db    5
                db    5
                db    5
                db    8
                db    7
                db    8
                db    8
                db  0Ah
                db  0Bh
                db  0Bh
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Dh
                db  0Ch
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Ch
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Ch
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Ch
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Ch
                db  0Ch
                db  0Ch
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
                db  0Dh
unk_40E930 :    db  90h                 ; DATA XREF: sub_404500+E3
                db    4
                db 0E0h
                db  0Fh
                db 0E0h
                db    7
                db 0E0h
                db  0Bh
                db 0E0h
                db    3
                db 0E0h
                db  0Dh
                db 0E0h
                db    5
                db 0E0h
                db    9
                db 0E0h
                db    1
                db 0B8h
                db    0
                db  62h
                db    0
                db 0E0h
                db  0Eh
                db 0E0h
                db    6
                db  22h
                db    0
                db 0E0h
                db  0Ah
                db 0E0h
                db    2
                db 0E0h
                db  0Ch
                db 0E0h
                db    4
                db 0E0h
                db    8
                db 0E0h
                db    0
                db  60h
                db  0Fh
                db  60h
                db    7
                db  60h
                db  0Bh
                db  60h
                db    3
                db  60h
                db  0Dh
                db  60h
                db    5
                db  40h
                db  12h
                db  60h
                db    9
                db  60h
                db    1
                db  60h
                db  0Eh
                db  60h
                db    6
                db  60h
                db  0Ah
                db  0Fh
                db    0
                db  50h
                db    2
                db  38h
                db    0
                db  60h
                db    2
                db  50h
                db    0
                db  60h
                db  0Ch
                db  90h
                db    3
                db 0D8h
                db    0
                db  42h
                db    0
                db    2
                db    0
                db  58h
                db    0
                db 0B0h
                db    1
                db  7Ch
                db    0
                db  29h
                db    0
                db  3Ch
                db    0
                db  98h
                db    0
                db  5Ch
                db    0
                db    9
                db    0
                db  1Ch
                db    0
                db  6Ch
                db    0
                db  2Ch
                db    0
                db  4Ch
                db    0
                db  18h
                db    0
                db  0Ch
                db    0
                db  74h
                db    0
                db 0E8h
                db    0
                db  68h
                db    0
                db  60h
                db    4
                db  90h
                db    0
                db  34h
                db    0
                db 0B0h
                db    0
                db  10h
                db    7
                db  60h
                db    8
                db  31h
                db    0
                db  54h
                db    0
                db  11h
                db    0
                db  21h
                db    0
                db  17h
                db    0
                db  14h
                db    0
                db 0A8h
                db    0
                db  28h
                db    0
                db    1
                db    0
                db  10h
                db    3
                db  30h
                db    1
                db  3Eh
                db    0
                db  64h
                db    0
                db  1Eh
                db    0
                db  2Eh
                db    0
                db  24h
                db    0
                db  10h
                db    5
                db  0Eh
                db    0
                db  36h
                db    0
                db  16h
                db    0
                db  44h
                db    0
                db  30h
                db    0
                db 0C8h
                db    0
                db 0D0h
                db    1
                db 0D0h
                db    0
                db  10h
                db    1
                db  48h
                db    0
                db  10h
                db    6
                db  50h
                db    1
                db  60h
                db    0
                db  88h
                db    0
                db 0A0h
                db  0Fh
                db    7
                db    0
                db  26h
                db    0
                db    6
                db    0
                db  3Ah
                db    0
                db  1Bh
                db    0
                db  1Ah
                db    0
                db  2Ah
                db    0
                db  0Ah
                db    0
                db  0Bh
                db    0
                db  10h
                db    2
                db    4
                db    0
                db  13h
                db    0
                db  32h
                db    0
                db    3
                db    0
                db  1Dh
                db    0
                db  12h
                db    0
                db  90h
                db    1
                db  0Dh
                db    0
                db  15h
                db    0
                db    5
                db    0
                db  19h
                db    0
                db    8
                db    0
                db  78h
                db    0
                db 0F0h
                db    0
                db  70h
                db    0
                db  90h
                db    2
                db  10h
                db    4
                db  10h
                db    0
                db 0A0h
                db    7
                db 0A0h
                db  0Bh
                db 0A0h
                db    3
                db  40h
                db    2
                db  40h
                db  1Ch
                db  40h
                db  0Ch
                db  40h
                db  14h
                db  40h
                db    4
                db  40h
                db  18h
                db  40h
                db    8
                db  40h
                db  10h
                db  40h
                db    0
                db  80h
                db  1Fh
                db  80h
                db  0Fh
                db  80h
                db  17h
                db  80h
                db    7
                db  80h
                db  1Bh
                db  80h
                db  0Bh
                db  80h
                db  13h
                db  80h
                db    3
                db  80h
                db  1Dh
                db  80h
                db  0Dh
                db  80h
                db  15h
                db  80h
                db    5
                db  80h
                db  19h
                db  80h
                db    9
                db  80h
                db  11h
                db  80h
                db    1
                db  80h
                db  1Eh
                db  80h
                db  0Eh
                db  80h
                db  16h
                db  80h
                db    6
                db  80h
                db  1Ah
                db  80h
                db  0Ah
                db  80h
                db  12h
                db  80h
                db    2
                db  80h
                db  1Ch
                db  80h
                db  0Ch
                db  80h
                db  14h
                db  80h
                db    4
                db  80h
                db  18h
                db  80h
                db    8
                db  80h
                db  10h
                db  80h
                db    0
                db    0
                db  1Fh
                db    0
                db  0Fh
                db    0
                db  17h
                db    0
                db    7
                db    0
                db  1Bh
                db    0
                db  0Bh
                db    0
                db  13h
                db 0A0h
                db  0Dh
                db 0A0h
                db    5
                db 0A0h
                db    9
                db 0A0h
                db    1
                db 0A0h
                db  0Eh
                db 0A0h
                db    6
                db 0A0h
                db  0Ah
                db 0A0h
                db    2
                db 0A0h
                db  0Ch
                db 0A0h
                db    4
                db 0A0h
                db    8
                db 0A0h
                db    0
                db  20h
                db  0Fh
                db  20h
                db    7
                db  20h
                db  0Bh
                db  20h
                db    3
                db  20h
                db  0Dh
                db  20h
                db    5
                db  20h
                db    9
                db  20h
                db    1
                db  20h
                db  0Eh
                db  20h
                db    6
                db  20h
                db  0Ah
                db  20h
                db    2
                db  20h
                db  0Ch
                db  20h
                db    4
                db  20h
                db    8
                db  20h
                db    0
                db 0C0h
                db  0Fh
                db 0C0h
                db    7
                db 0C0h
                db  0Bh
                db 0C0h
                db    3
                db 0C0h
                db  0Dh
                db 0C0h
                db    5
                db 0C0h
                db    9
                db 0C0h
                db    1
                db 0C0h
                db  0Eh
                db 0C0h
                db    6
                db 0C0h
                db  0Ah
                db 0C0h
                db    2
                db 0C0h
                db  0Ch
                db 0C0h
                db    4
                db 0C0h
                db    8
                db 0C0h
                db    0
                db  40h
                db  0Fh
                db  40h
                db    7
                db  40h
                db  0Bh
                db  40h
                db    3
                db    0
                db    3
                db  40h
                db  0Dh
                db    0
                db  1Dh
                db    0
                db  0Dh
                db    0
                db  15h
                db  40h
                db    5
                db    0
                db    5
                db    0
                db  19h
                db    0
                db    9
                db  40h
                db    9
                db    0
                db  11h
                db    0
                db    1
                db    0
                db  1Eh
                db    0
                db  0Eh
                db  40h
                db    1
                db    0
                db  16h
                db    0
                db    6
                db    0
                db  1Ah
                db  40h
                db  0Eh
                db  40h
                db    6
                db  40h
                db  0Ah
                db    0
                db  0Ah
                db    0
                db  12h
                db    0
                db    2
                db    0
                db  1Ch
                db    0
                db  0Ch
                db    0
                db  14h
                db    0
                db    4
                db    0
                db  18h
                db    0
                db    8
                db    0
                db  10h
unk_40EB2E :    db    0                 ; DATA XREF: sub_404500+B
                db    0 ;  
                db  50h ; P
                db  4Bh ; K
                db  57h ; W
                db  41h ; A
                db  52h ; R
                db  45h ; E
                db  20h ;  
                db  44h ; D
                db  61h ; a
                db  74h ; t
                db  61h ; a
                db  20h ;  
                db  43h ; C
                db  6Fh ; o
                db  6Dh ; m
                db  70h ; p
                db  72h ; r
                db  65h ; e
                db  73h ; s
                db  73h ; s
                db  69h ; i
                db  6Fh ; o
                db  6Eh ; n
                db  20h ;  
                db  4Ch ; L
                db  69h ; i
                db  62h ; b
                db  72h ; r
                db  61h ; a
                db  72h ; r
                db  79h ; y
                db  20h ;  
                db  66h ; f
                db  6Fh ; o
                db  72h ; r
                db  20h ;  
                db  57h ; W
                db  69h ; i
                db  6Eh ; n
                db  33h ; 3
                db  32h ; 2
                db  0Dh ;  
                db  0Ah ;  
                db  43h ; C
                db  6Fh ; o
                db  70h ; p
                db  79h ; y
                db  72h ; r
                db  69h ; i
                db  67h ; g
                db  68h ; h
                db  74h ; t
                db  20h ;  
                db  31h ; 1
                db  39h ; 9
                db  38h ; 8
                db  39h ; 9
                db  2Dh ; -
                db  31h ; 1
                db  39h ; 9
                db  39h ; 9
                db  35h ; 5
                db  20h ;  
                db  50h ; P
                db  4Bh ; K
                db  57h ; W
                db  41h ; A
                db  52h ; R
                db  45h ; E
                db  20h ;  
                db  49h ; I
                db  6Eh ; n
                db  63h ; c
                db  2Eh ; .
                db  20h ;  
                db  20h ;  
                db  41h ; A
                db  6Ch ; l
                db  6Ch ; l
                db  20h ;  
                db  52h ; R
                db  69h ; i
                db  67h ; g
                db  68h ; h
                db  74h ; t
                db  73h ; s
                db  20h ;  
                db  52h ; R
                db  65h ; e
                db  73h ; s
                db  65h ; e
                db  72h ; r
                db  76h ; v
                db  65h ; e
                db  64h ; d
                db  2Eh ; .
                db  0Dh ;  
                db  0Ah ;  
                db  50h ; P
                db  61h ; a
                db  74h ; t
                db  65h ; e
                db  6Eh ; n
                db  74h ; t
                db  20h ;  
                db  4Eh ; N
                db  6Fh ; o
                db  2Eh ; .
                db  20h ;  
                db  35h ; 5
                db  2Ch ; ,
                db  30h ; 0
                db  35h ; 5
                db  31h ; 1
                db  2Ch ; ,
                db  37h ; 7
                db  34h ; 4
                db  35h ; 5
                db  0Dh ;  
                db  0Ah ;  
                db  50h ; P
                db  4Bh ; K
                db  57h ; W
                db  41h ; A
                db  52h ; R
                db  45h ; E
                db  20h ;  
                db  44h ; D
                db  61h ; a
                db  74h ; t
                db  61h ; a
                db  20h ;  
                db  43h ; C
                db  6Fh ; o
                db  6Dh ; m
                db  70h ; p
                db  72h ; r
                db  65h ; e
                db  73h ; s
                db  73h ; s
                db  69h ; i
                db  6Fh ; o
                db  6Eh ; n
                db  20h ;  
                db  4Ch ; L
                db  69h ; i
                db  62h ; b
                db  72h ; r
                db  61h ; a
                db  72h ; r
                db  79h ; y
                db  20h ;  
                db  52h ; R
                db  65h ; e
                db  67h ; g
                db  2Eh ; .
                db  20h ;  
                db  55h ; U
                db  2Eh ; .
                db  53h ; S
                db  2Eh ; .
                db  20h ;  
                db  50h ; P
                db  61h ; a
                db  74h ; t
                db  2Eh ; .
                db  20h ;  
                db  61h ; a
                db  6Eh ; n
                db  64h ; d
                db  20h ;  
                db  54h ; T
                db  6Dh ; m
                db  2Eh ; .
                db  20h ;  
                db  4Fh ; O
                db  66h ; f
                db  66h ; f
                db  2Eh ; .
                db  0Dh ;  
                db  0Ah ;  
                db  56h ; V
                db  65h ; e
                db  72h ; r
                db  73h ; s
                db  69h ; i
                db  6Fh ; o
                db  6Eh ; n
                db  20h ;  
                db  31h ; 1
                db  2Eh ; .
                db  31h ; 1
                db  31h ; 1
                db    0 ;  
