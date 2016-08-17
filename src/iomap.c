#include "iomap.h"


//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
//#                                             ##
//##    ####   #####   #####   ##      ######   ##   ##      ######  ##  ##  ##  ##  ##  ##
//##   ##  ##  ##  ##  ##  ##  ##      ##       ##   ##        ##    ### ##  ##  ##   ####
//##   ######  #####   #####   ##      ####     ##   ##        ##    ## ###  ##  ##    ##
//##   ##  ##  ##      ##      ##      ##       ##   ##        ##    ##  ##  ##  ##   ####
//##   ##  ##  ##      ##      ######  ######   ##   ######  ######  ##  ##   ####   ##  ##
//#                                             ##
//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
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

   data_tmp = mmap( (caddr_t)0, file_stats.st_size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0 );
   if( data_tmp == (caddr_t)(-1) )
   {
      perror("Failed to map file into memory.\n\t");
      close(fd);
      exit(EXIT_FAILURE);
   }

   memcpy( data, data_tmp, data_size );

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

   if( munmap( (caddr_t)data_tmp, data_size ) < 0 )
   {
      perror("Failed to un-map file from memory.\n\t");
      close(fd);
      exit(EXIT_FAILURE);
   }

   close(fd);

}





//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
//#
//##    ##   ##  ######  ##  ##  #####    ####   ##   ##   ####
//##    ##   ##    ##    ### ##  ##  ##  ##  ##  ##   ##  ##
//##    ## # ##    ##    ## ###  ##  ##  ##  ##  ## # ##   ####
//##    #######    ##    ##  ##  ##  ##  ##  ##  #######      ##
//##     ## ##   ######  ##  ##  #####    ####    ## ##    ####
//#
//# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#elif defined(_WIN32)

#include <windows.h>
#include <direct.h>
#include <tchar.h>

// Note:
//    mmap() is a posix function, works on Linux, MacOS X, HP-UX, AIX, and Solaris.
//
//    Alternative for windows: MapViewOfFile().


// Todo:
//    add FormatMessage() support for error handling.

void imap( void *data, size_t data_size, const char * filename )
{
   BOOL error_flag;
   LPVOID mapped_data;
   HANDLE hfile, hmap;
   DWORD file_size;


   hfile = CreateFile(filename,
                     GENERIC_READ | GENERIC_WRITE,
                     FILE_SHARE_READ | FILE_SHARE_WRITE,
                     NULL,
                     OPEN_EXISTING,
                     FILE_ATTRIBUTE_NORMAL,
                     FILE_FLAG_SEQUENTIAL_SCAN);
   if( hfile == INVALID_HANDLE_VALUE)
   {
      _tprintf( TEXT("Failed to open file.\n") );
      exit(EXIT_FAILURE);
   }

   hmap = CreateFileMapping(hfile,
                           NULL,
                           PAGE_READWRITE,
                           0,
                           file_size,
                           NULL);
   if( hmap == NULL )
   {
      _tprintf( TEXT("File creation for mapping failed.\n") );
      exit(EXIT_FAILURE);
   }

   data_mapped = MapViewOfFile(hmap,
                              FILE_MAP_ALL_ACCESS,
                              0,
                              0,
                              data_size);
   if( data_mapped == NULL )
   {
      _tprintf( TEXT("Mapping of view file failed.\n") );
      exit(EXIT_FAILURE);
   }

   memcpy( data_mapped, data, data_size );

   if( !UnmapViewOfFile( data_mapped ) )
   {
      _tprintf( TEXT("Unmapping of view file failed.\n") );
      exit(EXIT_FAILURE);
   }

   if( !CloseHandle( hmap ) )
   {
      _tprintf( TEXT("Closing mapping object.\n") );
      exit(EXIT_FAILURE);
   }

   if( !CloseHandle( hfile ) )
   {
      _tprintf( TEXT("Closing file handle.\n") );
      exit(EXIT_FAILURE);
   }

}

void omap( const void *data, size_t data_size, const char * filename )
{
   BOOL error_flag;
   LPVOID mapped_data;
   HANDLE hfile, hmap;
   DWORD file_end, bytes_written;

   hfile = CreateFile(filename,
                     GENERIC_READ | GENERIC_WRITE,
                     FILE_SHARE_READ | FILE_SHARE_WRITE,
                     NULL,
                     CREATE_ALWAYS,
                     FILE_ATTRIBUTE_NORMAL,
                     FILE_FLAG_SEQUENTIAL_SCAN);
   if( hfile == INVALID_HANDLE_VALUE)
   {
      _tprintf( TEXT("Failed to open file.\n") );
      exit(EXIT_FAILURE);
   }

   file_end = SetFilePointer( hfile,
                              data_size-1,
                              NULL,
                              FILE_BEGIN);
   if( file_end == INVALID_SET_POINTER )
   {
      _tprintf( TEXT("Failed tochange file size.\n") );
      CloseHandle(hfile);
      exit(EXIT_FAILURE);
   }

   WriteFile(hfile, "", 1, &bytes_written, NULL );

   hmap = CreateFileMapping(hfile,
                           NULL,
                           PAGE_READWRITE,
                           0,
                           data_size,
                           NULL);
   if( hmap == NULL )
   {
      _tprintf( TEXT("File creation for mapping failed.\n") );
      exit(EXIT_FAILURE);
   }

   data_mapped = MapViewOfFile(hmap,
                              FILE_MAP_ALL_ACCESS,
                              0,
                              0,
                              data_size);
   if( data_mapped == NULL )
   {
      _tprintf( TEXT("Mapping of view file failed.\n") );
      exit(EXIT_FAILURE);
   }

   memcpy( data_mapped, data, data_size );

   if( !UnmapViewOfFile( data_mapped ) )
   {
      _tprintf( TEXT("Unmapping of view file failed.\n") );
      exit(EXIT_FAILURE);
   }

   if( !CloseHandle( hmap ) )
   {
      _tprintf( TEXT("Closing mapping object.\n") );
      exit(EXIT_FAILURE);
   }

   if( !CloseHandle( hfile ) )
   {
      _tprintf( TEXT("Closing file handle.\n") );
      exit(EXIT_FAILURE);
   }


}



#else
   #error "Current OS not supported."
#endif
