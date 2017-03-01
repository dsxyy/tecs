D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
E			[Ee][+-]?{D}+
FS		(f|F|l|L)
IS			(u|U|l|L)*

%{
#include "token.h"
%}

%%
[\t\v\n\f]		                 { ; }
";"			                     { ; }
","			                     { ; }

{L}({L}|{D})*		             { 
									//printf("IDENTIFIER yytext: %s\n",yytext);  
									return IDENTIFIER;
								 }
								 
0[xX]{H}+{IS}?		             { 
									//printf("INTEGER_HEX yytext: %s\n",yytext);  
									return INTEGER_HEX;
								 }
								 
0{D}+{IS}?		                 { 
									//printf("INTEGER_DEC yytext: %s\n",yytext);  
									return INTEGER_DEC;
							     }
							     
{D}+{IS}?		                 { 
									//printf("INTEGER_DEC yytext: %s\n",yytext);  
									return INTEGER_DEC;
								 }
								 
-{D}+{IS}?		                 { 
									//printf("INTEGER_DEC yytext: %s\n",yytext);  
									return INTEGER_DEC;
								 }
                                 
L?'(\\.|[^\\'])+'	                 { 
									//printf("STRING1 yytext: %s\n",yytext);  
								    return STRING;
								 }
								 
L?\"(\\.|[^\\"])*\"	             { 
									//printf("STRING2 yytext: %s\n",yytext);  
									return STRING;
								 }
								 
"("			                     { return '(';}
")"			                     { return ')';}
"="			                     { return '=';}
" "								 { ; }
.			                     { printf("syntax error:  %s\n",yytext); }
%%
 
int yywrap (yyscan_t yyscanner )
{
	return(1);
}
