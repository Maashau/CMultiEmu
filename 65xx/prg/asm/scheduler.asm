.IMPORT		__SCRSTART__

.SEGMENT "ZEROPAGE"
;------------------------------------------------------------------------------;
; Task RAM data
;------------------------------------------------------------------------------;
curr_task_idx:
	.BYTE	0
curr_task:
	.WORD	$0000

task_sp_offs_list:
	.BYTE	0 ; Task 1
	.BYTE	0 ; Task 2
	.BYTE	0 ; Task 3
task_state_list:
	.BYTE	0 ; Task 1
	.BYTE	0 ; Task 2
	.BYTE	0 ; Task 3

total_run_count:
	.BYTE	0

init_temp_y:
	.BYTE	0
temp_sp:
	.BYTE	0
main_stack:
	.BYTE	0
temp_addr:
	.WORD	0

task1_temp8:
	.BYTE	0

print_str_addr:
	.WORD	0

.SEGMENT "CODE"
;------------------------------------------------------------------------------;
; Entry point of the program.
;------------------------------------------------------------------------------;
cold_boot:
	JSR		reset_mem
	JSR		init_scheduler
	JMP		run_task

;------------------------------------------------------------------------------;
; init_scheduler - Setup first task from the task list
;------------------------------------------------------------------------------;
init_scheduler:
	LDY		#0
	TSX						; Store main stack pointer.
	STX		temp_sp
task_init_loop:
	; Set all tasks ready.
	LDA		#1
	STA		task_state_list,Y
	; Switch to tasks stack.
	LDA		task_stack_org_list,Y
	TAX
	TXS
	; Push tasks initial program counter.
	STY		init_temp_y
	LDA		init_temp_y
	CLC
	ADC		init_temp_y
	TAX
	INX
	LDA		task_list,X
	STA		temp_addr+1
	DEX
	LDA		task_list,X
	STA		temp_addr
	DEC		temp_addr
	LDA		#$FF
	CMP		temp_addr
	BNE		task_init_write_pc
	DEC		temp_addr+1
task_init_write_pc:
	LDA		temp_addr+1
	PHA		; PC MSB
	LDA		temp_addr
	PHA		; PC LSB
	; Push tasks initial status register and A, X, Y registers.
	LDA		#$00
	PHA		; SR
	PHA		; A
	PHA		; X
	PHA		; Y

	; Store tasks stack.
	TSX
	TXA
	STA		task_sp_offs_list,Y
	; Check if there are more tasks to initialize.
	INY
	CPY		task_count
	BNE		task_init_loop
	; Set first task to as the current task.
	LDA		#$FF
	STA		curr_task_idx
	LDX		temp_sp			; Restore main stack pointer.
	TXS
	JSR		next_task_idx
	RTS

;------------------------------------------------------------------------------;
; Initialize memory, zeropage with $00 and stack with $A5.
; TODO: initialize rest of the ram.
;------------------------------------------------------------------------------;
reset_mem:
	TSX
reset_mem_loop:
	LDA		#$00
	STA		$00,X		; Write to zeropage.
	LDA		#$A5
	STA		$100,X		; Write to stack.
	DEX
	CPX		#$FF
	BNE		reset_mem_loop
	TSX
	INX
reset_rem_zpg:
	STA		$00,X		; Write to zeropage.
	INX
	CPX		#$00
	BNE		reset_rem_zpg
	RTS

;------------------------------------------------------------------------------;
; Print null terminated string with position in leading byte.
;------------------------------------------------------------------------------;
print_str:
	; Push registers.
	PHA
	TXA
	PHA
	TYA
	PHA
	LDY		#0
	LDA		(print_str_addr),Y	; Load position.
	TAX
	; Copy text.
print_str_loop:
	INY
	LDA		(print_str_addr),Y
	CMP		#0
	BEQ		print_str_done
	STA		__SCRSTART__,X
	INX
	JMP		print_str_loop
print_str_done:
	; Pull registers.
	PLA
	TAY
	PLA
	TAX
	PLA
	RTS

;------------------------------------------------------------------------------;
; Update total task execution count.
;------------------------------------------------------------------------------;
total_count_update:
	LDA		__SCRSTART__+180
	CMP		#'T'
	BEQ		total_upd_count

	; Print header for the total count.
	SEI
	LDA		#<total_text
	STA		print_str_addr
	LDA		#>total_text
	STA		print_str_addr+1
	JSR		print_str
	CLI

total_upd_count:
	LDX		total_run_count
	INX
	STX		total_run_count
	TXA
	LDX		#185
print_u8:
	; Push Y
	TYA
	PHA
	LDY		#0
count_hundreds:
	CMP		#100
	BCC		write_hundreds
	SBC		#100
	INY
	JMP		count_hundreds
write_hundreds:
	PHA
	TYA
	CLC
	ADC		#'0'
	INX
	STA		__SCRSTART__,X
	PLA
	LDY		#0
	CLC
count_tens:
	CMP		#10
	BCC		write_tens
	SBC		#10
	INY
	JMP		count_tens
write_tens:
	PHA
	TYA
	CLC
	ADC		#'0'
	INX
	STA		__SCRSTART__,X
	PLA
	CLC
count_ones:
	CLC
	ADC		#'0'
	INX
	STA		__SCRSTART__,X
	; Pull Y
	PLA
	TAY
	RTS

