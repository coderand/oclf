;; This file is distributed under a MIT license. See LICENSE.txt for details.

global _p_tool1
global _p_tool2
global _p_tool3

section .data

_p_tool1 : incbin "oc_unpak.exe"
_p_tool2 : incbin "PKZIP25.EXE"
_p_tool3 : db 0
