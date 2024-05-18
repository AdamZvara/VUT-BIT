/**
 * @brief Main process for creating child processes
 * @file proj2.c
 * @author Adam Zvara, xzvara01
 * @date 22.4.2021
 */

#include "proj2.h"

//--------------- GLOBAL VARIABLES -------------------
sem_t *print_mutex;     ///< mutex for the global counter of all processes
sem_t *counters_mutex;  ///< mutex for elves and reindeers counters

sem_t *santa;           ///< santas sleeping semaphore
sem_t *all_hitched;     ///< santas semaphore to wait untill all RD are hitched
sem_t *helped_all;      ///< santas semaphore to wait untill he hepled all elves

sem_t *elf_mutex;       ///< elves's semaphore in front of workshop
sem_t *elf_barrier;     ///< elves's waiting semaphore untill they get help
sem_t *get_hitched;     ///< reindeers semaphore to get hitched by santa


shared_t *sh_vars;      ///< shared variables


//--------------- SEMAPHORE FUNCTIONS -------------------
int init_semaphores()
{
    print_mutex = sem_open("/print_mutex", O_CREAT, 0644, 1);
    if (print_mutex == SEM_FAILED) {return 1;}
    counters_mutex = sem_open("/counters_mutex", O_CREAT, 0644, 1);
    if (counters_mutex == SEM_FAILED) {return 1;}
    santa = sem_open("/santa", O_CREAT, 0644, 0);
    if (santa == SEM_FAILED) {return 1;}
    elf_mutex = sem_open("/elf_mutex", O_CREAT, 0644, 1);
    if (elf_mutex == SEM_FAILED) {return 1;}
    elf_barrier = sem_open("/elf_barrier", O_CREAT, 0644, 0);
    if (elf_barrier == SEM_FAILED) {return 1;}
    get_hitched = sem_open("/get_hitched", O_CREAT, 0644, 0);
    if (get_hitched == SEM_FAILED) {return 1;}
    all_hitched = sem_open("/all_hitched", O_CREAT, 0644, 0);
    if (all_hitched == SEM_FAILED) {return 1;}
    helped_all = sem_open("/helped_all", O_CREAT, 0644, 0);
    if (helped_all == SEM_FAILED) {return 1;}

    return 0;
}

void delete_semaphores()
{
    sem_close(print_mutex);     sem_unlink("/print_mutex");
    sem_close(santa);           sem_unlink("/santa");
    sem_close(counters_mutex);  sem_unlink("/counters_mutex");
    sem_close(elf_mutex);       sem_unlink("/elf_mutex");
    sem_close(elf_barrier);     sem_unlink("/elf_barrier");
    sem_close(get_hitched);     sem_unlink("/get_hitched");
    sem_close(all_hitched);     sem_unlink("/all_hitched");
    sem_close(helped_all);      sem_unlink("/helped_all");
}

//-------------- SHARED VARIABLE FUNCTIONS ------------------

