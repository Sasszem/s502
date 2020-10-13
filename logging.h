
#ifdef LOGGING

#define LOG(...) printf("\e[33mLOG:\e[39m\t"); printf(__VA_ARGS__)
#define LOGC(...) printf(__VA_ARGS__);

#else
#define LOG(...) 
#define LOGC(...)
#endif

#define ERROR(...) printf("\e[41mERROR\e[49m: "); printf(__VA_ARGS__)
#define ERRORC(...) printf(__VA_ARGS__);