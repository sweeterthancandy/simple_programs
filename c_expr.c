extern int printf(const char* c, ...);
extern unsigned long long strlen(const char* s);
extern int isdigit(int);
extern int isspace(int);
extern int isalpha(int);
extern int isalnum(int);
extern void* memset( void*, int , unsigned long long );
extern char* strcpy( char*, const char*);

#include <assert.h>

/*
   accepts string of the grammar

      digit  -> 0123456789

      digits -> digit+

      term   -> digits 
             |  ( expr )

      factor -> term
             |  factor * term
             |  factor / term

      expr   -> factor
             |  expr   + factor
             |  expr   - factor

    +---------+     +------+     +---------------+
    |Tokenizer| --> |Parser|  -> |Expression Tree|
    +---------+     +------+     +---------------+

*/


/*
        Tokenizer
        =========

        Main thing with the tokenizer is the peaking
        mechinism, ie 3 cases for getting tokens

                Initlization
                ============
                <null>  -> init to eos
                1       -> init to 

*/

typedef enum TokenizerState{
        TokenizerState_Vanilla = 0,
        TokenizerState_OneBeforeEos,
        TokenizerState_EOS,
        // gets in the error state getting the peak,
        // go parsing 
        //
        //       123 aaa  
        //       ^
        // will fail because the peak will fail.
        TokenizerState_Error,
}TokenizerState;

typedef enum TokenType{
        TokenType_invalid = 0, // so memset gives an invalid token
        TokenType_Literal,
        TokenType_Identifier,
        TokenType_Operator,
        TokenType_LParam,
        TokenType_RParam,
        TokenType_Comma
}TokenType;

typedef struct Token{
        TokenType type;
        int value;
        char name[128];
        char op;
}Token;

typedef struct TokenizerContext{
        const char* str;
        // (iter,end) corresping to peak
        const char* iter;
        const char* end; 
        int eos;           // end of stream flag
        int error;         // error flag, once this set
                           // theres no going back
        Token tok;
        Token peak;        // restrict outselves to grammars
                           // where one token ahead is enough
        TokenizerState state;
}TokenizerContext;

// make TokenizerContext::tok be the next token
//
// returns true is TokenizerContext::tok exists otherwise,
// either eos or error, which can be checked with the flags
//
// postconditons:
//      
int Tokenizer_Next(TokenizerContext* ctx){

        switch(ctx->state){
        case TokenizerState_Vanilla:
                break;
        case TokenizerState_OneBeforeEos:
                ctx->tok = ctx->peak;
                ctx->eos = 1;
                ctx->state = TokenizerState_EOS;
                return 1;
        case TokenizerState_EOS:
                assert( ctx->eos && "invariant failed");
                return 0;
        case TokenizerState_Error:
                assert( ctx->error && "invariant failed");
                return 0;
        }

        if( ctx->eos || ctx->error )
                return 0;
        ctx->tok = ctx->peak;
        
        memset( &ctx->peak, 0, sizeof(ctx->peak));

        // eat whitespace
        for( ; ctx->iter != ctx->end &&
               isspace(*ctx->iter) ; ++ctx->iter);

        if( ctx->iter == ctx->end ){
                ctx->state = TokenizerState_OneBeforeEos;
                return 1;
        }

        switch( *ctx->iter ){
                case '+':
                case '-':
                case '*':
                case '/':
                        ctx->peak.type = TokenType_Operator;
                        ctx->peak.op   = *ctx->iter;
                        ++ctx->iter;
                        break;
                case '(':
                        ctx->peak.type = TokenType_LParam;
                        ++ctx->iter;
                        break;
                case ')':
                        ctx->peak.type = TokenType_RParam;
                        ++ctx->iter;
                        break;
                case ',':
                        ctx->peak.type = TokenType_Comma;
                        ++ctx->iter;
                        break;
                default:{
                        if( isdigit(*ctx->iter)){
                                // try to get value, otherwise fail the whole stream
                                // because the peak is invalid
                                const char* end = ctx->iter;
                                ctx->peak.value = 0;
                                for(; end != ctx->end; ++end){
                                        if( ! isdigit(*end) )
                                                break;
                                        ctx->peak.value *= 10;
                                        ctx->peak.value += *end - '0';
                                }
                                ctx->peak.type  = TokenType_Literal;
                                ctx->iter = end;
                                return 1;
                        } else if( isalpha(*ctx->iter)){
                                const char* end = ctx->iter;
                                char* word_ptr = ctx->peak.name;
                                for(; end != ctx->end; ++end){
                                        if( ! ( isalnum(*end) || *end == '_' ) )
                                                break;
                                        *word_ptr = *end;
                                        ++word_ptr;
                                }
                                ctx->peak.type  = TokenType_Identifier;
                                ctx->iter = end;
                                return 1;
                        } else{
                                // fail here
                                ctx->state = TokenizerState_Error;
                                ctx->error = 1;
                                return 0;
                        }
                }
        }
        return 1;
}

