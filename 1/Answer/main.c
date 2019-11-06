
/* A simple One-Pass Compiler */
/* global.h */

#include <stdio.h>  /* include declarations for i/o routines */
#include <ctype.h>  /* ... and for character test routines */
#include <stdlib.h> /*and for some standard routines, such as exit*/
#include <string.h> /* ... and for string routines */

#define BSIZE  128  /* buffer size */
#define NONE   -1
#define EOS    '\0'

#define NUM    256
#define DIV    257
#define MOD    258
#define ID     259
#define DONE   260

extern int tokenval;   /*  value of token attribute */
extern int lineno;

struct entry {  /*  form of symbol table entry  */
  char *lexptr;
  int  token;
};

extern struct entry symtable[];  /* symbol table  */
void emit (int t, int tval);  /*  generates output  */
void error(char* m);  /* generates all error messages  */

//-----------------------------------------------------

/* lexer.c */

char lexbuf[BSIZE];
int  lineno = 1;
int  tokenval = NONE;

int lexan (FILE* fp){  /*  lexical analyzer  */
  int t;
  while(1) {
    t =  fgetc (fp);
    //printf("%c",t);
    if (t == ' ' || t == '\t')
      ;  /*  strip out white space  */

    else if(t=='/'){
	  t=fgetc(fp);

	  if(t=='/')
	    while(t!='\n'){

		    t=fgetc(fp);

		}
	  else
	  	ungetc(t,fp);
     }

    else if (t == '\n'){

      lineno = lineno + 1;
      //printf("line %d ",lineno);
    }
    else if (t=='"'){
        printf("STRING 100 // address of %c",t);
        do{
            t =  fgetc (fp);
            printf("%c",t);
        }while(t!='"');
        printf(" in strings buffer\n");
    }
    else if (isdigit (t)) {  /*  t is a digit  */
      ungetc(t, fp);
      fscanf(fp,"%d", &tokenval);
      //printf("*******%d",5);
      return NUM;
    }
    else if (isalpha(t)) {  /*  t is a letter */
      int p, b = 0;
      while (isalnum(t)) {  /* t is alphanumeric  */
        lexbuf [b] = t;
        t = fgetc (fp);
        b = b + 1;
        if (b >= BSIZE)
          error("compiler error");
      }
      lexbuf[b] = EOS;
      if (t != EOF)
        ungetc(t, fp);
      p = lookup (lexbuf);
      if (p == 0)
        p = insert (lexbuf, ID);
      tokenval = p;
      return symtable[p].token;
    }
    else if (t == EOF)
      return DONE;
    else {
      tokenval = t;
      return NONE;
    }
  }
}


//-----------------------------------------------------



//-----------------------------------------------------

/* emitter.c */
void emit (int t, int tval)  /*  generates output  */
{
  switch(t) {
  case '+' : case '-' : case '*' : case '/':
    printf("%c\n", t); break;
  case DIV:
    printf("DIV\n"); break;
  case MOD:
    printf("MOD\n"); break;
  case NUM:
    printf("%d NUM\n", tval); break;
  case ID:
    printf("ID %d //%s\n", tval-15,symtable[tval].lexptr); break;
  case 1: case 2 : case 3 : case 4 : case 5: case 6: case 7 : case 8 : case 9 : case 10: case 11: case 12 : case 13 : case 14 : case 15:
    printf("KEYWORD %d\n", tval); break;
  case NONE:
    printf("%c NONE\n", tval); break;
  default:
    printf("token %d, tokenval %d\n", t, tval);
  }
}

//-----------------------------------------------------

/* symbol.c */

#define STRMAX 999  /*  size of lexemes array  */
#define SYMMAX 100  /*  size of symbol table */

char lexemes[STRMAX];
int  lastchar = - 1;  /*  last used position in lexemes   */
struct entry symtable[SYMMAX];
int lastentry = 0;    /*  last used position in symtable  */

int lookup(char *s)      /*  returns position of entry for s */
{
  int p;
  for (p = lastentry; p > 0; p = p - 1)
    if (strcmp(symtable[p].lexptr, s) == 0)
      return p;
  return 0;
}

int insert(char *s, int tok)/*returns position of entry for s*/
{
  int len;
  len = strlen(s);  /*  strlen computes length of s     */
  if (lastentry + 1 >= SYMMAX)
    error ("symbol table full");
  if (lastchar + len + 1 >= STRMAX)
    error ("lexemes array full");
  lastentry = lastentry + 1;
  symtable[lastentry].token = tok;
  symtable[lastentry].lexptr = &lexemes[lastchar + 1];
  lastchar = lastchar + len + 1;
  strcpy(symtable[lastentry].lexptr, s);
  return lastentry;
}

//-----------------------------------------------------

/* init.c */

struct entry keywords[] = {
  { "array", 1 },
  { "boolean", 2 },
  { "char", 3 },
  { "else", 4 },
  { "false", 5 },
  { "for", 6 },
  { "function", 7 },
  { "if", 8 },
  { "integer", 9 },
  { "print", 10 },
  { "return", 11 },
  { "string", 12 },
  { "true", 13 },
  { "void", 14 },
  { "while", 15 }
};

void init()  /*  loads keywords into symtable  */
{
  struct entry *p;
  for (p = keywords; p->token; p++)
    insert(p->lexptr, p->token);
}


//-----------------------------------------------------

/* error.c */


void error(char* m)  /* generates all error messages  */
{
  fprintf(stderr, "line %d: %s\n", lineno, m);
  exit(EXIT_FAILURE);  /*  unsuccessful termination  */
}


//-----------------------------------------------------

/* main.c */


int main( )
{
  init();
  int result ;
  FILE *fp;
  fp = fopen("testcase01.b","rb+");
  if (fp == NULL)
      	error("File is not opened");
  while(1){
  result = lexan (fp);
  if(result!= DONE)
    emit(result,tokenval);
  else
    break;
  }
  fclose(fp);
  exit(0);    /*  successful termination  */
}



