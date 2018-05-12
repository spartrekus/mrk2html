
// mrk2html

//////////////////////////////////////////
//////////////////////////////////////////
//////////////////////////////////////////
#include <stdio.h>
#if defined(__linux__)
#define MYOS 1
#elif defined(_WIN32)
#define MYOS 2
#elif defined(_WIN64)
#define MYOS 3
#elif defined(__unix__)
#define MYOS 4
#define PATH_MAX 2500
#else
#define MYOS 0
#endif




int markup_output_format = 2;

////////////////////////////////////////
////////////////////////////////////////
////////////////////////////////////////
//#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h> 
#include <time.h>


/////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/// global vars which can be removed for better readability 
/////////////////////////////////////////////////////////
char slidebufferdata[200][PATH_MAX];
char slidebufferfigfile[PATH_MAX];
int  slidebufferfig=0;
int  foxy = 0;
int  slidebufferfoundsection = 0;
char slidemysection[PATH_MAX];
char mygraphicspath[PATH_MAX];
char myinputspath[PATH_MAX];





/////////////////////////////////
char *strampersand2txt(char *str)
{  
      char ptr[ 5* strlen(str)+1];
      int i,j=0;
      for(i=0; str[i]!='\0'; i++)
      {
        if ( str[i] == '&' ) 
	{
          ptr[j++]=':';
	}
        else
	{
          ptr[j++]=str[i];
	}
      } 
      ptr[j]='\0';
      size_t siz = 1 + sizeof ptr ; 
      char *r = malloc( 1 +  sizeof ptr );
      return r ? memcpy(r, ptr, siz ) : NULL;
}






/////////////////////////////////
/////////////////////////////////
char *strtxt2tex(char *str)
{  
      char ptr[ 5* strlen(str)+1];
      int i,j=0;
      for(i=0; str[i]!='\0'; i++)
      {

        if ( str[i] == '{' ) 
	{
          ptr[j++]='\\';
          ptr[j++]='{';
	}

        else if ( str[i] == '>' ) 
	{
          ptr[j++]='\\';
          ptr[j++]='e';
          ptr[j++]='n';
          ptr[j++]='s';
          ptr[j++]='u';
          ptr[j++]='r';
          ptr[j++]='e';
          ptr[j++]='m';
          ptr[j++]='a';
          ptr[j++]='t';
          ptr[j++]='h';
          ptr[j++]='{';
          ptr[j++]='>';
          ptr[j++]='}';
          ptr[j++]=' ';
	}

        else if ( str[i] == '<' ) 
	{
          ptr[j++]='\\';
          ptr[j++]='e';
          ptr[j++]='n';
          ptr[j++]='s';
          ptr[j++]='u';
          ptr[j++]='r';
          ptr[j++]='e';
          ptr[j++]='m';
          ptr[j++]='a';
          ptr[j++]='t';
          ptr[j++]='h';
          ptr[j++]='{';
          ptr[j++]='<';
          ptr[j++]='}';
          ptr[j++]=' ';
	}


        else if ( str[i] == '}' ) 
	{
          ptr[j++]='\\';
          ptr[j++]='}';
	}
        else if ( str[i] == '_' ) 
	{
          ptr[j++]='\\';
          ptr[j++]='_';
	}
        else if ( str[i] == '%' ) 
	{
          ptr[j++]='\\';
          ptr[j++]='%';
	}
        else if ( str[i] == '#' ) 
	{
          ptr[j++]='\\';
          ptr[j++]='#';
	}
        else if ( str[i] == '&' ) 
	{
          ptr[j++]='\\';
          ptr[j++]='&';
	}
        else
	{
          ptr[j++]=str[i];
	}
      } 
      ptr[j]='\0';
      size_t siz = 1 + sizeof ptr ; 
      char *r = malloc( 1 +  sizeof ptr );
      return r ? memcpy(r, ptr, siz ) : NULL;
}




/////////////////////////////////
char *strcsv2tex(char *str)
{  
      char ptr[ 5* strlen(str)+1];
      int i,j=0;
      for(i=0; str[i]!='\0'; i++)
      {
        if ( str[i] == ';' ) 
	{
          ptr[j++]=' ';
          ptr[j++]='&';
          ptr[j++]=' ';
	}
        else if ( str[i] == 9 ) 
	{
          ptr[j++]=' ';
          ptr[j++]='&';
          ptr[j++]=' ';
	}
        else
	{
          ptr[j++]=str[i];
	}
      } 
      ptr[j]='\0';
      size_t siz = 1 + sizeof ptr ; 
      char *r = malloc( 1 +  sizeof ptr );
      return r ? memcpy(r, ptr, siz ) : NULL;
}