void Tokenizer_Init(TokenizerContext* ctx, const char* source){
        memset( ctx, 0, sizeof(*ctx));

        ctx->str = source;
        ctx->iter = source;
        ctx->end = source + strlen(source);
        ctx->eos = 0;
        
        // tok = (null), peak = (null)

        // tok = (null), peak = <token_0>
        Tokenizer_Next(ctx);
        
        // tok = <token_0>, peak = <token_1>
        Tokenizer_Next(ctx);
}

void Tokenizer_DumpToken(Token* tok){
        switch(tok->type){
        case TokenType_invalid:
        default:
                printf("<invalid>\n");
                break;
        case TokenType_Literal:
                printf("literal(%d)\n", tok->value);
                break;
        case TokenType_Operator:
                printf("operator(%c)\n", tok->op);
                break;
        case TokenType_Identifier:
                printf("identifier(%s)\n", (const char*)tok->name);
                break;
        case TokenType_LParam:
                printf(" ( \n");
                break;
        case TokenType_RParam:
                printf(" ) \n");
                break;
        case TokenType_Comma:
                printf(" , \n");
                break;
        }
}
void Tokenizer_Dump(TokenizerContext* ctx){
      Tokenizer_DumpToken(&ctx->tok);  
}

void Tokenizer_dotest(const char* str){
        printf("BEGIN expr=\"%s\"\n", str);
        TokenizerContext ctx = {0};
        Tokenizer_Init(&ctx, str);
        for( ; ! ( ctx.eos || ctx.error ) ; ){
                Tokenizer_Dump(&ctx);
                Tokenizer_Next(&ctx);
        }
        printf("END\n\n");
}



void Tokenizer_runtest(){
        Tokenizer_dotest("");
        Tokenizer_dotest("1");
        Tokenizer_dotest("12");
        Tokenizer_dotest("  1");
        Tokenizer_dotest("1   ");
        Tokenizer_dotest("1+3");
        Tokenizer_dotest("1   *334   /2");
        Tokenizer_dotest("35454");
        Tokenizer_dotest("1+3sb");
}

typedef struct MemoryPool{
        char mem[4096];
        char* head;
}MemoryPool;

void MemoryPool_Init(MemoryPool* pool){
        pool->head = pool->mem;
}
void* MemoryPool_Alloc(MemoryPool* pool, unsigned long long size){
        void* ptr = pool->head;
        pool->head += size;
        return ptr;
}





typedef enum Exprkind{
        ExprKind_Value,
        ExprKind_Indentifier,
        ExprKind_Call,
        ExprKind_BinaryOperator
}kind;

typedef enum Operator{
        Operator_Add,
        Operator_Sub,
        Operator_Div,
        Operator_Mul
}Operator;
        
typedef struct Node{
        int kind;
        int value;
        char name[128];
        Operator op;
        struct Node* arg0;
        struct Node* arg1;
        struct Node* args[32];
}Node;

#if 0
typedef void(*NodeVisitorFunction)(struct NodeVisitor*, Node*);

typedef struct NodeVisitor{
        // meta
        NodeVisitorFunction onBegin;
        NodeVisitorFunction onEnd;
        
        // for terminals
        NodeVisitorFunction onValue;

        // for those with childen
        NodeVisitorFunction onBeginBinaryOperator;
        NodeVisitorFunction onEndBinaryOperator;
}NodeVisitor;

