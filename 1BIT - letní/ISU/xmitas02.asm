
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

;--- �kol 1 ---
;
;Naprogramujte funkci "getMax", kter� v poli 32bitov�ch ��sel bez znam�nka nalezne maximum a vr�t� jeho hodnotu.
;
;unsigned int getMax(ESI, ECX)
;  - vstup:
;    - ESI: ukazatel na za��tek pole 32bitov�ch prvk� bez znam�nka
;    - ECX: po�et prvk� pole (32bitov� hodnota bez znam�nka)
;  - v�stup:
;    - EAX: hodnota 32bitov�ho maxima bez znam�nka     
;  - funkce mus� zachovat obsah v�ech registr�, krom� registru EAX a p��znakov�ho registru    
    
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


;--- �kol 2 ---
;
;Naprogramujte funkci "addX", kter� k jednotliv�m prvk�m pole A p�i�te hodnotu x a jednotliv� v�sledky ulo�� do pole B.
;
;D�lka pol� je d�na parametrem N. Funkce mus� b�t naprogramov�na s vyu�it�m p�ed�v�n� parametr� na z�sobn�ku tak,
;�e parametry funkce se ukl�daj� na z�sobn�k od posledn�ho do prvn�ho (zprava doleva),
;parametry ze z�sobn�ku ukl�z� volaj�c� a v�sledek funkce se vrac� v registru EAX.
;
;int addX(unsigned int *pA, unsigned int N, unsigned int x, unsigned int *pB)
;  - vstup:
;    pA: ukazatel na pole A (pole A obsahuje 32bitov� hodnoty bez znam�nka)
;     N: po�et prvk� pole A (32bitov� ��slo bez znam�nka)
;     x: hodnota x (32bitov� ��slo bez znam�nka)
;    pB: ukazatel na pole B (pole B bude obsahovat 32bitov� hodnoty bez znam�nka)
;  - v�stup:
;    - EAX =  0: v p��pad�, �e nenastala ��dn� chyba  
;    - EAX = -1: v p��pad�, �e je neplatn� ukazatel *pA (tj. pA == 0)
;    - EAX = -2: v p��pad�, �e je neplatn� ukazatel *pB (tj. pB == 0)
;  - funkce mus� zachovat obsah v�ech registr�, krom� registru EAX a p��znakov�ho registru

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
;--- �kol 3 ---
;
;Naprogramujte funkci "sort", kter� sestupn� (od nejv�t��ho k nejmen��mu) se�ad� pole 32bitov�ch prvk� se znam�nkem A. 
;
;Uk�zka algoritmu �azen� v jazyce C:
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
;    ESI: ukazatel na pole A (pole A obsahuje 32bitov� hodnoty se znam�nkem)
;    ECX: po�et prvk� pole A (32bitov� ��slo bez znam�nka)
;  - funkce mus� zachovat obsah v�ech registr�, krom� registru EAX a p��znakov�ho registru     

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