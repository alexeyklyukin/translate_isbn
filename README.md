translate_isbn
==============

A PostgreSQL extension to convert between ISBN 10 and ISBN 13 numbers. It aims
to be compatible with translate_isbn1013 function from the Evergreen
open-source library software and is a rewrite of that function in C in an
attempt to improve its performance.

The notable compatibility exception is handling of codes with a valid checksum
but non-permitted group identifier or a publisher code, such as '9789997122575'.

The first issue is that Business::ISBN Perl library version 2.05_03 and below
returns Business::ISBN::BAD_CHECKSUM from is_valid_checksum for a code with a
valid check digit but invalid group or publisher code. The second problem is
that it considers a code as valid after the fix_checksum() call. As a result,
translate_isbn_10_13 perl script appends the version with the 'corrected' check
digit (which would be the same code as initial) together with the ISBN10
version for such codes. The actual output for the code above is:

 select public.translate_isbn1013('9789997122575');
           translate_isbn1013            
-----------------------------------------
 9789997122575 9789997122575 9997122577
(1 row)

The function provided by this module doesn't check the publisher code and group
identifier altogether, relying only on the check digit value. As a result, it
considers the code above correct and proceeds with conversion to ISBN10,
resulting in the following output:

 select public.translate_isbn1013('9789997122575');
           translate_isbn1013            
-----------------------------------------
 9789997122575 9997122577
(1 row)

Versions 2.05_04 of Business::ISBN fixed only the second problem
(https://github.com/briandfoy/business--isbn/issues/3) and still considers the
'9789997122575' invalid after the fix_checksum call. is_valid_checksum still
returns Business::ISBN::BAD_CHECKSUM there, preventing the conversion to ISBN10
to be performed, the result will be (note an extra space after the result code):

 select public.translate_isbn1013('9789997122575');
           translate_isbn1013            
-----------------------------------------
 9789997122575 
(1 row)

While compatibility issues listed below are quite serious, they don't matter at
all for the invocation of this function as a part of the search functionality
in the evergreen software: if the search term correspond to an invalid code the
search returns no results anyway.

If you need a thorough and feature-complete implementation of the ISBN data
type, please, refer to the
http://www.postgresql.org/docs/current/static/isn.html PostgreSQL contrib
module.

License:

translate_isbn extension is issued under the BSD license.
Commerical support is available from [Command Prompt, Inc](http://www.commandprompt.com). 
Please, address your requests at support@commandprompt.com
