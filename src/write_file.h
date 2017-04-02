/*===================================================================================================
  File:                    write_file.h
  Author:                  Jonathan Livingstone
  Email:                   seagull127@ymail.com
  Licence:                 Public Domain
                           No Warranty is offered or implied about the reliability,
                           suitability, or usability
                           The use of this code is at your own risk
                           Anyone can use this code, modify it, sell it to terrorists, etc.
  ===================================================================================================*/

#if !defined(_WRTIE_FILE_H)

#include "shared.h"

struct File;
struct ParseResult;

File write_data(ParseResult parse_result);

#define _WRTIE_FILE_H
#endif
