
%include "rw32-2017.inc"

%macro WRITE_ARRAY 4
	push esi
	push ecx
	push eax
	mov esi,%1
   	mov ecx,%2
%%writeElement:
  	lods%3
   	call Write%4
   	cmp ecx,1
   	jbe %%skipComma
   	mov al,','
   	call WriteChar
%%skipComma:
   	loop %%writeElement

   	call WriteNewLine
   	pop eax
   	pop ecx
   	pop esi
%endmacro

%macro F13_CALL 3
	mov esi,%2
	mov ecx,%3
	call %1
%endmacro

%macro F2_CALL 5
	push %5
	push %4
	push %3
	push %2
	call %1
	add esp,16
%endmacro

section .data
    A1_1 dd 851968,589824,1310720,65536,1245184
    A2_1 dd 851968,589824,1310720,65536,1245184
    B2 dd 778992600,1614556900,4047036106,1304306864,2477850392
    A3_1 dd 851968,589824,1310720,65536,1245184

section .text

;--- Úkol 1 ---
;
;Naprogramujte funkci "getMax", která v poli 32bitových èísel bez znaménka nalezne maximum a vrátí jeho hodnotu.
;
;unsigned int getMax(ESI, ECX)
;  - vstup:
;    - ESI: ukazatel na zaèátek pole 32bitových prvkù bez znaménka
;    - ECX: poèet prvkù pole (32bitová hodnota bez znaménka)
;  - výstup:
;    - EAX: hodnota 32bitového maxima bez znaménka     
;  - funkce musí zachovat obsah všech registrù, kromì registru EAX a pøíznakového registru    
    
getMax:
    push ebx ; backup ebx state before
    xor ebx, ebx ; clear ebx
    mov eax, [esi] ; reset eax and move first item
 
findMax:  
        mov ebx, [esi+(4*ecx)-4] ; loop through array while selecting elemets
        cmp ebx, eax ; compare eax to ebx
    
        jl noWrite ; if greate skip addition section
   
        mov eax, ebx ; replace
        noWrite: ; jump here item is smaller than the previous
   
    loop findMax ; proceed with the loop
    
    pop ebx ; restore ebx state
    ret    


;--- Úkol 2 ---
;
;Naprogramujte funkci "addX", která k jednotlivým prvkùm pole A pøiète hodnotu x a jednotlivé výsledky uloží do pole B.
;
;Délka polí je dána parametrem N. Funkce musí být naprogramována s využitím pøedávání parametrù na zásobníku tak,
;že parametry funkce se ukládají na zásobník od posledního do prvního (zprava doleva),
;parametry ze zásobníku uklízí volající a výsledek funkce se vrací v registru EAX.
;
;int addX(unsigned int *pA, unsigned int N, unsigned int x, unsigned int *pB)
;  - vstup:
;    pA: ukazatel na pole A (pole A obsahuje 32bitové hodnoty bez znaménka)
;     N: poèet prvkù pole A (32bitové èíslo bez znaménka)
;     x: hodnota x (32bitové èíslo bez znaménka)
;    pB: ukazatel na pole B (pole B bude obsahovat 32bitové hodnoty bez znaménka)
;  - výstup:
;    - EAX =  0: v pøípadì, že nenastala žádná chyba  
;    - EAX = -1: v pøípadì, že je neplatný ukazatel *pA (tj. pA == 0)
;    - EAX = -2: v pøípadì, že je neplatný ukazatel *pB (tj. pB == 0)
;  - funkce musí zachovat obsah všech registrù, kromì registru EAX a pøíznakového registru

addX:
    ; move to ebp
    mov ebp,esp
    ; backup registers
    push ebx
    push ecx
    
    ; assign params
    mov edi, [ebp+16] ; address of new array
    mov ebx, [ebp+12] ; number to be multiplied by
    mov ecx, [ebp+8] ; counter
    mov esi, [ebp+4] ; array address
    
; deciding logic on whenever to quit here or elsewhere
    cmp esi, 0
    je err1
    cmp edi, 0
    je err2
    
    jmp continue
    
    ; compare then exit function if is not present
err1:
    mov eax, -2
    jmp retr
err2:   
    mov eax, -1
    jmp retr
retr:
    ; retrieve values for registers
    pop ecx
    pop ebx
    ret
    
    ; otherwise continue
continue:
    ; set direction flag
    cld
addd:   
    lodsd
    add eax, ebx
    stosd
    loop addd
    
    ; succesful status
    mov eax, 0
    ; jmp to the return
    jmp retr    
    
    ret

;
;--- Úkol 3 ---
;
;Naprogramujte funkci "sort", která sestupnì (od nejvìtšího k nejmenšímu) seøadí pole 32bitových prvkù se znaménkem A. 
;
;Ukázka algoritmu øazení v jazyce C:
;
;int *pA, i, j, N;
;for(i = 0; i < N; i++) {
;    for(j = i + 1; j < N; j++) {
;        if (pA[i] < pA[j]) { tmp = pA[i]; pA[i] = pA[j]; pA[j] = tmp; }      
;    }
; }
;
;void sort(int *pA, unsigned int N)
;  - vstup:
;    ESI: ukazatel na pole A (pole A obsahuje 32bitové hodnoty se znaménkem)
;    ECX: poèet prvkù pole A (32bitové èíslo bez znaménka)
;  - funkce musí zachovat obsah všech registrù, kromì registru EAX a pøíznakového registru     

sort:
    push ebx
    push ecx
    push edx

    mov edi, 0 ; i counter
    
    for1:
        cmp edi, ecx ; end of the loop
        mov edx, edi ; j counter
        jae end1
       
        lea ebx, [esi + (edi * 4)] ; adresa prvniho prvku
        mov eax, [ebx] ; hodnota prvniho prvku
        
        for2:
            inc edx ; j = i + 1
            cmp edx, ecx ; end of the loop
            jae end2
            
            cmp [esi + 4*edx], eax
            jge for2
            
            lea ebx, [esi + (edx * 4)] ; adresa prvniho prvku 
            mov eax, [ebx] ;
            ; code itself
                
            jmp for2
        end2:
        
        xchg eax, [esi + 4*edi]
        mov [ebx], eax
        
        inc edi ; increment i counter
        jmp for1 ; mainting the loop
    end1:
    
    pop edx
    pop ecx
    pop ebx
    
    ; clear params and return
    ret  
     


main:
    push ebp
    mov ebp,esp

    WRITE_ARRAY A1_1, 5, d, UInt32
    F13_CALL getMax, A1_1, 5
    call WriteUInt32
    call WriteNewLine

    WRITE_ARRAY A2_1, 5, d, UInt32
    WRITE_ARRAY B2, 5, d, UInt32
    F2_CALL addX, A2_1, 5, 2041385568, B2
    call WriteUInt32
    call WriteNewLine
    call WriteFlags
    WRITE_ARRAY A2_1, 5, d, UInt32
    WRITE_ARRAY B2, 5, d, UInt32

    WRITE_ARRAY A3_1, 5, d, Int32
    F13_CALL sort, A3_1, 5
    call WriteInt32
    call WriteNewLine
    WRITE_ARRAY A3_1, 5, d, Int32

    pop ebp
    ret