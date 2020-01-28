#if ENABLE_MEMORY_TRACING == 1

#if !defined(DO_NOT_REDEFINE_NEW)
#	define new new (__FILE__, __LINE__)
#endif // DO_NOT_REDEFINE_NEW

#endif // ENABLE_MEMORY_TRACING
