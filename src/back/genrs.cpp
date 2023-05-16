#include "genrs.hpp"
#include<assert.h>
#include<string>
#include<map>
using namespace std;

map<string,string>fhb;
SYMBOL *nowsymbol;
INT *nowint;
int initz=4;
string stmt="";
int choose=0;

void chuli(string t,string &s)
{
    switch (choose)
    {
        case 0:
            s+="add "+t+", x0, x0\n";
            break;
        case 1:
            s+="lw "+t+", "+fhb[to_string(nowint->int_const)]+'\n';
            break;
        case 2:
            s+="lw "+t+", "+fhb[nowsymbol->symbol]+'\n';
    }
}

void GenRS::Visit(InitIR &ir)
{
    GenRS::rs+=".text\n";
    if(!ir.globalsymboldef_.empty())
    {

    }
    GenRS::rs+=".globl main\n";
    if(!ir.fundef_.empty())
    {
        for(FunDef* t:ir.fundef_)
        {
            t->accept(*this);
        }
    }
}

void GenRS::Visit(FunDef &ir)
{
    GenRS::rs+=ir.symbol->symbol;
    GenRS::rs+=":\n";
    ir.funbody->accept(*this);
}

void GenRS::Visit(FunBody &ir)
{
    if(!ir.block_.empty())
    {
        for(Block *t:ir.block_)
            t->accept(*this);
    }
}

void GenRS::Visit(Block &ir)
{
    if(!ir.statement_.empty())
    {
        for(Statement *t:ir.statement_)
            t->accept(*this);
    }
    ir.endstatement->accept(*this);
}

void GenRS::Visit(Statement &ir)
{
    switch (ir.tid)
    {
        case Statement::SyDeID:
            ir.symboldef->accept(*this);
            break;
    }
}

void GenRS::Visit(SymbolDef &ir)
{
    switch(ir.tid)
    {
        case SymbolDef::BiEpID:
            ir.binaryexpr->accept(*this);
            break;
    }
    ir.symbol->accept(*this);
    string h=ir.symbol->symbol;
    GenRS::rs+="sw t2, "+fhb[h]+'\n';
}

void GenRS::Visit(BinaryExpr &ir)
{
    string t0="t0",t1="t1";
    switch(ir.tid)
    {
        case BinaryExpr::neID:
            ir.value1->accept(*this);
            chuli(t0,GenRS::rs);
            ir.value2->accept(*this);
            chuli(t1,GenRS::rs);
            GenRS::rs+="sub t1, t0, t1\n";
            GenRS::rs+="snez t2, t1\n";
            break;
        case BinaryExpr::eqID:
            ir.value1->accept(*this);
            chuli(t0,GenRS::rs);
            ir.value2->accept(*this);
            chuli(t1,GenRS::rs);
            GenRS::rs+="sub t1, t0, t1\n";
            GenRS::rs+="seqz t2, t1\n";
            break;
        case BinaryExpr::gtID: 
            ir.value1->accept(*this);
            chuli(t0,GenRS::rs);
            ir.value2->accept(*this);
            chuli(t1,GenRS::rs);
            GenRS::rs+="sgt t2, t0, t1\n";
            break;
        case BinaryExpr::ltID:
            ir.value1->accept(*this);
            chuli(t0,GenRS::rs);
            ir.value2->accept(*this);
            chuli(t1,GenRS::rs);
            GenRS::rs+="slt t2, t0, t1\n";
            break; 
        case BinaryExpr::geID:
            ir.value1->accept(*this);
            chuli(t0,GenRS::rs);
            ir.value2->accept(*this);
            chuli(t1,GenRS::rs);
            GenRS::rs+="slt t2, t0, t1\n"; 
            GenRS::rs+="seqz  t2, t2\n";
            break;
        case BinaryExpr::leID:
            ir.value1->accept(*this);
            chuli(t0,GenRS::rs);
            ir.value2->accept(*this);
            chuli(t1,GenRS::rs);
            GenRS::rs+="sgt t2, t0, t1\n"; 
            GenRS::rs+="seqz  t2, t2\n";
            break; 
        case BinaryExpr::addID:
            ir.value1->accept(*this);
            chuli(t0,GenRS::rs);
            ir.value2->accept(*this);
            chuli(t1,GenRS::rs);
            GenRS::rs+="add t2, t0, t1\n"; 
            break; 
        case BinaryExpr::subID: 
            ir.value1->accept(*this);
            chuli(t0,GenRS::rs);
            ir.value2->accept(*this);
            chuli(t1,GenRS::rs);
            GenRS::rs+="sub t2, t0, t1\n"; 
            break; 
        case BinaryExpr::mulID:
            ir.value1->accept(*this);
            chuli(t0,GenRS::rs);
            ir.value2->accept(*this);
            chuli(t1,GenRS::rs);
            GenRS::rs+="mul t2, t0, t1\n"; 
            break;  
        case BinaryExpr::divID:
            ir.value1->accept(*this);
            chuli(t0,GenRS::rs);
            ir.value2->accept(*this);
            chuli(t1,GenRS::rs);
            GenRS::rs+="div t2, t0, t1\n"; 
            break;  
        case BinaryExpr::modID:
            ir.value1->accept(*this);
            chuli(t0,GenRS::rs);
            ir.value2->accept(*this);
            chuli(t1,GenRS::rs);
            GenRS::rs+="rem t2, t0, t1\n"; 
            break;  
        case BinaryExpr::andID:
            ir.value1->accept(*this);
            chuli(t0,GenRS::rs);
            ir.value2->accept(*this);
            chuli(t1,GenRS::rs);
            GenRS::rs+="and t2, t0, t1\n"; 
            break;  
        case BinaryExpr::orID:
            ir.value1->accept(*this);
            chuli(t0,GenRS::rs);
            ir.value2->accept(*this);
            chuli(t1,GenRS::rs);
            GenRS::rs+="or t2, t0, t1\n"; 
            break;  
        case BinaryExpr::xorID:
            ir.value1->accept(*this);
            chuli(t0,GenRS::rs);
            ir.value2->accept(*this);
            chuli(t1,GenRS::rs);
            GenRS::rs+="xor t2, t0, t1\n"; 
            break;  
        case BinaryExpr::shlID:
            return; 
        case BinaryExpr::shrID:
            return; 
        case BinaryExpr::sarID:
            return;
    }
}

