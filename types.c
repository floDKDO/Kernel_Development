#include <stdint.h>

/*
uint8_t byte = 0x12;
uint16_t word = 0x1234;
uint32_t double_word = 0x12345678;
uint64_t quad_word = 0x123456789abcdef0;
unsigned __int128 double_quad_word = (unsigned __int128)0x123456789abcdef0;
*/

/*
int8_t i = 0;
int8_t* ptr1 = (int8_t*)0x1234;
int8_t* ptr2 = &i;
*/

/*
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

struct bit_field
{
	int data1:8
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
*/

/*struct bit_field bf = 
{
	.data1 = 0x1234
};*/

/*
uint8_t a8[2] = {0x12, 0x34};
uint16_t a16[2] = {0x1234, 0x5678};
uint32_t a32[2] = {0x12345678, 0x9abcdef0};
uint64_t a64[2] = {0x123456789abcdef0, 0x123456789abcdef0};
*/

/*
uint8_t a2[2][2] = {
{0x12, 0x34},
{0x56, 0x78}
};

uint8_t a3[2][2][2] = {
{{0x12, 0x34},
{0x56, 0x78}},
{{0x9a, 0xbc},
{0xde, 0xff}},
};
*/

/*
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
*/


char c = 'z';
char* ptr = &c;
char** ptr_of_ptr = &ptr;

int main(int argc, char* argv[])
{	
	return 0;
}
