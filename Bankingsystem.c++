#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <cstddef>
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

class Transaction {
public:
    std::string type;
    double amount;
    std::time_t timestamp;

    Transaction(const std::string& type, double amount)
        : type(type), amount(amount), timestamp(std::time(0)) {}
};

class Account {
public:
    std::string accountNumber;
    std::string accountHolderName;
    double balance;
    double loanLimit;
    std::vector<Transaction> transactions;

    Account(const std::string& accountNumber, const std::string& accountHolderName, double loanLimit)
        : accountNumber(accountNumber), accountHolderName(accountHolderName), balance(0.0), loanLimit(loanLimit) {}

    void deposit(double amount) {
        Transaction transaction("Deposit", amount);
        transactions.push_back(transaction);
        balance += amount;
    }

    bool withdraw(double amount) {
        if (balance - amount >= 0) {
            Transaction transaction("Withdrawal", amount);
            transactions.push_back(transaction);
            balance -= amount;
            return true;
        }
        return false;
    }

    bool requestLoan(double amount) {
        if (amount <= loanLimit) {
            Transaction transaction("Loan", amount);
            transactions.push_back(transaction);
            balance += amount;
            return true;
        }
        return false;
    }
};

class User {
public:
    std::string username;
    std::string password;
    Account account;

    User(const std::string& username, const std::string& password, const std::string& accountNumber,
         const std::string& accountHolderName, double loanLimit)
        : username(username), password(password), account(accountNumber, accountHolderName, loanLimit) {}
};

class Bank {
private:
    std::vector<User> users;

public:
    void createUser(const std::string& username, const std::string& password,
                    const std::string& accountNumber, const std::string& accountHolderName,
                    double loanLimit) {
        User user(username, password, accountNumber, accountHolderName, loanLimit);
        users.push_back(user);
    }

    // Use references instead of pointers for safety
    User* login(const std::string& username, const std::string& password) {
        for (User& user : users) {
            if (user.username == username && user.password == password) {
                return &user;
            }
        }
        return nullptr; // Use nullptr instead of NULL
    }

    void displayAccountInfo(const User& user) {
        std::cout << "Account Number: " << user.account.accountNumber << std::endl;
        std::cout << "Account Holder: " << user.account.accountHolderName << std::endl;
        std::cout << "Balance: " << user.account.balance << std::endl;
    }

    const std::vector<User>& getUsers() const {
        return users;
    }
};

// Function to hide password input (Windows)
#ifdef _WIN32
std::string getHiddenPassword() {
    std::string password;
    char ch;
    while (true) {
        ch = _getch();
        if (ch == 13) // Enter key
            break;
        else if (ch == 8) // Backspace
        {
           if (!password.empty()) {
               password.erase(password.size() - 1);
               std::cout << "\b \b";
             }
        }
        else {
            password += ch;
            std::cout << '*';
        }
    }
    std::cout << std::endl;
    return password;
}

// Function to hide password input (Unix)
#else
std::string getHiddenPassword() {
    std::string password;
    termios oldTermios, newTermios;
    tcgetattr(STDIN_FILENO, &oldTermios);
    newTermios = oldTermios;
    newTermios.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newTermios);

    std::getline(std::cin, password);

    tcsetattr(STDIN_FILENO, TCSANOW, &oldTermios);
    return password;
}
#endif

int main() {
    Bank bank;

    bank.createUser("user1", "password1", "1234", "Brian", 1000.0);

    std::string username;
    std::string password;

    std::cout << "Enter Username: ";
    std::cin >> username;

    std::cout << "Enter Password: ";
    password = getHiddenPassword();

    User* loggedInUser = bank.login(username, password);
    if (loggedInUser != nullptr) {
        std::cout << "Login Successful!" << std::endl;
        std::cout << "Welcome, " << loggedInUser->username << "!" << std::endl;

        int choice;
        do {
            std::cout << "-----------------------------" << std::endl;
            std::cout << "1. Deposit" << std::endl;
            std::cout << "2. Withdraw" << std::endl;
            std::cout << "3. Request Loan" << std::endl;
            std::cout << "4. Account Info" << std::endl;
            std::cout << "5. Exit" << std::endl;
            std::cout << "Enter your choice: ";
            std::cin >> choice;

            switch (choice) {
                case 1: {
                    double amount;
                    std::cout << "Enter the amount to deposit: ";
                    std::cin >> amount;
                    loggedInUser->account.deposit(amount);
                    std::cout << "Deposit successful!" << std::endl;
                    break;
                }
                case 2: {
                    double amount;
                    std::cout << "Enter the amount to withdraw: ";
                    std::cin >> amount;
                    if (loggedInUser->account.withdraw(amount))
                        std::cout << "Withdrawal successful!" << std::endl;
                    else
                        std::cout << "Insufficient balance!" << std::endl;
                    break;
                }
                case 3: {
                    double amount;
                    std::cout << "Enter the loan amount: ";
                    std::cin >> amount;
                    if (loggedInUser->account.requestLoan(amount))
                        std::cout << "Loan request successful!" << std::endl;
                    else
                        std::cout << "Loan request failed! Exceeded loan limit." << std::endl;
                    break;
                }
                case 4:
                    bank.displayAccountInfo(*loggedInUser);
                    break;
                case 5:
                    std::cout << "Exiting..." << std::endl;
                    break;
                default:
                    std::cout << "Invalid choice. Please try again." << std::endl;
                    break;
            }
        } while (choice != 5);
    } else {
        std::cout << "Login failed. Invalid username or password." << std::endl;
        std::cout << "Entered username: " << username << std::endl;
        for (const User& user : bank.getUsers()) {
            std::cout << "Stored username: " << user.username << std::endl;
            std::cout << "Stored password: " << user.password << std::endl;
        }
    }

    return 0;
}
