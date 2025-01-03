#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define BUF_SIZE 1024

struct proc_stat {
   int pid;
   char * comm;
   char state;
   int ppid;
   int pgrp;
   int session;
   int tty_nr;
   int tpgid;
   unsigned int flags;
   unsigned long int minflt;
   unsigned long int cminflt;
   unsigned long int majflt;
   unsigned long int cmajflt;
   unsigned long int utime;
   unsigned long int stime;
   long int cutime;
   long int cstime;
   long int priority;
   long int nice;
   long int num_threads;
   long int itrealvalue;
   unsigned long long int starttime;
   unsigned long int vsize;
   long int rss;
   unsigned long int rsslim;
   unsigned long int startcode;
   unsigned long int endcode;
   unsigned long int startstack;
   unsigned long int kstkesp;
   unsigned long int signal;
   unsigned long int blocked;
   unsigned long int sigignore;
   unsigned long int sigcatch;
   unsigned long int wchan;
   unsigned long int nswap;
   unsigned long int cnswap;
   int exit_signal;
   int processor;
   unsigned int rt_priority;
   unsigned int policy;
   unsigned long long int delayacct_blkio_ticks;
   unsigned long int guest_time;
   long int cguest_time;
   unsigned long int start_data;
   unsigned long int end_data;
   unsigned long int start_brk;
   unsigned long int arg_start;
   unsigned long int arg_end;
   unsigned long int env_start;
   unsigned long int env_end;
   int exit_code;
} stat;

struct proc_stat stat;

double get_current_cpu_usage(char *buffer, double uptime, char *stat_path, long hertz) {
    unsigned long utime, stime, starttime;
    unsigned long total_time;
    int errnum;
    double elapsed_time, cpu_usage;
    FILE * stat_file;
    
    if (access(stat_path, F_OK) == 0) {

        stat_file = fopen(stat_path, "r");
        if (!stat_file) {
            perror("Failed to open /proc/[pid]/stat");
            return -1;
        }
    
        // Read data from stat file
        fgets(buffer, BUF_SIZE, stat_file);
        fclose(stat_file);

        errnum = sscanf(buffer,  "%d %m[(a-z)] %c" 
                    "%d %d %d %d %d"
                    "%u"
                    "%lu %lu %lu %lu %lu %lu"
                    "%ld %ld %ld %ld %ld %ld"
                    "%llu"
                    "%lu"
                    "%ld"
                    "%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu"
                    "%d %d"
                    "%u %u"
                    "%llu"
                    "%lu"
                    "%ld"
                    "%lu %lu %lu %lu %lu %lu %lu"
                    "%d", 
                    &stat.pid, &stat.comm, &stat.state,
                    &stat.ppid, &stat.pgrp, &stat.session, &stat.tty_nr, &stat.tpgid,
                    &stat.flags,
                    &stat.minflt, &stat.cminflt, &stat.majflt, &stat.cmajflt, &stat.utime, &stat.stime,
                    &stat.cutime, &stat.cstime, &stat.priority, &stat.nice, &stat.num_threads, &stat.itrealvalue,
                    &stat.starttime,
                    &stat.vsize,
                    &stat.rss, 
                    &stat.rsslim, &stat.startcode, &stat.endcode, &stat.startstack, &stat.kstkesp, &stat.signal, &stat.blocked, &stat.sigignore, &stat.sigcatch, &stat.wchan, &stat.nswap, &stat.cnswap,
                    &stat.exit_signal, &stat.processor,
                    &stat.rt_priority, &stat.policy,
                    &stat.delayacct_blkio_ticks, 
                    &stat.guest_time,
                    &stat.cguest_time, 
                    &stat.start_data, &stat.end_data, &stat.start_brk, &stat.arg_start, &stat.arg_end, &stat.env_start, &stat.env_end,
                    &stat.exit_code
                    );
                    
        total_time = (stat.utime + stat.stime) / hertz;
        elapsed_time = uptime - (stat.starttime / hertz);

        cpu_usage = total_time * 100.0 / elapsed_time;

        return cpu_usage; 
    } else 
        return -1;
}

int get_cpu_usage(long long int pid, double cpu_limit, int delay) {
    char stat_path[64];
    char buffer[1024];
    FILE *stat_file;

    // obtain number of clock ticks in seconds
    long hertz = sysconf(_SC_CLK_TCK);

    double uptime;
    double cpu_usage = 0.0;

    // Read system uptime
    FILE *uptime_file = fopen("/proc/uptime", "r");
    if (!uptime_file) {
        perror("Failed to open /proc/uptime");
        return -1;
    }
    
    fscanf(uptime_file, "%lf", &uptime);
    fclose(uptime_file);

    // Read process stat
    snprintf(stat_path, sizeof(stat_path), "/proc/%lld/stat", pid);

    while (1 && cpu_usage != -1) {
        cpu_usage = get_current_cpu_usage(buffer, uptime, stat_path, hertz);
        if (cpu_usage >= cpu_limit) {
            play_sound();
            printf("CPU Usage of process with pid (%lld): %.2f%%\n", pid, cpu_usage);
        }
        sleep(delay);       
    }

    return 0;
}


int main(int argc, char *argv[]) {

    if (argc < 4) {
        printf("Correct usage: ./set_cpu_alert `pid_of_executable' `cpu_limit' `polling_frequency'\n");
        printf("`pid_of_executable': e.g. 18920\n"
               "`cpu_limit': e.g. 0.02, means 0.02%%\n" 
               "`polling_frequency': e.g. 1, means /proc/[pid_of_executable]/stat will be polled in every 1 second\n"
               "Tip: check if the process really exists.\n");

        return -1;
    }

    setbuf(stdout, NULL);

    int pid;
    sscanf(argv[1], "%d", &pid);
    double cpu_limit;
    sscanf(argv[2], "%lf", &cpu_limit);
    int delay;
    sscanf(argv[3], "%d", &delay);

    get_cpu_usage(pid, cpu_limit, delay);

    return 0;
}


