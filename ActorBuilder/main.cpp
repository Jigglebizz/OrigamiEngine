#include "Origami/pch.h"


int main( int argc, char* argv[] )
{
  int i_arg = 0;
  while ( i_arg < argc )
  {
    char* current_arg = argv[ argc ];
    if (strcmp(current_arg, "-source") == 0 )
    {
      i_arg++
    }

    printf("arg %d: %s\n", i_arg, argv[i_arg]);
    i_arg++;
  }
  return 0;
}