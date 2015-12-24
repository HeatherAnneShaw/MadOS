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
; MIB
jmp short boot_code
nop
db "MFS"               ; MAGIC
dw 512                 ; Bytes per sector
db 1                   ; sectors per cluster
db 0                   ; number of journals
dd 211                 ; max files
times 22 db 0          ; reserved
db 0xFB                ; drive number
db 0                   ; flags
db 0                   ; signiture
dd 0x1337              ; volume serial number
db "MadOS Disk "       ; volume label
db "MFS   01"          ; identifier string

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
    db "MFS disk not bootable...", 0xd, 0xa

;dw 0xAA55 ; bootable partition signiture, uncomment to make the floppy boot
boot_code_end:
times 450 - (boot_code_end - boot_code) db 0

;;;;;;;;;;;;;;;;;;;;;
; DATA AREA START
superblock:
times 512 - ($ - superblock) db 0

journal:
times 512 - ($ - journal) db 0

; 2
root:
; root directory info
    db 0x19    ; attributes
    db 0       ; ownership
    dd 0       ; creation time
    dd 0       ; access time
    dq 512     ; size

    file:
        db "file"       ; filename
        times 10 db 0
        db 0x18         ; attributes: read / write
        db 0            ; ownership
        db 0            ; creation time
        db 0            ; access time
        dd 3            ; first data block
        dd 1            ; checksum
        dd 5            ; file size
; padd out the rest of the first root directory block
times (512) - ($ - root) db 0

; 3
file_data:
    db "test", 0xa
; padd out the rest of the block
times (512) - ($ - file_data) db 0


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; padd out the rest of the floppy
times ((1024 * 1024) + 461373) - ($ - $$) db 0












