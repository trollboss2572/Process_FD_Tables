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
            //Read all the files open in the process's fd table
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
            //Iterate through all the process files in the process FD table
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
                    if (FD_data->d_name[0] == '.')
                    {
                        continue;
                    }
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
            char FD_name_dir[1280];
            sprintf(FD_name_dir, "%s/%s", FD_dir_pth, FD_data->d_name);
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


//Prints the Inode table
void print_V_Nodes(long PID)
{
    //All local variables to directory paths and directory info
    struct dirent *direct_data;
    DIR *overhead_dir = opendir("/proc");
    DIR *curr_proc_dir;
    char proc_dir_pth[512];
    char proc_info_path[1024];
    printf("FD\tInode\n");
    printf("============\n");
    //Handle the case of printing all process FD tables
    if (PID == -1)
    {
        direct_data = readdir(overhead_dir);
        //while loop to iterate over all processes
        while(direct_data)
        {
            //Find the process's fdinfo directory, which contains the inode value for each file
            sprintf(proc_dir_pth, "/proc/%s", direct_data->d_name);
            sprintf(proc_info_path, "/proc/%s/fdinfo", direct_data->d_name);
            curr_proc_dir = opendir(proc_dir_pth);
            DIR *proc_FD_dir = opendir(proc_info_path);
            if (proc_FD_dir && isdigit(*(direct_data->d_name)) > 0)
            {
                int i = 0;
                FILE *fp;
                struct dirent *file_name;
                char file_path[526];
                file_name = readdir(proc_FD_dir);
                while (file_name)                
                {
                    if (*(file_name->d_name) == '.')
                    {
                        file_name = readdir(proc_FD_dir);
                        continue;
                    }
                    sprintf(file_path, "/proc/%s/fdinfo/%s", direct_data->d_name, file_name->d_name);
                    fp = fopen(file_path, "r");
                    if (fp)
                    {
                        i++;
                        char file_string[256];
                        int j = 0;
                        while(j < 3)
                        {
                            fgets(file_string, 256, fp);
                            j++;    
                        }
                        fgets(file_string, 256, fp);
                        char *I_Node = strstr(file_string, "ino:");
                        char *I_str = I_Node + 4;
                        long inode = strtol(I_str, NULL, 10);
                        printf("%d\t%ld\n", i, inode);
                        fclose(fp);
                    }
                    file_name = readdir(proc_FD_dir);
                }
                closedir(proc_FD_dir);
            }
            //Clean up after ourselves
            closedir(curr_proc_dir);
            //next step in our while loop
            direct_data = readdir(overhead_dir);
        }
        closedir(overhead_dir);
        return;
    }
    sprintf(proc_dir_pth, "/proc/%ld", PID);
    sprintf(proc_info_path, "/proc/%ld/fdinfo", PID);
    curr_proc_dir = opendir(proc_dir_pth);
    DIR *proc_FD_dir = opendir(proc_info_path);
    if (proc_FD_dir)
    {
        int i = 0;
        FILE *fp;
        struct dirent *file_name;
        char file_path[526];
        file_name = readdir(proc_FD_dir);
        while (file_name)                
        {
            if (*(file_name->d_name) == '.')
            {
                file_name = readdir(proc_FD_dir);
                continue;
            }
            sprintf(file_path, "/proc/%ld/fdinfo/%s", PID, file_name->d_name);
            fp = fopen(file_path, "r");
            if (fp)
            {
                i++;
                char file_string[256];
                int j = 0;
                while(j < 3)
                {
                    fgets(file_string, 256, fp);
                    j++;    
                }
                fgets(file_string, 256, fp);
                char *I_Node = strstr(file_string, "ino:");
                char *I_str = I_Node + 4;
                long inode = strtol(I_str, NULL, 10);
                printf("%d\t%ld\n", i, inode);
                fclose(fp);
            }
            file_name = readdir(proc_FD_dir);
        }
    }
    else
    {
        printf("Process directory not found\n");
    }
}

