SYSRICK_CONTROL EQU 0xE000E010

	AREA TYOS,CODE,READONLY

	EXPORT TYOS_Start
	EXPORT PendSV_Handler
	EXPORT SVC_Handler
	
	IMPORT _TYOS_Process_Switch
	IMPORT _TYOS_SVC_Handler_C

	IMPORT my_os
	
TYOS_Start	;启动TYOS
	MRS R0, CONTROL
	ORR R0, R0, #0x2	;R0|=1<<1
	MSR CONTROL, R0
	LDR R0, =my_os
	LDR R0, [R0]		;读取sysprocess地址
	LDR R1, [R0]		;读取栈顶地址
	MSR PSP, R0			;将PSP指向系统进程的栈顶
	LDR R1, [R0, #4]	;读取PC地址
	LDR R0, =SYSRICK_CONTROL	
	LDR R2, [R0]		;读取Systick控制寄存器的值
	ORR R2, R2, #1		;SysTick->CTRL|=1，使能SysTick
	STR R2, [R0]		;回写SysTick的控制寄存器
	MOV PC, R1

PendSV_Handler
	CPSID I
	;保存上文
	MRS R0, PSP				;读取PSP
	TST LR, #0x10			;检测浮点寄存器保存标志位
	IT EQ					;判断是否保存浮点寄存器
	VSTMDBEQ R0!, {S16-S31}
	MOV R3, LR
	STMDB R0!, {R3-R11}		;入栈R4-R11，LR
	LDR R1, =my_os			;读取my_os地址，也就是当前活跃进程的地址
	LDR R1, [R1]			;找到进程地址
	STR R0, [R1]			;更新栈顶地址
	;上下文切换
	BL _TYOS_Process_Switch
	;加载下文，此时R0保存着下一个进程的地址
	LDR R1, [R0]			;获取下一个进程的栈顶
	LDMIA R1!, {R3-R11}		;R4-R11，LR出栈
	MOV LR, R3
	TST LR, #0x10
	IT EQ
	VLDMIAEQ R1!, {S16-S31}
	MSR PSP, R1				;更新PSP
	CPSIE I
	BX LR

SVC_Handler
	TST LR, #4		;测试EXC_RETURN的bit2
	ITE EQ
	MRSEQ R0, MSP	;压栈使用的MSP
	MRSNE R0, PSP	;压栈使用的PSP
	B _TYOS_SVC_Handler_C

	END
		