完整版参考[这里](http://blog.csdn.net/monicary/article/details/53320700)。

## fopen
```
FILE *fopen(const char *filename, const char *mode)
```
"r"	打开一个文件进行读取。该文件必须存在。
"w"	创建一个空的书面文件。如果已经存在具有相同名称的文件，其内容被删除的文件被认为是一个新的空文件。
"a"	附加到文件中。写入操作的数据追加在文件末尾的。该文件被创建，如果它不存在。
"r+"	打开更新文件读取和写入。该文件必须存在。
"w+"	创建一个空文件，读取和写入。
"a+"	打开一个文件的​​读取和追加。

## fread
```
int fread(void *buf, int size, int count, FILE *fp);
```
从fp指向的文件中读取长度为size的count个数据项，并将它输入到以buf为首地址的缓冲区中。此时，文件指针fp会自动增加实际读入数据的字节数，即fp指向最后读入字符的下一个字符位置。
返回值：返回实际读入数据项的个数，即count值。若遇到错误或文件结束返回0。

## fscanf
```
int fscanf(FILE *fp, char *format[,argument...]);
```
## sscanf
```
int sscanf(char *string, char*format[,argument,...]);
```
## fgets
```
char *fgets(char *string, int n, FILE *fp);
```
函数功能：从fp所指的文件中读取一个长度为(n-1)的字符串，并将该字符串存入以string为起始地址的缓冲区中。fgets函数有三个参数，其中string为缓冲区首地址，n规定了要读取的最大长度，fp为文件指针。
返回值：返回地址string，若遇到文件结束符或出错，返回NULL。用feof 或ferror判断是否出错。

## fflush
```
int fflush(FILE *fp);
```
清除一个流，即清除文件缓冲区，当文件以写方式打开时，将缓冲区内容写入文件。也就是说，对于ANSI C规定的是缓冲文件系统，函数fflush用于将缓冲区的内容输出到文件中去。
返回值：如果成功刷新，fflush返回0。指定的流没有缓冲区或者只读打开时也返回0值。返回EOF指出一个错误。

## fclose




## fdopen
```
FILE *fdopen(int fd, const char *mode);
```
fdopen获取一个现有的文件描述符，并使一个标准的I/O流与该描述符相结合。此函数常用于由创建管道和网络通信函数返回的描述符。因为这些特殊类型的文件不能用标准I/Ofopen函数打开，所有我们必须先调用设备专用函数以获得一个文件描述符，然后用fopen使一个标准I/O与该描述符相关联。


## fwrite
```
int fwrite(void *buf, int size, int count, FILE *fp);
```
将buf所指向的count*size个字节的数据输出到文件指针fp所指向的文件中去。该函数与fread相对，输出数据后，文件指针fp自动指向输出的最后一个字符的下一个位置。该函数常用于分批将数据块输出到文件中。
返回值：返回实际写入文件数据项个数。

## fprintf
```
int fprintf(FILE *fp, char *format[, argument,...]);
```
## sprintf
```
int sprintf(char *string, char *farmat [,argument,...]);
```
## fputs

...

## fseek
```
int fseek(FILE *fp, long offset, int base);
```
重定位流上的文件指针，即将fp指向的文件的位置指针移向以base为基准，以offset为偏移量的位置。该函数有三个参数：fp为文件指针，offset为偏移量，即位移offset个字节，base为指针移动的基准，即起始点。其中，基准base用0、1或2表示。在ANSI C标准指定的名字如下表
  ANSI C标准指定的起始点名字
起始点
名字 
数字代码
文件当前位置
SEEK_CUR
1
文件开始位置
SEEK_SET
0
文件末尾位置
SEEK_END
2

偏移量用长整型数表示。ANSI C标准规定，在数的末尾加L就表示长整型数。该函数在随机读取较长的顺序文件时是很有用的。
返回值：成功返回0，否则返回非0。



