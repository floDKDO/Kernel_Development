# C and data section

## Types numériques
Soit le code C : 
```
#include <stdint.h>

uint8_t byte = 0x12;
uint16_t word = 0x1234;
uint32_t double_word = 0x12345678;
uint64_t quad_word = 0x123456789abcdef0;
unsigned __int128 double_quad_word = (unsigned __int128)0x123456789abcdef0;

int main(int argc, char* argv[])
{	
	return 0;
}
```

On obtient : 
```
Disassembly of section .data:

0000000000004010 <byte>:
    4010:	12 00                	adc    al,BYTE PTR [rax]

0000000000004012 <word>:
    4012:	34 12                	xor    al,0x12

0000000000004014 <double_word>:
    4014:	78 56                	js     406c <_end+0x34>
    4016:	34 12                	xor    al,0x12

0000000000004018 <quad_word>:
    4018:	f0 de bc 9a 78 56 34 	lock fidivr WORD PTR [rdx+rbx*4+0x12345678]
    401f:	12 

0000000000004020 <double_quad_word>:
    4020:	f0 de bc 9a 78 56 34 	lock fidivr WORD PTR [rdx+rbx*4+0x12345678]
    4027:	12 
    4028:	00 00                	add    BYTE PTR [rax],al
    402a:	00 00                	add    BYTE PTR [rax],al
    402c:	00 00                	add    BYTE PTR [rax],al
    402e:	00 00                	add    BYTE PTR [rax],al
```
=> on voit que chaque type est placé en mémoire selon sa taille. Les deux points à noter sont : 
- représentation en little-endian
- padding pour le type "byte" pour se retrouver sur une adresse multiple de 2


## Pointeurs

### Cas pointeurs simples
Soit le code C : 
```
int8_t i = 0;
int8_t* ptr1 = (int8_t*)0x1234;
int8_t* ptr2 = &i;

int main(int argc, char* argv[])
{	
	return 0;
}
```

On obtient : 
```
Disassembly of section .data:

0000000000004010 <ptr1>:
    4010:	34 12                	xor    al,0x12
    4012:	00 00                	add    BYTE PTR [rax],al
    4014:	00 00                	add    BYTE PTR [rax],al
    4016:	00 00                	add    BYTE PTR [rax],al

0000000000004018 <ptr2>:
    4018:	21 40 00             	and    DWORD PTR [rax+0x0],eax
    401b:	00 00                	add    BYTE PTR [rax],al
    401d:	00 00                	add    BYTE PTR [rax],al
    401f:	00                   	.byte 0
    
Disassembly of section .bss:

0000000000004021 <i>:
    4021:	00 00                	add    BYTE PTR [rax],al
    4023:	00 00                	add    BYTE PTR [rax],al
    4025:	00 00                	add    BYTE PTR [rax],al
    4027:	00                   	.byte 0
```

*ptr1* et *ptr2* stockent bien les adresses 0x1234 et 0x4021 (adresse de *i*) respectivement.
Points à noter : 
- code 64 bits => les pointeurs ont une taille de 64 bits, d'où les 00
- la variable *i* est placée dans *.bss* car elle est initialisée à 0

### Cas pointeurs de pointeurs
Soit le code C : 
```
char c = 'z';
char* ptr = &c;
char** ptr_of_ptr = &ptr;
```

