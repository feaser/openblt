# MISRA compliance

Static code analysis was performed to verify compliance with MISRA-C 2012. This document lists the compliance exceptions:

## Global

| Directive |   Type   | Rationale                                                    |
| :-------: | :------: | :----------------------------------------------------------- |
|    2.5    | advisory | Especially in reusable modules or peripheral drivers, macro definitions<br/>can remain unused in the module or driver itself, but should be kept<br/>for the end-user. For example version macros and configuration options. |
|   11.5    | advisory | Conversions from pointer to void to pointer to other type. This is needed<br>after allocating memory from the heap and then initializing a pointer to<br>point to this allocated memory. Used for example when allocating memory<br>to build a linked list. |

## Incidental

The exceptions listed in this section only apply to specific incidents in the code.

### External linkage for API functions

A standalone library module such as this one contains API functions. They are meant to be called by the application that integrates and uses the module. As such, it can happen that an API function is not referenced outside the source-file (translation unit) that holds the function implementation. Yet, it should keep its external linkage, otherwise it won't serve its purpose as an API function anymore. 

For this reason, an exception was added regarding MISRA rule:

| Directive |   Type   | Rationale                                                    |
| :-------: | :------: | :----------------------------------------------------------- |
|    8.7    | advisory | API functions should keep external linkage, even if they are not<br>referenced outside their translation unit. |

Suppression of the messages related to this MISRA directive exception are marked with:

```c
/* cppcheck-suppress [misra-c2012-8.7,unmatchedSuppression] 
 * MISRA exception: External linkage for API functions.
 */
```