int fexist(char *a_option)
{
  char dir1[PATH_MAX]; 
  char *dir2;
  DIR *dip;
  strncpy( dir1 , "",  PATH_MAX  );
  strncpy( dir1 , a_option,  PATH_MAX  );

  struct stat st_buf; 
  int status; 
  int fileordir = 0 ; 

  status = stat ( dir1 , &st_buf);
  if (status != 0) {
    fileordir = 0;
  }

  // this is compatible to check if a file exists
  FILE *fp2check = fopen( dir1  ,"r");
  if( fp2check ) {
  // exists
  fileordir = 1; 
  fclose(fp2check);
  } 

  if (S_ISDIR (st_buf.st_mode)) {
    fileordir = 2; 
  }
return fileordir;
/////////////////////////////
}




int beamercode = 0; 
int contentcode = 0; 
int option_system_call = 1;           // this will be 0 by default
int option_strtxt2tex_linefeed = 0;   // this shall be 0 by default for compact documents






char *fbasenoext(char* mystr)
{
    char *retstr;
    char *lastdot;
    if (mystr == NULL)
         return NULL;
    if ((retstr = malloc (strlen (mystr) + 1)) == NULL)
        return NULL;
    strcpy (retstr, mystr);
    lastdot = strrchr (retstr, '.');
    if (lastdot != NULL)
        *lastdot = '\0';
    return retstr;
}








///// some cat!! 
#include <fcntl.h>
#include <unistd.h>
static int cat_fd(int fd) 
{
  char buf[4096];
  ssize_t nread;

  while ((nread = read(fd, buf, sizeof buf)) > 0) 
  {
    ssize_t ntotalwritten = 0;
    while (ntotalwritten < nread) {
      ssize_t nwritten = write(STDOUT_FILENO, buf + ntotalwritten, nread - ntotalwritten);
      if (nwritten < 1)
        return -1;
      ntotalwritten += nwritten;
    }
  }

  return nread == 0 ? 0 : -1;
}




static int ncat_static(const char *fname) {
  int fd, success;
  if ((fd = open(fname, O_RDONLY)) == -1)
    return -1;

  success = cat_fd(fd);

  if (close(fd) != 0)
    return -1;

  return success;
}










///////////////////////////////
///////////////////////////////
///////////////////////////////
char *strtimestamp()
{
      long t;
      struct tm *ltime;
      time(&t);
      ltime=localtime(&t);
      char charo[50];  int fooi ; 
      fooi = snprintf( charo, 50 , "%04d%02d%02d%02d%02d%02d",
	1900 + ltime->tm_year, ltime->tm_mon +1 , ltime->tm_mday, 
	ltime->tm_hour, ltime->tm_min, ltime->tm_sec 
	);
    size_t siz = sizeof charo ; 
    char *r = malloc( sizeof charo );
    return r ? memcpy(r, charo, siz ) : NULL;
}





int strcount( char *str , int mychar )
{  
      int return_strcount = 0;
      char ptr[strlen(str)+1];
      int i,j = 0;
      for(i=0; str[i]!='\0'; i++)
      {
        if ( str[i] == mychar ) return_strcount++;
      } 
      return return_strcount;
}






void nrunwith( char *thecmd , char *thefile ){
       char cmdi[PATH_MAX];
       printf( "<CMD: %s>\n" , thecmd ) ; 
       printf( "<FILE: %s>\n" , thefile ) ; 
       strncpy( cmdi , "  " , PATH_MAX );
       strncat( cmdi , thecmd , PATH_MAX - strlen( cmdi ) -1 );
       strncat( cmdi , " \"" , PATH_MAX - strlen( cmdi ) -1 );
       strncat( cmdi , thefile , PATH_MAX - strlen( cmdi ) -1 );
       strncat( cmdi , "\"" , PATH_MAX - strlen( cmdi ) -1 );
       system( cmdi );
}







