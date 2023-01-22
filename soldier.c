#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "local.h"

#define MWIDTH 50
#define MHEIGHT 50
int isEnemy(int, char * );

int s_pid[10]; //stors the pid of all children read from a file
int flag;
int array[MHEIGHT][MWIDTH];
int n,n2, privatefifo, fp,fp2, publicfifo, buf;
static char buffer[B_SIZ];
static char buffer2[B_SIZ];
char focus [3];
char pid[6];
struct message msg;
struct message msg2;

/* struct that stores the information of each child and the counter of the places that the child was shot at and ths child's position and focus in the battlefield*/
struct Person
{
    int head_counter;
    int neck_counter;
    int chest_counter;
    int abdomen_counter;
    int hands_counter;
    int legs_counter;
    int miss;
    int charged;
    int posX;
    int posY;
    int focus ;
}
sol;

int main(int argc, char * argv[])
{
    //void signal_catcher(int);
    //printf("After Exec child is in the ARMY TEAM  => PID = %d\n", getpid());
    //fflush(stdout);
   
    void sigset_catcher2(int);
    void read_file();
    void dead_child();
    sigset(SIGUSR1, sigset_catcher2);//user defind signal that execute sigset_catcher2 fuction 
    sigset(SIGUSR2, read_file);//user defind signal that execute read_file fuction 
    
  

    /*Make the name for the private fifo */
    sprintf(msg.fifo_name, "%d", getpid());

    /* Generate the private FIFO */
    if (mknod(msg.fifo_name, S_IFIFO | 0666, 0) < 0)
    {
        perror("Private");
        exit(-8);
    }
    /* Generate the second private FIFO */
    sprintf(msg2.fifo_name, "P%d", getpid());
    if (mknod(msg2.fifo_name, S_IFIFO | 0666, 0) < 0)
    {
        perror("Private2");
        exit(-8);
    } 
   /* read the 2D array from a file and store it*/  
    FILE * f;
    int ii, jj;

    if ((f = fopen("position.txt", "r")) == NULL)
        exit(9);

    for (jj = 0; jj < MHEIGHT; jj++)
    {
        for (ii = 0; ii < MWIDTH; ii++)
        {
            if (fscanf(f, "%d", & array[jj][ii]) != -1)
            {

                if (array[jj][ii] == getpid())
                {
                    sol.posX = ii;
                    sol.posY = jj;

                }
            }
        }
    }
    fclose(f);
    
    
   /* read the pid of the children from a file and store it*/  
    FILE * f2;
    ii = 0;
    if ((f2 = fopen("PID.txt", "r")) == NULL)
        exit(10);

    for (ii = 0; ii < 10; ii++)
    {
        if (fscanf(f, "%d", & s_pid[ii]) != 1)
        {
            exit(1);
        }
    }
    fclose(f2);

    /* this is used to check that all children has been created this to make sure that no child write or read from an uncreated fifo*/
    if (strcmp(argv[1], "9") == 0)
    {
        //printf("All FiFO's are created => PID = %d\n", getpid());
        //fflush(stdout);
        for (int j = 0; j < 10; j++)
        {
            // sleep(1);
            kill(s_pid[j], SIGUSR1);
        }
    }


    while (1)
    {
      
        dead_child();// this will call the dead_child function that read the second private fifo to wether there are dead children
        
        /* make the child  sensitive to SIGUSR2 signal*/
        if (signal(SIGUSR2, read_file) == SIG_ERR) //signal handling, returns -1 (SIG_ERR) on failure
        {
            perror("Signal can not set SIGUSR2");
            exit(SIGUSR2);
        }
        /* if the child got two head shots this means that he is now dead and should inform all the children by writting on their pivate fifo and thern inform the 
        parent that he is dead by sending the type of the dead child (enemy or soldier) then the child kill itself*/
        if (sol.head_counter >= 2)
        {
            char chkill[50];
            char tmp2[50];
            /* loops at all children to write in their private fifo*/
            for (int j = 0; j < 10; j++)
            {
                sprintf(tmp2, "P%d", s_pid[j]);
                if ((fp2 = open(tmp2, O_RDWR) == -1))
                {

                    perror("open error for public fifo");
                }
                else
                {

                    sprintf(chkill, "%d %d\n", sol.posX, sol.posY);//send the children the location of the dead child so that they can stop aiming fo it
                    if(getpid()!=s_pid[j])
                    write(fp2, chkill, strlen(chkill) + 1);

                }
                close(fp2);
            }
            cyan();
            printf("\t\t==> I -- %d -- was dead from two bullets in the head and I am in %s team <==\n", getpid(),argv[0]);
            normals();
            fflush(stdout);
            /* send a pipe message to the parent informing them that this child is dead*/
            if (write(atoi(argv[3]), argv[0], strlen(argv[0])) == -1)
            {
                perror("Write");
                exit(-4);
            }
            
            kill(getpid(), SIGKILL);// kill this child

        }
       /* if the child got three neck  shots this means that he is now dead and should inform all the children by writting on their pivate fifo and thern inform the 
        parent that he is dead by sending the type of the dead child (enemy or soldier) then the child kill itself*/
        else if (sol.neck_counter >= 3 )

        {
            char chkill[50];
            char tmp2[50];
             /* loops at all children to write in their private fifo*/
            for (int j = 0; j < 10; j++)
            {
                sprintf(tmp2, "P%d", s_pid[j]);
                if ((fp2 = open(tmp2, O_RDWR) == -1))
                {

                    perror("open error for public fifo");
                }
                else
                {
                    sprintf(chkill, "%d %d\n", sol.posX, sol.posY);
                    if(getpid()!=s_pid[j])
                    write(fp2, chkill, strlen(chkill) + 1);//send the children the location of the dead child so that they can stop aiming fo it
                }
                close(fp2);
            }
            cyan();
            printf("\t\t==> I -- %d -- was dead from three bullets in the neck and I am in %s team <==\n", getpid(),argv[0]);
            normals();
            fflush(stdout);

             /* send a pipe message to the parent informing them that this child is dead*/
            if (write(atoi(argv[3]), argv[0], strlen(argv[0])) == -1)
            {
                perror("Write");
                exit(-4);
            }
            kill(getpid(), SIGKILL);// kill this child


        }
        
        /* if the child got three shots in chest  this means that he is now dead and should inform all the children by writting on their pivate fifo and thern inform the 
        parent that he is dead by sending the type of the dead child (enemy or soldier) then the child kill itself*/
        
         else if (sol.chest_counter >= 3)

        {
            char chkill[50];
            char tmp2[50];
             /* loops at all children to write in their private fifo*/
            for (int j = 0; j < 10; j++)
            {
                sprintf(tmp2, "P%d", s_pid[j]);
                if ((fp2 = open(tmp2, O_RDWR) == -1))
                {

                    perror("open error for public fifo");
                }
                else
                {
                    sprintf(chkill, "%d %d\n", sol.posX, sol.posY);
                    if(getpid()!=s_pid[j])
                    write(fp2, chkill, strlen(chkill) + 1);//send the children the location of the dead child so that they can stop aiming fo it
                }
                close(fp2);
            }
            cyan();
            printf("\t\t==> I -- %d -- was dead from three bullets in the chest and I am in %s team <==\n", getpid(),argv[0]);
            normals();
            fflush(stdout);

             /* send a pipe message to the parent informing them that this child is dead*/
            if (write(atoi(argv[3]), argv[0], strlen(argv[0])) == -1)
            {
                perror("Write");
                exit(-4);
            }
            kill(getpid(), SIGKILL);// kill this child


        }
        
        /* if the child got three abdomen shots this means that he is now dead and should inform all the children by writting on their pivate fifo and thern inform the 
        parent that he is dead by sending the type of the dead child (enemy or soldier) then the child kill itself*/
        else if ( sol.abdomen_counter >= 3 )

        {
            char chkill[50];
            char tmp2[50];
             /* loops at all children to write in their private fifo*/
            for (int j = 0; j < 10; j++)
            {
                sprintf(tmp2, "P%d", s_pid[j]);
                if ((fp2 = open(tmp2, O_RDWR) == -1))
                {

                    perror("open error for public fifo");
                }
                else
                {
                    sprintf(chkill, "%d %d\n", sol.posX, sol.posY);
                    if(getpid()!=s_pid[j])
                    write(fp2, chkill, strlen(chkill) + 1);//send the children the location of the dead child so that they can stop aiming fo it
                }
                close(fp2);
            }
            
            cyan();
            printf("\t\t==> I -- %d -- was dead from three bullets in the abdomen and I am in %s team <==\n", getpid(),argv[0]);
            normals();
            fflush(stdout);

             /* send a pipe message to the parent informing them that this child is dead*/
            if (write(atoi(argv[3]), argv[0], strlen(argv[0])) == -1)
            {
                perror("Write");
                exit(-4);
            }
            kill(getpid(), SIGKILL);// kill this child


        }
        
        
        /* if the child got 5 hand shots this means that he is now dead and should inform all the children by writting on their pivate fifo and thern inform the 
        parent that he is dead by sending the type of the dead child (enemy or soldier) then the child kill itself*/
        else if (  sol.hands_counter >= 5 )
        {
            char chkill[50];
            char tmp2[50];
             /* loops at all children to write in their private fifo*/
            for (int j = 0; j < 10; j++)
            {
                sprintf(tmp2, "P%d", s_pid[j]);
                if ((fp2 = open(tmp2, O_RDWR) == -1))
                {

                    perror("open error for public fifo");
                }
                else
                {

                    sprintf(chkill, "%d %d\n", sol.posX, sol.posY);
                    if(getpid()!=s_pid[j])
                    write(fp2, chkill, strlen(chkill) + 1);

                }
                close(fp2);
            }
            cyan();
            printf("\t\t==> I -- %d -- was dead from five bullets in the hands and I am in %s team <==\n", getpid(),argv[0]);
            normals();
            fflush(stdout);
             /* send a pipe message to the parent informing them that this child is dead*/
            if (write(atoi(argv[3]), argv[0], strlen(argv[0])) == -1)
            {
                perror("Write");
                exit(-4);
            }

            kill(getpid(), SIGKILL);// kill this child

        }
        /* if the child got 5 legs  shots this means that he is now dead and should inform all the children by writting on their pivate fifo and thern inform the 
        parent that he is dead by sending the type of the dead child (enemy or soldier) then the child kill itself*/
        else if ( sol.legs_counter>= 5 )
        {
            char chkill[50];
            char tmp2[50];
             /* loops at all children to write in their private fifo*/
            for (int j = 0; j < 10; j++)
            {
                sprintf(tmp2, "P%d", s_pid[j]);
                if ((fp2 = open(tmp2, O_RDWR) == -1))
                {

                    perror("open error for public fifo");
                }
                else
                {

                    sprintf(chkill, "%d %d\n", sol.posX, sol.posY);
                    if(getpid()!=s_pid[j])
                    write(fp2, chkill, strlen(chkill) + 1);

                }
                close(fp2);
            }
            cyan();
            printf("\t\t==> I -- %d -- was dead from five bullets in the legs and I am in %s team <==\n", getpid(),argv[0]);
            normals();
            fflush(stdout);
             /* send a pipe message to the parent informing them that this child is dead*/
            if (write(atoi(argv[3]), argv[0], strlen(argv[0])) == -1)
            {
                perror("Write");
                exit(-4);
            }

            kill(getpid(), SIGKILL);// kill this child

        }


        //printf("Charging.........%d\n",getpid());
        //fflush(stdout);
        
        /* check if the child weapon is not chared, the charging time takes random period from 1-3 sec*/
        else if (sol.charged == 0)
        {
            //  printf("Charging.........\n");
            //fflush(stdout);
            srand(time(0));
            int ch = (int)(1 + (rand() % 3));
            sleep(ch);
            //printf("Done Charging.........\n");
            //fflush(stdout);
            sol.charged = 1;//set the flag for the next time
        }
        else
        {

            /*this flag is to check weather all children have done creating their private fifo to prevent from writing to an uncreated file*/
            if (flag == 0)
                sigpause(SIGUSR1);// pause untill all  fifo's are created.This is done only one time

            float min = 100;
            int dis = 0;
            int minId;
            dead_child();
            //read_file();
            
            /* check were is the nearest process to shoot at it should be from the oposite team and the location should nothave the value zero, because zero means the location is not taken
            and make sure that it is not the process itself*/
            for (int jj = 0; jj < MHEIGHT; jj++)
            {
                for (int ii = 0; ii < MWIDTH; ii++)
                {
                    if (array[jj][ii] != getpid() && array[jj][ii] != 0 && isEnemy(array[jj][ii], argv[0]) == 1)
                    {
                        dis = ((ii - sol.posX) * (ii - sol.posX)) + ((jj - sol.posY) * (jj - sol.posY));
                        dis = sqrt(dis);
                        if (min > dis)
                        {
                            min = dis;
                            minId = array[jj][ii];
                        }
                    }
                }
            }
            sol.charged = 0;

            char tmp[200];
            sprintf(tmp, "%d", minId);

            /* Open private fifo to read returned command output */
            if ((fp = open(tmp, O_RDWR) == -1))
            {

                perror("open error for server fifo");
            }
            else
            {
                /* and write our private fifo name there */
                // sleep(1);
                sprintf(focus, "%d", sol.focus);
                // printf("i am %d ready to shoot process %d and my focus is %d \n", getpid(), minId, sol.focus);
                //fflush(stdout);
                write(fp, focus, strlen(focus) + 1);//write the focus of the process that shot to the private fifo of the process that is shot
                // printf("i want to shoot  %d and my focus is %s\n ", getpid(), focus );
                //fflush(stdout);
                kill(minId, SIGUSR2);// send a signal to the process that is shot to inform it.

            }
            /*now close our private fifo */
            close(fp);       

        }
        
    }

    return 0;

}