;------------------------------------------------------------------------------;
; Task 1
;------------------------------------------------------------------------------;
task1:
	LDY		#1

	; Print header for task 1 count.
	SEI
	LDA		#<task1_text
	STA		print_str_addr
	LDA		#>task1_text
	STA		print_str_addr+1
	JSR		print_str

	LDA		#11
	STA		stack_used_text
	LDA		#<stack_used_text
	STA		print_str_addr
	LDA		#>stack_used_text
	STA		print_str_addr+1
	JSR		print_str
	CLI
	BRK
	.BYTE $00	; Padding for RTI
task1_loop:
.IF 0
	; Calculate stack usage.
	LDY		curr_task_idx
	TSX
	STX		task1_temp8
	SEC
	LDA		task_stack_org_list,Y
	SBC		task1_temp8
	TAX
	LDA		#'#'
	CPX		#$00
	BNE		task1_no_stack_usage

task1_print_loop:
	STA		__SCRSTART__+11,X
	DEX
	CPX		#$FF
	BNE		task1_print_loop

task1_no_stack_usage:
.ENDIF
	TYA
	LDX		#5
	JSR		print_u8
	JSR		total_count_update
	; Let task 2 run after 2 times of task 1.
	TYA
	AND		#1
	BNE		task1_skip
	LDA		#1
	STA		task_state_list+1
task1_skip:
	INY
	BRK
	.BYTE $00	; Padding for RTI
	JMP		task1_loop

;------------------------------------------------------------------------------;
; Task 2
;------------------------------------------------------------------------------;
task2:
	LDY		#1

	; Print header for task 2 count.
	SEI
	LDA		#<task2_text
	STA		print_str_addr
	LDA		#>task2_text
	STA		print_str_addr+1
	JSR		print_str
	LDA		#0
	STA		task_state_list+1

	LDA		#71
	STA		stack_used_text
	LDA		#<stack_used_text
	STA		print_str_addr
	LDA		#>stack_used_text
	STA		print_str_addr+1
	JSR		print_str
	CLI
	BRK
	.BYTE $00	; Padding for RTI
task2_loop:
	TYA
	LDX		#65
	JSR		print_u8
	JSR		total_count_update
	INY
	LDA		#0
	STA		task_state_list+1
	; Let task 3 run after 2 times of task 2.
	TYA
	AND		#1
	BEQ		task2_skip
	LDA		#1
	STA		task_state_list+2
task2_skip:
	BRK
	.BYTE $00	; Padding for RTI
	JMP		task2_loop

;------------------------------------------------------------------------------;
; Task 3
;------------------------------------------------------------------------------;
task3:
	LDY		#1

	; Print header for task 1 count.
	SEI
	LDA		#<task3_text
	STA		print_str_addr
	LDA		#>task3_text
	STA		print_str_addr+1
	JSR		print_str
	LDA		#0
	STA		task_state_list+2

	LDA		#131
	STA		stack_used_text
	LDA		#<stack_used_text
	STA		print_str_addr
	LDA		#>stack_used_text
	STA		print_str_addr+1
	JSR		print_str
	CLI
	BRK
	.BYTE $00	; Padding for RTI
task3_loop:
	TYA
	LDX		#125
	JSR		print_u8
	JSR		total_count_update
	INY
	LDA		#0
	STA		task_state_list+2
	BRK
	.BYTE $00	; Padding for RTI
	JMP		task3_loop

;------------------------------------------------------------------------------;
; Calculate index of the next task.
;------------------------------------------------------------------------------;
next_task_idx:
	LDY		curr_task_idx
	INY
	CPY		task_count
	BNE		idx_ready
	LDY		#0
idx_ready:
	STY		curr_task_idx
	LDA		task_state_list,Y
	CMP		#1
	BNE		next_task_idx
	RTS

;------------------------------------------------------------------------------;
; Schedule next task
;------------------------------------------------------------------------------;
scheduler:
	; Push A, X and Y registers.
	PHA
	TXA
	PHA
	TYA
	PHA
	; Store current tasks stack pointer.
	LDY		curr_task_idx
	TSX
	TXA
	STA		task_sp_offs_list,Y
	; Switch to main stack.
	LDX		main_stack
	TXS
	; Get next task index to run.
	JSR		next_task_idx
run_task:
	TSX
	STX		main_stack
	; Pop the new task.
	LDY		curr_task_idx
	LDX		task_sp_offs_list,Y
	TXS
	; Pop A, X and Y registers.
	PLA
	TAY
	PLA
	TAX
	PLA
	RTI

;------------------------------------------------------------------------------;
; Task ROM data
;------------------------------------------------------------------------------;
task_count:
	.BYTE	3
task_list:
	.WORD	task1+1
	.WORD	task2+1
	.WORD	task3+1
task_stack_org_list:
	.BYTE	$10
	.BYTE	$20
	.BYTE	$30
task_stack_sz_list:
	.BYTE	$0F ; Task 1
	.BYTE	$0F ; Task 2
	.BYTE	$0F ; Task 3

;------------------------------------------------------------------------------;
; Strings
;------------------------------------------------------------------------------;
task1_text:
	.BYTE 0
	.BYTE "Task1:", 0
task2_text:
	.BYTE 60
	.BYTE "Task2:", 0
task3_text:
	.BYTE 120
	.BYTE "Task3:", 0
total_text:
	.BYTE 180
	.BYTE "total:", 0
	
stack_used_text:
	.BYTE 0
	.BYTE "****************", 0

;------------------------------------------------------------------------------;
; Vector table
;------------------------------------------------------------------------------;
.SEGMENT "VEC_NMI"
	.WORD	scheduler
.SEGMENT "VEC_RST"
	.WORD	cold_boot
.SEGMENT "VEC_IRQBRK"
	.WORD	scheduler