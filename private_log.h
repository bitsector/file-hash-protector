#ifndef PRIVATE_LOG_H
#define PRIVATE_LOG_H


#include <syslog.h>

#define BLACK "\033[30m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define ORANGE "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define GRAY "\033[37m"
#define DEF "\033[39m"

#define BLACK_BG "\033[40m"
#define RED_BG "\033[41m"
#define GREEN_BG "\033[42m"
#define ORANGE_BG "\033[43m"
#define BLUE_BG "\033[44m"
#define MAGENTA_BG "\033[45m"
#define CYAN_BG "\033[46m"
#define GRAY_BG "\033[47m"
#define DEF_BG "\033[49m"



#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define LOGE(...)  printf(ORANGE"%s"DEF":"RED" %s()"DEF":"BLUE" %d"DEF":", __FILENAME__,__func__,__LINE__  );\
					printf(__VA_ARGS__); \
					syslog(LOG_WARNING,__VA_ARGS__); \
					printf("\r\n");
					
#define LOGS(...) if (DBG) { LOGE(__VA_ARGS__) }

#define LOG(...) printf(__VA_ARGS__); \
					syslog(LOG_WARNING,__VA_ARGS__); \
					printf("\r\n");

#define LOG_ATTENTION(...) printf(RED); \
							printf(__VA_ARGS__); \
							printf(DEF); \
							syslog(LOG_WARNING,__VA_ARGS__); \
							printf("\r\n");

#define LOG_OK(...) printf(GREEN); \
							printf(__VA_ARGS__); \
							printf(DEF); \
							syslog(LOG_WARNING,__VA_ARGS__); \
							printf("\r\n");

#endif  // PRIVATE_LOG