/*this function returns 1 if the type of the process is from the oposite team anf zero if not*/
int isEnemy(int pid, char * type)
{

    if (strcmp(type, "S") == 0)
    {
        for (int k = 5; k < 10; k++)
        {
            if (s_pid[k] == pid)
            {

                return 1;
            }
        }
    }
    else
    {
        for (int k = 0; k < 5; k++)
        {
            if (s_pid[k] == pid)
            {

                return 1;
            }
        }
    }

    return 0;
}

/*set flag to 1 to make sure that the process dont pause again*/
void sigset_catcher2(int n)
{
    flag = 1;
}


/* This function is to read from private fifo the focus of the soldier that shoots and then generate random values determining the location of shooting */

void read_file()

{
  
    int location_of_shoot; // value to determine the location of the bullet to be shot 

    /* Open private fifo to read returned command output */

    privatefifo = open(msg.fifo_name, O_RDONLY | O_NONBLOCK);
    if ((n = read(privatefifo,  & buffer, B_SIZ)) > 0) // read from fifo the focus of the soldier that shoots
    {

        // write(fileno(stderr), buffer, n);
        // printf("\n\n buffer  %s\n\n",buffer );
        //fflush(stdout);
        buf = atoi(buffer); // convert the focus of soldier to integer
        //printf("\n\n buffer after convert %d\n\n",buf );
        //fflush(stdout);
        
        //focus is first initialized to 0

        if(buf >= -60 && buf <= 0) // if focus is larger than -60 and less than 0 meaning soldier has got approximately two bullets in hands 
        {
            srand(time(0));

            location_of_shoot = (int) (1 + (rand() % ( 7 - 1)));  //generate random value on all locations to be shot at
        }
        else if (buf >= -90 && buf <-60)  // if focus of soldier is less than -60 and larger than -90 meaning soldier has almost got one shot in head or other parts
        {
            srand(time(0));

            location_of_shoot = (int) (2 + (rand() % (7 - 2))); // generate random values on all locations to be shot without the head
        }

        else if (buf >= -130 && buf <-90) // if focus is less than -90 and larger than 130
        {
            srand(time(0));

            location_of_shoot = (int) (4 + (rand() % (7 - 4))); //soldier can only shoots on hands or legs or abdomen
        }
        
        else if (buf >= -160 && buf <-130) // if focus is less than -130 and larger than -160
        {
            srand(time(0));

            location_of_shoot = (int) (5 + (rand() % (7 - 5))); //soldier can only shoots on hands or legs
        }
        
        else if (buf <-160) // if focus is less than -160
        {
            srand(time(0));

            location_of_shoot = (int) (5 + (rand() % (8 - 5))); //soldier can only shoots on hands or legs and can be a miss shot
        }
        
/* here to determine the location of shoot giving them certain values */

        if(location_of_shoot == 1 ) // one for head
        {
            sol.head_counter++; //increment the head counter to determine when to die (after 2 bullets)
            sol.focus -= 20 ; // the focus of the soldier will decrease by 20
            
            // printf("i get a bullot in head %d \n", getpid());
            // printf("the focus of me %d is  %d : \n", getpid(), sol.focus);
        }
        else if (location_of_shoot == 2) // two for neck
        {
            sol.neck_counter++; //increment the neck counter to determine when to die (after 3 bullets)
            sol.focus -= 15; // the focus of the soldier will decrease by 15
            
            //printf("i get a bullot in neck %d \n", getpid());
            // printf("the focus of me %d is  %d : \n", getpid(), sol.focus);


        }
        else if (location_of_shoot == 3) // three foe chest
        {
            sol.chest_counter++; //increment the chest counter to determine when to die (after 3 bullets)
            sol.focus -= 15;   // the focus of the soldier will decrease by 15
            
            
            //printf("i get a bullot in chest %d \n", getpid());
            // printf("the focus of me %d is  %d : \n", getpid(), sol.focus);

        }
        else if (location_of_shoot == 4) // four for abdomen
        {
            sol.abdomen_counter++; //increment the abdomen counter to determine when to die (after 3 bullets)
            sol.focus -= 10;  // the focus of the soldier will decrease by 10
            
            // printf("i get  a bullot a abdomen %d \n", getpid());
            //printf("the focus of me %d is  %d : \n", getpid(), sol.focus);

        }
        else if (location_of_shoot == 5) //five for hands
        {
            sol.hands_counter++; //increment the hands counter to determine when to die (after 3 bullets)
            sol.focus -= 5;  // the focus of the soldier will decrease by 5
            //printf("i get a bullot in hands %d \n", getpid());
            // printf("the focus of me %d is  %d : \n", getpid(), sol.focus);

        }
        else if (location_of_shoot == 6) // six for legs 
        {
            sol.legs_counter++; //increment the legs counter to determine when to die (after 3 bullets)
            sol.focus -= 5; // the focus of the soldier will decrease by 5
            // printf("i get a bullot in legs %d \n", getpid());
            //printf("the focus of me %d is  %d : \n", getpid(), sol.focus);
        }

        else if (location_of_shoot == 7)
          {
           sol.miss++;
        //printf(" the shoot on me %d was miss\n", getpid());
        }




    }
    close(privatefifo);

    //unlink(msg.fifo_name);

}

/*this function is to read the second private fifo of this process which contains the location of the dead process*/
void dead_child()
{

    char buff[B_SIZ];
    char delim[] = " ";
    char * ptr;
    int x = 0;
    int y = 0;

    privatefifo= open(msg2.fifo_name, O_RDONLY | O_NONBLOCK);
    while ((n2 = read(publicfifo, & buffer2, B_SIZ)) > 0)
    {
        // write(fileno(stderr), buffer, n);
    
        ptr = strtok(buffer2, delim);
        x = atoi(ptr);
        ptr = strtok(NULL, delim);
        y = atoi(ptr);
        yellow();
        printf("\t\t==>The process id: %d was shot and can bee seen it's dead from %d process <==\n",array[y][x], getpid());
        //fflush(stdout);
        normals();
        array[y][x] = 0;//set the location of the dead process to zero, to make sure that no body aim for it.
        sleep(4);

    }

    close(publicfifo);

}


