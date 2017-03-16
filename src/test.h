/*===================================================================================================
  File:                    test.h
  Author:                  Jonathan Livingstone
  Email:                   seagull127@ymail.com
  Licence:                 Public Domain
                           No Warranty is offered or implied about the reliability,
                           suitability, or usability
                           The use of this code is at your own risk
                           Anyone can use this code, modify it, sell it to terrorists, etc.
  ===================================================================================================*/

#if !defined(_TEST_H)

#if INTERNAL
    Void run_tests();
#else
    #define run_tests(...) {}
#endif

#define _TEST_H
#endif
