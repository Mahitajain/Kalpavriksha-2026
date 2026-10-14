#include <stdio.h>

// Ticket categories and menu choices
enum TicketCategory {
    STANDARD = 1,
    PREMIUM = 2,
    RECLINER = 3
};

int main() {
    char customerName[50];
    int tickets, choice;

    const double STANDARD_PRICE = 150.0;
    const double PREMIUM_PRICE = 250.0;
    const double RECLINER_PRICE = 400.0;

    const double DISCOUNT_RATE = 0.10;

    double ticketPrice = 0.0;
    double subtotal, discount = 0.0, finalBill;
    const char *categoryName;

    // Get the customer's name
    printf("Enter customer name: ");
    if (scanf(" %49[^\n]", customerName) != 1) {
        printf("Invalid customer name.\n");
        return 1;
    }

    // Make sure the customer books between 1 and 20 tickets
    printf("Enter number of tickets (1-20): ");
    if (scanf("%d", &tickets) != 1 || tickets < 1 || tickets > 20) {
        printf("Invalid number of tickets.\n");
        return 1;
    }

    // Show the available categories and their prices
    printf("\nTicket Categories\n");
    printf("1. Standard - Rs. 150\n");
    printf("2. Premium  - Rs. 250\n");
    printf("3. Recliner - Rs. 400\n");

    printf("Enter ticket category: ");
    if (scanf("%d", &choice) != 1) {
        printf("Invalid ticket category.\n");
        return 1;
    }

    // Convert the user's choice into a ticket category
    enum TicketCategory category = (enum TicketCategory)choice;

    // Pick the price based on the selected category
    switch (category) {
        case STANDARD:
            ticketPrice = STANDARD_PRICE;
            categoryName = "Standard";
            break;

        case PREMIUM:
            ticketPrice = PREMIUM_PRICE;
            categoryName = "Premium";
            break;

        case RECLINER:
            ticketPrice = RECLINER_PRICE;
            categoryName = "Recliner";
            break;

        default:
            printf("Invalid ticket category.\n");
            return 1;
    }

    // Calculate the cost before applying any discount
    subtotal = ticketPrice * tickets;

    // Bookings of five or more tickets get a 10% discount
    if (tickets >= 5) {
        discount = subtotal * DISCOUNT_RATE;
    } else {
        discount = 0.0;
    }

    // Subtract the discount to get the final amount
    finalBill = subtotal - discount;

    // Display the complete booking summary
    printf("\n----- Booking Details -----\n");
    printf("Customer: %s\n", customerName);
    printf("Ticket Category: %s\n", categoryName);
    printf("Tickets: %d\n", tickets);
    printf("Subtotal: %.2f\n", subtotal);
    printf("Discount: %.2f\n", discount);
    printf("Final Bill: %.2f\n", finalBill);

    return 0;
}
