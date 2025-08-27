global page_table_l4
global page_table_l3
global page_table_l2
global page_table_l1

global setup_page_tables
global enable_paging

section .text
bits 32

setup_page_tables:
	mov eax, page_table_l3
	or eax, 0b11	; present, writable
	mov [page_table_l4], eax

	mov eax, page_table_l2
	or eax, 0b11	; present, writable]
	mov [page_table_l3], eax

	mov eax, page_table_l1
	or eax, 0b11	; present, writable]
	mov [page_table_l2], eax

	mov ecx, 0		; counter
	mov eax, 0b11	; present, writable
.loop:
	mov [page_table_l1 + ecx], eax

	add eax, 0x1000	; 4KiB
	add ecx, 8
	cmp ecx, 0x1000	; if whole table mapped
	jne .loop
	ret

enable_paging:
	mov eax, page_table_l4
	mov cr3, eax

	; enable PAE - Physical Address Extension
	mov eax, cr4
	or eax, 1 << 5
	mov cr4, eax

	; enable longmode
	mov ecx, 0xc0000080
	rdmsr
	or eax, 1 << 8
	wrmsr

	; enable paging
	mov eax, cr0
	or eax, 1 << 31
	mov cr0, eax
	ret

section .bss
align 4096	; 4K
page_table_l4:
	resb 4096
page_table_l3:
	resb 4096
page_table_l2:
	resb 4096
page_table_l1:
	resb 4096
