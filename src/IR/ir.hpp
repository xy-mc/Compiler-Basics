#pragma once
#include <memory>
#include <string>
#include <vector>
#include <iostream>

using namespace std;
class Visitor_;
class Type;
class ArrayType;
class PointerType;
class FunType;
class Value;
class SYMBOL;
class INT;
class Initializer;
class Aggregate;
class SymbolDef;
class GlobalSymbolDef;
class MemoryDeclaration;
class GlobalMemoryDeclaration;
class Load;
class Store;
class GetPointer;
class GetElementPointer;
class BinaryExpr;
class Branch;
class Jump;
class FunCall;
class Return;
class FunDef;
class Funparams;
class FunBody;
class Block;
class Statement;
class EndStatement;
class FunDecl;
class FunDeclparams;
class InitIR;

class BaseIR 
{
    public:
        virtual ~BaseIR() = default;
        virtual void getir(string &s) =0;
        virtual void accept(Visitor_ &visitor) = 0;
};

class Type :public BaseIR
{
    public:
        enum TypeID 
        {
            i32ID,
            funID,  // Functions
            arrayID,     // Arrays
            poterID,   // Pointer
        };
        explicit Type(TypeID tid_) : tid(tid_) {}
        TypeID tid;
        ArrayType *arraytype=nullptr;
        FunType *funtype=nullptr;
        PointerType *pointertype=nullptr;
        void getir(string &s) override;
        void accept(Visitor_ &visitor) override;
};

class ArrayType : public Type 
{
    public:
        ArrayType(Type *type_,int num_):Type(Type::arrayID),type(type_),num(num_){}
        Type *type;
        int num;
        void getir(string &s) override;
        void accept(Visitor_ &visitor) override;
};

class PointerType : public Type 
{
    public:
        PointerType(Type *type_):Type(Type::poterID),type(type_){}
        Type *type;
        void getir(string &s) override;
        void accept(Visitor_ &visitor) override;
};

class FunType : public Type 
{
    public:
        FunType(vector<Type *>typelist_,Type *type_):Type(Type::funID),typelist(typelist_),type(type_){}
        vector<Type *>typelist;
        Type *type;
        void getir(string &s) override;
        void accept(Visitor_ &visitor) override;
};


class Value : public BaseIR
{
    public:
        enum ValueID
        {
            SYMBOLID,
            INTID,
            undefID,
        };
        explicit Value(ValueID tid_) : tid(tid_) {} 
        ~Value() = default;
        ValueID tid;
        SYMBOL *symbol=nullptr;
        INT *i32=nullptr;
        void getir(string &s) override;
        void accept(Visitor_ &visitor) override;
};

class SYMBOL: public Value
{
    public:
        SYMBOL(string symbol_):Value(Value::SYMBOLID), symbol(symbol_) {}
        string symbol;
        void getir(string &s) override;
        void accept(Visitor_ &visitor) override;
};
class INT: public Value
{
    public:
        INT(int intconst):Value(Value::INTID),int_const(intconst){}
        int int_const;
        void getir(string &s) override;
        void accept(Visitor_ &visitor) override;
};

class Initializer: public BaseIR
{
    public:
        enum InID
        {
            intID,
            undefID,
            aggreID,
            zeroID,
        };
        Initializer(InID tid_,int i32_,Aggregate *aggregate_) : tid(tid_),i32(i32_),aggregate(aggregate_) {} 
        InID tid;
        int i32;
        Aggregate *aggregate=nullptr;
        void getir(string &s) override;
        void accept(Visitor_ &visitor) override;
};

class Aggregate: public BaseIR
{
    public:
        vector<Initializer*>initialzer_;   
        void getir(string &s) override;
        void accept(Visitor_ &visitor) override;
};

class SymbolDef: public BaseIR
{
    public:
        enum SymbolDefID
        {
            MemID,
            LoadID,
            GetPID,
            GetPmID,
            BiEpID,
            FuncID,
        };
        SymbolDef(SYMBOL *symbol_,SymbolDefID tid_,MemoryDeclaration *memorydeclaration_,Load *load_,
        GetPointer *getpointer_,GetElementPointer *getelemptr_,
        BinaryExpr *binaryexpr_,FunCall *funcall_):symbol(symbol_),tid(tid_),memorydeclaration(memorydeclaration_),load(load_)
        ,getpointer(getpointer_),getelemptr(getelemptr_),binaryexpr(binaryexpr_),funcall(funcall_){}
        SYMBOL *symbol;
        SymbolDefID tid;
        MemoryDeclaration *memorydeclaration;
        Load *load;
        GetPointer *getpointer;
        GetElementPointer *getelemptr;
        BinaryExpr *binaryexpr;
        FunCall *funcall;
        void getir(string &s) override;
        void accept(Visitor_ &visitor) override;
};

