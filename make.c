#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "plz.h"

void makeCardfile()
{
   FILE * fp = fopen("card.txt","w+b");
   char zero[4] = {0};
   card info[91];
   int i;

   fwrite(zero,sizeof(zero),1,fp);

   fputc(1,fp); fputc(0,fp); fputc(1,fp); fputc(0,fp);
   fputc(2,fp); fputc(0,fp); fputc(0,fp); fputc(0,fp);
   fputc(3,fp); fputc(0,fp); fputc(0,fp); fputc(0,fp);
   fputc(4,fp); fputc(0,fp); fputc(0,fp); fputc(0,fp);
   fputc(5,fp); fputc(0,fp); fputc(0,fp); fputc(0,fp);

   fputc(6,fp); fputc(1,fp); fputc(3,fp); fputc(0,fp);
   fputc(7,fp); fputc(2,fp); fputc(4,fp); fputc(0,fp);
   fputc(8,fp); fputc(3,fp); fputc(4,fp); fputc(0,fp);
   fputc(9,fp); fputc(4,fp); fputc(4,fp); fputc(0,fp);
   fputc(10,fp); fputc(1,fp); fputc(3,fp); fputc(0,fp);

   fputc(11,fp); fputc(1,fp); fputc(1,fp); fputc(0,fp);
   fputc(12,fp); fputc(1,fp); fputc(14,fp); fputc(0,fp);
   fputc(13,fp); fputc(1,fp); fputc(15,fp); fputc(0,fp);
   fputc(14,fp); fputc(1,fp); fputc(16,fp); fputc(0,fp);
   fputc(15,fp); fputc(1,fp); fputc(17,fp); fputc(0,fp);
   fputc(16,fp); fputc(1,fp); fputc(18,fp); fputc(0,fp);
   fputc(17,fp); fputc(1,fp); fputc(19,fp); fputc(0,fp);
   fputc(18,fp); fputc(1,fp); fputc(20,fp); fputc(0,fp);
   fputc(19,fp); fputc(1,fp); fputc(21,fp); fputc(0,fp);
   fputc(20,fp); fputc(1,fp); fputc(22,fp); fputc(0,fp);
   fputc(21,fp); fputc(1,fp); fputc(23,fp); fputc(0,fp);
   fputc(22,fp); fputc(1,fp); fputc(24,fp); fputc(0,fp);
   fputc(23,fp); fputc(1,fp); fputc(25,fp); fputc(0,fp);
   fputc(24,fp); fputc(1,fp); fputc(26,fp); fputc(0,fp);
   fputc(25,fp); fputc(1,fp); fputc(27,fp); fputc(0,fp);
   fputc(26,fp); fputc(1,fp); fputc(38,fp); fputc(0,fp);
   fputc(27,fp); fputc(1,fp); fputc(39,fp); fputc(0,fp);
   fputc(28,fp); fputc(1,fp); fputc(41,fp); fputc(0,fp);
   fputc(29,fp); fputc(1,fp); fputc(42,fp); fputc(0,fp);
   fputc(30,fp); fputc(1,fp); fputc(43,fp); fputc(0,fp);
   fputc(31,fp); fputc(1,fp); fputc(44,fp); fputc(0,fp);
   fputc(32,fp); fputc(1,fp); fputc(45,fp); fputc(0,fp);
   fputc(33,fp); fputc(1,fp); fputc(46,fp); fputc(0,fp);
   fputc(34,fp); fputc(1,fp); fputc(47,fp); fputc(0,fp);
   fputc(35,fp); fputc(1,fp); fputc(48,fp); fputc(0,fp);

   fputc(36,fp); fputc(2,fp); fputc(2,fp); fputc(0,fp);
   fputc(37,fp); fputc(2,fp); fputc(3,fp); fputc(0,fp);
   fputc(38,fp); fputc(2,fp); fputc(4,fp); fputc(0,fp);
   fputc(39,fp); fputc(2,fp); fputc(5,fp); fputc(0,fp);
   fputc(40,fp); fputc(2,fp); fputc(6,fp); fputc(0,fp);
   fputc(41,fp); fputc(2,fp); fputc(7,fp); fputc(0,fp);
   fputc(42,fp); fputc(2,fp); fputc(8,fp); fputc(0,fp);
   fputc(43,fp); fputc(2,fp); fputc(40,fp); fputc(0,fp);
   fputc(44,fp); fputc(2,fp); fputc(49,fp); fputc(0,fp);
   fputc(45,fp); fputc(2,fp); fputc(50,fp); fputc(0,fp);
   fputc(46,fp); fputc(2,fp); fputc(51,fp); fputc(0,fp);
   fputc(47,fp); fputc(2,fp); fputc(52,fp); fputc(0,fp);

   fputc(48,fp); fputc(3,fp); fputc(36,fp); fputc(0,fp);

   fputc(49,fp); fputc(4,fp); fputc(14,fp); fputc(0,fp);
   fputc(50,fp); fputc(4,fp); fputc(26,fp); fputc(0,fp);

   fputc(51,fp); fputc(5,fp); fputc(11,fp); fputc(0,fp);
   fputc(52,fp); fputc(5,fp); fputc(25,fp); fputc(0,fp);
   fputc(53,fp); fputc(5,fp); fputc(47,fp); fputc(0,fp);

   fputc(54,fp); fputc(6,fp); fputc(9,fp); fputc(0,fp);
   fputc(55,fp); fputc(6,fp); fputc(9,fp); fputc(0,fp);

   fputc(56,fp); fputc(7,fp); fputc(29,fp); fputc(0,fp);

   fputc(57,fp); fputc(8,fp); fputc(21,fp); fputc(0,fp);
   fputc(58,fp); fputc(8,fp); fputc(27,fp); fputc(0,fp);
   fputc(59,fp); fputc(8,fp); fputc(37,fp); fputc(0,fp);
   fputc(60,fp); fputc(8,fp); fputc(38,fp); fputc(0,fp);

   fputc(61,fp); fputc(9,fp); fputc(22,fp); fputc(0,fp);
   fputc(62,fp); fputc(9,fp); fputc(23,fp); fputc(0,fp);
   fputc(63,fp); fputc(9,fp); fputc(24,fp); fputc(0,fp);
   fputc(64,fp); fputc(9,fp); fputc(39,fp); fputc(0,fp);

   fputc(65,fp); fputc(10,fp); fputc(32,fp); fputc(0,fp);
   fputc(66,fp); fputc(10,fp); fputc(33,fp); fputc(0,fp);
   fputc(67,fp); fputc(10,fp); fputc(34,fp); fputc(0,fp);
   fputc(68,fp); fputc(10,fp); fputc(35,fp); fputc(0,fp);
   fputc(69,fp); fputc(10,fp); fputc(36,fp); fputc(0,fp);
   fputc(70,fp); fputc(10,fp); fputc(37,fp); fputc(0,fp);

   fputc(71,fp); fputc(11,fp); fputc(31,fp); fputc(0,fp);

   fputc(72,fp); fputc(12,fp); fputc(12,fp); fputc(0,fp);
   fputc(73,fp); fputc(12,fp); fputc(48,fp); fputc(0,fp);

   fputc(74,fp); fputc(100,fp); fputc(10,fp); fputc(1,fp);
   fputc(75,fp); fputc(100,fp); fputc(49,fp); fputc(1,fp);
   fputc(76,fp); fputc(100,fp); fputc(13,fp); fputc(2,fp);
   fputc(77,fp); fputc(100,fp); fputc(50,fp); fputc(2,fp);
   fputc(78,fp); fputc(100,fp); fputc(51,fp); fputc(2,fp);
   fputc(79,fp); fputc(100,fp); fputc(52,fp); fputc(3,fp);
   fputc(80,fp); fputc(100,fp); fputc(40,fp); fputc(4,fp);
   fputc(81,fp); fputc(100,fp); fputc(8,fp); fputc(5,fp);

   fputc(82,fp); fputc(101,fp); fputc(34,fp); fputc(0,fp);
   fputc(83,fp); fputc(101,fp); fputc(35,fp); fputc(0,fp);

   fputc(84,fp); fputc(102,fp); fputc(1,fp); fputc(0,fp);

   fputc(85,fp); fputc(103,fp); fputc(12,fp); fputc(0,fp);
   fputc(86,fp); fputc(103,fp); fputc(13,fp); fputc(0,fp);

   fputc(87,fp); fputc(104,fp); fputc(10,fp); fputc(0,fp);
   fputc(88,fp); fputc(104,fp); fputc(11,fp); fputc(0,fp);
   fputc(89,fp); fputc(104,fp); fputc(30,fp); fputc(0,fp);

   fputc(90,fp); fputc(105,fp); fputc(28,fp); fputc(0,fp);

}

void read_cards()
{
   FILE * fp1 = fopen("card.txt", "r+b");
   card info[91];
   int i;

   makeCardfile();

   fread(&info[0].num,1,1,fp1);
   fread(&info[0].type,1,1,fp1);
   fread(&info[0].shape,1,1,fp1);
   fread(&info[0].range,1,1,fp1);

   for(i=1;i<91;i++)
   {
      fread(&info[i].num,1,1,fp1);
      fread(&info[i].type,1,1,fp1);
      fread(&info[i].shape,1,1,fp1);
      fread(&info[i].range,1,1,fp1);
   }

   fclose(fp1);
   return ;
}

int main(void)
{
	makeCardfile();
}
