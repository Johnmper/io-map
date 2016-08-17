#include <stdio.h>
#include <stdlib.h>

#if defined(_WIN32)
   #include <windows.h>
#endif

void imap( void *data, size_t data_size, const char * filename );

void omap( const void *data, size_t data_size, const char * filename );
