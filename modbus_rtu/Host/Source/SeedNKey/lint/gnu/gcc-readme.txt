
                        Using FlexeLint with GCC

These notes describe how to use the option file co-gcc.lnt which serves
as a replacement for the older co-gnu.lnt and co-gnu3.lnt options files.

                           --- Quick Start ---

If you just want a quick-and-dirty Lint configuration (one that enables
you to use Standard Library headers and system headers) then use the
makefile 'co-gcc.mak' (supplied with the Lint distribution) in an
invocation of GNU Make like so:

        make -f co-gcc.mak

... which should generate the following files (provided that 'gcc', 'g++'
and 'awk' are each found in your $PATH):

        lint_cmac.h
        lint_cppmac.h
        gcc-include-path.lnt
        size-options.lnt

(co-gcc.lnt depends on these files, so they must be generated before you
do anything else.)

That's it!  You should now be able to run:

        flint  co-gcc.lnt  [source files]

Note that Lint must be able to find co-gcc.lnt and the files referenced
therein.  If you want to keep them in a separate directory (e.g.,
/usr/local/etc/flint) from the one where you will invoke Lint (e.g.,
~/some-project/src), you'll need to specify the former with a '-i' option
so that Lint will know where to look.  In that case, the invocation would
look like:

        flint -i /usr/local/etc/flint  co-gcc.lnt  [source files]

If your project is compiled without any explicit command-line switches
other than '-c' then this invocation alone might be all that you need to
get started linting your project.  Otherwise it probably won't suffice.
For a Lint configuration that better matches the way you compile your
code, you should at least read the 'usage' note at the top of co-gcc.mak.

For a motivation for the contents of co-gcc.mak, and to better understand
the configuration issues in general, please read on.

                           --- Introduction ---

The configuration of Lint for use with GCC-compiled sources is complicated
by two major issues:

First, GCC defines a large number of preprocessor macros internally (i.e.,
it defines a lot of macros for which there are no '#define' directives in
any source file that Lint can read.).  Unless this issue is resolved for
each project, Lint will not see the same sequence of C/C++ tokens as GCC
when it tries to analyze your program, and as a result it will not see the
same set of declarations; consequently you'll see lots of spurious
messages because the analysis will reflect that of a program that is
truly ill-formed (unlike the program seen by GCC).

Second, GCC is aware of several built-in functions.  Most of them can be
presented to Lint as ordinary forward-declarations of functions so as to
avoid undesired diagnostics claiming that these functions were not
declared before the first point of use.  Note, these declarations do not
necessarily need to be presented to GCC.

                 --- Solving the Preprocessor Problem ---

Before we get started, we'll need an empty C source file.  Call it
'empty.c' and save it to disk.  Next, use your favorite command
interpreter environment (such as sh in Unix-like environments or cmd.exe
on Windows) and go to the directory containing 'empty.c'.  Verify that you
can run GCC by doing the following:

        gcc -v

If your environment is properly set up you should see a version string.
(You'll also want to verify that this is the same GCC executable used to
compile your sources; check the PATH environment variable or try running
'which gcc'.)

Assuming that worked, you can now try the following (and note that case,
as with C/C++ identifiers, is important.):

        gcc -E -dM empty.c

On one system, we then see definitions like:

        #define __DBL_MIN_EXP__ (-1021)
        #define __FLT_MIN__ 1.17549435e-38F
        #define __DEC64_DEN__ 0.000000000000001E-383DD

... followed by about a hundred more '#define' directives.  What just
happened?  We passed two options to gcc; the first one, '-E', means,
"don't run the compiler; just run the preprocessor".  The option '-dM'
(not to be confused with '-DM', which is completely different) means,
"don't generate preprocessor output; instead, only dump all macro
definitions, including those defined internally".

With this output, we are now halfway to the point of having a preprocessor
configuration for Lint.  First, let's redirect those macros to a file:

        gcc -E -dM empty.c >lint_cmac.h

Next, let's test our configuration:  make a simple C source file
containing the "Hello, world" program in a file called 't.c' (in the same
directory as 'empty.c')

        #include <stdio.h>
        int main () {
            printf( "hello, world!\n" );
        }

To lint this program, first copy the lnt file (co-gcc.lnt) and the
associated header (co-gcc.h) into the same directory.  Next, create two
new empty files:

        size-options.lnt
        gcc-include-path.lnt

... and save them to disk.  Then make a file called 'std.lnt' which will
(at first) contain only:

        co-gcc.lnt

This tells Lint, "process the arguments in co-gcc.lnt".  Two of those
arguments are:

        -header(co-gcc.h) // #include's headers generated by GCC.
        +libh(co-gcc.h)   // Marks co-gcc.h as library code.

'-header(co-gcc.h)' means "behave as if each primary source file began
with '#include "co-gcc.h"'.  (Note that co-gcc.h includes "lint_cmac.h"
when Lint runs in C language mode.)

Finally, try running:

        /path/to/flint std.lnt t.c

... or, on Windows:

        [drive-letter]:\path\to\lint-nt std.lnt t.c

Next we'll see output similar to the following:

FlexeLint for C/C++ (Unix/386) Vers. 9.00c, Copyright Gimpel Software
1985-2009

--- Module:   t.c (C)
         _
#include <stdio.h>
         t.c  3  Error 322: Unable to open include file 'stdio.h'

What went wrong?  The problem is that we haven't yet addressed  the other
half of the preprocessor configuration, namely: the ordered sequence of
directories to search for system headers.  Fortunately GCC already knows
this list and gives us a way to discover it.  Just run:

        gcc -c -v empty.c

