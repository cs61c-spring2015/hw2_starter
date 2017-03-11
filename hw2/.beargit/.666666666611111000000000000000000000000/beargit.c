#include <stdio.h>
#include <string.h>
#include <dirent.h>
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

  FILE* fbranches = fopen(".beargit/.branches", "w");
  fprintf(fbranches, "%s\n", "master");
  fclose(fbranches);
   
  write_string_to_file(".beargit/.prev", "0000000000000000000000000000000000000000");
  write_string_to_file(".beargit/.current_branch", "master");

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
		for(int i=10;i<=40;i++)
		{
      if(commit_id[i] == start)
      {
			  commit_id[i] = new;
				break;
			}
		}
}
void next_commit_id_hw1(char* commit_id) {
  /* COMPLETE THE REST */
	if(commit_id[39] == '0')
	  generatenewid(commit_id,'0','1');
	else if(commit_id[39] == '1')
	{
		generatenewid(commit_id,'1','c');
	}
	else if(commit_id[39] == 'c')
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
int beargit_commit_hw1(const char* msg) {
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
	while(commit_dir_stream)
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

// ---------------------------------------
// HOMEWORK 2 
// ---------------------------------------

// This adds a check to beargit_commit that ensures we are on the HEAD of a branch.
int beargit_commit(const char* msg) {
  char current_branch[BRANCHNAME_SIZE];
  read_string_from_file(".beargit/.current_branch", current_branch, BRANCHNAME_SIZE);

  if (strlen(current_branch) == 0) {
    fprintf(stderr, "ERROR: Need to be on HEAD of a branch to commit\n");
    return 1;
  }

  return beargit_commit_hw1(msg);
}

const char* digits = "61c";

void next_commit_id(char* commit_id) {
  char current_branch[BRANCHNAME_SIZE];
  read_string_from_file(".beargit/.current_branch", current_branch, BRANCHNAME_SIZE);

  // The first COMMIT_ID_BRANCH_BYTES=10 characters of the commit ID will
  // be used to encode the current branch number. This is necessary to avoid
  // duplicate IDs in different branches, as they can have the same pre-
  // decessor (so next_commit_id has to depend on something else).
 // strtok(current_branch,"\n");
	int n = get_branch_number(current_branch);
  for (int i = 0; i < COMMIT_ID_BRANCH_BYTES; i++) {
    // This translates the branch number into base 3 and substitutes 0 for
    // 6, 1 for 1 and c for 2.
    commit_id[i] = digits[n%3];
    n /= 3;
  }
  // Use next_commit_id to fill in the rest of the commit ID.
  next_commit_id_hw1(commit_id);
}


// This helper function returns the branch number for a specific branch, or
// returns -1 if the branch does not exist.
int get_branch_number(const char* branch_name) {
  FILE* fbranches = fopen(".beargit/.branches", "r");

  int branch_index = -1;
  int counter = 0;
  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), fbranches)) {
	  strtok(line,"\n");
    if (strcmp(line, branch_name) == 0) {
      branch_index = counter;
    }
    counter++;
  }

  fclose(fbranches);

  return branch_index;
}

/* beargit branch
 *
 * See "Step 6" in the homework 1 spec.
 *
 */

int beargit_branch() {
  /* COMPLETE THE REST */
  FILE* fbranches = fopen(".beargit/.branches","r");
	FILE* fcurrent = fopen(".beargit/.current_branch","r");
  char current_branch[FILENAME_SIZE];
	char branch[FILENAME_SIZE];
	fgets(current_branch,sizeof(current_branch),fcurrent);
	strtok(current_branch,"\n");
  while(fgets(branch,sizeof(branch),fbranches) != NULL)
	{
	  strtok(branch,"\n");
    if(strcmp(branch,current_branch) == 0)
			printf("*<%s>\n",branch);
		else
		{
		  printf("<%s>\n",branch);
	  }
	}
	fclose(fcurrent);
	fclose(fbranches);
	return 0;
}

/* beargit checkout
 *
 * See "Step 7" in the homework 1 spec.
 *
 */

