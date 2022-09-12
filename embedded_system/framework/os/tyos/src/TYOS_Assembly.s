SYSRICK_CONTROL EQU 0xE000E010

	AREA TYOS,CODE,READONLY

	EXPORT TYOS_Start
	EXPORT PendSV_Handler
	EXPORT SVC_Handler
	
	EXPORT sys_call
	EXPORT TYOS_Global_Interrupt_Disable
	EXPORT TYOS_Global_Interrupt_Enable
	EXPORT TYOS_Stop_Schedule
	EXPORT TYOS_Resume_Schedule
	EXPORT TYOS_Schedule_Immediately

	EXPORT TYOS_Create_Metux
	EXPORT TYOS_Delete_Metux
	EXPORT TYOS_Mutex_Regist
	EXPORT TYOS_Mutex_Unregist
	EXPORT TYOS_Mutex_Unregist_All
	EXPORT TYOS_Mutex_Lock
	EXPORT TYOS_Mutex_Lock_Direct
	EXPORT TYOS_Mutex_Lock_Try
	EXPORT TYOS_Mutex_Lock_Direct_Try
	EXPORT TYOS_Mutex_Release
	EXPORT TYOS_Mutex_Is_Locked
	EXPORT TYOS_Mutex_Is_Registed
	
	
	IMPORT _TYOS_Process_Switch
	IMPORT _TYOS_SVC_Handler_C

	IMPORT my_os
	
TYOS_Start	;启动TYOS
	MRS R0, CONTROL
	ORR R0, R0, #0x2	;R0|=2，使用PSP
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
	MRS R0, CONTROL
	ORR R0, R0, #0x1	;R0|=1，进入用户级
	MSR CONTROL, R0
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
	LDR R1, [R0, #16]
	B _TYOS_SVC_Handler_C

sys_call
	MOV R12, SP
	SVC #100
	BX LR

TYOS_Global_Interrupt_Disable
	SVC #0
	BX LR

TYOS_Global_Interrupt_Enable
	SVC #1
	BX LR

TYOS_Stop_Schedule
	SVC #2
	BX LR
	
TYOS_Resume_Schedule
	SVC #3
	BX LR

TYOS_Schedule_Immediately
	SVC #4
	BX LR

TYOS_Create_Metux
	MOV R4, #0
	PUSH {R4}
	MOV R12, SP
	SVC #5
	BX LR

TYOS_Delete_Metux
	MOV R4, #1
	PUSH {R4}
	MOV R12, SP
	SVC #5
	BX LR

TYOS_Mutex_Regist
	MOV R4, #2
	PUSH {R4}
	MOV R12, SP
	SVC #5
	BX LR

TYOS_Mutex_Unregist
	MOV R4, #3
	PUSH {R4}
	MOV R12, SP
	SVC #5
	BX LR

TYOS_Mutex_Unregist_All
	MOV R4, #4
	PUSH {R4}
	MOV R12, SP
	SVC #5
	BX LR

TYOS_Mutex_Lock
	MOV R4, #5
	PUSH {R4}
	MOV R12, SP
	SVC #5
	BX LR

TYOS_Mutex_Lock_Direct
	MOV R4, #6
	PUSH {R4}
	MOV R12, SP
	SVC #5
	BX LR

TYOS_Mutex_Lock_Try
	MOV R4, #7
	PUSH {R4}
	MOV R12, SP
	SVC #5
	BX LR

TYOS_Mutex_Lock_Direct_Try
	MOV R4, #8
	PUSH {R4}
	MOV R12, SP
	SVC #5
	BX LR

TYOS_Mutex_Release
	MOV R4, #9
	PUSH {R4}
	MOV R12, SP
	SVC #5
	BX LR

TYOS_Mutex_Is_Locked
	MOV R4, #10
	PUSH {R4}
	MOV R12, SP
	SVC #5
	BX LR

TYOS_Mutex_Is_Registed
	MOV R4, #11
	PUSH {R4}
	MOV R12, SP
	SVC #5
	BX LR

END
		