On one system (specifically, Mac OS X on Intel), that yields a lot of
verbose output including these lines:

        #include "..." search starts here:
        #include <...> search starts here:
         /usr/local/include
         /usr/lib/gcc/i686-apple-darwin8/4.0.1/include
         /usr/include
         /System/Library/Frameworks
         /Library/Frameworks
        End of search list.

(Naturally, that list will look a bit different in different
environments.)  To make Lint search for system headers in the same way, we
need to take that output from GCC and use it to make a set of options of
the form --i[directory].  E.g. on the same system, that means that in
gcc-include-path.lnt, I should place the following --i options:

        --i/usr/local/include
        --i/usr/lib/gcc/i686-apple-darwin8/4.0.1/include
        --i/usr/include
        --i/System/Library/Frameworks
        --i/Library/Frameworks

(Note, arguments to Lint are processed in order, so the --i options must
appear before t.c or they will not take effect in time.)

In this case, none of the directory names contains a space, but if one of
them did we would have to surround its name with quotes as in:

        --i"/usr/local/include"

Now let's try linting "hello, world" again:

        /path/to/flint std.lnt t.c

This time we should see no error messages.  Are we done yet?  Not quite.
First, let's test our configuration against all of the C Standard Library
headers.  Modify t.c to:

        #include <assert.h>
        #include <iso646.h>
        #include <setjmp.h>
        #include <stdio.h>
        #include <time.h>
        #include <ctype.h>
        #include <limits.h>
        #include <signal.h>
        #include <stdlib.h>
        #include <wchar.h>
        #include <errno.h>
        #include <locale.h>
        #include <stdarg.h>
        #include <string.h>
        #include <wctype.h>
        #include <float.h>
        #include <math.h>
        #include <stddef.h>

        int main () {
            printf( "hello, world!\n" );
        }


... and try linting again:

        /path/to/flint std.lnt t.c

This time you should see a number of messages; in particular, you should
see Info 766 issued for most of these headers since they were not used.
you may also see some legitimate warnings, and you may see an Error or two
about 'wchar_t'.  (Note, if it seems as though your port of GCC recognizes
wchar_t as a keyword instead of an identifier then uncomment the use of
'+fwc' in your copy of co-gcc.lnt).

[NOTE: we should point out that the option -wlib(1) is in co-gcc.lnt for a
reason.  Please note that we *strongly* recommend against the use of
options like -wlib(0) or -elib(*).  If you see syntax error messages about
library header code, odds are that something is wrong with the Lint
configuration; so you'll do yourself no favors by silencing Lint when it
sees constructs that Lint's parser doesn't know how to handle.  If you need
help with correcting your configuration, please check the Lint manual or
contact us.]

We should really be done now, right?  Well, not quite.  A remaining issue
is that the set of predefined macros (dumped into "lint_cmac.h" earlier)
depends not only on things like the target system and the version of GCC;
it also depends on the options that you pass to GCC when you compile your
program.  E.g., if we invoke:

        gcc -O3 -E -dM empty.c  >lint_cmac.h

... then (with the version of GCC we tested) lint_cmac.h will no longer
contain a definition for the macro '__NO_INLINE__' and contains a new
definition for '__OPTIMIZE__'.  So when you generate macros, you should be
careful to pass in the full set of options used when you compile.
Ideally, you should establish a target in your build system that
re-generates the predefined macro header whenever your build configuration
changes; that way you'll seldom need to think about it again and Lint's
preprocessor configuration will always match the compiler's.

Assuming you've generated the right set of predefined macros for your
build settings, you should now try Linting a single primary source file in
your project.  Since we're just starting out, let's run with -w1 (i.e.,
with the warning level set to one) so that we can deal with any syntax
errors that pop up:

        flint std.lnt -u -w1 some-project-file.c

Again, remember that syntax errors at this stage are likely due to a
misconfiguration; please check the Lint manual for likely remedies or
contact us if the solution is not obvious.

Once you've taken care of all syntax errors, try doing the same with all
project files.  We recommend placing the name of each project file in a
.lnt file (often called project.lnt); e.g.:

        file1.c
        file2.c
        [...etc.]

... and invoke Lint like so:

        flint std.lnt -w1 project.lnt

When all syntax errors are resolved, you can begin turning on Warning
messages (i.e., those listed in section 17.4 of the Lint manual).  You can
turn them on individually after '-w1' or you can instead use '-w2' and
then suppress the Warnings that are less severe.  (For details on message
suppression, see section 5.2 of the Lint manual.)

At this point, you should be well-equipped to Lint any C program compiled
with GCC.  However, C++ users have a couple more points to consider.

The command used to invoke GCC also affects the set of predefined macros.
Observe the difference in '#define' output when you invoke:

        g++ -O3 -E -dM empty.c >lint_cppmac.h
        diff lint_cmac.h lint_cppmac.h

Also, note that the list of directories to search for Standard Library
headers has some new additions when you use 'g++ -v -c empty.c' instead of
'gcc -v -c empty.c'.  Your sequence of --i options should be set
accordingly.

That's about it for the preprocessor.

                           --- Size options ---

Finally, you'll need to establish sizes of primitive types.  E.g. for a
64-bit platform this includes setting '-sp8' (indicating that pointers are
8 bytes wide).  It often also involves setting '-sl8' (indicating that
'long' is eight bytes wide).  As suggested earlier, the makefile
co-gcc.mak can generate these options for you.

If you find this tutorial to be lacking in some way, please contact us
(support@gimpel.com) with your suggestions for improvement.
