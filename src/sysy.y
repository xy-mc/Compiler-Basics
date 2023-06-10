%code requires {
  #include <memory>
  #include <string>
  #include "AST/ast.hpp"
  #include <iostream>
}

%{

#include <iostream>
#include <memory>
#include <string>
#include "AST/ast.hpp"
// 声明 lexer 函数和错误处理函数
int yylex();
void yyerror(std::unique_ptr<BaseAST> &ast, const char *s);

using namespace std;

%}

// 定义 parser 函数和错误处理函数的附加参数
// 我们需要返回一个字符串作为 AST, 所以我们把附加参数定义成字符串的智能指针
// 解析完成后, 我们要手动修改这个参数, 把它设置成解析得到的字符串
%parse-param { std::unique_ptr<BaseAST> &ast }

// yylval 的定义, 我们把它定义成了一个联合体 (union)
// 因为 token 的值有的是字符串指针, 有的是整数
// 之前我们在 lexer 中用到的 str_val 和 int_val 就是在这里被定义的
// 至于为什么要用字符串指针而不直接用 string 或者 unique_ptr<string>?
// 请自行 STFW 在 union 里写一个带析构函数的类会出现什么情况
%union {
  std::string *str_val;
  int int_val;
  BaseAST *ast_val;
  BlockItemListAST *block_val;
  ConstDefListAST *codef_val;
  VarDefListAST *vadef_val;
  CompUnitAST *comp_unit;
  FuncFParamsAST *funcf_params;
  FuncRParamsAST *funcr_params;
}

// lexer 返回的所有 token 种类的声明
// 注意 IDENT 和 INT_CONST 会返回 token 的值, 分别对应 str_val 和 int_val
%token INT RETURN EQ NEQ AND OR GTE LTE CONST IF ELSE WHILE BREAK CONTINUE VOID
%token <str_val> IDENT
%token <int_val> INT_CONST
%left OR AND
%left EQ NEQ
%left GTE LTE GT LT

%nonassoc LOWER_THEN_ELSE
%nonassoc ELSE
%start Program
// 非终结符的类型定义
%type <ast_val> FuncDef Block Stmt Exp PrimaryExp Number UnaryExp AddExp MulExp
%type <ast_val> RelExp EqExp LAndExp LOrExp Decl ConstDecl BType ConstDef ConstInitVal BlockItem
%type <ast_val> LVal ConstExp VarDecl VarDef InitVal FuncFParam DeclDef
%type <block_val> BlockItemList
%type <codef_val> ConstDefList
%type <vadef_val> VarDefList
%type <comp_unit> CompUnit;
%type <funcf_params> FuncFParams
%type <funcr_params> FuncRParams
%%
Program
  :CompUnit 
  {
    ast = unique_ptr<CompUnitAST>($1);
	}
	;

// 编译单元
CompUnit
  :CompUnit DeclDef 
  {
    auto ast = $1;
    ast->decldef_.push_back(unique_ptr<BaseAST>($2));
    $$=ast;
	}
	|DeclDef 
  {
    auto ast=new CompUnitAST();
		ast->decldef_.push_back(unique_ptr<BaseAST>($1));
    $$=ast;
	}
	;

//声明或者函数定义
DeclDef
  : Decl 
  {
		auto ast = new DeclDefAST();
		ast->decl = unique_ptr<BaseAST>($1);
    ast->tid=DeclDefAST::declID;
    $$=ast;
	}
	|FuncDef 
  {
    auto ast =  new DeclDefAST();
	  ast->funcdef = unique_ptr<BaseAST>($1);
    ast->tid=DeclDefAST::funcID;
    $$=ast;
	}
  ;

FuncDef
  : BType IDENT '(' ')' Block 
  {
    auto ast = new FuncDefAST();
    ast->btype = unique_ptr<BaseAST>($1);
    ast->ident = *unique_ptr<string>($2);
    ast->block = unique_ptr<BaseAST>($5);
    $$ = ast;
  }
  | BType IDENT '(' FuncFParams ')' Block
  {
    auto ast = new FuncDefAST();
    ast->btype = unique_ptr<BaseAST>($1);
    ast->ident = *unique_ptr<string>($2);
    ast->funcfparams=unique_ptr<BaseAST>($4);
    ast->block = unique_ptr<BaseAST>($6);
    $$ = ast;
  }
  ;