void Node_Apply(NodeVisitor* v, Node* node){
        if( node == 0 ){
                // this is bad, don't know what to do
                return;
        }
        switch(node->kind){
        case ExprKind_Value:
                if( v->onValue )
                        v->onValue( v, node);
                break;
        case ExprKind_BinaryOperator:
                if( v->onBeginBinaryOperator )
                        v->onBeginBinaryOperator( v, node);

                Node_Apply(v, node->arg0);
                Node_Apply(v, node->arg1);
                
                if( v->onEndBinaryOperator )
                        v->onEndBinaryOperator( v, node);
                break;
        defualt:
                printf("<invalid>");
                break;
        }
}
#endif

int Node_Eval(Node* node){
        if( node == 0 ){
                return -1;
        }
        switch(node->kind){
                case ExprKind_Value:
                        return node->value;
                case ExprKind_Indentifier:
                        // TODO
                        return 1;
                case ExprKind_Call:
                        // TODO
                        return 1;
                case ExprKind_BinaryOperator: {
                        int lp = Node_Eval(node->arg0);
                        int rp = Node_Eval(node->arg1);
                        int ret = -1;
                        switch(node->op){
                        case Operator_Add: ret = lp + rp; break;
                        case Operator_Sub: ret = lp - rp; break;
                        case Operator_Mul: ret = lp * rp; break;
                        case Operator_Div: ret = lp / rp; break;
                        }
                        return ret;
                }
                default:
                        return -1;
        }
}

void Node_Dump(Node* node){
        if( node == 0 ){
                printf("<null>");
                return;
        }
        switch(node->kind){
        case ExprKind_Value:
                printf("value(%d)", node->value);
                break;
        case ExprKind_Indentifier:
                printf("identifier(%s)", (const char*)node->name);
                break;
        case ExprKind_Call:
                printf("call(%s,", node->name );
                for(Node** iter = node->args; *iter != 0; ++iter){
                        Node_Dump(*iter);
                }
                printf(")");
                break;
        case ExprKind_BinaryOperator:
                printf("operator("); 
                switch(node->op){
                case Operator_Add: printf("+"); break;
                case Operator_Sub: printf("-"); break;
                case Operator_Div: printf("/"); break;
                case Operator_Mul: printf("*"); break;
                }
                printf(" ,");
                Node_Dump(node->arg0);
                printf(" ,");
                Node_Dump(node->arg1);
                printf(")");
                break;
        defualt:
                printf("<invalid>");
                break;
        }
}

void Node_DumpPretty(Node* node){
        if( node == 0 ){
                printf("<null>");
                return;
        }
        switch(node->kind){
        case ExprKind_Value:
                printf("%d", node->value);
                break;
        case ExprKind_Indentifier:
                printf("%s", node->name );
                break;
        case ExprKind_Call:
                printf("%s(", node->name );
                for(Node** iter = node->args; *iter != 0; ++iter){
                        Node_DumpPretty(*iter);
                }
                printf(")");
                break;
        case ExprKind_BinaryOperator:
                printf("(");
                Node_DumpPretty(node->arg0);
                switch(node->op){
                case Operator_Add: printf("+"); break;
                case Operator_Sub: printf("-"); break;
                case Operator_Div: printf("/"); break;
                case Operator_Mul: printf("*"); break;
                }
                Node_DumpPretty(node->arg1);
                printf(")");
                break;
        defualt:
                printf("<invalid>");
                break;
        }
}

void Node_InitValue(Node* ptr, int value){
        memset(ptr, 0, sizeof(*ptr));
        ptr->kind  = ExprKind_Value;
        ptr->value = value;
}
void Node_InitIdentifer(Node* ptr, const char* name){
        memset(ptr, 0, sizeof(*ptr));
        ptr->kind  = ExprKind_Indentifier;
        strcpy(ptr->name, name);
}
// args is null-terminated
void Node_InitCall(Node* ptr, const char* name, Node** args){
        memset(ptr, 0, sizeof(*ptr));
        ptr->kind  = ExprKind_Call;
        strcpy(ptr->name, name);
        for(Node** iter = args, **dest = ptr->args; *iter != 0; ++iter){
                *dest = *iter;
        }
}
void Node_InitOperator(Node* ptr, Operator op, Node* left, Node* right){
        memset(ptr, 0, sizeof(*ptr));
        ptr->kind   = ExprKind_BinaryOperator;
        ptr->op     = op;
        ptr->arg0   = left;
        ptr->arg1   = right;
}

