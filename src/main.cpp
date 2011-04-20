#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <iostream> 
using namespace std; 
#include <unistd.h> 
#include <sys/wait.h>

void addHeaders(FILE * fp)
{
    fprintf(fp, "#include <signal.h>\n");
    fprintf(fp, "#include <sys/ipc.h>\n");
    fprintf(fp, "#include <sys/types.h>\n");
    fprintf(fp, "#include <sys/shm.h>\n");
    fprintf(fp, "#include <sys/sem.h>\n");
    fprintf(fp, "#include <sys/user.h>\n");
    fprintf(fp, "#include <stdio.h>\n");
    fprintf(fp, "#include <string.h>\n");
    fprintf(fp, "#include <stdlib.h>\n");
    fprintf(fp, "#include <errno.h>\n");
    fprintf(fp, "#include <math.h>\n");
}

char * newString(const char * a ) { char * q = new char[strlen(q)+1]; strcpy( q, a ); return q; }
extern char **environ;

char exe[512];
char cpp[512];



void fatal(const char * a)
{
    fprintf( stderr, "gpp-error: %s\n", a); 
    if (exe[0]) remove(exe);
    if( cpp[0]) remove(cpp);
    sleep(1);
    exit(121);
}


void catchChild(int sig_num) {
    /* cuando estamos aquí, sabemos que hay un proceso hijo “zombie” esperando */
    int	childStatus;
    wait(&childStatus);
    if ( WIFEXITED(childStatus) ) {
        int rc =WEXITSTATUS(childStatus);
        remove(cpp);
        remove(exe);
        exit(rc);
    }

    
}



int executeMe( const char * exe, int argc, const char *argv[])
{
	char * * p = new char*[argc+1];
	int P = 0;
	p[P++] = newString(exe);
	int i;
	for(i=1;i<argc;i++)
	{	
		if (!strcmp( argv[i], "-e")) i++; else p[P++]=newString(argv[i]);
	}
	p[P] = 0;
   // { int m; for(m=0;m<argc;m++) printf("-argv[%d]=[%s]\n", m, argv[m] );}
   // { int m; for(m=0;p[m];m++) printf("-p[%d]=[%s]\n", m, p[m] );}


    signal(SIGCHLD, catchChild);
    
   // sigignore(SIGCHLD);
    
    pid_t cpid, w;
    int status;
    cpid = fork();
    if (cpid == -1) { perror("fork"); exit(EXIT_FAILURE); }
    if (cpid == 0) {            /* Code executed by child */
            
        
        int r = execvp( p[0], p );
        _exit(r);
        
        
    } else {                    /* Code executed by parent */
        do {
            
            
                     
            w = waitpid(cpid, &status, WUNTRACED | WCONTINUED);
            if (w == -1) { perror("waitpid"); exit(EXIT_FAILURE); }
            if (WIFEXITED(status)) {
        int rc =WEXITSTATUS(status);
        remove(cpp);
        remove(exe);
        exit(rc);


            } else if (WIFSIGNALED(status)) {
                printf("killed by signal %d\n", WTERMSIG(status));
            } else if (WIFSTOPPED(status)) {
                printf("stopped by signal %d\n", WSTOPSIG(status));
            } else if (WIFCONTINUED(status)) {
                printf("continued\n");
            }
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
   
        
        exit(EXIT_SUCCESS);
    }
    /*
    
    int status;
    
    {
        pid_t child_pid;
        child_pid = fork();
        if(child_pid != 0) {
            
           waitpid(child_pid, &status, 0);
            
        
            if ( WIFEXITED(status) ) {
                int rc =WEXITSTATUS(status);
                printf("status=%d\n", rc );
                return rc;
            }
            printf("status=%d\n", status );

            
            return status;
        } else {
            
        
            _exit(5);
            
//            int r = execv(p[0], p );
  //          printf("r==%d\n", r );
        }
    }
    
    
    wait(&status);
        

        fatal("bad exit");
    }
    fatal("can't never be here");
 */   return 0;
}



int main(int argc, const char * argv[])
{
	
	pid_t pid = getpid();
    
	sprintf( exe ,"/tmp/gpp_%ld", (long)pid );
	sprintf( cpp ,"/tmp/gpp_%ld.cpp", (long)pid );
	
	FILE * fp = fopen( cpp, "wb");
	if (!fp) { fprintf( stderr, "gpp-error: can't create tmp file (%s)\n", cpp ); exit(101); }
    
	addHeaders(fp);

	int do_maths = 0;

	fprintf( fp, "int main(int argc, const char * argv[]) {\n" );
	
	if (argc>1 && !strcmp(argv[1],"-m"))
	{
		int i;
		fprintf( fp, "double res = (");
	
		for(i=2;i<argc;i++) fprintf( fp, "%s ", argv[i] );	

		fprintf( fp, ");");

		fprintf( fp, "if(trunc(res)==res) printf(\"%%d\\n\", (int)res ); else printf(\"%%f\\n\", res );" );
	}
	else
	{
	int e;	
	for(e=1;e<argc-1;e++)
	{
        if(!strcmp(argv[e],"-e"))
        {
            fprintf( fp, "%s\n", argv[e+1] ); e++;
        }
	}
	}
	fprintf( fp, "}\n");
	fclose(fp);
	
	int L = 0;
	int e; 
	for(e=0;e<argc;e++) L+=strlen(argv[e]);
	char * cmd = new char [2*L+argc*100];
    
	sprintf( cmd, "g++ -o %s %s", exe, cpp );
#ifdef DEBUG
	fprintf(stderr, "gpp-debug: %s\n", cmd);
#endif
	int r = system(cmd); 
	if (r) { fprintf( stderr, "gpp-error: error compiling or linking (g++ returned %d)\n", r); remove(exe); remove(cpp); exit(102); }
    
    
    
#ifdef DEBUG
	fprintf( stderr, "gpp-debug: [%s]\n", cmd);
#endif
	r = executeMe( exe, argc, argv );
    
#ifdef DEBUG
	fprintf( stderr, "gpp-debug: returned %d %d\n", r, r%256 );
#else
	// remove(cpp);
    //	remove(exe);
#endif
	delete cmd; 
	return r;
}

