;////////////////////////////////////////////////////////////////////////////////
;// THE SCOTCH-WARE LICENSE (Revision 0):
;// <aaronryool/gmail.com> wrote this file. As long as you retain this notice you
;// can do whatever you want with this stuff. If we meet some day, and you think
;// this stuff is worth it, you can buy me a shot of scotch in return
;////////////////////////////////////////////////////////////////////////////////
; This assembles to a floppy disk image
[bits 16]
[org 0x7C00]

; BIOS Parameter Block
jmp short boot_code
nop
db "MadOSv01"       ; OEM identifier
dw 512              ; Bytes per sector
db 1                ; sectors per cluster
dw 1                ; reserved sectors
db 1                ; number of fats
dw 112              ; directory entries
dw 1280             ; logical sectors
db 0xFB             ; media descriptor type
dw 2                ; sectors per fat, fat12/fat16 only
dw 18               ; sectors per track
dw 2                ; number of heads / sides on media
dd 0                ; number of hidden sectors
dd 0                ; Large amount of sector on media. This field is set if there are more than 65535 sectors in the volume.

; Extended Boot Record
db 0                ; drive number
db 0                ; reserved / NT flags (maybe use for MadOS, More research needed)
db 0x29             ; signiture, 0x28 or 0x29 for fat12 /fat16
dd 0x1337           ; volume serial number
db "MadOS Disk "    ; volume label
db "FAT12   "       ; identifier string

; Boot code
boot_code:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, boot_code_end + (1025 * 4)

    lea si, [message]
    call print_string
    jmp $

print_string:
    lodsb
    or al, al
    jz done
    mov ah, 0x0E
    int 0x10
    jmp print_string
done:
   ret

message:
    db "MadOS disk not bootable...", 0xd, 0xa

boot_code_end:
times 448 - (boot_code_end - boot_code) db 0
dw 0xAA55 ; bootable partition signiture

; First fat entry
db 0xFB, 0xFF, 0xFF, 0, 0xF0, 0xFF, 0x0F
times 1017 db 0

; root directory
db "IMAFILE    "
db 0x20                ; attributes: a
db 0                   ; reserved
db 0x64                ; creation time in tenths of seconds
dw 0xA496              ; creation time (h,m,s)
dw 0x4793              ; creation date
dw 0xA496              ; access time (h,m,s)
dw 0x4793              ; access date
dw 0                   ; high order bits of first cluster address, 0 on fat12
dw 0                   ; modified date
dw 0                   ; low order bits of first cluster address
dw 0                   ; file size
times ((1024 * 1024) + 461373) - ($ - $$) db 0



