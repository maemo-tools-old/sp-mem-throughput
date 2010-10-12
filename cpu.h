#ifndef CPU_H
#define CPU_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <sched.h>

#ifdef __cplusplus
extern "C" {
#endif

cpu_set_t *system_possible_cpuset(void);
cpu_set_t *process_possible_cpuset(void);

char *system_possible_cpus(void);
char *process_possible_cpus(void);

struct cpu_scaling {
	unsigned *avail_freqs;
	unsigned avail_freqs_cnt;
	char **avail_governors;
	unsigned avail_governors_cnt;
	unsigned cur_freq;
	char *driver;
	char *governor;
	unsigned max_freq;
	unsigned min_freq;
};

struct cpu_scaling *get_cpu_scaling(int cpu);
int bind_to_one_cpu(void);

#ifdef __cplusplus
}
#endif

#endif /* CPU_H */
