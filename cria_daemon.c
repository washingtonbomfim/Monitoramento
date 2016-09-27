#include <stdlib.h>
#include <signal.h>
#include <sys/resource.h>
#include <fcntl.h>

void daemonize(void) {
    int i = 0;
    pid_t pid = 0;
    struct rlimit rlimit;
    
    pid = fork();
    if (pid >= 0) {
        if (pid != 0) { 
            exit(0);
        }
    } else {
        exit(1);
    }
   
    if (setsid() < 0) {
        exit(1);
    }
   
    if ((signal(SIGHUP, SIG_IGN) == SIG_ERR)) {
        exit(1);
    }
    
    pid = fork();
    if (pid >= 0) {
        if (pid != 0) {
            exit(0);
        }
    } else {
        exit(1);
    }
    umask(0);
    
    //if (chdir("/") < 0) {
      //  exit(1);
    //}

    if (getrlimit(RLIMIT_NOFILE, &rlimit) < 0) {
        exit(1);
    }
    for (i = 0; i < rlimit.rlim_max; i++) {
        close(i);
    }
   
    open("/dev/null", O_RDONLY);
    open("/dev/null", O_RDWR);
    open("/dev/null", O_RDWR);
}