typedef struct ParserContext{
        TokenizerContext tokenizer;
        MemoryPool       pool;
        void*            stack_mem[128];
        void**           stack_ptr;
        int              fail; // fail flag
}ParserContext;

int ParserContext_Eval(ParserContext* ctx, Node* node){
        if( ctx->fail ||
            ctx->stack_ptr - ctx->stack_mem != 1 )
                return 0;

        int result = Node_Eval( ctx->stack_mem[0] );
        return result;
}

int parse_expr(ParserContext* ctx);

int parse_term(ParserContext* ctx){
        switch(ctx->tokenizer.tok.type){
        case TokenType_Literal: {
                // we got a literal
                Node* ptr = MemoryPool_Alloc(&ctx->pool, sizeof(Node));
                Node_InitValue(ptr, ctx->tokenizer.tok.value);
                *ctx->stack_ptr = ptr;
                ++ctx->stack_ptr;
                // eat it
                Tokenizer_Next(&ctx->tokenizer);
                return 1;
        }
        case TokenType_Identifier: {
                // this can be a function call

                Token idTok = ctx->tokenizer.tok;
                // eat it
                Tokenizer_Next(&ctx->tokenizer);

                // it's a function call when 
                //      <identifier> ( ...
                if( ctx->tokenizer.tok.type == TokenType_LParam ){
                        // eat the (
                        Tokenizer_Next(&ctx->tokenizer);


                        Node* args[32] = {0};
                        Node** arg_ptr = args;

                        // check if not a nullary function call
                        if( ctx->tokenizer.tok.type != TokenType_RParam ){
                                printf("parsign function args\n");
                                for(;;){
                                        if( ! parse_expr(ctx) ){
                                                printf("failed to parse expression for function\n");
                                                ctx->fail = 1;
                                                return 0;
                                        }

                                        // put it on the arg stack
                                        --ctx->stack_ptr;
                                        *arg_ptr = *ctx->stack_ptr;

                                        Node_Dump( *arg_ptr );
                                        ++arg_ptr;
                                        printf("stack_size = %d\n", ctx->stack_ptr - ctx->stack_mem );

                                        switch( ctx->tokenizer.tok.type ){
                                        case TokenType_Comma:
                                                // eat it
                                                Tokenizer_Next(&ctx->tokenizer);
                                                continue;
                                        }

                                        // XXX
                                        break;
                                }
                        }
                        if( ctx->tokenizer.tok.type != TokenType_RParam ){
                                printf("expected ) token\n");
                                ctx->fail = 1;
                                return 0;
                        }
                        // eat it
                        Tokenizer_Next(&ctx->tokenizer);
                
                        // we got a function call
                        Node* ptr = MemoryPool_Alloc(&ctx->pool, sizeof(Node));
                        Node_InitCall(ptr, idTok.name, args);
                        *ctx->stack_ptr = ptr;
                        ++ctx->stack_ptr;
                        return 1;
                } else {



                        // we got a idenfier
                        Node* ptr = MemoryPool_Alloc(&ctx->pool, sizeof(Node));
                        Node_InitIdentifer(ptr, ctx->tokenizer.tok.name);
                        *ctx->stack_ptr = ptr;
                        ++ctx->stack_ptr;
                        return 1;
                }
        }

        case TokenType_LParam:
                // eat it
                Tokenizer_Next(&ctx->tokenizer);
                parse_expr(ctx);
                if(ctx->tokenizer.tok.type != TokenType_RParam){

                        printf("bad pathing of ( expr )\n");
                        ctx->fail = 1;
                        return 0;
                }
                // eat it
                Tokenizer_Next(&ctx->tokenizer);
                return 1;
        default:
                printf("bad token\n");
                Tokenizer_Dump(&ctx->tokenizer);
                return 0;
        }
}

