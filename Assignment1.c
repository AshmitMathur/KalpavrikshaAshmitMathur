/* 
	Created By: Ashmit Mathur
	Date: 01/10/26
*/
#include <stdio.h>
#include <ctype.h>

int main(){
	char expression[1000];
	printf("Enter Expression: ");
	fgets(expression, sizeof(expression), stdin);
	
	int i = 0;
	int result = 0;
	int term = 0;
	int Num;
	char operation = '+';
	
	while(expression[i] != '\0'){

		// Skip Empty Space from Beginning
		// Adapts to All WhiteSpaces
		while(isspace(expression[i])) i++;
		
		// Only Basic Space Character Handle
		//	while(expression[i] == ' ') i++;
		
		// Empty Expression
		if(expression[i] == '\0'){
			printf("Please Enter an Expression");
			return 0;
		}
		
		// Operand needs to be Digit
		if(!isdigit(expression[i])){
			printf("Error: Invalid Expression");
			return 0;
		}
		
		Num = 0;
		
		// Build Entire Number. Ex->123
		while(isdigit(expression[i])){
			Num = Num * 10 + (expression[i] - '0');
			i++;
		}
		
		// If operator +/-, wait.
		// If operator */(/) perform Operation
		if(operation == '+'){
			result += term;
			term = Num;
		} else if(operation == '-'){
			result += term;
			term = -Num;
		} else if(operation == '*'){
			term *= Num;
		} else if(operation == '/'){
			if(Num == 0){
				printf("Error: Division by Zero");
				return 0;
			}
			term /= Num;
		}
		
		// Empty Space Skip
		while(isspace(expression[i])) i++;
		//	while(expression[i] == ' ') i++;
		

		if(expression[i] == '\0') break;
		
		if(expression[i] != '+' && 
		expression[i] != '-' && expression[i] != '*' &&
		expression[i] != '/'){
			printf("Error: Invalid Expression");
			return 0;
		}
		operation = expression[i];
		i++;
	}
	result += term;
	printf("%d", result);
	return 0;
}
