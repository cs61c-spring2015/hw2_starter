#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>

#include "beargit.h"
#include "util.h"
#define MAXBUFFSIZE 512
/* Implementation Notes:
 *
 * - Functions return 0 if successful, 1 if there is an error.
 * - All error conditions in the function description need to be implemented
 *   and written to stderr. We catch some additional errors for you in main.c.
 * - Output to stdout needs to be exactly as specified in the function description.
 * - Only edit this file (beargit.c)
 * - You are given the following helper functions:
 *   * fs_mkdir(dirname): create directory <dirname>
 *   * fs_rm(filename): delete file <filename>
 *   * fs_mv(src,dst): move file <src> to <dst>, overwriting <dst> if it exists
 *   * fs_cp(src,dst): copy file <src> to <dst>, overwriting <dst> if it exists
 *   * write_string_to_file(filename,str): write <str> to filename (overwriting contents)
 *   * read_string_from_file(filename,str,size): read a string of at most <size> (incl.
 *     NULL character) from file <filename> and store it into <str>. Note that <str>
 *     needs to be large enough to hold that string.
 *  - You NEED to test your code. The autograder we provide does not contain the
 *    full set of tests that we will run on your code. See "Step 5" in the homework spec.
 */

/* beargit init
 *
 * - Create .beargit directory
 * - Create empty .beargit/.index file
 * - Create .beargit/.prev file containing 0..0 commit id
 *
 * Output (to stdout):
 * - None if successful
 */

int beargit_init(void) {
  fs_mkdir(".beargit");

  FILE* findex = fopen(".beargit/.index", "w");
  fclose(findex);
  
  write_string_to_file(".beargit/.prev", "0000000000000000000000000000000000000000");

  return 0;
}


/* beargit add <filename>
 * 
 * - Append filename to list in .beargit/.index if it isn't in there yet
 *
 * Possible errors (to stderr):
 * >> ERROR: File <filename> already added
 *
 * Output (to stdout):
 * - None if successful
 */

int beargit_add(const char* filename) {
  FILE* findex = fopen(".beargit/.index", "r");
  FILE *fnewindex = fopen(".beargit/.newindex", "w");

  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");
    if (strcmp(line, filename) == 0) {
      fprintf(stderr, "ERROR: File %s already added\n", filename);
      fclose(findex);
      fclose(fnewindex);
      fs_rm(".beargit/.newindex");
      return 3;
    }

    fprintf(fnewindex, "%s\n", line);
  }

  fprintf(fnewindex, "%s\n", filename);
  fclose(findex);
  fclose(fnewindex);

  fs_mv(".beargit/.newindex", ".beargit/.index");

  return 0;
}


/* beargit rm <filename>
 * 
 * See "Step 2" in the homework 1 spec.
 *
 */

int beargit_rm(const char* filename) {
  /* COMPLETE THE REST */
	FILE* findex = fopen(".beargit/.index","r");
  FILE* copyfile = fopen(".beargit/.copyindex","w");
	char line[FILENAME_SIZE];
	int found = 0;
	while(fgets(line,sizeof(line),findex))
	{
		strtok(line,"\n");
		if(strcmp(line,filename) != 0)
		{
			fprintf(copyfile,"%s\n",line);
		}
		else
		{
			found = 1;
		}	
	}
	if(!found)
	{
		fprintf(stderr,"ERROR: File %s not tracked\n",filename);
		fclose(findex);
		fclose(copyfile);
		fs_rm(".beargit/.copyindex");
		return 1;
	}
	fclose(findex);
	fclose(copyfile);
  fs_mv(".beargit/.copyindex", ".beargit/.index");
	return 0;
}

/* beargit commit -m <msg>
 *
 * See "Step 3" in the homework 1 spec.
 *
 */

const char* go_bears = "GO BEARS!";

int is_commit_msg_ok(const char* msg) {
  /* COMPLETE THE REST */
  if(strstr(msg,go_bears)!= NULL)
		return 1;
	return 0;

}
void generatenewid(char* commit_id,char start,char new)
{
		for(int i=39;i>0;i--)
		{
      if(commit_id[i] == start)
      {
			  commit_id[i] = new;
				break;
			}
		}
}
void next_commit_id(char* commit_id) {
  /* COMPLETE THE REST */
	if(commit_id[0] == '0')
	  generatenewid(commit_id,'0','1');
	else if(commit_id[0] == '1')
	{
		generatenewid(commit_id,'1','c');
	}
	else if(commit_id[0] == 'c')
	{
		generatenewid(commit_id,'c','6');
	}
}

