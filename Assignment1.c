/* 
    Created By: Ashmit Mathur
    Date: 05/10/26
    Refactored to modular, status-driven architecture
*/
#include <stdio.h>

// Status Code
#define STATUS_SUCESS 0
#define STATUS_ERR_EMPTY 1
#define STATUS_ERR_INVALID 2
#define STATUS_ERR_DIV_ZERO 3

// Explicit Character Checks
int isSpaceChar(char ch){
    return (ch == ' ' || ch == '\n' || ch == '\t' 
    || ch == '\r' || ch == '\v' || ch == '\f');
}
int isDigitChar(char ch){
    return (ch >= '0' && ch <= '9');
}
int isOperatorChar(char ch){
    return (ch == '+' || ch == '-' || ch == '*' || ch == '/');
}

// Error Handling
void handleError(int statusCode){
    switch(statusCode){
        case STATUS_ERR_EMPTY:
        printf("Error: Please enter an Expression\n");
        break;
        case STATUS_ERR_INVALID:
        printf("Error: Invalid Expression\n");
        break;
        case STATUS_ERR_DIV_ZERO:
        printf("Error: Division by Zero\n");
        break;
        default:
        printf("Error: Unknown Error\n");
    }
}

// Expression Evaluation Logic
int evaluateExpression(char *expr, int *finalResult){
    int i=0;
    int result = 0;
    int term = 0;
    int num;

    char op = '+';
    int hasExpression = 0;
    int expectingNumber = 1;

    while(expr[i] != '\0'){
        // Skip WhiteSpaces
        while(isSpaceChar(expr[i])){
            i++;
        }

        // End Expression
        if(expr[i] == '\0') break;

        if(!isDigitChar(expr[i])){
            return STATUS_ERR_INVALID;
        }

        hasExpression = 1;
        num = 0;

        // Build Number
        while(isDigitChar(expr[i])){
            num = num * 10 + (expr[i] - '0');
            i++;
        }
        expectingNumber = 0;

        // Perform Operations
        if(op == '+'){
            result += term;
            term = num;
        } else if(op == '-'){
            result += term;
            term = -num;
        } else if(op == '*'){
            term *= num;
        } else if(op == '/'){
            if(num == 0){
                return STATUS_ERR_DIV_ZERO;
            }
            term /= num;
        }

        // Skip WhiteSpace
        while(isSpaceChar(expr[i])) i++;

        // Expression ended after a Number
        if(expr[i] == '\0') break;

        // Next Character must be an operator
        if(!isOperatorChar(expr[i])) return STATUS_ERR_INVALID;

        op = expr[i];
        i++;
        
        expectingNumber = 1;
    }
    if(!hasExpression) return STATUS_ERR_EMPTY;
    if(expectingNumber) return STATUS_ERR_INVALID;

    *finalResult = result + term;
    return STATUS_SUCESS;
}

int main(){
    char expression[1000];
    int result = 0;
    int status;

    printf("Enter Expression: ");
    fgets(expression, sizeof(expression), stdin);

    status = evaluateExpression(expression, &result);

    if(status != STATUS_SUCESS){
        handleError(status);
    } else{
        printf("%d\n", result);
    }
    return 0;
}