FuncFParams
  : FuncFParam
  {
    auto ast=new FuncFParamsAST();
    ast->funcfparam_.push_back(unique_ptr<BaseAST>($1));
    $$=ast;
  }
  | FuncFParams ',' FuncFParam
  {
    auto ast=$1;
    ast->funcfparam_.push_back(unique_ptr<BaseAST>($3));
    $$=ast;
  }
  ;

FuncFParam
  :BType IDENT
  {
    auto ast=new FuncFParamAST();
    ast->btype=unique_ptr<BaseAST>($1);
    ast->ident=*unique_ptr<string>($2);
    $$=ast;
  }
  ;

BType
  : INT                           
  {
    auto ast = new BTypeAST();
    ast->tid=BTypeAST::intID;
    $$=ast;
  }
  |VOID
  {
    auto ast = new BTypeAST();
    ast->tid=BTypeAST::voidID;
    $$=ast;
  }
  ;


// 语句块
Block
  :	'{' '}' 
  {
		auto ast = new BlockAST();
    $$=ast;
	}
	|'{' BlockItemList '}' 
  {
		auto ast = new BlockAST();
		ast->blockitem_.swap($2->blockitem_);
    $$=ast;
	}	
	;

// 语句块项列表
BlockItemList
  : BlockItem	
  {
		auto ast = new BlockItemListAST();
		ast->blockitem_.push_back(unique_ptr<BaseAST>($1));
    $$=ast;
	}
	| BlockItemList BlockItem 
  {
		auto ast = $1;
		ast->blockitem_.push_back(unique_ptr<BaseAST>($2));
    $$=ast;
	}		
	;

// 语句块项
BlockItem
  :Decl 
  {
		auto ast = new BlockItemAST();
		ast->decl = unique_ptr<BaseAST>($1);
    ast->tid=BlockItemAST::declID;
    $$=ast;
	}
	|Stmt 
  {
		auto ast = new BlockItemAST();
		ast->stmt = unique_ptr<BaseAST>($1);
    ast->tid=BlockItemAST::stmtID;
    $$=ast;
	}	
	;

Stmt
  : LVal '=' Exp ';'
  {
    auto ast = new StmtAST();
    ast->lval=unique_ptr<BaseAST>($1);
    ast->exp = unique_ptr<BaseAST>($3);
    ast->tid=StmtAST::lvalID;
    $$ = ast;
  }
  | Exp ';'
  {
    auto ast = new StmtAST();
    ast->exp = unique_ptr<BaseAST>($1);
    ast->tid=StmtAST::expID;
    $$ = ast;
  }
  | ';'
  {
    auto ast = new StmtAST();
    ast->tid=StmtAST::nexpID;
    $$ = ast;
  }
  | Block
  {
    auto ast = new StmtAST();
    ast->block=unique_ptr<BaseAST>($1);
    ast->tid=StmtAST::blockID;
    $$ = ast;
  }
  | RETURN Exp ';' 
  {
    auto ast = new StmtAST();
    ast->exp = unique_ptr<BaseAST>($2);
    ast->tid=StmtAST::rexpID;
    $$ = ast;
  }
  | RETURN ';'
  {
    auto ast = new StmtAST();
    ast->tid=StmtAST::rnexpID;
    $$ = ast;
  }
  | IF '(' Exp ')' Stmt %prec LOWER_THEN_ELSE 
  {
		auto ast = new StmtAST();
    ast->exp=unique_ptr<BaseAST>($3);
    ast->ifstmt=unique_ptr<BaseAST>($5);
    ast->tid=StmtAST::ifID;
    $$ = ast;
	}
	| IF '(' Exp ')' Stmt ELSE Stmt 
  {
		auto ast = new StmtAST();
    ast->exp=unique_ptr<BaseAST>($3);
    ast->ifstmt=unique_ptr<BaseAST>($5);
    ast->elsestmt=unique_ptr<BaseAST>($7);
    ast->tid=StmtAST::ifelID;
    $$ = ast;
	}
  | WHILE '(' Exp ')' Stmt
  {
    auto ast = new StmtAST();
    ast->exp = unique_ptr<BaseAST>($3);
    ast->whilestmt=unique_ptr<BaseAST>($5);
    ast->tid=StmtAST::whileID;
    $$ = ast;
  }   
  |BREAK ';'
  {
    auto ast=new StmtAST();
    ast->tid=StmtAST::breakID;
    $$ = ast;
  }
  |CONTINUE ';'
  {
    auto ast=new StmtAST();
    ast->tid=StmtAST::continueID;
    $$ = ast;
  }
  ;