int copy(char* src,char* dst)
{
	FILE* srcfile;
  FILE* dstfile;

	if((srcfile = fopen(src,"r")) == NULL)
	{
    fprintf(stderr,"Error in open the file %s\n",srcfile);
	  exit(0);
	}
  if((dstfile = fopen(dst,"w+")) == NULL)
	{
		fprintf(stderr,"Error in open the file %s\n",dstfile);
		exit(0);
	}
	char ch;
	while( (ch = fgetc(srcfile))!= EOF )
	{
	  fputc(ch,dstfile); 
	}
	fclose(srcfile);
	fclose(dstfile);
}
int beargit_commit(const char* msg) {
  if (!is_commit_msg_ok(msg)) {
    fprintf(stderr, "ERROR: Message must contain \"%s\"\n", go_bears);
    return 1;
  }

  char commit_id[COMMIT_ID_SIZE];
  read_string_from_file(".beargit/.prev", commit_id, COMMIT_ID_SIZE);
  next_commit_id(commit_id);
	char foldername[512];//max file name 512 + 40 ID length
	char copyfile[512];//copy file of .prve
	char msgfile[512];//create an array for the msgfilename
	strcpy(foldername,".beargit/.");
  strcat(foldername,commit_id);
	fs_mkdir(foldername);
	FILE* beargitdir;
	if((beargitdir = fopen(".beargit/.index","r"))== NULL)
	{
		fprintf(stderr,"Error in open .beargit/.index");
		exit(1);
	}
	char trackedfile[512];
  while(fgets(trackedfile,512,beargitdir)!=NULL)
	{
		strtok(trackedfile,"\n");
    char tempdirname[512];
		strcpy(tempdirname,foldername);
		strcat(tempdirname,"/");
		strcat(tempdirname,trackedfile);
		copy(trackedfile,tempdirname);
	}
	strcpy(copyfile,foldername);
	strcpy(msgfile,foldername);
	strcat(msgfile,"/.msg");
	strcat(foldername,"/.index");
	strcat(copyfile,"/.prev");
	copy(".beargit/.index",foldername);
	copy(".beargit/.prev",copyfile);
	
	write_string_to_file(msgfile,msg);
	write_string_to_file(".beargit/.prev",commit_id);
  return 0;
}
/* beargit status
 *
 * See "Step 1" in the homework 1 spec.
 *
 */

int beargit_status() {
  /* COMPLETE THE REST */
	//char buff[MAXBUFFSIZE];
	char buff[MAXBUFFSIZE];
	FILE *f;
	if((f = fopen(".beargit/.index","r")) == NULL)
	{
		fprintf(stdout,"Can't find the file .beargit/.index");
	}
	int count =0;
	while(fgets(buff,MAXBUFFSIZE,f)!=NULL)
	{
		int length = strlen(buff);
		buff[length-1] = '\0';
		printf("<%s>\n",buff);
		count++;
	}
	printf("<%d>files total\n",count);
	fclose(f);

  return 0;
}

/* beargit log
 *
 * See "Step 4" in the homework 1 spec.
 *
 */

int beargit_log() {
  /* COMPLETE THE REST */
	char commitdir[COMMIT_ID_BYTES];
  read_string_from_file(".beargit/.prev",commitdir,COMMIT_ID_BYTES);
	char last_commit_dir[FILENAME_SIZE];
	strcpy(last_commit_dir,".beargit/.");
	strcat(last_commit_dir,commitdir);
	/*loop to read the commit directory and msgdir*/
  FILE* commit_dir_stream = fopen(last_commit_dir,"r");
	if(commit_dir_stream == NULL)
	{
	  fprintf(stderr,"ERROR: There are no commits!\n");
		return 1;	
  }
	while(commit_dir_stream != NULL)
	{
		char commit_nextID[COMMIT_ID_BYTES];
    char commit_msg[FILENAME_SIZE];
	
		char commit_prev_path[FILENAME_SIZE];
		char commit_msg_path[FILENAME_SIZE];

		/*make the .prev file path*/
		strcpy(commit_prev_path,last_commit_dir);
    strcat(commit_prev_path,"/.prev");
		/*make the .msg file path*/
		strcpy(commit_msg_path,last_commit_dir);
		strcat(commit_msg_path,"/.msg");
		/*read content*/
	  read_string_from_file(commit_prev_path,commit_nextID,COMMIT_ID_BYTES); 
		read_string_from_file(commit_msg_path,commit_msg,FILENAME_SIZE);
    /*format the output*/
    printf("\n");
		printf("commit <%s>\n",commitdir);
		printf("  <%s>\n",commit_msg);
    /*change the dir for next loop*/
    strcpy(commitdir,commit_nextID);
		strcpy(last_commit_dir,".beargit/.");
		strcat(last_commit_dir,commitdir);
		fclose(commit_dir_stream);
		commit_dir_stream = fopen(last_commit_dir,"r");
	}
	printf("\n");
  


	return 0;

}
