;////////////////////////////////////////////////////////////////////////////////
;// THE SCOTCH-WARE LICENSE (Revision 0):
;// <aaronryool/gmail.com> wrote this file. As long as you retain this notice you
;// can do whatever you want with this stuff. If we meet some day, and you think
;// this stuff is worth it, you can buy me a shot of scotch in return
;////////////////////////////////////////////////////////////////////////////////
; This assembles to a floppy disk image
[bits 16]
[org 0x7C00]

;;;;;;;;;;;;;;;;;;;;;;;;
; BIOS Parameter Block
jmp short boot_code
nop
db "MadOSv01"          ; OEM identifier
dw 512                 ; Bytes per sector:  512, 1024, 2048 or 4096.
                       ;    the value 512 should be used for max compat.
db 1                   ; sectors per cluster:  1, 2, 4, 8, 16, 32, 64, and 128.
dw 0                   ; reserved sectors: must not be 0. For FAT12 and FAT16 volumes must be 1
db 2                   ; number of fats: always contain the value 2
dw 0                   ; root directory entries
dw 0                   ; logical sectors
db 0                   ; media descriptor type: 0xF8 -> fixed, 0xF0 -> removable
dw 0                   ; sectors per fat, fat12/fat16 only, number of sectors occupied by one fat
dw 0                   ; sectors per track
dw 0                   ; number of heads / sides on media
dd 0                   ; number of hidden sectors
dd 0                   ; Large amount of sector on media. This field is set if there are more than 65535 sectors in the volume.

; Extended Boot Record
db 0                   ; drive number
db 0                   ; reserved / NT flags (maybe use for MadOS, More research needed)
db 0x29                ; signiture, 0x28 or 0x29 for fat12 /fat16
dd 0x1337              ; volume serial number
db "MadOS Disk "       ; volume label
db "FAT12   "          ; identifier string

; Boot code
boot_code:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, boot_code_end + (1024 * 4)

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

;dw 0xAA55 ; bootable partition signiture, uncomment to make the floppy boot
boot_code_end:
times 450 - (boot_code_end - boot_code) db 0

;;;;;;;;;;;;;;;;;;;;;
; DATA AREA START
;;;;;;;;;;;;;;;;;;;;;
; root directory
;;;;;;;;;;;;;;;;;;;;;
; END -> FIN

