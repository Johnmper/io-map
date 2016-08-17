#include "iomap.h"


//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
//#                                             ##
//##    ####   #####   #####   ##      ######   ##   ##      ######  ##  ##  ##  ##  ##  ##
//##   ##  ##  ##  ##  ##  ##  ##      ##       ##   ##        ##    ### ##  ##  ##   ####
//##   ######  #####   #####   ##      ####     ##   ##        ##    ## ###  ##  ##    ##
//##   ##  ##  ##      ##      ##      ##       ##   ##        ##    ##  ##  ##  ##   ####
//##   ##  ##  ##      ##      ######  ######   ##   ######  ######  ##  ##   ####   ##  ##
//#                                             ##
//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#if defined(__linux__) || defined(__APPLE__)


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>



void imap( void *data, size_t data_size, const char * filename )
{

   int fd;
   const void * data_tmp;
   struct stat file_stats;


   fd = open( filename, O_RDWR , 0666);
   if( fd < 0 )
   {
      perror("Failed to open file.\n\t");
      exit(EXIT_FAILURE);
   }

   if( stat( filename, &file_stats ) < 0 )
   {
      perror("Failed to get file stats.\n\t");
      close(fd);
      exit(EXIT_FAILURE);
   }

   if( file_stats.st_size > data_size )
   {
      perror("File size bigger than object size.\n\t");
      exit(EXIT_FAILURE);
   }

   data_tmp = mmap( (caddr_t)0, file_stats.st_size, PROT_WRITE | PROT_READ, MAP_PRIVATE, fd, 0 );
   if( data_tmp == (caddr_t)(-1) )
   {
      perror("Failed to map file into memory.\n\t");
      close(fd);
      exit(EXIT_FAILURE);
   }

   memcpy( data, data_tmp, data_size );

   // Check error in copy
   if( memcmp( data, data_tmp, data_size ) != 0 )
   {
      perror("Failed to memory copy.\n\t");
      close(fd);
      exit(EXIT_FAILURE);
   }

   if( munmap( (caddr_t)data_tmp, data_size ) < 0 )
   {
      perror("Failed to un-map file from memory.\n\t");
      close(fd);
      exit(EXIT_FAILURE);
   }

   close( fd );

}

void omap( const void *data, size_t data_size, const char * filename )
{
   int fd, file_end;
   void * data_tmp;


   fd = open( filename, O_RDWR | O_CREAT, 0666 );
   if( fd < 0 )
   {
      perror("Failed to open file.\n\t");
      exit(EXIT_FAILURE);
   }


   file_end = lseek( fd, data_size-1, SEEK_SET );
   write( fd, "", 1);   // '\0' writed to end of file (changes file size)
   if( file_end < 0 )
   {
      perror("Failed to change file size.\n\t");
      close(fd);
      exit(EXIT_FAILURE);
   }


   data_tmp = mmap( (caddr_t)0, data_size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0 );
   if( data_tmp == (caddr_t)(-1) )
   {
      perror("Failed to map file into memory.\n\t");
      close(fd);
      exit(EXIT_FAILURE);
   }


   memcpy( data_tmp, data, data_size );


   // Check error in copy
   if( memcmp( data, data_tmp, data_size ) != 0)
   {
      perror("Failed to copy mapped memory.\n\t");
      close(fd);
      exit(EXIT_FAILURE);
   }


   if( munmap( (caddr_t)data_tmp, data_size ) < 0 )
   {
      perror("Failed to un-map file from memory.\n\t");
      close(fd);
      exit(EXIT_FAILURE);
   }

   close(fd);

}





//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
//#
//##    ##   ##  ######  ##  ##  #####    ####   ##   ##   ####
//##    ##   ##    ##    ### ##  ##  ##  ##  ##  ##   ##  ##
//##    ## # ##    ##    ## ###  ##  ##  ##  ##  ## # ##   ####
//##    #######    ##    ##  ##  ##  ##  ##  ##  #######      ##
//##     ## ##   ######  ##  ##  #####    ####    ## ##    ####
//#
//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#elif defined(_WIN32)

#include <windows.h>
#include <direct.h>


// Note:
//    mmap() is a posix function, works on Linux, MacOS X, HP-UX, AIX, and Solaris.
//
//    Alternative for windows: MapViewOfFile().

void imap( void *data, size_t data_size, const char * filename )
{
   
}

void omap( const void *data, size_t data_size, const char * filename )
{
   
}






#else
   #error "Current OS not supported."
#endif