char *strcut( char *str , int myposstart, int myposend )
{  
      char ptr[strlen(str)+1];
      int i,j=0;
      for(i=0; str[i]!='\0'; i++)
      {
        if ( ( str[i] != '\0' ) && ( str[i] != '\0') )
         if ( ( i >=  myposstart-1 ) && (  i <= myposend-1 ) )
           ptr[j++]=str[i];
      } 
      ptr[j]='\0';
      size_t siz = sizeof ptr ; 
      char *r = malloc( sizeof ptr );
      return r ? memcpy(r, ptr, siz ) : NULL;
}



char *strtrim(char *str)
{  
      // right side to to finish
      char ptr[strlen(str)+1];
      int strposmax = strlen( str );
      int lastposchar = strposmax;
      int i,j=0;
      int foundspace = 1;

      /// find last char
      foundspace = 1;
      for( i= strposmax-1 ; i >= 0 ; i--)
      {
         //printf( "|%d-%d-%c| ", i , lastposchar , str[i] );
	 // find where to space
         if ( foundspace == 1 ) 
         if ( str[i] == ' ' ) 
   	    lastposchar = i-1;

         if ( str[i] != ' ' ) 
           foundspace = 0;
      } 

      // add the content
      foundspace = 1;
      for(i=0; i <= lastposchar ; i++)
      {
        if ( foundspace == 1 ) 
         if ( ( str[i] != ' ' )  && ( str[i] != 9 ) ) //added! with 9 for a TAB!!
          foundspace = 0;

        if ( foundspace == 0 ) 
           ptr[j++]=str[i];
      } 
      ptr[j]='\0';

      size_t siz = sizeof ptr ; 
      char *r = malloc( sizeof ptr );
      return r ? memcpy(r, ptr, siz ) : NULL;
}











/////////////////////////////////////
/////////////////////////////////////
/////////////////////////////////////
void filenew( char *fileout)
{
    FILE *fp5;
    fp5 = fopen( fileout , "wb+");
    fclose( fp5 );
}
/////////////////////////////////////
/////////////////////////////////////
/////////////////////////////////////
void fileappendbegin( char *fileout)
{
    FILE *fp5;
    fp5 = fopen( fileout , "ab+");
    //  fputs( "\\documentclass{article}\n", fp5 );
    //  fputs( "\\begin{document}\n", fp5 );
    fclose( fp5 );
}
/////////////////////////////////////
/////////////////////////////////////
/////////////////////////////////////
void fileappendend( char *fileout)
{
    FILE *fp5;
    fp5 = fopen( fileout , "ab+");
    // fputs( "\\end{document}\n", fp5 );
    fclose( fp5 );
}


/// fontsize
void fileappendfontbegin( char *fileout)
{
    FILE *fp5;
    fp5 = fopen( fileout , "ab+");
    fputs( "<font size=\"5\">\n", fp5 );
    fputs( "\n", fp5 );
    fclose( fp5 );
}
void fileappendfontend( char *fileout)
{
    FILE *fp5;
    fp5 = fopen( fileout , "ab+");
    fputs( "</font>\n", fp5 );
    fputs( "\n", fp5 );
    fclose( fp5 );
}



///////////////////////////////////////////////////////////////////
void filerawcat(  char *fileout, char *filein )
{
  int fetchi;
  FILE *fp5;
  FILE *fp6;
  char fetchline[PATH_MAX];
  char fetchlinetmp[PATH_MAX];

  /////////////////////////////////////////////////
  if ( fexist( filein ) == 1 )
  {
    fp5 = fopen( fileout , "ab+");
    fp6 = fopen( filein , "rb");
    while( !feof(fp6) ) 
    {
          fgets(fetchlinetmp, PATH_MAX, fp6); 
          strncpy( fetchline, "" , PATH_MAX );
          for( fetchi = 0 ; ( fetchi <= strlen( fetchlinetmp ) ); fetchi++ )
            if ( fetchlinetmp[ fetchi ] != '\n' )
              fetchline[fetchi]=fetchlinetmp[fetchi];

         if ( !feof(fp6) )
         {
 	      fputs( fetchline , fp5 );
  	      fputs( "\n", fp5 );
	 }
     }
     fclose( fp5 );
     fclose( fp6 );
   }
}





