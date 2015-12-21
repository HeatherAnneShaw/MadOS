;////////////////////////////////////////////////////////////////////////////////
;// THE SCOTCH-WARE LICENSE (Revision 0):
;// <aaronryool/gmail.com> wrote this file. As long as you retain this notice you
;// can do whatever you want with this stuff. If we meet some day, and you think
;// this stuff is worth it, you can buy me a shot of scotch in return
;////////////////////////////////////////////////////////////////////////////////
; This assembles to a fat32 floppy disk image
[bits 16]
[org 0x7C00]

%define bps 512
%define spc 1
%define rs 32
%define nf 2
%define re 0
%define fs 1280

;;;;;;;;;;;;;;;;;;;;;;;;
; BIOS Parameter Block
jmp short boot_code
nop
db "MadOSv01"          ; OEM identifier
dw bps                 ; Bytes per sector
db spc                 ; sectors per cluster
dw rs                  ; reserved sectors
db nf                  ; number of fats
dw re                  ; root directory entries
dw 0                   ; logical sectors 16
db 0xFB                ; media descriptor type
dw 1                   ; sectors per fat, fat12/fat16 only
dw 18                  ; sectors per track
dw 2                   ; number of heads / sides on media
dd 0                   ; number of hidden sectors

; Extended Boot Record
dd fs                  ; logical sectors 32
dw 0                   ; extended flags
dw 0                   ; fs version
dd 2                   ; root cluster start
dw 2                   ; fsinfo structure secter
dw 0                   ; backup bootsector
times 12 db 0          ; reserved
db 0                   ; drive number
db 0                   ; reserved / NT flags (maybe use for MadOS, More research needed)
db 0x29                ; signiture, 0x28 or 0x29 for fat12 /fat16
dd 0x1337              ; volume serial number
db "MadOS Disk "       ; volume label
db "FAT32   "          ; identifier string

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
times 512 - ($ - $$) db 0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
!!!!! from here down needs to be coded using constants and stuff !!!!!
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;
; First fat entries
fat1:
    db 0xFB, 0xFF, 0xFF, 0x00, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF
    times 512 - ($ - fat1) db 0
fat2:
    times 512 - ($ - fat2) db 0
;;;;;;;;;;;;;;;;;;
; root directory
root:
    folder:
        db "folder     "
        db 0x20 | 0x10         ; attributes: a, d
        dw 0                   ; reserved
        dw 0xA496              ; creation time (h,m,s)
        dw 0x4793              ; creation date
        dw 0x4793              ; access date
        dw 0                   ; high order bits of first cluster address, 0 on fat12
        dw 0xA496              ; write time (h,m,s)
        dw 0x4793              ; write date
        dw 3                   ; low order bits of first cluster address
        dd 64                  ; file size

    file:
        db "file       "
        db 0x20                ; attributes: a
        dw 0                   ; reserved
        dw 0xA496              ; creation time (h,m,s)
        dw 0x4793              ; creation date
        dw 0x4793              ; access date
        dw 0                   ; high order bits of first cluster address, 0 on fat12
        dw 0xA496              ; write time (h,m,s)
        dw 0x4793              ; write date
        dw 2                   ; low order bits of first cluster address
        dd 40                  ; file size

; padd out the rest of the root directory
times (512 * 4) - ($ - root) db 0

;;;;;;;;;;;;;;;;;;
; Data area
times 1024 db 0     ; first two data blocks are reserved

; 2 file
file_data:
    db "This is a file in the root directory :D", 0xa
; cluster padding
times 512 - ($ - file_data) db 0

; 3 folder
folder_data:
    file2:
        db "file2      "
        db 0x20                ; attributes: a
        dw 0                   ; reserved
        dw 0xA496              ; creation time (h,m,s)
        dw 0x4793              ; creation date
        dw 0x4793              ; access date
        dw 0                   ; high order bits of first cluster address, 0 on fat12
        dw 0xA496              ; write time (h,m,s)
        dw 0x4793              ; write date
        dw 4                   ; low order bits of first cluster address
        dd 34                  ; file size
    folder2:
        db "folder2    "
        db 0x20 | 0x10         ; attributes: a, d
        dw 0                   ; reserved
        dw 0xA496              ; creation time (h,m,s)
        dw 0x4793              ; creation date
        dw 0x4793              ; access date
        dw 0                   ; high order bits of first cluster address, 0 on fat12
        dw 0xA496              ; write time (h,m,s)
        dw 0x4793              ; write date
        dw 5                   ; low order bits of first cluster address
        dd 32                  ; file size
; cluster padding
times 512 - ($ - file2) db 0

; 4 file2
file2_data:
    db "This is a file in a sub directory", 0xa
; cluster padding
times 512 - ($ - file2_data) db 0


; 5 folder2_data
folder2_data:
    file3:
        db "file3      "
        db 0x20                ; attributes: a
        dw 0                   ; reserved
        dw 0xA496              ; creation time (h,m,s)
        dw 0x4793              ; creation date
        dw 0x4793              ; access date
        dw 0                   ; high order bits of first cluster address, 0 on fat12
        dw 0xA496              ; write time (h,m,s)
        dw 0x4793              ; write date
        dw 6                   ; low order bits of first cluster address
        dd 0                   ; file size
; cluster padding
times 512 - ($ - folder2_data) db 0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; padd out the rest of the floppy
times ((1024 * 1024) + 461373) - ($ - $$) db 0
