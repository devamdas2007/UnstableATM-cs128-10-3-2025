#ifndef CATCH_CONFIG_MAIN
#  define CATCH_CONFIG_MAIN
#endif

#include "atm.hpp"
#include "catch.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////
//                             Helper Definitions //
/////////////////////////////////////////////////////////////////////////////////////////////

bool CompareFiles(const std::string& p1, const std::string& p2) {
  std::ifstream f1(p1);
  std::ifstream f2(p2);

  if (f1.fail() || f2.fail()) {
    return false;  // file problem
  }

  std::string f1_read;
  std::string f2_read;
  while (f1.good() || f2.good()) {
    f1 >> f1_read;
    f2 >> f2_read;
    if (f1_read != f2_read || (f1.good() && !f2.good()) ||
        (!f1.good() && f2.good()))
      return false;
  }
  return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Test Cases
/////////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Example: Create a new account", "[ex-1]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.30);
  auto accounts = atm.GetAccounts();
  REQUIRE(accounts.contains({12345678, 1234}));
  REQUIRE(accounts.size() == 1);

  Account sam_account = accounts[{12345678, 1234}];
  REQUIRE(sam_account.owner_name == "Sam Sepiol");
  REQUIRE(sam_account.balance == 300.30);

  auto transactions = atm.GetTransactions();
  REQUIRE(accounts.contains({12345678, 1234}));
  REQUIRE(accounts.size() == 1);
  std::vector<std::string> empty;
  REQUIRE(transactions[{12345678, 1234}] == empty);
}

TEST_CASE("Example: Simple widthdraw", "[ex-2]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.30);
  atm.WithdrawCash(12345678, 1234, 20);
  auto accounts = atm.GetAccounts();
  Account sam_account = accounts[{12345678, 1234}];

  REQUIRE(sam_account.balance == 280.30);
}

TEST_CASE("Example: Print Prompt Ledger", "[ex-3]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.30);
  auto& transactions = atm.GetTransactions();
  transactions[{12345678, 1234}].push_back(
      "Withdrawal - Amount: $200.40, Updated Balance: $99.90");
  transactions[{12345678, 1234}].push_back(
      "Deposit - Amount: $40000.00, Updated Balance: $40099.90");
  transactions[{12345678, 1234}].push_back(
      "Deposit - Amount: $32000.00, Updated Balance: $72099.90");
  atm.PrintLedger("./prompt.txt", 12345678, 1234);
  REQUIRE(CompareFiles("./ex-1.txt", "./prompt.txt"));
}

TEST_CASE("Double Register Account", "[Test1]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Abe Lincoln", 453.58);
  REQUIRE_THROWS(atm.RegisterAccount(12345678, 1234, "Bob", 2389.42));
}
TEST_CASE("Overdrawn Account", "[Test2]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Abe Lincoln", 453.58);
  REQUIRE_THROWS(atm.WithdrawCash(12345678, 1234, 2389.42));
  REQUIRE_THROWS(atm.WithdrawCash(12345678, 1234, -5));
  REQUIRE_THROWS(atm.WithdrawCash(12345778, 1334, 200));
}

TEST_CASE("Deposit Cash Exceptions", "[Test3]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Abe Lincoln", 453.58);
  REQUIRE_THROWS(atm.DepositCash(12345678, 1324, 2389.42));
  REQUIRE_THROWS(atm.DepositCash(12345978, 1324, 2389.42));
  REQUIRE_THROWS(atm.DepositCash(12345778, 1324, 2389.42));
  REQUIRE_THROWS(atm.DepositCash(12345678, 1234, -1));
}
TEST_CASE("Deposit Cash Amount", "[Test3]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Abe Lincoln", 400.00);
  atm.DepositCash(12345678, 1234, 100.00);
  atm.DepositCash(12345678, 1234, 300.00);
  atm.DepositCash(12345678, 1234, 600.00);
  atm.DepositCash(12345678, 1234, 50.00);
  atm.DepositCash(12345678, 1234, 35.00);
  REQUIRE(atm.CheckBalance(12345678, 1234) == 1485);
}