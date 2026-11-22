.section .rodata
.global ep_font_data
ep_font_data:
.incbin "assets/font.bin"
.global sample_hello
sample_hello:
.incbin "examples/Hello_World.py.tns"
.byte 0
.global sample_dice
sample_dice:
.incbin "examples/Dice_Roller.py.tns"
.byte 0
.global sample_functions
sample_functions:
.incbin "examples/Functions.py.tns"
.byte 0
.section .note.GNU-stack,""
