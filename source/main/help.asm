section .rodata

global KefirHelpContent

KefirHelpContent:
    incbin 'source/main/help.txt'
    db 0
