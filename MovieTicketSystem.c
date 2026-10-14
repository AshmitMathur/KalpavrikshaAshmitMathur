/* 
    Created By: Ashmit Mathur
    Date: 14/10/2026
    Title: Movie Ticket Booking System
*/

#include <stdio.h>

// Enum Declaration
typedef enum{
    CATEGORY_STANDARD = 1,
    CATEGORY_PREMIUM = 2,
    CATEGORY_RECLINER = 3
} TicketCategory;

// Constant Declaration
const double PRICE_STANDARD = 150.0;
const double PRICE_PREMIUM = 250.0;
const double PRICE_RECLINER = 400.0;
const double DISCOUNT_RATE = 0.10;
const int DISCOUNT_MIN_TICKETS = 5;
const int MAX_TICKETS = 20;
const int MIN_TICKETS = 1;

// Function Prototypes
int input(char customerName[50], int *numTickets, int *CategoryChoice);
void output(char *name, char *Categoryname, int tickets, double subtotal, double discount, double finalbill);
double getPriceAndName(TicketCategory category, char **CategoryName);
double calculateSubtotal(int count, double price);
double calculateDiscount(int count, double subtotal);

// Takes user Input of Name, Number of Tickets and Category
int input(char customerName[50], int *numTickets, int *CategoryChoice){
    // scanf("%49s %d %d", customerName, numTickets, CategoryChoice);
    if(scanf("%49s %d %d", customerName, numTickets, CategoryChoice) != 3){
        printf("Invalid Input Format \n");
        return 1;
    }
    if(*numTickets < MIN_TICKETS || *numTickets > MAX_TICKETS){
        printf("Error: Number of tickets must be between 1 and 20.\n");
        return 1;
    }
    return 0;
}

// Prints output according to required format
void output(char *name, char *CategoryName, int tickets, double subtotal, double discount, double finalbill){
    printf("Customer: %s\n", name);
    printf("Ticket Category: %s\n", CategoryName);
    printf("Tickets: %d\n", tickets);
    printf("Subtotal: %.2f\n", subtotal);
    printf("Discount: %.2f\n", discount);
    printf("Final Bill: %.2f\n", finalbill);
}

// Gets Price and Name of Seat based on Category Selection
double getPriceAndName(TicketCategory category, char **CategoryName){
    switch(category){
        case CATEGORY_STANDARD: 
        *CategoryName = "Standard";
        return PRICE_STANDARD;
        case CATEGORY_PREMIUM:
        *CategoryName = "Premium";
        return PRICE_PREMIUM;
        case CATEGORY_RECLINER:
        *CategoryName = "Recliner";
        return PRICE_RECLINER;
        default: return -1.0;
    }
    return -1.0;
}

// Calculates Subtotal Sum without Discount
double calculateSubtotal(int count, double price){
    return count * price;
}

// Calculates Discount to be subtracted in finalBill
double calculateDiscount(int count, double subtotal){
    if(count >= DISCOUNT_MIN_TICKETS){
        return DISCOUNT_RATE * subtotal;
    }
    return 0.0;
}


int main(){
    char customerName[50];
    int numTickets;
    int CategoryChoice;

    if(input(customerName, &numTickets, &CategoryChoice) != 0) return 1;

    TicketCategory category = (TicketCategory)CategoryChoice;
    char *categoryName = "";
    double ticketPrice = getPriceAndName(category, &categoryName);

    if(ticketPrice < 0.0){
        printf("Error: Invalid ticket category choice\n");
        return 1;
    }

    double subtotal = calculateSubtotal(numTickets, ticketPrice);
    double discount = calculateDiscount(numTickets, subtotal);
    double finalbill = subtotal - discount;

    output(customerName, categoryName, numTickets, subtotal, discount, finalbill);
    return 0;
}