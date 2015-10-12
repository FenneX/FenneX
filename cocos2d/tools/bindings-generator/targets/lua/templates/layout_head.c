\#include "${out_file}.hpp"
#if $macro_judgement
$macro_judgement
#end if 
#for header in $headers
\#include "${os.path.basename(header)}"
#end for
\#include "tolua_fix.h"
\#include "LuaBasicConversions.h"
#if $cpp_headers
#for header in $cpp_headers
\#include "${header}"
#end for
#end if 

