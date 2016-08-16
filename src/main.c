


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iomap.h"

struct full_date
{
   unsigned char day;      // 0<->255
   unsigned char month;    // 0<->255
   short int year;         // -32768<->32767
}; // 4bytes long

struct user_data
{
   unsigned int id;  // 4 bytes
   struct full_date birthday; // 4 bytes
   char name[101];   // 101 bytes
}; // 112 bytes long (padding)

int main( int argc, const char *argv[] )
{
   struct user_data user;

   if( argc != 3 )
   {
      fprintf(stderr,"iomap: illegal option\n"
                     "usage: iomap [-io] <file>\n"
                     "example:\n"
                     " iomap -i \"storage.dat\"\n");
   }
   else if( argc == 3 && (strcmp( "-o", argv[1] ) == 0) )
   {
      fprintf(stderr,"HERE\n");
      user.id = 1001334;
      user.birthday.day = 13;
      user.birthday.month = 13;
      user.birthday.year = 500;
      strcpy(user.name, "Robert Jordan");

      fprintf(stdout,"Name: %s\n"
                     "Id: %u\n"
                     "Birthday: \n"
                     "  Day: %hu \n"
                     "  Month: %hu \n"
                     "  Year: %hd \n",
                     user.name, user.id,
                     (unsigned short)user.birthday.day,
                     (unsigned short)user.birthday.month,
                     user.birthday.year );

      omap( (void*)&user, sizeof(struct user_data), argv[2] );
   }
   else if( argc == 3 && (strcmp( "-i", argv[1]) == 0) )
   {
      fprintf(stderr,"HERE2\n");
      imap( &(user), sizeof(struct user_data), argv[2] );

      fprintf(stdout,"Name: %s\n"
                     "Id: %u\n"
                     "Birthday: \n"
                     "  Day: %hu \n"
                     "  Month: %hu \n"
                     "  Year: %hd \n",
                     user.name, user.id,
                     (unsigned short)user.birthday.day,
                     (unsigned short)user.birthday.month,
                     user.birthday.year );
   }

   return 0;
}