int initialize_shared()
{
    int fd = shm_open("/shared_vars", O_RDWR|O_CREAT|O_TRUNC, 0644);
    if (fd == -1) {return 1;}
    
    if (ftruncate(fd, sizeof(sh_vars)) == -1) {return 1;}
    
    sh_vars = mmap(NULL, sizeof(sh_vars), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (sh_vars == MAP_FAILED) {return 1;}

    sh_vars->proc_count = 0;
    sh_vars->elves = 0;
    sh_vars->reindeers = 0;
    sh_vars->workshop_closed = false;

    return 0;
}

void delete_shared()
{
    shm_unlink("/shared_vars");
}

//---------------- OTHER FUNCTIONS --------------------
int random_number(int min, int max)
{
    int number = rand() % (max-min+1);
    return number+min;
}

void print_msg(FILE *fr, const char *msg, ...)
{
    va_list args;
    va_start(args, msg);

    sem_wait(print_mutex);
        sh_vars->proc_count++;
        fprintf(fr, "%d: ", sh_vars->proc_count);
        vfprintf(fr, msg, args);
        fflush(fr);
    sem_post(print_mutex);

    va_end(args);
}

//--------------- PROCESS FUNCTIONS -------------------
void santa_function(FILE *fr, args_t args)
{
    print_msg(fr, "Santa: going to sleep\n");

    while (1)
    {
        //santa waits untill someone wakes him up
        sem_wait(santa);
        sem_wait(counters_mutex);
            if (sh_vars->reindeers == args.NR) //all RD returned home
            {
                print_msg(fr, "Santa: closing workshop\n");
                sem_post(elf_barrier); //let all elves through the barrier
                sh_vars->workshop_closed = true;
                sh_vars->reindeers = 0; //this will become counter of hitched RD
                
                for (int i = 0; i < args.NR; i++)
                {
                    sem_post(get_hitched);
                }
                
                sem_post(counters_mutex);
                break;
            }
            else if (sh_vars->elves == 3)
            {
                print_msg(fr, "Santa: helping elves\n");

                for (int i = 0; i < 3; i++)
                {
                    //let 3 elves through the barrier
                    sem_post(elf_barrier);
                }
                
                sem_post(counters_mutex);
                
                //wait untill all elves have received help
                sem_wait(helped_all);
                print_msg(fr, "Santa: going to sleep\n");
            }
    }

    //at this point the workshop is closed and santa is waiting for RD to be hitched
    sem_wait(all_hitched);
    print_msg(fr, "Santa: Christmas started\n");

}

void elf_function(FILE *fr, int my_id, int max_time)
{
    print_msg(fr, "Elf %d: started\n", my_id);
    bool need_help = false;

    while (1)
    {
        //simulate work
        usleep(random_number(0,max_time)*1000);
        
        sem_wait(counters_mutex);
        if (sh_vars->workshop_closed == true) //workshop closed, go on holidays
        {
            sem_post(counters_mutex);
            break;
        }
        sem_post(counters_mutex);

        sem_wait(elf_mutex);
        sem_wait(counters_mutex);
            sh_vars->elves++;
            print_msg(fr, "Elf %d: need help\n", my_id);
            need_help = true;
            if (sh_vars->elves == 3) //3 elves are waiting, wake up santa
            {
                sem_post(santa);
            }
            else
            {
                sem_post(elf_mutex); //let more elves in
            }
        sem_post(counters_mutex);

        sem_wait(elf_barrier); //wait for all elves
    
        sem_wait(counters_mutex);
        if (sh_vars->workshop_closed == true)
        {
            //let all elves continue and go on holiday
            sem_post(counters_mutex);
            sem_post(elf_mutex);
            break;
        }
        sem_post(counters_mutex);

        print_msg(fr, "Elf %d: get help\n", my_id);
        need_help = false;
    
        sem_wait(counters_mutex);
            (sh_vars->elves)--;
            if (sh_vars->elves == 0)
            {
                //notify santa and let other elves in the workshop
                sem_post(helped_all);
                sem_post(elf_mutex);
            }
        sem_post(counters_mutex);

    }
    sem_post(counters_mutex);

    if (need_help == false)
    {
        print_msg(fr, "Elf %d: need help\n", my_id);
    }
    
    print_msg(fr, "Elf %d: taking holidays\n", my_id);
    sem_post(elf_barrier);
}

void reindeer_function(FILE *fr, int my_id, args_t args)
{
    print_msg(fr, "RD %d: rstarted\n", my_id);
    
    //simulate vacation
    usleep(random_number(args.TR/2,args.TR)*1000);
    
    sem_wait(counters_mutex);
        sh_vars->reindeers++;
        print_msg(fr, "RD %d: return home\n", my_id);
        if ((sh_vars->reindeers) == args.NR)
        {
            //wake santa because all RD returned home
            sem_post(santa);
        }

    sem_post(counters_mutex);

    //wait for santa to hitch us
    sem_wait(get_hitched);

    print_msg(fr, "RD %d: get hitched\n", my_id);    
    sem_wait(counters_mutex);
        sh_vars->reindeers++;
        if (sh_vars->reindeers == args.NR)
        {
            //all RD are hitched - notify santa
            sem_post(all_hitched);   
        }
    sem_post(counters_mutex);
}


//---------------- MAIN FUNCTION ----------------------
int main(int argc, char *argv[])
{
    pid_t wpid;
    int status = 0;

    args_t args;
    if (parse_arguments(argc, argv, &args))
    {
        usage();
        return 1;
    }

    
    FILE *fr = fopen("proj2.out","w");
    if (fr == NULL) 
    {
        fprintf(stderr, "Subor sa nepodarilo otvorit\n");
        return 1;
    }
    
    //initialize semaphores
    if (init_semaphores()){
        fclose(fr);
        fprintf(stderr, "Chyba pri alokovani semaforov\n");
        return 1;
    }
    
    if (initialize_shared())
    {
        delete_semaphores();
        fclose(fr);
        fprintf(stderr, "Chyba pri alokovani zdielanej pamate\n");
        return 1;
    }


    //create one santa process
    pid_t pid = fork();

    if (pid == 0) //santa process
    {
        santa_function(fr, args);
        exit(1);
    }
    else if (pid == -1)
    {
        fclose(fr);
        delete_semaphores();
        delete_shared();
        printf("Fork failed");
    }
    

    //create elf processes
    for(int i = 0; i < args.NE; i++)
    {
        pid = fork();
        
        int elf_id = i+1;
        if (pid == 0) //elf process
        {   
            //generate new random seed for each process
            srand((int)time(0) % getpid());

            elf_function(fr, elf_id, args.TE);
            exit(1);
        }
        else if (pid == -1)
        {
            fclose(fr);
            delete_semaphores();
            delete_shared();
            printf("Fork failed");
        }
    }

    for (int i = 0; i < args.NR; i++)
    {
        pid = fork();
        
        int rd_id = i+1;
        if (pid == 0) //reindeer process
        {
            srand((int)time(0) % getpid());
            
            reindeer_function(fr, rd_id, args);
            exit(1);
        }
        else if (pid == -1)
        {
            fclose(fr);
            delete_semaphores();
            delete_shared();
            printf("Fork failed");
        }
    }

    while ((wpid = wait(&status)) > 0);

    delete_semaphores();
    delete_shared();
    
    fclose(fr);
    return 0;
}