int checkout_commit(const char* commit_id) {
  /* COMPLETE THE REST */
	/* The following code form the path 
	 * for .index and copy the .index from
	 * the commit point .index.*/
	char commit_id_index[512];
	DIR *d;
	strcpy(commit_id_index,".beargit/.");
	strcat(commit_id_index,commit_id);
	d = opendir(commit_id_index);
	strcat(commit_id_index,"/.index");
	fs_cp(commit_id_index,".beargit/.index");
	if(d)
	{
	  struct dirent *dir;
	  while((dir = readdir(d))!= NULL)
		{
	    if(dir->d_name[0] != '.' )
      {
		    char commitfilename[512];
	      char beargitfile[512];
		    strcpy(commitfilename,".beargit/.");
		    strcat(commitfilename,commit_id);
		    strcat(commitfilename,"/");
		    strcat(commitfilename,dir->d_name);

				//strcpy(beargitfile,"./");
		    strcat(beargitfile,dir->d_name);
				fs_rm(beargitfile);
		    fs_cp(commitfilename,beargitfile);
				memset(beargitfile,0,sizeof(beargitfile));
			}
		}
  } 	
  /*The following code recover the .prev file*/ 
  return 0;
}

int is_it_a_commit_id(const char* commit_id) {
  /* COMPLETE THE REST */
  if(strlen(commit_id) ==40)
	{
	  int i;
	  for(i=0;i<40;i++)
		{
		  char temp = commit_id[i];
		  if(temp != '0')
			{
			  if(temp != '1')
				{
				  if(temp != '6')
				  {
					  if(temp != 'c')
						  return 0;
					}
				}
			}
		}
		return 1;
	}
}

int beargit_checkout(const char* arg, int new_branch) {
  // Get the current branch
  char current_branch[BRANCHNAME_SIZE];
  read_string_from_file(".beargit/.current_branch", current_branch, BRANCHNAME_SIZE);
  
	strtok(current_branch,"\n");
  // If not detached, update the current branch by storing the current HEAD into that branch's file...
  // Even if we cancel later, this is still ok.
  if (strlen(current_branch)) {
    char current_branch_file[BRANCHNAME_SIZE+50];
    sprintf(current_branch_file, ".beargit/.branch_%s", current_branch);
    fs_cp(".beargit/.prev", current_branch_file);
  }

  // Check whether the argument is a commit ID. If yes, we just stay in detached mode
  // without actually having to change into any other branch.
  if (is_it_a_commit_id(arg)) {
    char commit_dir[FILENAME_SIZE] = ".beargit/.";
    strcat(commit_dir, arg);
    if (!fs_check_dir_exists(commit_dir)) {
      fprintf(stderr, "ERROR: Commit %s does not exist\n", arg);
      return 1;
    }

    // Set the current branch to none (i.e., detached).
    write_string_to_file(".beargit/.current_branch", "");

    return checkout_commit(arg);
  }

  // Just a better name, since we now know the argument is a branch name.
  const char* branch_name = arg;

  // Read branches file (giving us the HEAD commit id for that branch).
  int branch_exists = (get_branch_number(branch_name) >= 0);

  // Check for errors.
  if (!(!branch_exists || !new_branch)) {
    fprintf(stderr, "ERROR: A branch named %s already exists\n", branch_name);
    return 1;
  } else if (!branch_exists && !new_branch) {
    fprintf(stderr, "ERROR: No branch %s exists\n", branch_name);
    return 1;
  }

  // File for the branch we are changing into.
  char* branch_file = ".beargit/.branch_"; 
  strcat(branch_file, branch_name);

  // Update the branch file if new branch is created (now it can't go wrong anymore)
  if (new_branch) {
    FILE* fbranches = fopen(".beargit/.branches", "a");
    fprintf(fbranches, "%s\n", branch_name);
    fclose(fbranches);
    fs_cp(".beargit/.prev", branch_file); 
  }

  write_string_to_file(".beargit/.current_branch", branch_name);

  // Read the head commit ID of this branch.
  char branch_head_commit_id[COMMIT_ID_SIZE];
  read_string_from_file(branch_file, branch_head_commit_id, COMMIT_ID_SIZE);

  // Check out the actual commit.
  return checkout_commit(branch_head_commit_id);
	}
