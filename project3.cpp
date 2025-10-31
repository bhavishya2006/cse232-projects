// In this project, you will be creating a bank account calculator that can process withdraws and deposits, calculate interest, and apply overdraft fees. This is a task that is more involved than it might first appear. Programs like this are commonly used in banking and fraud detection and this project might help you grasp the subtleties of writing financial code.
// main.cpp
#include "bank_account.hpp"
#include <iostream>
#include <string>

int main() {
    std::string input {
        "2016-09-02 Deposit $500\n"
        "2016-09-02 Withdraw $23.54\n"
        "2016-09-04 Withdraw $58\n"
        "2016-11-02 Deposit $200\n"
        "2017-03-02 Withdraw $1000\n"
        "2019-11-02 Deposit $500\n"};

    std::string result = process_commands(input, 7.0);

    std::string expected {
        "On 2016-09-02: Instructed to perform \"Deposit $500\"\n"
        "Balance: 500.00\n"
        "On 2016-09-02: Instructed to perform \"Withdraw $23.54\"\n"
        "Balance: 476.46\n"
        "On 2016-09-04: Instructed to perform \"Withdraw $58\"\n"
        "Balance: 418.46\n"
        "On 2016-11-02: Instructed to perform \"Deposit $200\"\n"
        "Since 2016-09-04, interest has accrued 2 times.\n"
        "$4.89 interest has been earned.\n"
        "Balance: 623.35\n"
        "On 2017-03-02: Instructed to perform \"Withdraw $1000\"\n"
        "Since 2016-11-02, interest has accrued 4 times.\n"
        "$14.65 interest has been earned.\n"
        "Overdraft!\n"
        "Balance: -397.00\n"
        "On 2019-11-02: Instructed to perform \"Deposit $500\"\n"
        "Since 2017-03-02, interest has accrued 32 times.\n"
        "$0.00 interest has been earned.\n"
        "Balance: 103.00\n"};

    std::cout << result;
    if (result != expected) {
        std::cout << "\nERROR: Result does not match expected output.\n";
    } else {
        std::cout << "\nSUCCESS: Result matches expected output.\n";
    }

    return 0;
}

//bankaccount.cpp
#include "bank_account.hpp"
#include <cmath>
#include <sstream>
#include <iomanip>
#include <algorithm>

double round_to_cent(double value) {
    return std::round(value * 100.0) / 100.0;
}

void deposit(double& balance, double amount) {
    balance = round_to_cent(balance + amount);
}

bool withdraw(double& balance, double amount) {
    if (balance >= amount) {
        balance = round_to_cent(balance - amount);
        return true;
    }
    return false;
}

void overdraft(double& balance, double amount) {
    balance = round_to_cent(balance - amount - 35.0);
}

double interest_for_month(double balance, double apr) {
    double monthly_rate = apr / 100.0 / 12.0;
    double interest = balance * monthly_rate;
    
    return std::floor(interest * 100.0) / 100.0;
}

void string_date_to_ints(const std::string& date, int& year, int& month, int& day) {
    std::stringstream ss(date);
    char dash;
    ss >> year >> dash >> month >> dash >> day;
}

int number_of_first_of_months(const std::string& date_early, const std::string& date_late) {
    int early_y, early_m, early_d;
    int late_y, late_m, late_d;
    string_date_to_ints(date_early, early_y, early_m, early_d);
    string_date_to_ints(date_late, late_y, late_m, late_d);
    int months = (late_y - early_y) * 12 + (late_m - early_m);
    return std::max(0, months);
}

double interest_earned(double balance, double apr, const std::string& date_early, const std::string& date_late) {
    int num_months = number_of_first_of_months(date_early, date_late);
    double total_interest = 0.0;
    for (int i = 0; i < num_months; ++i) {
        if (balance > 0.0) {
            double interest = interest_for_month(balance, apr);
            balance += interest;
            total_interest += interest;
        }
    }
    return round_to_cent(total_interest);
}

std::string process_command(const std::string& line, std::string& previous_date, double& balance, double apr) {
    std::stringstream ss(line);
    std::string date, command, amount_token;
    ss >> date >> command >> amount_token;

    if (!amount_token.empty() && amount_token[0] == '$') {
        amount_token = amount_token.substr(1);
    }

    double amount = std::stod(amount_token);
    std::string result;

    
    result += "On " + date + ": Instructed to perform \"" + command + " $" + amount_token + "\"\n";

    
    if (!previous_date.empty()) {
        double interest = interest_earned(balance, apr, previous_date, date);
        int num_accruals = number_of_first_of_months(previous_date, date);
        if (num_accruals > 0) {
            std::stringstream interest_ss;
            interest_ss << std::fixed << std::setprecision(2) << interest;
            result += "Since " + previous_date + ", interest has accrued " +
                      std::to_string(num_accruals) + " times.\n$" +
                      interest_ss.str() + " interest has been earned.\n";
        }
        balance = round_to_cent(balance + interest);
    }

    if (command == "Deposit") {
        deposit(balance, amount);
    } else if (command == "Withdraw") {
        if (!withdraw(balance, amount)) {
            overdraft(balance, amount);
            result += "Overdraft!\n";
        }
    }

    std::stringstream balance_ss;
    balance_ss << std::fixed << std::setprecision(2) << balance;
    result += "Balance: " + balance_ss.str() + "\n";

    previous_date = date;
    return result;
}

std::string process_commands(const std::string& input, double apr) {
    std::stringstream ss(input);
    std::string line;
    std::string result;
    std::string previous_date = "";
    double balance = 0.0;

    while (std::getline(ss, line)) {
        if (!line.empty()) {
            result += process_command(line, previous_date, balance, apr);
        }
    }

    return result;
}

// bankaccount.hpp
#ifndef BANK_ACCOUNT_HPP
#define BANK_ACCOUNT_HPP

#include <string>

double round_to_cent(double value);
void deposit(double& balance, double amount);
bool withdraw(double& balance, double amount);
void overdraft(double& balance, double amount);
double interest_for_month(double balance, double apr);
void string_date_to_ints(const std::string& date, int& year, int& month, int& day);
int number_of_first_of_months(const std::string& date_early, const std::string& date_late);
double interest_earned(double balance, double apr, const std::string& date_early, const std::string& date_late);
std::string process_command(const std::string& line, std::string& previous_date, double& balance, double apr);
std::string process_commands(const std::string& input, double apr);

#endif
