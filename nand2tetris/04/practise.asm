//本文件是自己用作练习用的
//功能，将一个n值放在R0
//计算1+2+...+n的值，将结果放在R1中

//初始化n,i.sum
(BEGIN)

@R0
D=M // D=*R0
@n    //18是n
M=D // n=*R0


@0  
D=A //D=0
@sum    //16是sum
M=D //sum=0

@1
D=A //D=1
@i     //17是i
M=D //i=1



(LOOP)
//判断n和i的大小
@n
D=M //D=n
@i
D=D-M //D=n-i

@STOP
D;JLT // 如果n-i小于0了，就跳转到结尾



@sum
D=M
@i
D=D+M
@sum
M=D


//将i加1
@1
D=A
@i
D=D+M
@i
M=D

@LOOP
0;JMP



(STOP)
@sum
D=M
@R1
M=D




(END)
@END
0;JMP


