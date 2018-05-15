%token	END 0
%token	IMPORT "import" PACKAGE "package" IDENTIFIER STRINGLITERAL
%%

program:		statements;

statements:		statements statement
|				statement;

statement: 		package
| 				imports;

package:		"package" IDENTIFIER;

import:			"import" importitem
|				"import" '(' importitems ')';

imports:		imports import
|				import;

importitem:		STRINGLITERAL
|				IDENTIFIER STRINGLITERAL;

importitems:	importitems importitem
|				importitem;