int parse_factor(ParserContext* ctx){
        // this should populate the stack with a term
        if ( ! parse_term(ctx) ){
                return 0;
        }

        for(; ctx->tokenizer.tok.type == TokenType_Operator ;){
                Token tokOp = ctx->tokenizer.tok;
                Operator mappedOp;
                switch(tokOp.op){
                case '*' : mappedOp = Operator_Mul; break;
                case '/' : mappedOp = Operator_Div; break;
                default:
                           return 1;
                }

                // eat the operator
                Tokenizer_Next(&ctx->tokenizer);
                if( ! parse_term(ctx) ){
                        printf("bad term\n");
                        ctx->fail = 1;
                        return 0;
                }
                --ctx->stack_ptr;
                Node* rp = *ctx->stack_ptr;
                --ctx->stack_ptr;
                Node* lp = *ctx->stack_ptr;

                Node* result = MemoryPool_Alloc(&ctx->pool, sizeof(Node));
                *ctx->stack_ptr = result;
                ++ctx->stack_ptr;


                Node_InitOperator( result, mappedOp, lp, rp);
        }
        return 1;
}

int parse_expr(ParserContext* ctx){
        // this should populate the stack with a term
        if ( ! parse_factor(ctx) ){
                return 0;
        }

        for(; ctx->tokenizer.tok.type == TokenType_Operator ;){
                Token tokOp = ctx->tokenizer.tok;
                Operator mappedOp;
                switch(tokOp.op){
                case '+' : mappedOp = Operator_Add; break;
                case '-' : mappedOp = Operator_Sub; break;
                default:
                           return 1;
                }

                // eat the operator
                Tokenizer_Next(&ctx->tokenizer);
                if( ! parse_factor(ctx) ){
                        ctx->fail = 1;
                        return 0;
                }
                --ctx->stack_ptr;
                Node* rp = *ctx->stack_ptr;
                --ctx->stack_ptr;
                Node* lp = *ctx->stack_ptr;

                Node* result = MemoryPool_Alloc(&ctx->pool, sizeof(Node));
                *ctx->stack_ptr = result;
                ++ctx->stack_ptr;


                Node_InitOperator( result, mappedOp, lp, rp);
        }
        return 1;
}

int eval(const char* str){
        ParserContext ctx = {0};
        Tokenizer_Init(&ctx.tokenizer, str);
        MemoryPool_Init(&ctx.pool);
        ctx.stack_ptr = ctx.stack_mem;

        parse_expr(&ctx);

        if( ! (ctx.stack_ptr - ctx.stack_mem == 1  )){
                printf("bad stack, stack_size = %d\n", ctx.stack_ptr - ctx.stack_mem );
                return -1;
        }

        int result = Node_Eval( ctx.stack_mem[0] );
        
        #if 1
        Node_Dump( ctx.stack_mem[0] );
        printf("\n");
        Node_DumpPretty( ctx.stack_mem[0] );
        printf("\n");
        printf("eval -> %d\n", result  );
        printf("\n");
        #endif
        return result;

}


int main(){
        #if 0
        #define EVAL_TEST( EXPR ) do{\
                int result = eval(#EXPR);\
                printf(#EXPR " = %d\n", result); \
                if( result != (EXPR) ){\
                        printf("eval failed on "#EXPR"\n");\
                }\
        }while(0)

        EVAL_TEST(11);
        EVAL_TEST(11+22);
        EVAL_TEST(11- 444);
        EVAL_TEST(11- 444 + 222);
        EVAL_TEST(11- 444 + 222 + 999);

        EVAL_TEST(1+2+3+4+5);
        EVAL_TEST(1+2+3+4*5);
        EVAL_TEST(1+2+3*4+5);
        EVAL_TEST(1+2*3+4+5);
        EVAL_TEST(1*2+3+4+5);
        
        EVAL_TEST(1+2*(3+4)+5);
        EVAL_TEST(1*(2+3)+4+5);

        EVAL_TEST( 234 + 8788 / 2334); 
        EVAL_TEST( 234 * 8788 / 2334); 

        eval("hello");
        eval("hello+world");

        Tokenizer_dotest("f(1)");
        Tokenizer_dotest("f(1+2)");
        Tokenizer_dotest("f(1,2)");
        #endif
        
        
        eval("f(1)");
        eval("f(1+2)");
        eval("f(1,2)");
        eval("f()");
        
        eval(" 23 * f(1,2 / 5) - 3");
        
        #if 0
        eval("");
        eval("1");
        eval("12");
        eval("  1");
        eval("1   ");
        eval("1+3");
        eval("1   *334   /2");
        eval("35454");
        eval("1+3sb");
        #endif
}