void GenRS::Visit(EndStatement &ir)
{
    ir.ret->accept(*this);
    GenRS::rs+="ret";
}

void GenRS::Visit(Return &ir)
{
    ir.value->accept(*this);
    chuli("t2",GenRS::rs);
    GenRS::rs+="add a0, t2, x0\n";
}

void GenRS::Visit(Value &ir)
{   
    switch (ir.tid)
    {
        case Value::SYMBOLID:
            ir.symbol->accept(*this);
            break;
        case Value::INTID:
            ir.i32->accept(*this);
            break;
        case Value::undefID:
            return;
    }
}
 
void GenRS::Visit(INT &ir)
{
    choose=1;
    string h=to_string(ir.int_const);
    if(ir.int_const==0)
    {
        choose=0;
        fhb[h]="x0";
    }
    else if(fhb.find(h)==fhb.end())
    {
        fhb[h]=to_string(initz)+"(sp)";
        initz+=4;
        GenRS::rs+="li t2, "+h+'\n';
        GenRS::rs+="sw t2, "+fhb[h]+'\n';
    }
    nowint=&ir;
}

void GenRS::Visit(SYMBOL &ir)
{
    choose=2;
    string h=ir.symbol;
    if(fhb.find(h)==fhb.end())
    {
        fhb[h]=to_string(initz)+"(sp)";
        initz+=4;
    }
    nowsymbol=&ir;
}

void GenRS::Visit(Type &ir)
{
    return;
}

void GenRS::Visit(ArrayType &ir)
{
    return;
}

void GenRS::Visit(PointerType &ir)
{
    return;
}

void GenRS::Visit(FunType &ir)
{
    return;
}

void GenRS::Visit(Initializer &ir)
{
    return;
}

void GenRS::Visit(Aggregate &ir)
{
    return;
}

void GenRS::Visit(GlobalSymbolDef &ir)
{
    return;
}

void GenRS::Visit(MemoryDeclaration &ir)
{
    return;
}

void GenRS::Visit(GlobalMemoryDeclaration &ir)
{
    return;
}

void GenRS::Visit(Load &ir)
{
    return;
}

void GenRS::Visit(Store &ir)
{
    return;
}

void GenRS::Visit(GetPointer &ir)
{
    return;
}

void GenRS::Visit(GetElementPointer &ir)
{
    return;
}    

void GenRS::Visit(Branch &ir)
{
    return;
}   

void GenRS::Visit(Jump &ir)
{
    return;
}

void GenRS::Visit(FunCall &ir)
{
    return;
}

void GenRS::Visit(Funparams &ir)
{
    return;
}   

void GenRS::Visit(FunDecl &ir)
{
    return;
}  

void GenRS::Visit(FunDeclparms &ir)
{
    return;
}  
        