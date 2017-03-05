extern int printf(const char* c, ...);
extern unsigned long long strlen(const char* s);
extern int isdigit(int);
extern int isspace(int);
extern void* memset( void*, int , unsigned long long );

#include <assert.h>

/*
   accepts string of the grammar
      digit  -> 0123456789
      digits -> digit+
      term   -> digits
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
        TokenType_Operator
}TokenType;

typedef struct Token{
        TokenType type;
        int value;
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
                default:{
                        // try to get value, otherwise fail the whole stream
                        // because the peak is invalid
                        const char* end = ctx->iter;
                        int value = 0;
                        for(; end != ctx->end; ++end){
                                if( ! isdigit(*end) )
                                        break;
                                value *= 10;
                                value += *end - '0';
                        }
                        if( end != ctx->iter ){
                                ctx->peak.type  = TokenType_Literal;
                                ctx->peak.value = value;
                                ctx->iter = end;
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
        Operator op;
        struct Node* arg0;
        struct Node* arg1;
}Node;

void Node_Dump(Node* node){
        if( node == 0 ){
                printf("<null>");
                return;
        }
        switch(node->kind){
        case ExprKind_Value:
                printf("value(%d)", node->value);
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

void Node_InitValue(Node* ptr, int value){
        memset(ptr, 0, sizeof(*ptr));
        ptr->kind  = ExprKind_Value;
        ptr->value = value;
}
void Node_InitOperator(Node* ptr, Operator op, Node* left, Node* right){
        memset(ptr, 0, sizeof(*ptr));
        ptr->kind   = ExprKind_BinaryOperator;
        ptr->op     = op;
        ptr->arg0   = left;
        ptr->arg1  = right;
}

typedef struct ParserContext{
        TokenizerContext tokenizer;
        MemoryPool       pool;
        void*            stack_mem[128];
        void**           stack_ptr;
        int              fail; // fail flag
}ParserContext;

int parse_term(ParserContext* ctx){
        switch(ctx->tokenizer.tok.type){
        case TokenType_Literal: 
                break;
        default:
                return 0;
        }
        // we got a literal
        Node* ptr = MemoryPool_Alloc(&ctx->pool, sizeof(Node));
        Node_InitValue(ptr, ctx->tokenizer.tok.value);
        *ctx->stack_ptr = ptr;
        ++ctx->stack_ptr;
        // eat it
        Tokenizer_Next(&ctx->tokenizer);
        return 1;
}

int parse_factor(ParserContext* ctx){
        // this should populate the stack with a term
        if ( ! parse_term(ctx) ){
                return 0;
        }

        for(; ctx->tokenizer.tok.type == TokenType_Operator ;){
                Token tokOp = ctx->tokenizer.tok;
                // eat the operator
                Tokenizer_Next(&ctx->tokenizer);
                if( ! parse_term(ctx) ){
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

                Operator mappedOp;
                switch(tokOp.op){
                case '+' : mappedOp = Operator_Add; break;
                case '-' : mappedOp = Operator_Sub; break;
                case '/' : mappedOp = Operator_Add; break;
                case '*' : mappedOp = Operator_Sub; break;
                default:
                           ctx->fail = 1;
                           return 0;
                }

                Node_InitOperator( result, mappedOp, lp, rp);
        }
        return 1;
}

void driver(const char* str){
        ParserContext ctx = {0};
        Tokenizer_Init(&ctx.tokenizer, str);
        MemoryPool_Init(&ctx.pool);
        ctx.stack_ptr = ctx.stack_mem;

        printf(" str = %s\n", str);
        
        parse_factor(&ctx);

        printf("stack_size = %d\n", ctx.stack_ptr - ctx.stack_mem );

        if( ! (ctx.stack_ptr - ctx.stack_mem == 1  )){
                printf("bad stack, stack_size = %d\n", ctx.stack_ptr - ctx.stack_mem );
                return;
        }

        Node_Dump( ctx.stack_mem[0] );
        printf("\n\n");

}


int main(){
        driver("11");
        driver("11+22");
        driver("11/ 444");
        driver("11/ 444 * 222");
        driver("11/ 444 * 222 + 999");
        #if 0
        driver("");
        driver("1");
        driver("12");
        driver("  1");
        driver("1   ");
        driver("1+3");
        driver("1   *334   /2");
        driver("35454");
        driver("1+3sb");
        #endif
}
