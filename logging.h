
#ifdef LOGGING

#define LOG(...) printf("\e[33mLOG:\e[39m\t"); printf(__VA_ARGS__)

#else
#define LOG(...) 
#endif