//Helper function for print_composite. Prints a line of the print_composite table given an open directory to that process and the PID of that process
void print_process_info(DIR *curr_proc, char *PID)
{
    if (isdigit(PID[0]) == 0)
    {
        return;
    }
    char FD_dir_pth[1024];
    char proc_info_pth[200];
    struct dirent *proc_data;
    //Iterate through the fd and fdinfo directory to find all the metadata and file data in the fd table and fd info table
    sprintf(FD_dir_pth, "/proc/%s/fd", PID);
    sprintf(proc_info_pth, "/proc/%s/fdinfo", PID);
    DIR * FD = opendir(FD_dir_pth);
    DIR * FD_info = opendir(proc_info_pth);
    int i = 0;
    struct dirent *FD_inode_data;
    if (FD)
    {
        while ((proc_data = readdir(FD)))
        {
            FD_inode_data = readdir(FD_info);
            if (proc_data->d_name[0] == '.')
            {
                continue;
            }
            char filepath[600];
            sprintf(filepath, "%s/%s", proc_info_pth, FD_inode_data->d_name);
            FILE * inode_file = fopen(filepath, "r");
            char file_string[1024];
            int j = 0;
            while(j < 4)
            {
                fgets(file_string, 256, inode_file);
                j++;
            }
            char *I_Node = strstr(file_string, "ino:");
            char *I_str = I_Node + 4;
            long inode = strtol(I_str, NULL, 10);
            char FD_name[1024];
            char FD_path[1500];
            //Print the value stored at the fd value stored here
            sprintf(FD_path, "%s/%s", FD_dir_pth, proc_data->d_name);
            readlink(FD_path, FD_name, 1024);
            printf("%s\t%d\t%s\t%ld\n", PID, i, FD_name, inode);
            i++;
        }
    }
}


//Prints composite table
void print_composite(long PID)
{
    //All local variables to directory paths and directory info
    struct dirent *direct_data;
    DIR *overhead_dir = opendir("/proc");
    DIR *curr_proc_dir;
    char proc_dir_pth[512];
    printf("PID\tFD\tFilename\tInode\n");
    printf("=======================================\n");
    //Handle the case of printing all process FD tables
    if (PID == -1)
    {
        //while loop to iterate over all processes
        while((direct_data = readdir(overhead_dir)))
        {
            sprintf(proc_dir_pth, "/proc/%s", direct_data->d_name);
            //Call our above helper function to print a specific line for us
            curr_proc_dir = opendir(proc_dir_pth);
            print_process_info(curr_proc_dir, direct_data->d_name);
            closedir(curr_proc_dir);
        }
        closedir(overhead_dir);
        return;
    }
    //Consider the case of only reading the FD table of one process
    sprintf(proc_dir_pth, "/proc/%ld", PID);
    curr_proc_dir = opendir(proc_dir_pth);
    if (curr_proc_dir)
    {
        char PID_as_str[50];
        sprintf(PID_as_str, "%ld", PID);
        print_process_info(curr_proc_dir, PID_as_str);
        closedir(curr_proc_dir);
    }
    else
    {
        printf("Process ID not found\n");
    }
    closedir(overhead_dir);

}

//Prints all processes with files open in their FD greater than our threshold
void print_threshold(int threshold)
{
    printf("## Offending Processes ##\n");
    //All local variables to directory paths and directory info
    struct dirent *direct_data;
    DIR *overhead_dir = opendir("/proc");
    DIR *curr_proc_dir;
    DIR *FD_dir;
    char proc_dir_pth[512];
    char FD_dir_pth[1024];
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
                //Iterate over the FD table subdirectory to increment our i.
                while ((FD_data = readdir(FD_dir)))
                {
                    i++;
                }
                //If i exceeds our threshold, then we print the process as an offending process
                if (i >= threshold)
                {
                    printf("%s (%d)\n", direct_data->d_name, i);
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


//checks all inputs and prints only what is defined to be printed by the user. Returns 0 in case of successful printing
int print_tables (int process, int system_wide, int V_Nodes, int composite, int threshold, long PID)
{
    //All 0, so print all
    if (process == 0 && system_wide == 0 && V_Nodes == 0 && composite == 0)
    {
        print_process(PID);
        print_system_wide(PID);
        print_V_Nodes(PID);
        print_composite(PID);
        if (threshold >= 0 )
        {
            print_threshold(threshold);
        }
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
    if (composite == 1)
    {
        print_composite(PID);
    }
    if (threshold >= 0)
    {
        print_threshold(PID);
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
            char *arg = (*(argv+i)) + 12;
            *threshold = strtol(arg, NULL, 10);
            continue;
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