    .section .rodata
    .balign 4
    .global eh_art_data
eh_art_data:
    .incbin "run038_art.bin"
    .size eh_art_data, .-eh_art_data
    .section .note.GNU-stack,"",%progbits