Exp 
  : LOrExp
  {
    auto ast=new ExpAST();
    ast->lorexp=unique_ptr<BaseAST>($1);
    $$=ast;
  }
  ;

MulExp
  :UnaryExp
  {
    auto ast=new MulExpAST();
    ast->tid=MulExpAST::unarID;
    ast->unaryexp=unique_ptr<BaseAST>($1);
    $$=ast;
  }
  | MulExp '*' UnaryExp
  {
    auto ast=new MulExpAST();
    ast->tid=MulExpAST::mulID;
    ast->mulexp=unique_ptr<BaseAST>($1);
    ast->unaryexp=unique_ptr<BaseAST>($3);
    $$=ast;
  }
  | MulExp '/' UnaryExp
  {
    auto ast=new MulExpAST();
    ast->tid=MulExpAST::divID;
    ast->mulexp=unique_ptr<BaseAST>($1);
    ast->unaryexp=unique_ptr<BaseAST>($3);
    $$=ast;
  }
  | MulExp '%' UnaryExp
  {
    auto ast=new MulExpAST();
    ast->tid=MulExpAST::modID;
    ast->mulexp=unique_ptr<BaseAST>($1);
    ast->unaryexp=unique_ptr<BaseAST>($3);
    $$=ast;
  }
  ;

AddExp 
  : MulExp
  {
    auto ast=new AddExpAST();
    ast->tid=AddExpAST::mulID;
    ast->mulexp=unique_ptr<BaseAST>($1);
    $$=ast;
  }    
  | AddExp '+' MulExp
  {
    auto ast=new AddExpAST();
    ast->tid=AddExpAST::addID;
    ast->addexp=unique_ptr<BaseAST>($1);
    ast->mulexp=unique_ptr<BaseAST>($3);
    $$=ast;
  }
  | AddExp '-' MulExp
  {
    auto ast=new AddExpAST();
    ast->tid=AddExpAST::subID;
    ast->addexp=unique_ptr<BaseAST>($1);
    ast->mulexp=unique_ptr<BaseAST>($3);
    $$=ast;
  }
  ;

RelExp 
  : AddExp
  {
    auto ast=new RelExpAST();
    ast->tid=RelExpAST::addID;
    ast->addexp=unique_ptr<BaseAST>($1);
    $$=ast;
  }
  | RelExp LT AddExp
  {
    auto ast=new RelExpAST();
    ast->tid=RelExpAST::ltID;
    ast->relexp=unique_ptr<BaseAST>($1);
    ast->addexp=unique_ptr<BaseAST>($3);
    $$=ast;
  }
  | RelExp GT AddExp
  {
    auto ast=new RelExpAST();
    ast->tid=RelExpAST::gtID;
    ast->relexp=unique_ptr<BaseAST>($1);
    ast->addexp=unique_ptr<BaseAST>($3);
    $$=ast;
  }
  | RelExp LTE AddExp
  {
    auto ast=new RelExpAST();
    ast->tid=RelExpAST::leID;
    ast->relexp=unique_ptr<BaseAST>($1);
    ast->addexp=unique_ptr<BaseAST>($3);
    $$=ast;
  }
  | RelExp GTE AddExp
  {
    auto ast=new RelExpAST();
    ast->tid=RelExpAST::geID;
    ast->relexp=unique_ptr<BaseAST>($1);
    ast->addexp=unique_ptr<BaseAST>($3);
    $$=ast;
  }
  ;

