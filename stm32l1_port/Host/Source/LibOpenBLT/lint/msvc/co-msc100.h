// ---------------------------------------------------------------------
// This file is provided by Gimpel Software (www.gimpel.com) for use with
// its products PC-lint and FlexeLint.
//
// Redistribution and use of this file, with or without modification, is
// permitted provided that any such redistribution retains this notice.
// ---------------------------------------------------------------------

#ifndef LINT_SUPPORT_CO_MSC100_H_
#define LINT_SUPPORT_CO_MSC100_H_

// co-msc100.h  --
// This header is automatically included in every module if you use
// co-msc100.lnt.  (There is a -header(co-msc100.h) option therein
// for this purpose.)

// Microsoft's C++11 implementation does not yet include built-in support
// for the new character types, so we need to disable the relevant
// keywords here.  (This must be done before the inclusion of any other
// header file because Microsoft's library headers contain declarations of
// typedef names with the same spelling.)
//lint -rw(char16_t,char32_t)

// Next we must compensate for the fact that class typeinfo is available
// for use in the MSC compilers without an explicit definition.  According
// to the standard this class definition is not (available by default).
#ifdef __cplusplus

#include <typeinfo>

#else // C mode

// co-msc100.lnt defines some macros that the Microsoft compiler defines
// in C++ modes but not in C mode; we must un-define them here:
#undef _CPPRTTI
#undef _NATIVE_WCHAR_T_DEFINED
#undef _WCHAR_T_DEFINED
#undef _NATIVE_NULLPTR_SUPPORTED

#endif

#endif /* LINT_SUPPORT_CO_MSC100_H_ */