/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
char *strrlf(char *str)
{  
      char ptr[strlen(str)+1];
      int i,j=0;
      for(i=0; str[i]!='\0'; i++)
      {
        if (str[i] != '\n' && str[i] != '\n') 
        ptr[j++]=str[i];
      } 
      ptr[j]='\0';
      size_t siz = sizeof ptr ; 
      char *r = malloc( sizeof ptr );
      return r ? memcpy(r, ptr, siz ) : NULL;
}


/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
char *strsplit(char *str , int mychar , int myitemfoo )
{  
      char ptr[strlen(str)+1];
      int myitem = myitemfoo +1;
      int i,j=0;
      int fooitem = 0;
      for(i=0; str[i]!='\0'; i++)
      {
        if ( str[i] == mychar ) 
           fooitem++;
        //else if ( str[i] != mychar &&  fooitem == myitem-1  ) 
        else if ( str[i] != mychar &&  fooitem == myitem-2  ) 
           ptr[j++]=str[i];
      } 
      ptr[j]='\0';
      size_t siz = sizeof ptr ; 
      char *r = malloc( sizeof ptr );
      return r ? memcpy(r, ptr, siz ) : NULL;
}






/// customed one
char *strdelimit(char *str , int mychar1, int mychar2,  int mycol )
{ 
      char ptr[strlen(str)+1];
      char ptq[strlen(str)+1];
      strncpy( ptr, strsplit( str, mychar1 , mycol+1 ), strlen(str)+1 );
      strncpy( ptq, strsplit( ptr, mychar2 , 1 ), strlen(str)+1 );
      size_t siz = sizeof ptq ; 
      char *r = malloc( sizeof ptq );
      return r ? memcpy(r, ptq, siz ) : NULL;
}







char *fbasename(char *name)
{
  //char *name;
  char *base = name;
  while (*name)
    {
      if (*name++ == '/')
	{
	  base = name;
	}
    }
  return (base);
}



char *fextension(char *str)
{ 
      char ptr[strlen(str)+1];
      int i,j=0;
      for(i=strlen(str)-1 ; str[i] !='.' ; i--)
      {
        if ( str[i] != '.' ) 
            ptr[j++]=str[i];
      } 
      ptr[j]='\0';

      char ptrout[strlen(ptr)+1];  
      j = 0; 
      for( i=strlen(ptr)-1 ;  i >= 0 ; i--)
            ptrout[j++]=ptr[i];
      ptrout[j]='\0';

      size_t siz = sizeof ptrout ; 
      char *r = malloc( sizeof ptrout );
      return r ? memcpy(r, ptrout, siz ) : NULL;
}









//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////////////////////////////
char *cat_returnline( char *thefilefoo, int theline  )
{
  char catfetchlinetmp[PATH_MAX];
  char catfetchline[PATH_MAX];
  char buf[PATH_MAX];
  char ptr[PATH_MAX];
  ssize_t nread;
  int i,j,k;
  double linenbr = 0;
  i = 0; 
  j = 0;
  int beginend = 0;
    FILE *fp9;
    fp9 = fopen( thefilefoo , "rb");
    while( !feof(fp9) ) 
    {
          strncpy( catfetchline, "" , PATH_MAX );
          fgets(catfetchlinetmp, PATH_MAX, fp9); 
          linenbr++;
          for( i = 0 ; ( i <= strlen( catfetchlinetmp ) ); i++ )
            if ( catfetchlinetmp[ i ] != '\n' )
              catfetchline[i]=catfetchlinetmp[i];

         if ( !feof(fp9) )
         {
	      //printf( "%s", catfetchline );
	      //printf( "\n" );
              if ( linenbr == theline ) 
                 strncpy( ptr, catfetchline, PATH_MAX );
	 }
     }
     fclose( fp9 );

     size_t siz = 1 + sizeof ptr ; 
     char *r = malloc( 1 +  sizeof ptr );
     return r ? memcpy(r, ptr, siz ) : NULL;
}











//////////////////////////
/// depreciated - will be replaced!!
//////////////////////////
char *filename_newext( char *str , char *newext )
{
      char ptr[strlen(str)+1];
      int i,j=0;
      int foundpoint = 0; 
      for(i=strlen(str)-1 ; i!=-1 ; i--)
      {
          if ( str[i] == '.' ) foundpoint = i; 
      } 

     if ( foundpoint >= 1 ){
      int maxsize = strlen(str)+1+strlen(newext) ;
      char ptrnew[ maxsize ];
      strncpy( ptrnew, strcut( str, 1, foundpoint+1 ),  maxsize  ); 

      ptrnew[ foundpoint ] = '.';
      for(i=0 ; i<= strlen( newext)-1 ; i++)
         ptrnew[ foundpoint+1 +i ] = newext[ i ];
      ptrnew[ foundpoint +i+1] = '\0';

       size_t siz = sizeof ptrnew ; 
       char *r = malloc( sizeof ptrnew );
       return r ? memcpy(r, ptrnew, siz ) : NULL;
    } else return ""; 
}