On obtient : 
```
Disassembly of section .data:

0000000000004010 <c>:
    4010:	7a 00                	jp     4012 <c+0x2>
    4012:	00 00                	add    BYTE PTR [rax],al
    4014:	00 00                	add    BYTE PTR [rax],al
    4016:	00 00                	add    BYTE PTR [rax],al

0000000000004018 <ptr>:
    4018:	10 40 00             	adc    BYTE PTR [rax+0x0],al
    401b:	00 00                	add    BYTE PTR [rax],al
    401d:	00 00                	add    BYTE PTR [rax],al
    401f:	00                 	add    BYTE PTR [rax],bl

0000000000004020 <ptr_of_ptr>:
    4020:	18 40 00             	sbb    BYTE PTR [rax+0x0],al
    4023:	00 00                	add    BYTE PTR [rax],al
    4025:	00 00                	add    BYTE PTR [rax],al
    4027:	00                   	.byte 0
```
On voit que *c* est stockée à l'adresse 0x4010, *ptr* stocke l'adresse 0x4010 (= l'adresse de *c*) et *ptr_of_ptr* stocke l'adresse 0x4017 (= l'adresse de *ptr*).


## Bitfields
Soit le code C : 
```
struct s1
{
	int data1:8;
	int data2:8;
	int data3:8;
	int data4:8;
	char data5:4;
};

struct s2
{
	int data1;
	int data2;
	int data3;
	int data4;
	char data5;
};

struct s1 s1 = 
{
	.data1 = 0x12,
	.data2 = 0x34,
	.data3 = 0x56,
	.data4 = 0x78,
	.data5 = 0x9
};

struct s2 s2 = 
{
	.data1 = 0x12,
	.data2 = 0x34,
	.data3 = 0x56,
	.data4 = 0x78,
	.data5 = 0x9
};
```

On obtient : 
```
Disassembly of section .data:

0000000000004010 <s1>:
    4010:	12 34 56             	adc    dh,BYTE PTR [rsi+rdx*2]
    4013:	78 09                	js     401e <s1+0xe>
    4015:	00 00                	add    BYTE PTR [rax],al
    4017:	00 00                	add    BYTE PTR [rax],al
    4019:	00 00                	add    BYTE PTR [rax],al
    401b:	00 00                	add    BYTE PTR [rax],al
    401d:	00 00                	add    BYTE PTR [rax],al
    401f:	00                 	add    BYTE PTR [rdx],dl

0000000000004020 <s2>:
    4020:	12 00                	adc    al,BYTE PTR [rax]
    4022:	00 00                	add    BYTE PTR [rax],al
    4024:	34 00                	xor    al,0x0
    4026:	00 00                	add    BYTE PTR [rax],al
    4028:	56                   	push   rsi
    4029:	00 00                	add    BYTE PTR [rax],al
    402b:	00 78 00             	add    BYTE PTR [rax+0x0],bh
    402e:	00 00                	add    BYTE PTR [rax],al
    4030:	09 00                	or     DWORD PTR [rax],eax
    4032:	00 00                	add    BYTE PTR [rax],al
```
On voit bien que la struct *s1* qui contient des bitfields n'est pas représentée pareil que la struct *s2* qui n'en contient pas. 


## Tableaux

### Tableaux 1D
Soit le code C : 
```
uint8_t a8[2] = {0x12, 0x34};
uint16_t a16[2] = {0x1234, 0x5678};
uint32_t a32[2] = {0x12345678, 0x9abcdef0};
uint64_t a64[2] = {0x123456789abcdef0, 0x123456789abcdef0};
```

On obtient : 
```
Disassembly of section .data:

0000000000004010 <a8>:
    4010:	12 34              	adc    dh,BYTE PTR [rsp+rsi*1]

0000000000004012 <a16>:
    4012:	34 12                	xor    al,0x12
    4014:	78 56                	js     406c <_end+0x34>
    4016:	00 00                	add    BYTE PTR [rax],al

0000000000004018 <a32>:
    4018:	78 56                	js     4070 <_end+0x38>
    401a:	34 12                	xor    al,0x12
    401c:	f0 de bc 9a    	lock fidivr WORD PTR [rdx+rbx*4-0x65432110]
    4023:
    
0000000000004020 <a64>:
    4020:	f0 de bc 9a 78 56 34 	lock fidivr WORD PTR [rdx+rbx*4+0x12345678]
    4027:	12 
    4028:	f0 de bc 9a 78 56 34 	lock fidivr WORD PTR [rdx+rbx*4+0x12345678]
    402f:	12 
```
On voit bien que les éléments des tableaux sont stockées de manière contiguë en mémoire (toujours en little-endian).

### Tableaux multi-dimensionnels
Soit le code C : 
```
uint8_t a2[2][2] = 
{
  {0x12, 0x34},
  {0x56, 0x78}
};

uint8_t a3[2][2][2] = 
{
  {{0x12, 0x34},
  {0x56, 0x78}},
  {{0x9a, 0xbc},
  {0xde, 0xff}},
};
```

On obtient : 
```
Disassembly of section .data:

0000000000004010 <a2>:
    4010:	12 34 56             	adc    dh,BYTE PTR [rsi+rdx*2]
    4013:	78 00                	js     4015 <a2+0x5>
    4015:	00 00                	add    BYTE PTR [rax],al
    4017:	00                 	add    BYTE PTR [rdx],dl

0000000000004018 <a3>:
    4018:	12 34 56             	adc    dh,BYTE PTR [rsi+rdx*2]
    401b:	78 9a                	js     3fb7 <_DYNAMIC+0x1b7>
    401d:	bc                   	.byte 0xbc
    401e:	de ff                	fdivp  st(7),st
```
On voit que les tableaux multi-dimensionnels sont représentés comme un tableau 1D en mémoire, la taille du tableau étant une multiplication de la taille de chaque dimension (ici 2 * 2 = 4 octets pour *a2* et 2 * 2 * 2 = 8 octets pour *a3*)

## Tableaux de caractères (2D) vs tableaux de pointeurs sur caractère
Soit le code C : 
```
char names[2][10] = 
{
	"John Doe",
	"Jane Doe"
};

char* names_ptr[2] = 
{
	"John Doe",
	"Jane Doe"
};

```

On obtient : 
```
Disassembly of section .rodata:

0000000000002000 <_IO_stdin_used>:
    2000:	01 00                	add    DWORD PTR [rax],eax
    2002:	02 00                	add    al,BYTE PTR [rax]
    2004:	4a 6f                	rex.WX outs dx,DWORD PTR ds:[rsi]
    2006:	68 6e 20 44 6f       	push   0x6f44206e
    200b:	65 00 4a 61          	add    BYTE PTR gs:[rdx+0x61],cl
    200f:	6e                   	outs   dx,BYTE PTR ds:[rsi]
    2010:	65 20 44 6f 65       	and    BYTE PTR gs:[rdi+rbp*2+0x65],al
    2015:	00                   	.byte 0

Disassembly of section .data:

0000000000004010 <names>:
    4010:	4a 6f                	rex.WX outs dx,DWORD PTR ds:[rsi]
    4012:	68 6e 20 44 6f       	push   0x6f44206e
    4017:	65 00 00             	add    BYTE PTR gs:[rax],al
    401a:	4a 61                	rex.WX (bad)
    401c:	6e                   	outs   dx,BYTE PTR ds:[rsi]
    401d:	65 20 44 6f 65       	and    BYTE PTR gs:[rdi+rbp*2+0x65],al
    4022:	00 00                	add    BYTE PTR [rax],al
    4024:	00 00                	add    BYTE PTR [rax],al
    4026:	00 00                	add    BYTE PTR [rax],al
    4028:	00 00                	add    BYTE PTR [rax],al
    402a:	00 00                	add    BYTE PTR [rax],al
    402c:	00 00                	add    BYTE PTR [rax],al
    402e:	00 00                	add    BYTE PTR [rax],al

0000000000004030 <names_ptr>:
    4030:	04 20                	add    al,0x20
    4032:	00 00                	add    BYTE PTR [rax],al
    4034:	00 00                	add    BYTE PTR [rax],al
    4036:	00 00                	add    BYTE PTR [rax],al
    4038:	0d 20 00 00 00       	or     eax,0x20
    403d:	00 00                	add    BYTE PTR [rax],al
    403f:	00                   	.byte 0

```
On voit que le tableau 2D de caractères stocke le code ASCII de chaque lettre de manière contiguë en mémoire alors que le tableau de pointeurs sur caractère stocke l'adresse de début de chacune des deux chaînes dans .rodata => *names_ptr* stocke deux adresses.
