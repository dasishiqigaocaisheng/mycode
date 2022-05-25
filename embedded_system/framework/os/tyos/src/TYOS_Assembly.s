	AREA TYOS,CODE,READONLY
	
	EXPORT TYOS_SaveEnvironment
	EXPORT TYOS_RebuildEnvironment
	EXPORT TYOS_Start
	EXPORT PendSV_Handler
		
	IMPORT recent_prs
	IMPORT process1
	IMPORT process2
		
TYOS_SaveEnvironment	;
	MRS R1, MSP			;读取栈顶地址
	STR R1, [R0]
	BX LR
		
TYOS_RebuildEnvironment	;重置SP指针
	MSR MSP, R0
	BX LR
	
TYOS_Start	;启动TYOS，此函数有两个参数：1.要跳转的PC值，2.进程栈顶地址
	MSR MSP, R1
	ORR R0, #1	;确保地址最低位为1
	MOV PC, R0

PendSV_Handler
	CPSID I
	PUSH {R4-R11}		;入栈R4~R11
	TST LR, #0x10
	BNE FPU_NO_PUSH
	VPUSH {S16-S31}
FPU_NO_PUSH
	PUSH {LR}
	MRS R0, MSP			;读入MSP
	LDR R2, =recent_prs	;读取recent_prs地址
	LDR R1, [R2]		;读入recent_prs的值，也就是实例的地址
	STR R0, [R1]		;MSP存入recent_prs->StackTop
	LDR R0, =process1	;读入process1地址
	CMP R0, R1			;比较recent_prs和process1的地址
	BNE LABEL0			;不同则跳转
	;如果 recent_prs==process1
	LDR R0, =process2	;获取process2的地址
	STR R0, [R2]		;更新recent_prs
	LDR R0, [R0]		;读取process2的栈顶
	MSR MSP, R0			;MSP更新为process2的栈顶
	POP {LR}
	TST LR, #0x10
	BNE FPU_NO_POP
	VPOP {S16-S31}
FPU_NO_POP
	POP {R4-R11}		;R4~R11出栈
	CPSIE I
	BX LR
LABEL0
	STR R0, [R2]		;更新recent_prs
	LDR R0, [R0]		;读取process1的栈顶
	MSR MSP, R0			;MSP更新为process1的栈顶
	POP {LR}
	TST LR, #0x10
	BNE FPU_NO_POP
	VPOP {S16-S31}
	POP {R4-R11}		;R4~R11出栈
	CPSIE I
	BX LR
	
	
	
	
	

	END
		