///////////////////////////////////////////////////////////////////
void nfileunimark( char *fileout, char *filein )
{
  char poolfilename[PATH_MAX];

  int fetchi;
  FILE *fp5;
  FILE *fp6;
  char fetchline[PATH_MAX];
  char fetchlinetmp[PATH_MAX];
  char fetchcmdline[PATH_MAX];
  int fetchcmdlinelen = 0;

  char charin[PATH_MAX];
  char charout[PATH_MAX];
  char lineout[PATH_MAX];

  char mkdirdir[PATH_MAX];
  char ndirfig[PATH_MAX];
  char usertextfieldone[PATH_MAX];
  char usertextfieldtwo[PATH_MAX];
  char wgetcmd[PATH_MAX];
  char extcmd[PATH_MAX];

  int foundcode = 0; 
  int advancedcode = 1; 
  int commenton = 0; 
  int beginitemize = 0; 

  int itemlevel = 0; 

  int beamerlevel = 0; 
  int numberinglevel = 0; 
  int unilistemptyline = 0; 
  int fooi; 
  //int fetchcmdpos=0;
  int foundcmd = 0;

  char mycurrenttag[PATH_MAX];
  strncpy( mycurrenttag, "", PATH_MAX );


  int uninewline = 0;

  /////////////////////////////////////////////////
  if ( fexist( filein ) == 1 )
  {
    fp5 = fopen( fileout , "ab");
    fp6 = fopen( filein , "rb");
    while( !feof(fp6) ) 
    {
          // vars
	  foundcode = 0; 
	  foundcmd = 0; 

	  //
	  // this fgets is there to be easy to port to any systems
	  // fgets can be easily ported to things such as PHP !!
	  //
          uninewline = 0;
          fgets( fetchlinetmp, PATH_MAX, fp6); 
          strncpy( fetchline , "" , PATH_MAX );
          strncpy( fetchcmdline , "" , PATH_MAX );
	  //fetchcmdpos=0;


          // original remove lf
	  /*
          for( fetchi = 0 ; ( fetchi <= strlen( fetchlinetmp ) ); fetchi++ )
            if ( fetchlinetmp[ fetchi ] != '\n' )
              fetchline[fetchi]=fetchlinetmp[fetchi];
	  */


          // 20170811-180203
          // remove lf and more
          for( fetchi = 0 ; ( fetchi <= strlen( fetchlinetmp ) ); fetchi++ )
            if ( fetchlinetmp[ fetchi ] != '\n' )
	    {
              fetchline[fetchi]=fetchlinetmp[fetchi];
              if ( fetchlinetmp[ fetchi ] == '!' ) 
	         foundcmd = 1 ;
              else if ( fetchlinetmp[ fetchi ] == '|' ) 
	         foundcmd = 1 ;
	    }


          
          strncpy( fetchcmdline, strtrim( fetchline ) , PATH_MAX );
          fetchcmdlinelen = strlen(fetchcmdline);


           


           if ( !feof(fp6) )
           {

            if ( strcmp( fetchcmdline, "" ) == 0 )    uninewline = 1;

            /////////////////////////////////////
            /////////////////////////////////////
            /////////////////////////////////////
            /////////////////////////////////////
            ////  for comment mycomment
	    ////  type 1 like in C language
            //////////////////////////////////
            if ( foundcode == 0 )
            if ( fetchline[0] == '/' )
            if ( fetchline[1] == '/' )
            {
  	      foundcode = 1;
            }

            //////////////////////////////////
            //////////////////////////////////
            //////////////////////////////////
            if ( foundcode == 0 )
            if ( fetchcmdline[0] == '/' )
            if ( fetchcmdline[1] == '/' )
            {
  	      foundcode = 1;
            }





            /////////////////////////////////////
            if ( foundcode == 0 )
            if (( fetchcmdline[0] == '/' ) && ( fetchcmdline[1] == '*' ))
            {
  	      commenton = 1;
    	      foundcode = 1;
            }
            ////////////////////////////////////
            if ( foundcode == 0 )
            if ( ( fetchcmdline[ fetchcmdlinelen -2 ] == '*' ) && ( fetchcmdline[ fetchcmdlinelen -1 ] == '/' ))
            {
  	      commenton = 0;
    	      foundcode = 1;
            }
            ////////////////////////////////////
            //fetchcmdlinelen = strlen(fetchcmdline);
            ////////////////////////////////////
            if ( foundcode == 0 )
            if ( ( fetchcmdline[0] == '*' ) && ( fetchcmdline[1] == '/' ))
            {
  	      commenton = 0;
    	      foundcode = 1;
            }

	    //// active if needed
            if ( commenton == 1 )
            {
    	      foundcode = 1;
            }

            // =================================
            if ( foundcode == 0 )
            if ( fetchcmdline[0] == '=' ) 
	    if ( fetchcmdline[1] == '=' ) 
	    if ( fetchcmdline[2] == '=' ) 
	    if ( fetchcmdline[3] == '=' ) 
	    if ( fetchcmdline[4] == '=' ) 
	    if ( fetchcmdline[5] == '=' ) 
	    if ( fetchcmdline[6] == '=' ) 
	    if ( fetchcmdline[7] == '=' ) 
	    if ( fetchcmdline[8] == '=' )
            {
  	      foundcode = 1;
            }
            // =================================
            if ( foundcode == 0 )
            if ( fetchcmdline[0] == '-' ) 
	    if ( fetchcmdline[1] == '-' ) 
	    if ( fetchcmdline[2] == '-' ) 
	    if ( fetchcmdline[3] == '-' ) 
	    if ( fetchcmdline[4] == '-' ) 
	    if ( fetchcmdline[5] == '-' )
	    if ( fetchcmdline[6] == '-' ) 
	    if ( fetchcmdline[7] == '-' ) 
	    if ( fetchcmdline[8] == '-' )
            {
  	      foundcode = 1;
            }


            /////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////
            if ( foundcode == 0 )  // deprecated, but ok, that's too modern
            if ( fetchline[0] == '!' ) 
            if ( fetchline[1] == 's' ) 
            if ( fetchline[2] == 'e' ) 
            if ( fetchline[3] == 'c' ) 
            if ( fetchline[4] == ' ' ) 
            if ( markup_output_format == 2 )
            {
 	        fputs( "<h1>" , fp5 );
 	        fputs( strtrim( strcut( fetchline, 4+2, strlen( fetchline ))) , fp5 );
  	        fputs( "</h1>\n", fp5 );
  	      foundcode = 1;
            }

            /////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////
            if ( foundcode == 0 )  // deprecated, but ok, that's too modern
            if ( fetchline[0] == '!' ) 
            if ( fetchline[1] == '=' ) 
            if ( fetchline[2] == ' ' ) 
            if ( markup_output_format == 2 )
            {
 	        fputs( "<h1>" , fp5 );
 	        fputs( strtrim( strcut( fetchline, 2+2, strlen( fetchline ))) , fp5 );
  	        fputs( "</h1>\n", fp5 );
  	      foundcode = 1;
            }

            /////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////
            if ( foundcode == 0 ) // deprecated, but ok, that's too modern
            if ( fetchline[0] == '!' ) 
            if ( fetchline[1] == '=' ) 
            if ( fetchline[2] == '=' ) 
            if ( fetchline[3] == ' ' ) 
            if ( markup_output_format == 2 )
            {
 	        fputs( "<h2>" , fp5 );
 	        fputs( strtrim( strcut( fetchline, 3+2, strlen( fetchline ))) , fp5 );
  	        fputs( "</h2>\n", fp5 );
  	      foundcode = 1;
            }


            /////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////
            if ( foundcode == 0 ) // deprecated, but ok, that's too modern
            if ( fetchline[0] == '!' ) 
            if ( fetchline[1] == '=' ) 
            if ( fetchline[2] == '=' ) 
            if ( fetchline[3] == '=' ) 
            if ( fetchline[4] == ' ' ) 
            if ( markup_output_format == 2 )
            {
 	      fputs( "<h3>" , fp5 );
 	      fputs( strtrim( strcut( fetchline, 4+2, strlen( fetchline ))) , fp5 );
  	      fputs( "</h3>\n", fp5 );
  	      foundcode = 1;
            }




            /////////////////////////////
	    if ( foundcode == 0 )
	    {
	        if ( beginitemize == 1 )
		{
		  //fputs( "\\iexitemend\n" , fp5 ); 
		  //fputs( "\n" , fp5 ); 
		}
 	        fputs( fetchline , fp5 );

                if ( markup_output_format == 2 )
                 if ( foundcode == 0 )
                 {
                    //if ( uninewline == 1 )
 	            fputs( "<br>" , fp5 );
  	            fputs( "\n", fp5 );
                 }

 	        //fputs( "<br>" , fp5 );
  	        //fputs( "\n", fp5 );

	        beginitemize = 0;
            }



         //////// END
	 }
     }
     fclose( fp5 );
     fclose( fp6 );
   }
}