class GlobalSymbolDef: public BaseIR
{
    public:
        GlobalSymbolDef(SYMBOL *symbol_,GlobalMemoryDeclaration *globalmemorydeclaration_):
        symbol(symbol_),globalmemorydeclaration(globalmemorydeclaration_){}
        SYMBOL *symbol;
        GlobalMemoryDeclaration *globalmemorydeclaration;
        void getir(string &s) override;
        void accept(Visitor_ &visitor) override;
};

class MemoryDeclaration: public BaseIR
{
    public:
        MemoryDeclaration(Type *type_):type(type_){}
        Type *type;
        void accept(Visitor_ &visitor) override;
        void getir(string &s) override;
};

class GlobalMemoryDeclaration: public BaseIR
{
    public:
        GlobalMemoryDeclaration(Type *type_,Initializer *initializer_):
        type(type_),initializer(initializer_){}
        Type *type;
        Initializer *initializer;
        void getir(string &s) override;
        void accept(Visitor_ &visitor) override;
};

class Load: public BaseIR
{
    public:
        Load(SYMBOL *symbol_): symbol(symbol_){}
        SYMBOL *symbol;
        void accept(Visitor_ &visitor) override;
        void getir(string &s) override;
};

class Store: public BaseIR
{
    public:
        enum StoreID
        {
            valueID,
            initID,
        };
        Store(StoreID tid_,Value *value_,Initializer *initializer_,SYMBOL *symbol_):tid(tid_),value(value_),
        initializer(initializer_),symbol(symbol_){}
        StoreID tid;
        Value *value;
        Initializer *initializer;
        SYMBOL *symbol;
        void accept(Visitor_ &visitor) override;
        void getir(string &s) override;
};

class GetPointer: public BaseIR
{
    public:
        GetPointer(SYMBOL *symbol_,Value *value_):symbol(symbol_),value(value_){}
        SYMBOL *symbol;
        Value *value;
        void accept(Visitor_ &visitor) override;
        void getir(string &s) override;
};

class GetElementPointer: public BaseIR
{
    public:
        GetElementPointer(SYMBOL *symbol_,Value *value_):symbol(symbol_),value(value_){}
        SYMBOL *symbol;
        Value *value;
        void accept(Visitor_ &visitor) override;
        void getir(string &s) override;
};

class BinaryExpr: public BaseIR
{
    public:
        enum BEID
        {
            neID, 
            eqID,
            gtID, 
            ltID, 
            geID, 
            leID, 
            addID, 
            subID, 
            mulID, 
            divID, 
            modID, 
            andID, 
            orID, 
            xorID, 
            shlID, 
            shrID, 
            sarID,
        };
        BinaryExpr(BEID tid_,Value *value1_,Value *value2_):tid(tid_),value1(value1_),value2(value2_){}
        BEID tid; 
        Value *value1=nullptr;
        Value *value2=nullptr;
        void getir(string &s) override;
        void accept(Visitor_ &visitor) override;       
};

class Branch: public BaseIR
{
    public:
        Branch(Value *value_,SYMBOL *symbol1_,SYMBOL *symbol2_):value(value_),symbol1(symbol1_),
        symbol2(symbol2_){}
        Value *value;
        SYMBOL *symbol1;
        SYMBOL *symbol2;
        void getir(string &s) override;
        void accept(Visitor_ &visitor) override;
};

class Jump: public BaseIR
{
    public:
        Jump(SYMBOL *symbol_):symbol(symbol_){}
        SYMBOL *symbol;
        void getir(string &s) override;
        void accept(Visitor_ &visitor) override;
};

class FunCall: public BaseIR
{
    public:
        FunCall(SYMBOL *symbol_):symbol(symbol_){}
        SYMBOL *symbol;
        vector<Value *>value_;
        void accept(Visitor_ &visitor) override;
        void getir(string &s) override;
};

class Return: public BaseIR
{
    public:
        Return(Value* value_) :value(value_){}
        Value *value=nullptr;
        void getir(string &s) override;
        void accept(Visitor_ &visitor) override;
};

class FunDef: public BaseIR
{
    public:
        FunDef(SYMBOL *symbol_,Funparams *funparams_,Type *type_,FunBody *funbody_,int def_nym_,
        bool is_call_,int max_num_):symbol(symbol_),funparams(funparams_),type(type_),funbody(funbody_),
        def_num(def_nym_),is_call(is_call_),max_num(max_num_){}
        SYMBOL *symbol=nullptr;
        Funparams *funparams=nullptr;
        Type *type=nullptr;
        FunBody *funbody=nullptr;
        int def_num;
        bool is_call;
        int max_num;
        void getir(string &s) override;
        void accept(Visitor_ &visitor) override;
};

