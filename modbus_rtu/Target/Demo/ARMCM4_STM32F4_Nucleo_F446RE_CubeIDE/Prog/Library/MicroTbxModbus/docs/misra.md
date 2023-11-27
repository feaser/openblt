# MISRA compliance

Static code analysis was performed to verify compliance with MISRA-C 2012. This document lists the compliance exceptions:

## Global

| Directive |   Type   | Rationale                                                    |
| :-------: | :------: | :----------------------------------------------------------- |
|    2.5    | advisory | Especially in reusable modules or peripheral drivers, macro definitions<br/>can remain unused in the module or driver itself, but should be kept<br/>for the end-user. For example version macros and configuration options. |
|   11.5    | advisory | Conversions from pointer to void to pointer to other type. This is needed<br>after allocating memory from the heap and then initializing a pointer to<br>point to this allocated memory. Used for example when allocating memory<br>to build a linked list. |