///////////////////////////////////
int question_continue()
{
	    char c;
	    int foo;
	    printf("Do you want to continue (y/n)? ");
	    scanf(" %c",&c); 
	    c = toupper(c);
	    if   ( ( c == 'Y' ) ||    ( c == 'y' ) )
	    {
	       printf( "Yes\n");
	       foo = 1;
	    }
	    else if ( c == '1' ) 
	    {
	       printf( "Yes\n");
	       foo = 1;
	    }
	    else  
	    {
	      printf( "No\n");
	      foo = 0;
	    }
	    return foo; 
}



void show_unimark_logo()
{
         printf("================= \n");
         printf("|||- UNIMARK -||| \n");
         printf("================= \n");
}







////////////////////////////////
/// check for enddoc 
////////////////////////////////
int cat_fdinfo( char *filein )
{
  char fetchlinetmp[4096];
  char fetchline[4096];
  char buf[4096];
  char ptr[4096];
  ssize_t nread;
  int i,j,k;
  i = 0 ; j = 0;
  int beginend = 0;
    FILE *fp6;
    fp6 = fopen( filein , "rb");
    while( !feof(fp6) ) 
    {
          fgets(fetchlinetmp, 4096, fp6); 
          strncpy( fetchline, "" , 4096 );
          for( i = 0 ; ( i <= strlen( fetchlinetmp ) ); i++ )
            if ( fetchlinetmp[ i ] != '\n' )
              fetchline[i]=fetchlinetmp[i];

         if ( !feof(fp6) )
         {
              if ( strcmp( fetchline , "!enddoc" ) == 0 )
                   beginend++;
              else if ( strcmp( fetchline , "\\end{document}" ) == 0 ) 
                   beginend++;
	 }
     }
     fclose( fp6 );

  printf( "Info Begin+End: %d \n" , beginend );
  return beginend;
}











