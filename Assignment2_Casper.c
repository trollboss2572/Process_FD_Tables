#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
//prints the process FD table
int print_process(long *off_PID[], int *num_processes[], int ind, long PID, int threshold)
{
    if (PID >= 0)
    {
        getPIDs
    }
    printf("PID\tFD\n");
    printf("============\n")
}

//checks all inputs and prints only what is defined to be printd by the user
int print_tables (int process, int system_wide, int V_Nodes, int composite, int threshold, long PID)
{
    //An array to pointers which store all processes which are larger than our threshold
    long offending_PID[1024];
    int num_processes[1024];
    //The largest index to write to
    int ind = 0;
    //Either all 1 or all 0, so print all
    if (process == system_wide == V_Nodes == composite)
    {
        ind = print_process(offending_PID, num_processes, ind, PID, threshold);
        ind = print_system_wide(offending_PID, num_processes, ind, PID, threshold);
        ind = print_V_Nodes(offending_PID, num_processes, ind, PID, threshold);
        ind = print_composite(offending_PID, num_processes, ind, PID, threshold);
        return 0;
    }
    if (process == 1)
    {
        ind = print_process(offending_PID, num_processes, ind, PID, threshold);
    }
    if (system_wide == 1)
    {
        ind = print_system_wide(offending_PID, num_processes, ind, PID, threshold);
    }
    if (V_Nodes == 1)
    {
        ind = print_V_Nodes(offending_PID, num_processes, ind, PID, threshold);
    }
    if (composite == 1)
    {
        ind = print_composite(offending_PID, num_processes, ind, PID, threshold);
    }
    if (threshold >= 0)
    {
        print_threshold(offending_PID, num_processes, ind, PID);
    }
    return 0;
}

//helper function to parse the arguments of the user input
void parse_flags(int argc, char **argv, int *process, int *system_wide, int *V_Nodes, int *composite, int *threshold, long *PID)
{
    for (int i = 1; i < argc; i++)
    {
        if(strcmp(argv[i], "--per-process") == 0)
        {
            *process = 1;
            continue;
        }
        if(strcmp(argv[i], "--systemWide") == 0)
        {
            *system_wide = 1;
            continue;
        }
        if(strcmp(argv[i], "--Vnodes") == 0)
        {
            *V_Nodes = 1;
            continue;
        }
        if(strcmp(argv[i], "--composite") == 0)
        {
            *composite = 1;
            continue;
        }
        if(strncmp(argv[i], "--threshold=", 12) == 0)
        {
            char *t_num = memchr(argv[i], '=', strlen(argv[i]));
            if (isdigit(t_num[0]) > 0)
            {
                *threshold = strtol(t_num, NULL, 10);
                continue;
            }
        }
        if (isdigit(argv[i][0] > 0))
        {
            *PID = strtol(argv[i], NULL, 10);
            continue;
        }
    }
}

int main(int argc, char **argv)
{
    //The integer values for process up to composite determine whether those arguments are true. (0 is false, 1 is true) Threshold starts at 0 but can be moved up depending on user input
    int process = 0;
    int system_wide = 0;
    int V_Nodes = 0;
    int composite = 0;
    int threshold = -1;
    long PID = -1;
    //Use helper function in order to initialize our values
    parse_flags(argc, argv, &process, &system_wide, &V_Nodes, &composite, &threshold, &PID);
    int err = print_tables(process, system_wide, V_Nodes, composite, threshold, PID);
    return err;
}