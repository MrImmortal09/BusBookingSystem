#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int busNumbers[60];
char sources[60][30];
char destinations[60][30];
char departureTimes[60][10];
char arrivalTimes[60][10];

int activeSeatsRemaining[60][40];
int bookedSeats[60][40];
int numBuses = 0;

void readAvailableBuses() {
    FILE *file = fopen("buses.csv", "r");

    if (file != NULL) {
        while (fscanf(file, "%d ,%49[^,],%49[^,],%9[^,],%9[^\n]", &busNumbers[numBuses], sources[numBuses], destinations[numBuses], departureTimes[numBuses], arrivalTimes[numBuses]) == 5) {
            for (int i = 0; i < 40; ++i) {
                activeSeatsRemaining[numBuses][i] = 1;
                bookedSeats[numBuses][i] = 0;
            }
            ++numBuses;
            if (numBuses >= 60) {
                printf("Maximum number of buses reached.\n");
                break;
            }
        }
        fclose(file);
    } else {
        printf("Error opening file for reading available buses.\n");
    }
}

void printTicketDesign(int busNumber, int ticketNumber, int seatNumber) {
    char filename[20];
    sprintf(filename, "ticket_%d.txt", ticketNumber);

    FILE *ticketFile = fopen(filename, "w");
    if (ticketFile != NULL) {
        fprintf(ticketFile, "********************************************\n");
        fprintf(ticketFile, "*************** TICKET DETAILS **************\n");
        fprintf(ticketFile, "********************************************\n");
        fprintf(ticketFile, "Bus Number: %d\n", busNumber);
        fprintf(ticketFile, "Ticket Number: %d\n", ticketNumber);
        fprintf(ticketFile, "Seat Number: %d\n", seatNumber);
        fprintf(ticketFile, "Source: %s\n", sources[busNumber - 1]);
        fprintf(ticketFile, "Destination: %s\n", destinations[busNumber - 1]);
        fprintf(ticketFile, "Departure Time: %s\n", departureTimes[busNumber - 1]);
        fprintf(ticketFile, "Arrival Time: %s\n", arrivalTimes[busNumber - 1]);
        fprintf(ticketFile, "********************************************\n");

        fclose(ticketFile);
    } else {
        printf("Error creating ticket file.\n");
    }
}

void bookTicket() {
    int busNumber, seatNumber;

    printf("Enter Bus Number: ");
    scanf("%d", &busNumber);

    if (busNumber < 1 || busNumber > numBuses) {
        printf("Invalid Bus Number. Please try again.\n");
        return;
    }

    printf("Enter Seat Number: ");
    scanf("%d", &seatNumber);

    if (seatNumber < 1 || seatNumber > 40) {
        printf("Invalid Seat Number. Please try again.\n");
        return;
    }

    if (activeSeatsRemaining[busNumber - 1][seatNumber - 1] == 1) {
        bookedSeats[busNumber - 1][seatNumber - 1] = 1;
        activeSeatsRemaining[busNumber - 1][seatNumber - 1] = 0;

        FILE *bus = fopen("meta.csv", "r");
        int ticketNumber;
        if (bus != NULL) {
            fscanf(bus, "%d", &ticketNumber);
            fclose(bus);
        } else {
            printf("Error opening file for reading ticket number.\n");
            return;
        }
        printTicketDesign(busNumber, ticketNumber, seatNumber);

        FILE *bus2 = fopen("meta.csv", "w");

        if (bus2 != NULL) {
            fprintf(bus2, "%d", ticketNumber + 1);
            fclose(bus2);
        } else {
            printf("Error opening file for reading ticket number.\n");
            return;
        }
        FILE *bookedSeatsFile = fopen("booked_seats.csv", "a");
        if (bookedSeatsFile != NULL) {
            fprintf(bookedSeatsFile, "%d,%d,%d\n", busNumber, bookedSeats[busNumber - 1][seatNumber - 1], seatNumber);
            fclose(bookedSeatsFile);
        } else {
            printf("Error opening file for booked seats.\n");
            return;
        }

        FILE *remainingSeatsFile = fopen("remaining_seats.csv", "w");
        if (remainingSeatsFile != NULL) {
            fprintf(remainingSeatsFile, "BusNumber,SeatNumber,ActiveSeatsRemaining\n");
            for (int i = 0; i < numBuses; ++i) {
                for (int j = 0; j < 40; ++j) {
                    fprintf(remainingSeatsFile, "%d,%d,%d\n", i + 1, j + 1, activeSeatsRemaining[i][j]);
                }
            }
            fclose(remainingSeatsFile);
        } else {
            printf("Error opening file for remaining seats.\n");
            return;
        }

        printf("Ticket booked successfully!\n");
    } else {
        printf("Seat already booked. Please choose another seat.\n");
    }
}

void Menu() {
    printf("1. View Available Buses\n");
    printf("2. Book a Ticket\n");
    printf("3. View Booked Tickets\n");
    printf("4. Exit\n");
}

void viewAvailableBuses() {
    printf("Available Buses:\n");
    for (int i = 0; i < numBuses; ++i) {
        printf("Bus %d: %s to %s\n", busNumbers[i], sources[i], destinations[i]);
        printf("   Departure Time: %s\n", departureTimes[i]);
        printf("   Arrival Time: %s\n", arrivalTimes[i]);
    }
    printf("\n");
}

void editAvailableBuses() {
    if (numBuses < 60) {
        int busNumber;
        char source[30];
        char destination[30];
        char departureTime[10];
        char arrivalTime[10];

        printf("Enter Bus Number: ");
        scanf("%d", &busNumber);

        for (int i = 0; i < numBuses; ++i) {
            if (busNumbers[i] == busNumber) {
                printf("Bus with this number already exists. Please try again.\n");
                return;
            }
        }

        printf("Enter Source: ");
        scanf("%49s", source);
        printf("Enter Destination: ");
        scanf("%49s", destination);
        printf("Enter Departure Time: ");
        scanf("%9s", departureTime);
        printf("Enter Arrival Time: ");
        scanf("%9s", arrivalTime);

        busNumbers[numBuses] = busNumber;
        strcpy(sources[numBuses], source);
        strcpy(destinations[numBuses], destination);
        strcpy(departureTimes[numBuses], departureTime);
        strcpy(arrivalTimes[numBuses], arrivalTime);
        ++numBuses;

        FILE *file = fopen("buses.csv", "a");
        if (file != NULL) {
            fprintf(file, "%d,%s,%s,%s,%s\n", busNumber, source, destination, departureTime, arrivalTime);
            fclose(file);
            printf("Bus route added successfully!\n");
        } else {
            printf("Error opening file for bus route addition.\n");
        }
    } else {
        printf("Maximum buses reached.\n");
    }
}

void viewBookedTickets() {
    printf("Booked Tickets:\n");
    for (int i = 0; i < numBuses; ++i) {
        for (int j = 0; j < 40; ++j) {
            if (bookedSeats[i][j] == 1) {
                printf("Bus %d - Seat %d\n", busNumbers[i], j + 1);
            }
        }
    }
    printf("\n");
}

int main() {
    readAvailableBuses();

    int choice;

    do {
        Menu();
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                viewAvailableBuses();
                break;
            case 2:
                bookTicket();
                break;
            case 3:
                viewBookedTickets();
                break;
            case 9: // hidden as customer should not be able to edit buses. only admin should be able to
                editAvailableBuses();
                break;
            case 4:
                printf("Exiting program. Goodbye!\n");
                break;
            default:
                printf("Invalid choice. Please enter a valid option.\n");
        }
    } while (choice != 4);

    return 0;
}
