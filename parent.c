/*
 * The fork() function
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include "local.h"

int counter_of_round_team_en = 0; //store the number of rounds the enemy team has won
int counter_of_round_team_sol = 0; //store the number of rounds the soldier team has won
int counter_of_round = 1; //store the number of the current round
int position[50][50]; //two dimentional array that represents the battlefield
int counter = 0;
int S_dead = 0; //to store the number of dead soldiers
int E_dead = 0; //to store the number of dead enemy
int f_des[2];  //array for pipe, [0] for read & [1] for write
char *posFile = "position.txt";
char *pidFile  =  "PID.txt";
static char message[BUFSIZ];


void sigset_catcher(int);
int i, status;
pid_t pid, pid_array[10];//array to store all pid's of forked children
int x, y, fp = 0;//x and y to store each child position


int main()
{
    /* this loop loops at most 10 times, this loops until  on of the teams won 5 rounds */
    for(int yy = 0 ; yy < 10 ; yy++)
    {

        printf("\n\t-------------------------------------------------------------------------------------------------------------\n");
        fflush(stdout);
        cyan();
        printf("\n\t\t Enemy team is: %d -- Soldier team is: %d || Round: %d || \n\n", counter_of_round_team_en,counter_of_round_team_sol, counter_of_round);
        normals();
        fflush(stdout);
        /* set this signal to the handler function sigset_catcher*/
        sigset(SIGUSR1, sigset_catcher);
        struct message msg;



       // Parent creates the pipe to talk to all soldiers
        if (pipe(f_des) == -1)
        {
            perror("Pipe");
            exit(-1);
        }
        //this for loop is to  create (fork) 10 children
        for (i = 0; i < 10; i++)
        {
            pid = fork();//create a child
            if (pid == -1)
                exit(-1);//exit if there was a problem when creating 
            /*if pid returns 0 then we are in the child process*/
            else if (pid == 0)
            {
                // printf("I am a child with => PID = %d\n", getpid());
                // fflush(stdout);
                 /*making children sensitive to the user defind signal SIGUSR1*/
                while (1)
                {
                    sigpause(SIGUSR1); /* Wait for USR1 signal */
                }
                exit(-2);
            }
            else
            {

                counter++;// counter that stores number of children that were created
                srand(time(0));
                /* the x and y coordenats of each child  is randomlly assigned by the parent with rang from 0 - 49*/
                x = (int)(rand() % 49);
                y = (int)(rand() % 49);
                  /* this means that the position is not taken by any process*/
                if (position[y][x] == 0)
                {
                    position[y][x] = pid;// store the pid  of the child that took this position
                }
                /* this means that the position is taken by a process*/
                else
                {
                     /* wait until we find ax and y that are not taken*/
                    while (position[y][x] != 0)
                    {
                        x = (int)(rand() % 49);
                        y = (int)(rand() % 49);
                    }
                    position[y][x] = pid;// store the pid  of the child that took this
                }

                printf("\n\n\t\tI am the parent => PID = %d, child ID = %d ==> x = %d  y = %d\n", getpid(), pid, x, y);
                fflush(stdout);
                pid_array[i] = pid; //make the parent store the pid of every forked child
                /* when counter is 10 this means that all 10 children has been forked */
                if (counter == 10)
                {
                    // print the position in file
                    FILE * fp = fopen(posFile, "w");//open the file to write
                    if (fp != NULL)
                    {
                        for (int row = 0; row < 50; row++)
                        {
                            for (int col = 0; col < 50; col++)
                            {

                                fprintf(fp, "%d ", position[row][col]); //store the two dimentional array that represents the battlefield and the location of the soldiers in a file 

                            }
                            fprintf(fp, "\n");
                        }
                        fclose(fp);
                    }
                    // print the  pid of all proccces in file
                    FILE * fp2 = fopen(pidFile, "w");
                    if (fp != NULL)
                    {
                        for (int row = 0; row < 10; row++)
                        {
                            fprintf(fp, "%d\n", pid_array[row]);
                        }
                        fclose(fp2);
                    }

                    for (int j = 0; j < 10; j++)
                    {

                        sleep(1);
                        kill(pid_array[j], SIGUSR1);//send signal to all children to inform them that all children were created and know they can exec
                        /*this is to make sure that the text file that contains the battlefied array is written and now all children can read it*/

                    }
                }
            }
        }


        red();
        //printf("\n\n            Now the battlefield is ready \n            All soldiers are in their positions \n ");
        printf("\n\n\t\t\t                    Now the battlefield is ready \n\t\t\t                   All soldiers are in their positions \n\n ");
        Purple();
        for (int row = 0; row < 50; row++)
        {   
            printf("\t\t");
            for (int col = 0; col < 50; col++)
            {
                if(position[row][col] > 0)
                {
                    green();
                    printf("%d ", position[row][col]);
                    Purple();  
                }
                else
                {
                    printf("%d ", position[row][col]);
                }
            }
            printf( "\n");
        }
        blue();
        printf( "\n\t\t                       The Primitive War starts simulation \n\n ");
        normals();
        fflush(stdout);


        while(1)
        {

            close(f_des[1]);//close for writing
            if (read(f_des[0], message, BUFSIZ) != -1)
            {
                /* if the message send from one of the children through a pipe is 'S' this means that the soldier team have won this round*/
                if(message[strlen(message)-1]=='S')
                {

                    S_dead++;
                    red();
                    printf ("\t\t Processes from Soldiers team dead are: %d\n", S_dead);
                    normals();
                    fflush(stdout);

                }
                 /* if the message send from one of the children through a pipe is 'E' this means that the enemy team have won this round*/
                if(message[strlen(message)-1]=='E')
                {
                    E_dead++;
                    red();
                    printf ("\t\t Processes from Enemey team dead are: %d\n", E_dead);
                    normals();
                    fflush(stdout);

                }
            }
            else
            {
                printf("not in read !!!\n");
                fflush(stdout);

            }
             /* if s_dead counter in 5 this means that the soldier team has won this rounds and should be declared the winner of this round by the parents and break from for loop
            and we should reset the counters and remove the created private fifo's and kill the remainig members of the enemy team
            */
            if(S_dead == 5)
            {
                cyan();
                printf("\n\t\tThe Winner Is The Enemey Team and number of soldiers dead is ==> Enemey: %d VS Soldier: %d\n", E_dead, S_dead);
                normals();
                fflush(stdout);
                counter_of_round_team_en++;
                counter_of_round++;
                S_dead = 0;
                E_dead = 0;
                counter = 0;
                char str[11];
                char str3[11];
                char str2[] ="P";
                for(int j = 0 ; j < 10 ; j++ )
                {
                    kill(pid_array[j],SIGKILL);//signal to kill the remainig members of the enemy team
                    sprintf(str, "%d", pid_array[j]);
                    if(remove(str) == -1 )
                    {
                        printf("Not Done");
                        perror("why not \n");
                        fflush(stdout);

                    }
                     //delete private fifo of this pid
                    else if(remove(str) == 0)
                    {
                        printf("done deleting process file \n");
                        fflush(stdout);

                    }

                    strcpy(str3, str2);
                    strcat(str3, str);
                    if(remove(str3) == -1 )
                    {
                        printf("Not Done");
                        perror("why not \n");
                        fflush(stdout);

                    }
                    else if(remove(str3) == 0)
                    {
                        printf("done deleting process position and its id \n");
                    }

                    pid_array[i] = 0;

                }
                 /* reset the 2D battlefield array*/
                for (int row = 0; row < 50; row++)
                {
                    for (int col = 0; col < 50; col++)
                    {
                        position[row][col] = 0;
                    }
                }
                green();
                printf("\n\t\t\t\t\t\t Wait for the next round \t\t\t\t\t\t\n\n");
                normals();
                fflush(stdout);
                sleep(1);

                break;
            }
             /* if E_dead counter in 5 this means that the enemy team has won this rounds and should be declared the winner of this round by the parents and break from for loop
            and we should reset the counters and remove the created private fifo's and kill the remainig members of the enemy team
            */
            if(E_dead == 5)
            {
                cyan();
                printf("\n\t\tThe Winner Is The Soldier Team and number of soldiers dead is ==> Enemey: %d VS Soldier: %d\n", E_dead, S_dead);
                normals();
                fflush(stdout);
                counter_of_round_team_sol++;
                counter_of_round++;
                E_dead = 0;
                S_dead = 0;
                counter = 0;
                char str[11];
                char str3[11];
                char str2[] ="P";
                for(int j = 0 ; j < 10 ; j++ )
                {
                    kill(pid_array[j],SIGKILL);//signal to kill the remainig members of the enemy team
                    sprintf(str, "%d", pid_array[j]);
                    if(remove(str) == -1 )
                    {
                        printf("Not Done");
                        perror("why not \n");
                        fflush(stdout);
                    }
                    else if(remove(str) == 0)
                    {
                        printf("done deleting process file \n");
                        fflush(stdout);
                    }
                    strcpy(str3, str2);
                    strcat(str3, str);
                    if(remove(str3) == -1 )
                    {
                        printf("Not Done");
                        perror("why not \n");
                        fflush(stdout);

                    }
                    else if(remove(str3) == 0)
                    {
                        printf("done deleting process position and its id \n");
                        fflush(stdout);
                    }

                    pid_array[j] = 0;
                }
                for (int row = 0; row < 50; row++)
                {
                    for (int col = 0; col < 50; col++)
                    {
                        position[row][col] = 0;
                    }
                }
                green();
                printf("\n\t\t\t\t\t\t Wait for the next round \t\t\t\t\t\t\n\n");
                normals();
                fflush(stdout);

                sleep(1);

                break;
            }
            //printf("Process ID %d has terminated\t status = %d\n", pid_array[i], status);
            //fflush(stdout);
            // }
        }
         /* this means a draw*/
        if(counter_of_round_team_sol == 5 || counter_of_round_team_en == 5 ){
            break;
        }
    }

     /* this means that the enemy team has won 5 rounds and should be declared the winner of the game by the parents and break from for loop*/
    if(counter_of_round_team_en > counter_of_round_team_sol )
    {
        red();
        printf("\n\t\t The Winner of all rounds is the Enemy team with score ==> Enemy team: %d -- Soldier team: %d \n\n", counter_of_round_team_en,counter_of_round_team_sol);
        return (0);
        //exit(0);
        fflush(stdout);
       
    }
     /* this means that the soldier team has won 5 rounds and should be declared the winner of the game by the parents and break from for loop*/
    else if (counter_of_round_team_en < counter_of_round_team_sol)
    {
        cyan();
        printf("\n\n\t\t The Winner of all rounds is the Soldier team with score ==> Enemy team: %d -- Soldier team: %d \n\n", counter_of_round_team_en,counter_of_round_team_sol);
        normals();
        fflush(stdout);
        return (0);

    }
      /* this means a draw between the two teams*/
    else
    {
        cyan();
        printf("\n\n\t\t The two teams have the same score\n\n");
        normals();
        fflush(stdout);
        // exit(0);
       
    }


    return (0);
}
/*this sinal catcher is executed when all the children are created we assumed thet the first 5 pid's are for the soldier team and the last 5 are for the enemy team
when we exec  to the soldier c file we send the type of the child (E or S) and the pipe array [0] for read & [1] for write, and we send the counter of the current child*/
void sigset_catcher(int n)
{

    //printf("Beginnin important stuff  %d\n",counter);
    //fflush(stdout);
    char tmp[5];
    sprintf(tmp, "%d", counter);
    char tmp2[10];
    sprintf(tmp2, "%d", f_des[0]);
    char tmp3[10];
    sprintf(tmp3, "%d", f_des[1]);
    if (counter < 5)
    {
        char * arg_list[] =
        {
            "S",
            "",
            "",
            "",
            NULL
        };
        arg_list[1] = tmp;
        arg_list[2] = tmp2;
        arg_list[3] = tmp3;
        //printf("c= %s\n",arg_list[1]);
        execv("./soldier", arg_list);
        perror("execv -- soldier");
        exit(-3);
    }
    else
    {
        char * arg_list[] =
        {
            "E",
            "",
            "",
            "",
            NULL
        };
        arg_list[1] = tmp;
        arg_list[2] = tmp2;
        arg_list[3] = tmp3;
        execv("./soldier", arg_list);
        perror("execv -- Enemy");
        exit(-4);
    }

    // printf("Beginnin important stuff  %d\n",getpid());
    // fflush(stdout);
}






