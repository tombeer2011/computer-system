// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input. 
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel. When no key is pressed, the
// program clears the screen, i.e. writes "white" in every pixel.

// Put your code here.
// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input. 
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel. When no key is pressed, the
// program clears the screen, i.e. writes "white" in every pixel.

// Put your code here.


//检测键盘KBD寄存器是不是0，如果是0，执行全部写满屏幕黑色的命令
//如果键盘寄存器不是0，

//主循环
(LOOP)

//键盘检测其实是一个瞬间的事件，因为刷新屏幕需要很长的时间

@KBD
D=M
@BLACKEN
D;JGT

@KBD
D=M
@WHITE
D;JEQ

@LOOP
0;JMP

//设置
(BLACKEN)

@SCREEN //设置屏幕的起点arr = 屏幕起点
D=A
@arr
M=D

@8190
D=A
@n
M=D

@i
M=0

(LOOPINBLACK)

//判断i和n的大小关系
@i
D=M
@n
D=D-M
@LOOP
D;JEQ

//开始设置黑色

@arr
D=M


//如果是比较复杂的数字可以采用下面的解决方案

//@i
//D=D+M //D存放的是操作的地址
////先将地址放到一个临时变量里面
//@tmp
//M=D

//@32767 //这里需要设置成
//D=A

//@tmp
//A=M
//M=D


//因为是-1，所以可以直接设置
@i
A=D+M
M=-1

// i++
@i
M=M+1



@LOOPINBLACK
0;JMP


(WHITE)
//为了方便观察省略这个函数
@LOOP
0;JMP
