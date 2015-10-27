// Copyright 2015, Alessandro Fabbri and Stefano Sinigardi
// for any question, please mail rommacc@yahoo.it

/************************************************************************
* This program is free software: you can redistribute it and/or modify  *
* it under the terms of the GNU General Public License as published by  *
* the Free Software Foundation, either version 3 of the License, or     *
* (at your option) any later version.                                   *
*                                                                       *
* This program is distributed in the hope that it will be useful,       *
* but WITHOUT ANY WARRANTY; without even the implied warranty of        *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
* GNU General Public License for more details.                          *
*                                                                       *
* You should have received a copy of the GNU General Public License     *
* along with this program.  If not, see <http://www.gnu.org/licenses/>. *
************************************************************************/

#include <stdio.h>        // due to printf()
#include <string.h>       // due to memset(), memcpy()
#include <limits.h>       // due to UINT_MAX, ...
#include <stdlib.h>       // due to malloc()


#define SIZE      10

// ------------ Define your custom struct ----------------------------
// The typedef keyword is used to enable friendly declarations of struct
// like in C++, see lines 26, 27 and 28
typedef struct{
    unsigned char c;                // 1 byte  +
    unsigned long long int lli;     // 8 bytes +
    unsigned short s;               // 2 bytes +
    unsigned int i;                 // 4 bytes = 15 bytes
} numbers;                          // 


// ------------ Function to test endianness --------------------------
int IsLittleEndian(){
  union {
    unsigned int i;
    unsigned char c[4];
  } bint = { 0x04030201 };

  return bint.c[0] == 1;
}



int main(){
  printf("---- FUN WITH STRUCT ----\n\n");


// ------------ Allocation -------------------------------------------
  int  i, size_of_members, count=0;
  numbers a_num;
  numbers n_num[SIZE];
  numbers * ptr_num;
  unsigned char * ptr, * s_num;


// ------------ Sizes ------------------------------------------------
  printf(">>> SIZE\n");
// The different sizes are due to memory padding, as explored in next section  
  size_of_members = sizeof(char)+sizeof(short)+sizeof(long long int)+sizeof(int);
  printf("Size of members            : %d = %d + %d + %d + %d\n", size_of_members,sizeof(char),sizeof(short),sizeof(long long int),sizeof(int));
  printf("Size of a struct           : %d\n", sizeof(a_num));
  printf("Size of padding bytes      : %d = %d - %d\n", sizeof(a_num)-size_of_members, sizeof(a_num), size_of_members);
  printf("Size of a struct array     : %d = %d * %d\n\n", sizeof(n_num),sizeof(a_num),SIZE);


// ------------ Hex dump and memory padding --------------------------
  printf(">>> HEX and PADDING\n");
// First we set to 0 every byte in the struct
// to clear out any possible pre-existing junk  
  memset(&a_num, 0, sizeof(a_num));
// Then we initialize with appropriate values to keep track of:
// - endianess
// - memory padding
  a_num.c   = 254;                       //               0xfe
  a_num.s   = USHRT_MAX-1;               //             0xfffe
  a_num.lli = ULLONG_MAX-1;              // 0xfffffffffffffffe
  a_num.i   = UINT_MAX-1;                //         0xfffffffe
// Simple way to get through the struct bytes by making use of a pointer. 
// You will see several zeros between the members (INTERNAL PADDING) and 
// maybe even at the end (EXTERNAL PADDING). No predictions can be made 
// on this behavior since padding is not standardized and strongly 
// machine- and compiler-dependent
  printf("Struct content             : c = %d   lli = %llu   s = %d   i = %u\n",a_num.c, a_num.lli, a_num.s, a_num.i);
  printf("Struct hex dump            : ");
  ptr = (unsigned char *) &a_num;
  for(i=0; i<sizeof(a_num); i++) printf("%02x ", ptr[i]);
  printf("\nMark long long int         : ");
  for(i=0; i<sizeof(a_num); i++) printf("%s ", 
    ( &ptr[i]>=(unsigned char *) &a_num.lli && &ptr[i]<(unsigned char *) &a_num.lli + sizeof(long long int) )?"xx":"--");
  printf("\n\n");
  

// ------------ Use of memset ----------------------------------------
  printf(">>> MEMSET\n");
// We can use memset to clear out the whole struct, using the 
// struct address as starting point and sizeof() as size, see line 46,
// or only a specific part (we choose to clear out the first 6 MSB of 
// long long int member which amounts to set it equal to short member) 
// by using the pointer and offsetting it to the desired position  
  ptr = ( (unsigned char *) &(a_num.lli) ) + 2*IsLittleEndian();        // brackets emphasizes order of operations
  memset(ptr, 0, 6*sizeof(char));                                  
  printf("Struct content             : c = %d   lli = %llu   s = %d   i = %u\n",a_num.c, a_num.lli, a_num.s, a_num.i);
  printf("Struct hex dump            : ");
  ptr = (unsigned char *) &a_num;
  for(i=0; i<sizeof(a_num); i++) printf("%02x ", ptr[i]);
  printf("\nMark long long int         : ");
  for(i=0; i<sizeof(a_num); i++) printf("%s ", 
    ( &ptr[i]>=(unsigned char *) &a_num.lli && &ptr[i]<(unsigned char *) &a_num.lli + sizeof(long long int) )?"xx":"--");
  printf("\n\n");


// ------------ Use of memcpy ----------------------------------------
  printf(">>> MEMCPY\n");
// Here we show two way of using memcpy():
// - copying : clean and compact way to duplicate struct content
  memcpy(&n_num[0], &a_num, sizeof(a_num));                       // equivalent to n_num[0]=a_num
  printf("Copied struct content      : c = %d   lli = %llu   s = %d   i = %u\n",n_num[0].c, n_num[0].lli, n_num[0].s, n_num[0].i);
  printf("Copied struct hex dump     : ");
  ptr = (unsigned char *) &n_num[0];
  for(i=0; i<sizeof(a_num); i++) printf("%02x ", ptr[i]);
  printf("\n");
// - serialization : store the struct byte-wise
  s_num = (unsigned char *) malloc(sizeof(a_num));
  memcpy(s_num, &a_num, sizeof(a_num));
  printf("Serialized struct hex dump : ");
  for(i=0; i<sizeof(a_num); i++) printf("%02x ", s_num[i]);
  printf("\n\n");


// ------------ Struct and Pointers ----------------------------------
  printf(">>> STRUCT POINTERS\n");
// The serialized struct can be still easily accessed by using a pointer
  ptr_num = (numbers *) s_num;
  printf("Serialized struct content  : c = %d   lli = %llu   s = %d   i = %u\n",ptr_num->c, ptr_num->lli, ptr_num->s, ptr_num->i);

  return 0;
}