EqExp 
  : RelExp
  {
    auto ast=new EqExpAST();
    ast->tid=EqExpAST::relID;
    ast->relexp=unique_ptr<BaseAST>($1);
    $$=ast;
  }
  | EqExp EQ RelExp
  {
    auto ast=new EqExpAST();
    ast->tid=EqExpAST::eqID;
    ast->eqexp=unique_ptr<BaseAST>($1);
    ast->relexp=unique_ptr<BaseAST>($3);
    $$=ast;
  }
  | EqExp NEQ RelExp
  {
    auto ast=new EqExpAST();
    ast->tid=EqExpAST::neID;
    ast->eqexp=unique_ptr<BaseAST>($1);
    ast->relexp=unique_ptr<BaseAST>($3);
    $$=ast;
  }
  ;

LAndExp 
  : EqExp
  {
    auto ast=new LAndExpAST();
    ast->tid=LAndExpAST::eqID;
    ast->eqexp=unique_ptr<BaseAST>($1);
    $$=ast;
  } 
  | LAndExp AND EqExp
  {
    auto ast=new LAndExpAST();
    ast->tid=LAndExpAST::landID;
    ast->landexp=unique_ptr<BaseAST>($1);
    ast->eqexp=unique_ptr<BaseAST>($3);
    $$=ast;
  }
  ;

LOrExp      
  : LAndExp
  {
    auto ast=new LOrExpAST();
    ast->tid=LOrExpAST::landID;
    ast->landexp=unique_ptr<BaseAST>($1);
    $$=ast;
  }
  | LOrExp OR LAndExp
  {
    auto ast=new LOrExpAST();
    ast->tid=LOrExpAST::lorID;
    ast->lorexp=unique_ptr<BaseAST>($1);
    ast->landexp=unique_ptr<BaseAST>($3);
    $$=ast;
  }
  ;

PrimaryExp
  :'(' Exp ')'
  {
    auto ast=new PrimaryExpAST();
    ast->exp=unique_ptr<BaseAST>($2);
    ast->tid=PrimaryExpAST::expID;
    $$=ast;
  }
  |Number
  {
    auto ast=new PrimaryExpAST();
    ast->number=unique_ptr<BaseAST>($1);
    ast->tid=PrimaryExpAST::numID;
    $$=ast;
  }
  |LVal
  {
    auto ast=new PrimaryExpAST();
    ast->lval=unique_ptr<BaseAST>($1);
    ast->tid=PrimaryExpAST::lvalID;
    $$=ast;
  }
  ;

LVal
  :IDENT
  {
    auto ast=new LValAST();
    ast->ident=*unique_ptr<string>($1);
    $$=ast;
  }
  ;

Number
  : INT_CONST 
  {
    auto ast = new NumberAST();
    ast->int_const=(int)($1);
    $$=ast;
  }
  ;

UnaryExp
  :PrimaryExp
  {
    auto ast=new UnaryExpAST();
    ast->primaryexp=unique_ptr<BaseAST>($1);
    ast->tid=UnaryExpAST::priID;
    $$=ast;
  }
  |'+' UnaryExp
  {
    auto ast=new UnaryExpAST();
    ast->unaryexp=unique_ptr<BaseAST>($2);
    ast->tid=UnaryExpAST::poID;
    $$=ast;
  }
  |'-' UnaryExp
  {
    auto ast=new UnaryExpAST();
    ast->unaryexp=unique_ptr<BaseAST>($2);
    ast->tid=UnaryExpAST::neID;
    $$=ast;
  }
  |'!' UnaryExp
  {
    auto ast=new UnaryExpAST();
    ast->unaryexp=unique_ptr<BaseAST>($2);
    ast->tid=UnaryExpAST::noID;
    $$=ast;
  }
  | IDENT '(' FuncRParams ')'
  {
    auto ast=new UnaryExpAST();
    ast->ident=*unique_ptr<string>($1);
    ast->funcrparams=unique_ptr<BaseAST>($3);
    ast->tid=UnaryExpAST::funcID;
    $$=ast;
  }
  | IDENT '(' ')'
  {
    auto ast=new UnaryExpAST();
    ast->ident=*unique_ptr<string>($1);
    ast->tid=UnaryExpAST::nfuncID;
    $$=ast;
  }
  ;

