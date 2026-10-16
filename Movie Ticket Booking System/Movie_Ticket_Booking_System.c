
#include <stdio.h>
#include <string.h>

#define NAME_SIZE 50
#define MIN_TICKETS 1
#define MAX_TICKETS 20
#define DISCOUNT_MIN_TICKETS 5

// Ticket categories and menu choices
enum TicketCategory {
    STANDARD = 1,
    PREMIUM = 2,
    RECLINER = 3
};

// Ticket prices and discount rate
const double STANDARD_PRICE = 150.0;
const double PREMIUM_PRICE = 250.0;
const double RECLINER_PRICE = 400.0;
const double DISCOUNT_RATE = 0.10;

// Stores the calculated bill details
struct BookingBill {
    double subtotal;
    double discount;
    double finalBill;
};

// Reads the customer's name
int getCustomerName(char customerName[]) {
    printf("Enter customer name: ");

    if (fgets(customerName, NAME_SIZE, stdin) == NULL) {
        printf("Invalid customer name.\n");
        return 0;
    }

    // Remove the newline added by fgets
    customerName[strcspn(customerName, "\n")] = '\0';

    if (customerName[0] == '\0') {
        printf("Customer name cannot be empty.\n");
        return 0;
    }

    // Check whether the complete name fits in the input buffer
    if (strchr(customerName, '\n') == NULL &&
        !feof(stdin)) {
        int ch = getchar();

        if (ch != '\n' && ch != EOF) {
            while (ch != '\n' && ch != EOF) {
                ch = getchar();
            }

            printf("Customer name is too long.\n");
            return 0;
        }
    }

    return 1;
}

// Reads and validates the number of tickets
int getTicketCount(int *tickets) {
    printf("Enter number of tickets (1-20): ");

    if (scanf("%d", tickets) != 1 ||
        *tickets < MIN_TICKETS ||
        *tickets > MAX_TICKETS) {
        printf("Invalid number of tickets.\n");
        return 0;
    }

    return 1;
}

// Displays available ticket categories
void displayTicketCategories(void) {
    printf("\nTicket Categories\n");
    printf("1. Standard - Rs. %.2f\n", STANDARD_PRICE);
    printf("2. Premium  - Rs. %.2f\n", PREMIUM_PRICE);
    printf("3. Recliner - Rs. %.2f\n", RECLINER_PRICE);
}

// Reads and validates the ticket category
int getTicketCategory(enum TicketCategory *category) {
    int choice;

    displayTicketCategories();

    printf("Enter ticket category: ");

    if (scanf("%d", &choice) != 1) {
        printf("Invalid ticket category.\n");
        return 0;
    }

    if (choice < STANDARD || choice > RECLINER) {
        printf("Invalid ticket category.\n");
        return 0;
    }

    *category = (enum TicketCategory)choice;
    return 1;
}

// Gets the price and name for the selected category
int getTicketDetails(
    enum TicketCategory category,
    double *ticketPrice,
    const char **categoryName
) {
    switch (category) {
        case STANDARD:
            *ticketPrice = STANDARD_PRICE;
            *categoryName = "Standard";
            return 1;

        case PREMIUM:
            *ticketPrice = PREMIUM_PRICE;
            *categoryName = "Premium";
            return 1;

        case RECLINER:
            *ticketPrice = RECLINER_PRICE;
            *categoryName = "Recliner";
            return 1;

        default:
            return 0;
    }
}

// Calculates the subtotal, discount, and final bill
struct BookingBill calculateBill(double ticketPrice, int tickets) {
    struct BookingBill bill;

    bill.subtotal = ticketPrice * tickets;
    bill.discount = 0.0;

    // Apply a 10% discount for five or more tickets
    if (tickets >= DISCOUNT_MIN_TICKETS) {
        bill.discount = bill.subtotal * DISCOUNT_RATE;
    }

    bill.finalBill = bill.subtotal - bill.discount;

    return bill;
}

// Displays the complete booking summary
void displayBookingSummary(
    const char customerName[],
    const char categoryName[],
    int tickets,
    struct BookingBill bill
) {
    printf("\n----- Booking Details -----\n");
    printf("Customer: %s\n", customerName);
    printf("Ticket Category: %s\n", categoryName);
    printf("Tickets: %d\n", tickets);
    printf("Subtotal: Rs. %.2f\n", bill.subtotal);
    printf("Discount: Rs. %.2f\n", bill.discount);
    printf("Final Bill: Rs. %.2f\n", bill.finalBill);
}

// Coordinates the booking process
int main(void) {
    char customerName[NAME_SIZE];
    int tickets;
    enum TicketCategory category;
    double ticketPrice;
    const char *categoryName;
    struct BookingBill bill;

    if (!getCustomerName(customerName)) {
        return 1;
    }

    if (!getTicketCount(&tickets)) {
        return 1;
    }

    if (!getTicketCategory(&category)) {
        return 1;
    }

    if (!getTicketDetails(category, &ticketPrice, &categoryName)) {
        printf("Invalid ticket category.\n");
        return 1;
    }

    bill = calculateBill(ticketPrice, tickets);

    displayBookingSummary(
        customerName,
        categoryName,
        tickets,
        bill
    );

    return 0;
}