int main( int argc, char *argv[])
{
    char targetfile[PATH_MAX];
    strncpy( slidemysection, "", PATH_MAX );
    strncpy( mygraphicspath, "" , PATH_MAX );
    strncpy( myinputspath , "" , PATH_MAX );
    FILE *fpout;






    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    printf("================= \n");
    printf("|||- UNIMARK -||| \n");
    printf("================= \n");
    printf("- An Universal Markup Language - \n");
    printf("- Cross-Platform, using plain C language - \n");
    printf("- Author: GITHUB Spartrekus - \n");
    printf("- LICENCE : GNU - \n");



    ////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////
    if ( argc == 2)
      if ( fexist( argv[1] ) == 1 )
      {
          strncpy( targetfile, fbasenoext( argv[ 1 ] ) , PATH_MAX );
          strncat( targetfile , ".html" , PATH_MAX - strlen( targetfile ) -1 );
          printf( "Target: %s\n" , targetfile );
          printf("  >SRC: %s => TRG: %s \n", argv[1] , targetfile  );
          filenew( targetfile );
          fileappendfontbegin( targetfile );
          nfileunimark( targetfile , argv[1] );
          fileappendfontend( targetfile );
          fileappendend( targetfile );
          return 0;
      }



    /////////////////////////////
    /////////////////////////////
    /////////////////////////////
    if ( argc == 2)      printf( "Usage: unimark filein.mrk fileout.tex \n" );
    else if ( argc == 1) printf( "Usage: unimark filein.mrk fileout.tex \n" );

    return 0;

}





