#include <avr/io.h>   ; IO defs

.global transferState ; Make function available in c++
.global setup         
.global delay250

.extern beginSerial   ; Use C++ function
.extern beginEntropy

.section .bss         ; Initialize variables
mode:  .fill 1
counter: .fill 1

.text                 ; Code section

transferState:        ; Warning: this function misuses the T flag in SREG
  brts endTransfer    ; Jump to end of function if T flag set
  set                 ; Set T flag
  cbi 0x05, PB2       ; Drive PB2 LOW, (D10)
  out 0x2e, r24       ; Copy low byte of parameter to SPDR and initiate transaction
 waitloop0:
  in r18, 0x2d        ; Load SPSR to r18
  sbrs r18, SPIF      ; Skip next instruction if SPIF bit set
  rjmp waitloop0      ; Relative jump to waitloop0
  out 0x2e, r25       ; Copy high byte of parameter to SPDR and initiate transaction
 waitloop1:
  in r18, 0x2d        ; Same wait cycle
  sbrs r18, SPIF      ; I didn't use functions because of the overhead
  rjmp waitloop1
  sbi 0x05, PB2       ; Drive PB2 HIGH (D10)
  clt                 ; Clear T flag in SREG
 endTransfer:
  ret                 ; Return

setup:
  sts mode, r1        ; Clear mode. R1 is supposed to be set to 0
  call beginSerial    ; Initialize Serial
  call beginEntropy   ; Initialize Entropy
  in r19, 0x3f        ; Save SREG
  cli                 ; Clear I bit in SREG
  ldi r20, 44         ; Load _BV(DDB5) | _BV(DDB3) | _BV(DDB2) to r20
  out 0x04, r20       ; Copy r20 to DDRB
  ldi r20, 112        ; Load _BV(SPE) | _BV(MSTR) | _BV(DORD) to r20
  out 0x2c, r20       ; Copy r20 to SPCR
  ldi r20, 1          ; Load _BV(SPI2X) to r20
  out 0x2d, r20       ; Store r20 in SPSR
  out 0x3f, r19       ; Restore SREG
  clr r25             ; Clear registers 25 and 24
  clr r24
  jmp transferState   ; Jump to transferState with data 0

delay250:
  ldi  r18, 21        ; Set r18 to 21
  ldi  r19, 75        ; Set r19 to 75
  ldi  r20, 191       ; Set r20 to 191
loop1: 
  dec  r20            ; Decrement r20
  brne loop1          ; If r20 != 0 return to loop1
  dec  r19            ; Decrement r19
  brne loop1          ; If r19 != 0 return to loop1
  dec  r18            ; Decrement r18
  brne loop1          ; If r18 != 0 return to loop1
  ret                 ; Return


  