FuncRParams
  :Exp
  {
    auto ast=new FuncRParamsAST();
    ast->exp_.push_back(unique_ptr<BaseAST>($1));
    $$=ast;
  }
  |FuncRParams ',' Exp
  {
    auto ast=$1;
    ast->exp_.push_back(unique_ptr<BaseAST>($3));
    $$=ast;
  }
  ;

Decl 
  :ConstDecl
  {
    auto ast=new DeclAST();
    ast->constdecl=unique_ptr<BaseAST>($1);
    ast->tid=DeclAST::constID;
    $$=ast;
  }
  |VarDecl
  {
    auto ast=new DeclAST();
    ast->vardecl=unique_ptr<BaseAST>($1);
    ast->tid=DeclAST::varID;
    $$=ast;
  }
  ;

ConstDecl 
  : CONST BType ConstDefList ';'
  {
    auto ast=new ConstDeclAST();
    ast->btype=unique_ptr<BaseAST>($2);
    ast->constdef_.swap($3->constdef_);
    $$=ast;
  }
  ;

ConstDefList   
  : ConstDef
  {
    auto ast=new ConstDefListAST();
    ast->constdef_.push_back(unique_ptr<BaseAST>($1));
    $$=ast;
  }
  | ConstDefList ',' ConstDef
  {
    auto ast=$1;
    ast->constdef_.push_back(unique_ptr<BaseAST>($3));
    $$=ast;
  }
  ;

ConstDef       
  : IDENT '=' ConstInitVal
  {
    auto ast=new ConstDefAST();
    ast->ident = *unique_ptr<string>($1);
    ast->constinitval=unique_ptr<BaseAST>($3);
    $$=ast;
  }
  ;

ConstInitVal   
  : ConstExp
  {
    auto ast=new ConstInitValAST();
    ast->constexp=unique_ptr<BaseAST>($1);
    $$=ast;
  }
  ;

ConstExp
  :Exp
  {
    auto ast=new ConstExpAST();
    ast->exp=unique_ptr<BaseAST>($1);
    $$=ast;
  }
  ;

VarDecl 
  : BType VarDefList ';'
  {
    auto ast=new VarDeclAST();
    ast->btype=unique_ptr<BaseAST>($1);
    ast->vardef_.swap($2->vardef_);
    $$=ast;
  }
  ;

VarDefList   
  : VarDef
  {
    auto ast=new VarDefListAST();
    ast->vardef_.push_back(unique_ptr<BaseAST>($1));
    $$=ast;
  }
  | VarDefList ',' VarDef
  {
    auto ast=$1;
    ast->vardef_.push_back(unique_ptr<BaseAST>($3));
    $$=ast;
  }
  ;

VarDef       
  : IDENT '=' InitVal
  {
    auto ast=new VarDefAST();
    ast->ident = *unique_ptr<string>($1);
    ast->initval=unique_ptr<BaseAST>($3);
    $$=ast;
  }
  | IDENT
  {
    auto ast=new VarDefAST();
    ast->ident = *unique_ptr<string>($1);
    $$=ast;
  }
  ;
  
InitVal
  : Exp
  {
    auto ast=new InitValAST();
    ast->exp=unique_ptr<BaseAST>($1);
    $$=ast;
  }
  ;







%%

// 定义错误处理函数, 其中第二个参数是错误信息
// parser 如果发生错误 (例如输入的程序出现了语法错误), 就会调用这个函数
void yyerror(unique_ptr<BaseAST> &ast, const char *s) {
  cerr << "error: " << s << endl;
}
