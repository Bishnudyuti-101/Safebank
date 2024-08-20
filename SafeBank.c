#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_CUSTOMERS 100
#define FILE_NAME "customers.dat"

typedef struct {
    char name[50];
    char username[20];
    char password[64]; // For hashed passwords
    int accountNumber;
    float balance;
} Customer;

Customer customers[MAX_CUSTOMERS];
int customerCount = 0;

void loadCustomers();
void saveCustomers();
void createAccount();
int login();
void displayDetails(int index);
void deposit(int index, float amount);
void withdraw(int index, float amount);
void clearInputBuffer();

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void hashPassword(const char *password, char *hashedPassword) {
    // Simple hashing function (for demonstration only)
    unsigned long hash = 5381;
    int c;
    while ((c = *password++))
        hash = ((hash << 5) + hash) + c;
    sprintf(hashedPassword, "%lx", hash);
}

void createAccount() {
    if (customerCount >= MAX_CUSTOMERS) {
        printf("Maximum number of customers reached.\n");
        return;
    }

    Customer newCustomer;
    printf("Enter name: ");
    fgets(newCustomer.name, sizeof(newCustomer.name), stdin);
    newCustomer.name[strcspn(newCustomer.name, "\n")] = '\0';

    printf("Enter username: ");
    fgets(newCustomer.username, sizeof(newCustomer.username), stdin);
    newCustomer.username[strcspn(newCustomer.username, "\n")] = '\0';

    printf("Enter password: ");
    char password[20];
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = '\0';
    hashPassword(password, newCustomer.password);

    printf("Enter initial balance: ");
    if (scanf("%f", &newCustomer.balance) != 1) {
        printf("Invalid input.\n");
        clearInputBuffer();
        return;
    }

    newCustomer.accountNumber = customerCount + 1001;
    customers[customerCount++] = newCustomer;

    saveCustomers();

    printf("Account created. Your account number is %d.\n", newCustomer.accountNumber);
}

int login() {
    char username[20], password[20];
    printf("Enter username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0';

    printf("Enter password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = '\0';
    char hashedPassword[64];
    hashPassword(password, hashedPassword);

    for (int i = 0; i < customerCount; i++) {
        if (strcmp(customers[i].username, username) == 0 && strcmp(customers[i].password, hashedPassword) == 0) {
            return i;
        }
    }
    return -1;
}

void displayDetails(int index) {
    printf("Account Number: %d\n", customers[index].accountNumber);
    printf("Name: %s\n", customers[index].name);
    printf("Balance: %.2f\n", customers[index].balance);
}

void deposit(int index, float amount) {
    if (amount <= 0) {
        printf("Deposit amount must be positive.\n");
        return;
    }
    customers[index].balance += amount;
    printf("Deposited %.2f. New balance: %.2f\n", amount, customers[index].balance);
    saveCustomers();
}

void withdraw(int index, float amount) {
    if (amount <= 0) {
        printf("Withdrawal amount must be positive.\n");
        return;
    }
    if (customers[index].balance >= amount) {
        customers[index].balance -= amount;
        printf("Withdrew %.2f. New balance: %.2f\n", amount, customers[index].balance);
        saveCustomers();
    } else {
        printf("Insufficient funds.\n");
    }
}

void loadCustomers() {
    FILE *file = fopen(FILE_NAME, "rb");
    if (file != NULL) {
        fread(&customerCount, sizeof(int), 1, file);
        fread(customers, sizeof(Customer), customerCount, file);
        fclose(file);
    }
}

void saveCustomers() {
    FILE *file = fopen(FILE_NAME, "wb");
    if (file != NULL) {
        fwrite(&customerCount, sizeof(int), 1, file);
        fwrite(customers, sizeof(Customer), customerCount, file);
        fclose(file);
    }
}

int main() {
    int choice;
    int loggedInIndex = -1;

    loadCustomers();

    while (1) {
        printf("\nBank Management System\n");
        printf("1. Create Account\n");
        printf("2. Login\n");
        printf("3. Display Account Details\n");
        printf("4. Deposit\n");
        printf("5. Withdraw\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input.\n");
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1:
                createAccount();
                break;
            case 2:
                loggedInIndex = login();
                if (loggedInIndex != -1) {
                    printf("Login successful.\n");
                } else {
                    printf("Login failed.\n");
                }
                break;
            case 3:
                if (loggedInIndex != -1) {
                    displayDetails(loggedInIndex);
                } else {
                    printf("Please log in first.\n");
                }
                break;
            case 4:
                if (loggedInIndex != -1) {
                    float amount;
                    printf("Enter amount to deposit: ");
                    if (scanf("%f", &amount) != 1) {
                        printf("Invalid input.\n");
                        clearInputBuffer();
                        continue;
                    }
                    deposit(loggedInIndex, amount);
                } else {
                    printf("Please log in first.\n");
                }
                break;
            case 5:
                if (loggedInIndex != -1) {
                    float amount;
                    printf("Enter amount to withdraw: ");
                    if (scanf("%f", &amount) != 1) {
                        printf("Invalid input.\n");
                        clearInputBuffer();
                        continue;
                    }
                    withdraw(loggedInIndex, amount);
                } else {
                    printf("Please log in first.\n");
                }
                break;
            case 6:
                printf("Exiting.\n");
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }

    return 0;
}

