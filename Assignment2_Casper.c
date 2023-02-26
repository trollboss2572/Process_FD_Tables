#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>


//prints the process FD table
void print_process(long PID)
{
    //All local variables to directory paths and directory info
    struct dirent *direct_data;
    DIR *overhead_dir = opendir("/proc");
    DIR *curr_proc_dir;
    DIR *FD_dir;
    char proc_dir_pth[512];
    char FD_dir_pth[1024];
    printf("PID\tFD\n");
    printf("============\n");
    //Handle the case of printing all process FD tables
    if (PID == -1)
    {
        direct_data = readdir(overhead_dir);
        //while loop to iterate over all processes
        while(direct_data)
        {
            sprintf(proc_dir_pth, "/proc/%s", direct_data->d_name);
            sprintf(FD_dir_pth, "/proc/%s/fd", direct_data->d_name);
            curr_proc_dir = opendir(proc_dir_pth);
            FD_dir = opendir(FD_dir_pth);
            int i = 0;
            //Check that we are allowed to read the FD table
            if (FD_dir && isdigit(*(direct_data->d_name)) > 0)
            {
                //Iterate over the FD table subdirectory to increment our i
                while (readdir(FD_dir))
                {
                    printf("%s\t%d\n", direct_data->d_name, i);
                    i++;
                }
            }
            //Clean up after ourselves
            closedir(curr_proc_dir);
            closedir(FD_dir);
            direct_data = readdir(overhead_dir);
        }
        closedir(overhead_dir);
        return;
    }
    //Consider the case of only reading the FD table of one process
    sprintf(proc_dir_pth, "/proc/%ld", PID);
    sprintf(FD_dir_pth, "%s/fd", proc_dir_pth);
    curr_proc_dir = opendir(proc_dir_pth);
    FD_dir = opendir(FD_dir_pth);
    readdir(curr_proc_dir);
    if(FD_dir)
    {
        for (int i = 0; readdir(FD_dir); i++)
        {
            printf("%ld\t%d\n", PID, i);
        }
    }
    else
    {
        printf("This process ID cannot be found.\n");
    }
    closedir(FD_dir);
    closedir(curr_proc_dir);
    closedir(overhead_dir);
}



//Prints the system FD table
void print_system_wide(long PID)
{
     //All local variables to directory paths and directory info
    struct dirent *direct_data;
    DIR *overhead_dir = opendir("/proc");
    DIR *curr_proc_dir;
    DIR *FD_dir;
    char proc_dir_pth[512];
    char FD_dir_pth[1024];
    printf("PID\tFD\tFilename\n");
    printf("===========================\n");
    //Handle the case of printing all process FD tables
    if (PID == -1)
    {
        direct_data = readdir(overhead_dir);
        //while loop to iterate over all processes
        while(direct_data)
        {
            sprintf(proc_dir_pth, "/proc/%s", direct_data->d_name);
            sprintf(FD_dir_pth, "/proc/%s/fd", direct_data->d_name);
            curr_proc_dir = opendir(proc_dir_pth);
            FD_dir = opendir(FD_dir_pth);
            struct dirent *FD_data;
            int i = 0;
            //Check that we are allowed to read the FD table
            if (FD_dir && isdigit(*(direct_data->d_name)) > 0)
            {
                //Iterate over the FD table subdirectory to increment our i
                while ((FD_data = readdir(FD_dir)))
                {
                    char FD_name[1024];
                    char FD_name_dir[1024];
                    sprintf(FD_name_dir, "/proc/%s/fd/%s", direct_data->d_name, FD_data->d_name);
                    readlink(FD_name_dir, FD_name, 1024);
                    printf("%s\t%d\t%s\n", direct_data->d_name, i, FD_name);
                    i++;
                }
            }
            //Clean up after ourselves
            closedir(curr_proc_dir);
            closedir(FD_dir);
            direct_data = readdir(overhead_dir);
        }
        closedir(overhead_dir);
        return;
    }
    //Consider the case of only reading the FD table of one process
    sprintf(proc_dir_pth, "/proc/%ld", PID);
    sprintf(FD_dir_pth, "%s/fd", proc_dir_pth);
    curr_proc_dir = opendir(proc_dir_pth);
    FD_dir = opendir(FD_dir_pth);
    direct_data = readdir(curr_proc_dir);
    struct dirent *FD_data;
    if(FD_dir)
    {
        for (int i = 0; (FD_data = readdir(FD_dir)); i++)
        {
            char FD_name[1024];
            char FD_name_dir[1024];
            sprintf(FD_name_dir, "/proc/%s/fd/%s", direct_data->d_name, FD_data->d_name);
            readlink(FD_name_dir, FD_name, 1024);
            printf("%ld\t%d\t%s\n", PID, i, FD_name);
        }
    }
    else
    {
        printf("This process ID cannot be found\n");
    }
    closedir(FD_dir);
    closedir(curr_proc_dir);
    closedir(overhead_dir);
}

void print_V_Nodes(long PID)
{
    //All local variables to directory paths and directory info
    struct dirent *direct_data;
    DIR *overhead_dir = opendir("/proc");
    DIR *curr_proc_dir;
    char proc_dir_pth[512];
    char proc_map_path[1024];
    printf("FD\tInode\n");
    printf("============\n");
    //Handle the case of printing all process FD tables
    if (PID == -1)
    {
        direct_data = readdir(overhead_dir);
        //while loop to iterate over all processes
        while(direct_data)
        {
            sprintf(proc_dir_pth, "/proc/%s", direct_data->d_name);
            sprintf(proc_map_path, "/proc/%s/maps", direct_data->d_name);
            curr_proc_dir = opendir(proc_dir_pth);
            FILE *proc_map = fopen(proc_map_path, "r");
            int i = 0;
            if (proc_map && isdigit(*(direct_data->d_name)) > 0)
            {
                char I_Node[1024];
                //Iterate over the FD table subdirectory to increment our i
                while (fgets(I_Node, 512, proc_map))
                {
                    strtok(I_Node, " ");
                    for (int j = 0; j < 5; j++)
                    {
                        I_Node = strtok(NULL, " ");
                    }
                    long inode = strtol(I_Node, NULL, 16);
                    printf("%d\t%ld\n", i, inode);
                    i++;
                }
                fclose(proc_map);
            }
            //Clean up after ourselves
            closedir(curr_proc_dir);
            //next step in our while loop
            direct_data = readdir(overhead_dir);
        }
        closedir(overhead_dir);
        return;
    }
}




//checks all inputs and prints only what is defined to be printed by the user. Returns 0 in case of successful printing
int print_tables (int process, int system_wide, int V_Nodes, int composite, int threshold, long PID)
{
    //Either all 1 or all 0, so print all
    if (process == 0 && system_wide == 0 && V_Nodes == 0 && composite == 0)
    {
        print_process(PID);
        print_system_wide(PID);
        print_V_Nodes(PID);
        return 0;
    }
    if (process == 1)
    {
        print_process(PID);
    }
    if (system_wide == 1)
    {
        print_system_wide(PID);
    }
    if (V_Nodes == 1)
    {
        print_V_Nodes(PID);
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
        if (isdigit(**(argv + i)) > 0)
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