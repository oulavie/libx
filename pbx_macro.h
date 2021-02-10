#ifndef PBX_MACRO_H
#define PBX_MACRO_H

#define STRINGIFY(x) #x                            
#define TOSTRING(x) STRINGIFY(x)                  
#define AT " (" __FILE__ ":" TOSTRING(__LINE__) ")"

#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#endif

// vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent:

