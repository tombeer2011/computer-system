# 8.26

>      以图 8-22（sample/ch08/shellex.c）中的示例作为开始点，编写一个支持作业控制的
>      外壳程序。外壳必须具有以下特征：

>      o 用户输入的命令行由一个 name、零个或者多个参数组成，它们都是由一个或者多个
>        空格分隔开的。如果 name 是一个内置命令，那么外壳就立刻处理它，并等待下一个
>        命令行。否则，外壳就假设 name 是一个可执行的文件，在一个初始的子进程（作业）
>        的上下文中加载并运行它。作业的进程组 ID 与子进程的 PID 相同。

>      o 每个作业是由一个进程 ID（PID）或者一个作业 ID（JID）来标识的，它是由一个外
>        壳分配的任意的小正整数。JID 在命令行上用前缀“%”来表示。比如，“%5”表示
>        JID 5，而“5”表示 PID 5。

>      o 如果命令行以 & 结束，那么外壳程序就在后台运行这个作业。否则，外壳就在前台
>        运行这个作业。

>      o 输入 ctrl-c（ctrl-z），使得外壳发送一个 SIGINT（SIGTSTP）信号给前台进程组
>        中的每个进程。

>      o 内置命令 jobs 列出所有的后台作业。

>      o 内置命令 bg <job> 通过发送一个 SIGCONT 信号重启 <job>，然后在后台运行它。
>        <job>参数可以是一个 PID，也可以是一个 JID。

>      o 内置命令 fg <job> 通过发送一个 SIGCONT 信号重启 <job>，然后在前台运行它。

>      o 外壳回收它所有的僵死子进程。如果任何作业因为它收到一个未捕获的信号而终止，
>        那么外壳就输出一条信息到终端，包含该作业的 PID 和对违规信号的描述。

>      下面展示了一个示例的外壳会话：

>      unix> ./shell # Run your shell program
>      > bogus
>      bogus: Command not found. # Execve can't find executable
>      > foo 10
>      Job 5035 terminated by signal: Interrupt # User types ctrl-c
>      > foo 100 &
>      [1] 5036 foo 100 &
>      > foo 200 &
>      [2] 5037 foo 200 &
>      > fg %1
>      Job [1] 5036 stopped by signal: Stopped # User types ctrl-z
>      > jobs
>      [1] 5036 Stopped foo 100 &
>      [2] 5037 Running foo 200 &
>      > bg 5035
>      5035: No such process
>      > bg 5036
>      [1] 5036 foo 100 &
>      > /bin/kill 5036
>      Job 5036 terminated by signal: Terminated
>      >  fg %2 # Wait for fg job to finish.
>      > quit
>      unix> # Back to the UNIX shell