class Funparam: public BaseIR
{
    public:
        Funparam(SYMBOL *symbol_,Type *type_):symbol(symbol_),type(type_){}
        SYMBOL *symbol=nullptr;
        Type *type=nullptr;
        void getir(string &s) override;
        void accept(Visitor_ &visitor) override;
};

class Funparams: public BaseIR
{
    public:
        Funparams(vector<Funparam *>funparam):funparam_(funparam){}
        vector<Funparam *> funparam_;
        void getir(string &s) override;
        void accept(Visitor_ &visitor) override;
};

class FunBody: public BaseIR
{
    public:
        FunBody(vector<Block*>block):block_(block){}
        vector<Block*> block_;
        void getir(string &s) override;
        void accept(Visitor_ &visitor) override;
};

class Block: public BaseIR
{
    public:
        Block(SYMBOL*symbol_,vector<Statement *>stmt_,EndStatement*estmt):
        symbol(std::move(symbol_)), statement_(stmt_), endstatement(std::move(estmt)) {}
        SYMBOL *symbol=nullptr;
        vector<Statement*> statement_;
        EndStatement *endstatement=nullptr;
        void getir(string &s) override;
        void accept(Visitor_ &visitor) override;
};

class Statement: public BaseIR
{
    public:
        enum StmtID
        {
            SyDeID,
            StoreID,
            FuncID,
        };
        Statement(StmtID tid_,SymbolDef *symboldef_,Store *store_,FunCall *funcall_):tid(tid_),
        symboldef(symboldef_),store(store_),funcall(funcall_){}
        StmtID tid;
        SymbolDef *symboldef=nullptr;
        Store *store=nullptr;
        FunCall *funcall=nullptr;
        void getir(string &s) override;
        void accept(Visitor_ &visitor) override;
};

class EndStatement: public BaseIR
{
    public:
        enum esID
        {
            branchID,
            jumpID,
            returnID,
        };
        EndStatement(esID tid_,Branch *branch_,Jump *jump_,Return *ret_):tid(tid_),branch(branch_),
        jump(jump_),ret(ret_){}
        esID tid;
        Branch *branch=nullptr;
        Jump *jump=nullptr;
        Return *ret=nullptr;
        void getir(string &s) override;
        void accept(Visitor_ &visitor) override;
};

class FunDecl: public BaseIR
{
    public:
        FunDecl(SYMBOL *symbol_,FunDeclparams *fundeclparams_,Type *type_):
        symbol(symbol_),fundeclparams(fundeclparams_),type(type_){}
        SYMBOL *symbol=nullptr;
        FunDeclparams *fundeclparams=nullptr;
        Type *type=nullptr;
        void getir(string &s) override;
        void accept(Visitor_ &visitor) override;
};

class FunDeclparams: public BaseIR
{
    public:
        FunDeclparams(vector<Type*>type):type_(type){}
        vector<Type*>type_;
        void getir(string &s) override;
        void accept(Visitor_ &visitor) override;
};


class InitIR : public BaseIR
{
    public:
        vector<GlobalSymbolDef*>globalsymboldef_;
        vector<FunDef*>fundef_;
        void getir(string &s) override;
        void accept(Visitor_ &visitor) override;
};

class Visitor_
{
    public:
        virtual void Visit(InitIR &ir)=0;
        virtual void Visit(FunDef &ir)=0;
        virtual void Visit(SYMBOL &ir)=0;
        virtual void Visit(FunBody &ir)=0;
        virtual void Visit(Block &ir)=0;
        virtual void Visit(Statement &ir)=0;
        virtual void Visit(SymbolDef &ir)=0;
        virtual void Visit(BinaryExpr &ir)=0;
        virtual void Visit(EndStatement &ir)=0;
        virtual void Visit(Return &ir)=0;
        virtual void Visit(Value &ir)=0;
        virtual void Visit(INT &ir)=0;
        virtual void Visit(Type &ir)=0;
        virtual void Visit(ArrayType &ir)=0;
        virtual void Visit(PointerType &ir)=0;
        virtual void Visit(FunType &ir)=0;
        virtual void Visit(Initializer &ir)=0;
        virtual void Visit(Aggregate &ir)=0;
        virtual void Visit(GlobalSymbolDef &ir)=0;
        virtual void Visit(MemoryDeclaration &ir)=0;
        virtual void Visit(GlobalMemoryDeclaration &ir)=0;
        virtual void Visit(Load &ir)=0;
        virtual void Visit(Store &ir)=0;
        virtual void Visit(GetPointer &ir)=0;
        virtual void Visit(GetElementPointer &ir)=0;
        virtual void Visit(Branch &ir)=0;
        virtual void Visit(Jump &ir)=0;
        virtual void Visit(FunCall &ir)=0;
        virtual void Visit(Funparams &ir)=0;
        virtual void Visit(Funparam &ir)=0;
        virtual void Visit(FunDecl &ir)=0;
        virtual void Visit(FunDeclparams &ir)=0;
};