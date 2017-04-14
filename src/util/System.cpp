/*
 * System.cpp
 *
 *  Created on: 2016年5月13日
 *      Author: 58
 */

#include <wedis/base/util/System.h>
#include <wedis/base/util/Log.h>
#include <wedis/base/util/SocketAPI.h>
#include <wedis/base/util/FileAPI.h>
#include <wedis/base/util/Tools.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>  /* for struct timeval */
#include <execinfo.h>  /* for backtrace */
#include <sys/resource.h> /* for rlimit */
#include <signal.h>

namespace wRedis {

//static inline unsigned long long get_tick_count()
//{
//	uint32_t lo, hi;
//	__asm__ __volatile__ (
//			"rdtscp" : "=a"(lo), "=d"(hi)
//			);
//	return ((unsigned long long)lo) | (((unsigned long long)hi) << 32);
//}

pid_t System::pid() {
//	char **p = (char**)pthread_self();
//		return p ? *(pid_t*)(p + 18) : getpid();
	return ::getpid();
}

ThreadID System::currentThreadId() {
	return pthread_self();
}

I32 System::cpuCount() {
	return 1;
}

I32 System::memorySize() {
	return 1;
}

void System::sleep(TIMET millionseconds/* = 0*/) {
	timeval val;
	val.tv_sec = 0;
	val.tv_usec = millionseconds * 1000;
	SocketAPI::select_ex(0, NULLPTR, NULLPTR, NULLPTR, &val);
}

void System::dumpStack(const I8* reason) {
	Logger.error("reason:%s", reason);
	void* dumpArray[256] = {0};
	I32	nSize =	backtrace(dumpArray,256);
	I8** symbols = backtrace_symbols(dumpArray, nSize);
	if(symbols) {
		if(nSize>256) nSize= 256;
		if(nSize>0) {
			for(I32	i=0; i < nSize; i++) {
				if(symbols[i]) {
					Logger.error("%s", symbols[i]);
				}
			}
		}
		W_FREE(symbols);
	}
}

void System::setRLimitFileSize(const I32 maxFd) {
	struct rlimit rlim;
	memset(&rlim, 0, sizeof(rlim));
	if (getrlimit(RLIMIT_NOFILE, &rlim) == 0) {
		if ((int)rlim.rlim_max < maxFd) {
			rlim.rlim_cur = rlim.rlim_max;
	        setrlimit(RLIMIT_NOFILE, &rlim);
	        rlim.rlim_cur = maxFd;
	        rlim.rlim_max = maxFd;
	        setrlimit(RLIMIT_NOFILE, &rlim);
		}
	}
}

UI32 System::getIP(const I8* ifstr) {
	// 还有另一种方法
//	// define ifaddrs
//	struct ifaddrs *ifaddr, *ifa;
//	// get ifaddrs
//	if (getifaddrs(&ifaddr) == INVALID_VALUE) {
//		return 0;
//	}

	I8 ifname[16] = {0};
	if (!ifstr) {
		Tools::Strncpy(ifname, "eth0", sizeof(ifname) - 1);
	}
	else if(0 == Tools::Strncmp(ifname, "all", sizeof(ifname))) {
		// all IP 是 0.0.0.0
		return 0U;
	}
	else {
	     Tools::Strncpy(ifname, ifstr, sizeof(ifname) -1);
	}

	register I32 fd, intrface;
	struct ifreq buf[10];
	struct ifconf ifc;
	UI32 ip = 0;

	if ((fd = SocketAPI::socket_ex(AF_INET, SOCK_DGRAM, 0)) >= 0) {
		ifc.ifc_len = sizeof(buf);
	    ifc.ifc_buf = (caddr_t)buf;

	    if (FileAPI::ioctl_ex(fd, SIOCGIFCONF, (I8*)&ifc)) {
	    	intrface = ifc.ifc_len / sizeof(struct ifreq);

	        while (intrface-- > 0) {
	        	if (Tools::Strcmp(buf[intrface].ifr_name, ifname) == 0) {
	        		if ((FileAPI::ioctl_ex(fd, SIOCGIFADDR, (I8*)&buf[intrface])))
	                    ip = (unsigned)((SOCKADDR_IN *)(&buf[intrface].ifr_addr))->sin_addr.s_addr;

	                break;
	            }
	        }//end of while
	    }

	    SocketAPI::closesocket_ex(fd);
	}

	return ip;
}


I8* System::getEth0IP() {
	static I8 g_s_eth0_ip[MAX_IP_SIZE] = {0};

	if(0 == g_s_eth0_ip[0]) {
		printf("-----------------------------------------------\n");
		struct in_addr addr;
		addr.s_addr = getIP("eth0");
		I8* tmpIP = ::inet_ntoa(addr);
		Tools::Strncpy(g_s_eth0_ip, tmpIP, MAX_IP_SIZE - 1);
	}

	return g_s_eth0_ip;
}

BOOL System::checkProcessExist(pid_t pid) {
	I32 ret = ::kill(pid, 0);
	return (ret == 0 || errno != ESRCH);
}


}
