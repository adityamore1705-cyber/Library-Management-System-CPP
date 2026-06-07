#include <algorithm>
#include <chrono>
#include <cstdint>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;

namespace {
constexpr const char* BOOK_FILE = "books.dat";
constexpr const char* MEMBER_FILE = "members.dat";
constexpr const char* TRANSACTION_FILE = "transactions.dat";
constexpr int LOAN_PERIOD_DAYS = 14;
constexpr double FINE_PER_DAY = 2.0;

// ----------------------- Utility Functions -----------------------
template <typename T>
void writePrimitive(std::ofstream& out, const T& value) {
    out.write(reinterpret_cast<const char*>(&value), sizeof(T));
}

template <typename T>
void readPrimitive(std::ifstream& in, T& value) {
    in.read(reinterpret_cast<char*>(&value), sizeof(T));
}

void writeString(std::ofstream& out, const string& value) {
    std::size_t size = value.size();
    writePrimitive(out, size);
    out.write(value.c_str(), static_cast<std::streamsize>(size));
}

void readString(std::ifstream& in, string& value) {
    std::size_t size = 0;
    readPrimitive(in, size);
    value.resize(size);
    if (size > 0) {
        in.read(&value[0], static_cast<std::streamsize>(size));
    }
}

string toLower(string text) {
    std::transform(text.begin(), text.end(), text.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return text;
}

string formatDate(std::int64_t timestamp) {
    std::time_t rawTime = static_cast<std::time_t>(timestamp);
    std::tm* local = std::localtime(&rawTime);
    if (!local) {
        return "N/A";
    }

    std::ostringstream out;
    out << std::put_time(local, "%Y-%m-%d");
    return out.str();
}

std::int64_t getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    return static_cast<std::int64_t>(std::chrono::system_clock::to_time_t(now));
}

std::int64_t addDays(std::int64_t timestamp, int days) {
    return timestamp + static_cast<std::int64_t>(days) * 24 * 60 * 60;
}

int daysBetween(std::int64_t start, std::int64_t end) {
    if (end <= start) {
        return 0;
    }
    return static_cast<int>((end - start) / (24 * 60 * 60));
}

int readInt(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        cout << "Invalid input. Please enter a valid integer.\n";
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

string readLine(const string& prompt) {
    cout << prompt;
    string input;
    std::getline(cin, input);
    return input;
}
}  // namespace

// ----------------------- Book Class -----------------------
class Book {
private:
    int bookId{};
    string title;
    string author;
    string isbn;
    bool isAvailable{true};

public:
    Book() = default;

    Book(int id, string title, string author, string isbn)
        : bookId(id), title(std::move(title)), author(std::move(author)), isbn(std::move(isbn)) {}

    int getBookId() const { return bookId; }
    const string& getTitle() const { return title; }
    const string& getAuthor() const { return author; }
    const string& getIsbn() const { return isbn; }
    bool available() const { return isAvailable; }

    void setAvailability(bool available) { isAvailable = available; }

    void display() const {
        cout << "ID: " << bookId << " | Title: " << title << " | Author: " << author
             << " | ISBN: " << isbn << " | Status: " << (isAvailable ? "Available" : "Issued") << '\n';
    }

    void serialize(std::ofstream& out) const {
        writePrimitive(out, bookId);
        writeString(out, title);
        writeString(out, author);
        writeString(out, isbn);
        writePrimitive(out, isAvailable);
    }

    void deserialize(std::ifstream& in) {
        readPrimitive(in, bookId);
        readString(in, title);
        readString(in, author);
        readString(in, isbn);
        readPrimitive(in, isAvailable);
    }
};

// ----------------------- Member Class -----------------------
class Member {
private:
    int memberId{};
    string name;
    string contact;
    vector<int> borrowedBookIds;

public:
    Member() = default;

    Member(int id, string name, string contact)
        : memberId(id), name(std::move(name)), contact(std::move(contact)) {}

    int getMemberId() const { return memberId; }
    const string& getName() const { return name; }
    const string& getContact() const { return contact; }
    const vector<int>& getBorrowedBookIds() const { return borrowedBookIds; }

    void borrowBook(int bookId) { borrowedBookIds.push_back(bookId); }

    void returnBook(int bookId) {
        borrowedBookIds.erase(std::remove(borrowedBookIds.begin(), borrowedBookIds.end(), bookId),
                              borrowedBookIds.end());
    }

    bool hasBorrowedBook(int bookId) const {
        return std::find(borrowedBookIds.begin(), borrowedBookIds.end(), bookId) != borrowedBookIds.end();
    }

    void display() const {
        cout << "ID: " << memberId << " | Name: " << name << " | Contact: " << contact << "\n";
        cout << "Borrowed Books: ";
        if (borrowedBookIds.empty()) {
            cout << "None";
        } else {
            for (std::size_t i = 0; i < borrowedBookIds.size(); ++i) {
                cout << borrowedBookIds[i];
                if (i + 1 < borrowedBookIds.size()) {
                    cout << ", ";
                }
            }
        }
        cout << "\n";
    }

    void serialize(std::ofstream& out) const {
        writePrimitive(out, memberId);
        writeString(out, name);
        writeString(out, contact);

        std::size_t borrowedCount = borrowedBookIds.size();
        writePrimitive(out, borrowedCount);
        for (int id : borrowedBookIds) {
            writePrimitive(out, id);
        }
    }

    void deserialize(std::ifstream& in) {
        readPrimitive(in, memberId);
        readString(in, name);
        readString(in, contact);

        std::size_t borrowedCount = 0;
        readPrimitive(in, borrowedCount);
        borrowedBookIds.clear();
        borrowedBookIds.reserve(borrowedCount);

        for (std::size_t i = 0; i < borrowedCount; ++i) {
            int id;
            readPrimitive(in, id);
            borrowedBookIds.push_back(id);
        }
    }
};

// ----------------------- Transaction Class -----------------------
class Transaction {
private:
    int transactionId{};
    int bookId{};
    int memberId{};
    std::int64_t issueDate{};
    std::int64_t dueDate{};
    std::int64_t returnDate{};
    bool returned{false};
    double fineAmount{0.0};

public:
    Transaction() = default;

    Transaction(int transactionId, int bookId, int memberId, std::int64_t issueDate, std::int64_t dueDate)
        : transactionId(transactionId),
          bookId(bookId),
          memberId(memberId),
          issueDate(issueDate),
          dueDate(dueDate) {}

    int getTransactionId() const { return transactionId; }
    int getBookId() const { return bookId; }
    int getMemberId() const { return memberId; }
    std::int64_t getIssueDate() const { return issueDate; }
    std::int64_t getDueDate() const { return dueDate; }
    std::int64_t getReturnDate() const { return returnDate; }
    bool isReturned() const { return returned; }
    double getFineAmount() const { return fineAmount; }

    void markReturned(std::int64_t returnTimestamp, double fine) {
        returnDate = returnTimestamp;
        fineAmount = fine;
        returned = true;
    }

    void display() const {
        cout << "TxnID: " << transactionId << " | BookID: " << bookId << " | MemberID: " << memberId
             << " | Issue: " << formatDate(issueDate) << " | Due: " << formatDate(dueDate)
             << " | Returned: " << (returned ? "Yes" : "No");

        if (returned) {
            cout << " | Return Date: " << formatDate(returnDate) << " | Fine: Rs. " << std::fixed
                 << std::setprecision(2) << fineAmount;
        }
        cout << '\n';
    }

    void serialize(std::ofstream& out) const {
        writePrimitive(out, transactionId);
        writePrimitive(out, bookId);
        writePrimitive(out, memberId);
        writePrimitive(out, issueDate);
        writePrimitive(out, dueDate);
        writePrimitive(out, returnDate);
        writePrimitive(out, returned);
        writePrimitive(out, fineAmount);
    }

    void deserialize(std::ifstream& in) {
        readPrimitive(in, transactionId);
        readPrimitive(in, bookId);
        readPrimitive(in, memberId);
        readPrimitive(in, issueDate);
        readPrimitive(in, dueDate);
        readPrimitive(in, returnDate);
        readPrimitive(in, returned);
        readPrimitive(in, fineAmount);
    }
};

// ----------------------- Library Class -----------------------
class Library {
private:
    vector<Book> books;
    vector<Member> members;
    vector<Transaction> transactions;

    int nextBookId{1001};
    int nextMemberId{501};
    int nextTransactionId{1};

    Book* findBookById(int bookId) {
        for (auto& book : books) {
            if (book.getBookId() == bookId) {
                return &book;
            }
        }
        return nullptr;
    }

    Member* findMemberById(int memberId) {
        for (auto& member : members) {
            if (member.getMemberId() == memberId) {
                return &member;
            }
        }
        return nullptr;
    }

    Transaction* findActiveTransaction(int bookId, int memberId) {
        for (auto& transaction : transactions) {
            if (!transaction.isReturned() && transaction.getBookId() == bookId &&
                transaction.getMemberId() == memberId) {
                return &transaction;
            }
        }
        return nullptr;
    }

    void refreshCounters() {
        for (const auto& book : books) {
            nextBookId = std::max(nextBookId, book.getBookId() + 1);
        }
        for (const auto& member : members) {
            nextMemberId = std::max(nextMemberId, member.getMemberId() + 1);
        }
        for (const auto& transaction : transactions) {
            nextTransactionId = std::max(nextTransactionId, transaction.getTransactionId() + 1);
        }
    }

public:
    Library() { loadData(); }

    ~Library() { saveData(); }

    void addBook() {
        string title = readLine("Enter book title: ");
        string author = readLine("Enter author name: ");
        string isbn = readLine("Enter ISBN: ");

        if (title.empty() || author.empty() || isbn.empty()) {
            cout << "Book details cannot be empty.\n";
            return;
        }

        books.emplace_back(nextBookId++, title, author, isbn);
        cout << "Book added successfully! Book ID: " << books.back().getBookId() << "\n";
        saveData();
    }

    void registerMember() {
        string name = readLine("Enter member name: ");
        string contact = readLine("Enter contact details: ");

        if (name.empty() || contact.empty()) {
            cout << "Member details cannot be empty.\n";
            return;
        }

        members.emplace_back(nextMemberId++, name, contact);
        cout << "Member registered successfully! Member ID: " << members.back().getMemberId() << "\n";
        saveData();
    }

    void issueBook() {
        int bookId = readInt("Enter Book ID to issue: ");
        int memberId = readInt("Enter Member ID: ");

        Book* book = findBookById(bookId);
        if (!book) {
            cout << "Book not found.\n";
            return;
        }

        Member* member = findMemberById(memberId);
        if (!member) {
            cout << "Member not found.\n";
            return;
        }

        if (!book->available()) {
            cout << "Book is currently not available for issue.\n";
            return;
        }

        if (member->hasBorrowedBook(bookId)) {
            cout << "Member already has this book issued.\n";
            return;
        }

        std::int64_t issueDate = getCurrentTimestamp();
        std::int64_t dueDate = addDays(issueDate, LOAN_PERIOD_DAYS);

        book->setAvailability(false);
        member->borrowBook(bookId);
        transactions.emplace_back(nextTransactionId++, bookId, memberId, issueDate, dueDate);

        cout << "Book issued successfully!\n";
        cout << "Issue Date: " << formatDate(issueDate) << "\n";
        cout << "Due Date: " << formatDate(dueDate) << "\n";

        saveData();
    }

    void returnBook() {
        int bookId = readInt("Enter Book ID to return: ");
        int memberId = readInt("Enter Member ID: ");

        Book* book = findBookById(bookId);
        if (!book) {
            cout << "Book not found.\n";
            return;
        }

        Member* member = findMemberById(memberId);
        if (!member) {
            cout << "Member not found.\n";
            return;
        }

        if (!member->hasBorrowedBook(bookId)) {
            cout << "This member has not borrowed the specified book.\n";
            return;
        }

        Transaction* txn = findActiveTransaction(bookId, memberId);
        if (!txn) {
            cout << "No active transaction found for this book-member pair.\n";
            return;
        }

        std::int64_t returnDate = getCurrentTimestamp();
        int overdueDays = daysBetween(txn->getDueDate(), returnDate);
        double fine = overdueDays > 0 ? overdueDays * FINE_PER_DAY : 0.0;

        txn->markReturned(returnDate, fine);
        member->returnBook(bookId);
        book->setAvailability(true);

        cout << "Book returned successfully!\n";
        cout << "Return Date: " << formatDate(returnDate) << "\n";
        if (fine > 0.0) {
            cout << "Overdue by " << overdueDays << " day(s). Fine: Rs. " << std::fixed
                 << std::setprecision(2) << fine << "\n";
        } else {
            cout << "No fine applicable.\n";
        }

        saveData();
    }

    void searchBooks() const {
        string query = readLine("Enter title/author to search: ");
        if (query.empty()) {
            cout << "Search query cannot be empty.\n";
            return;
        }

        string loweredQuery = toLower(query);
        bool found = false;

        cout << "\n--- Search Results ---\n";
        for (const auto& book : books) {
            string titleLower = toLower(book.getTitle());
            string authorLower = toLower(book.getAuthor());

            if (titleLower.find(loweredQuery) != string::npos ||
                authorLower.find(loweredQuery) != string::npos) {
                book.display();
                found = true;
            }
        }

        if (!found) {
            cout << "No books matched your search.\n";
        }
    }

    void displayAllBooks() const {
        cout << "\n--- Library Books ---\n";
        if (books.empty()) {
            cout << "No books available in library.\n";
            return;
        }

        for (const auto& book : books) {
            book.display();
        }
    }

    void displayAllMembers() const {
        cout << "\n--- Library Members ---\n";
        if (members.empty()) {
            cout << "No members registered.\n";
            return;
        }

        for (const auto& member : members) {
            member.display();
            cout << "-------------------------\n";
        }
    }

    void displayTransactions() const {
        cout << "\n--- Transaction History ---\n";
        if (transactions.empty()) {
            cout << "No transactions available.\n";
            return;
        }

        for (const auto& transaction : transactions) {
            transaction.display();
        }
    }

    void deleteBook() {
        int bookId = readInt("Enter Book ID to delete: ");

        auto it = std::find_if(books.begin(), books.end(),
                               [bookId](const Book& book) { return book.getBookId() == bookId; });

        if (it == books.end()) {
            cout << "Book not found.\n";
            return;
        }

        if (!it->available()) {
            cout << "Cannot delete: Book is currently issued.\n";
            return;
        }

        books.erase(it);
        cout << "Book deleted successfully.\n";
        saveData();
    }

    void deleteMember() {
        int memberId = readInt("Enter Member ID to delete: ");

        auto it = std::find_if(members.begin(), members.end(),
                               [memberId](const Member& member) { return member.getMemberId() == memberId; });

        if (it == members.end()) {
            cout << "Member not found.\n";
            return;
        }

        if (!it->getBorrowedBookIds().empty()) {
            cout << "Cannot delete: Member has borrowed books.\n";
            return;
        }

        members.erase(it);
        cout << "Member deleted successfully.\n";
        saveData();
    }

    void loadData() {
        loadBooks();
        loadMembers();
        loadTransactions();
        refreshCounters();
    }

    void saveData() const {
        saveBooks();
        saveMembers();
        saveTransactions();
    }

    void run() {
        int choice;
        do {
            cout << "\n============================================\n";
            cout << "      Library Management System (C++)       \n";
            cout << "============================================\n";
            cout << "1. Add New Book\n";
            cout << "2. Register New Member\n";
            cout << "3. Issue Book\n";
            cout << "4. Return Book\n";
            cout << "5. Search Books\n";
            cout << "6. Display All Books\n";
            cout << "7. Display All Members\n";
            cout << "8. Delete Book\n";
            cout << "9. Delete Member\n";
            cout << "10. Display Transactions\n";
            cout << "0. Save & Exit\n";

            choice = readInt("Enter your choice: ");

            switch (choice) {
                case 1:
                    addBook();
                    break;
                case 2:
                    registerMember();
                    break;
                case 3:
                    issueBook();
                    break;
                case 4:
                    returnBook();
                    break;
                case 5:
                    searchBooks();
                    break;
                case 6:
                    displayAllBooks();
                    break;
                case 7:
                    displayAllMembers();
                    break;
                case 8:
                    deleteBook();
                    break;
                case 9:
                    deleteMember();
                    break;
                case 10:
                    displayTransactions();
                    break;
                case 0:
                    saveData();
                    cout << "Data saved successfully. Exiting program...\n";
                    break;
                default:
                    cout << "Invalid choice. Please select a valid menu option.\n";
            }
        } while (choice != 0);
    }

private:
    void saveBooks() const {
        std::ofstream out(BOOK_FILE, std::ios::binary | std::ios::trunc);
        if (!out) {
            cout << "Warning: Unable to save books data.\n";
            return;
        }

        std::size_t count = books.size();
        writePrimitive(out, count);
        for (const auto& book : books) {
            book.serialize(out);
        }
    }

    void loadBooks() {
        std::ifstream in(BOOK_FILE, std::ios::binary);
        if (!in) {
            return;  // First run: file may not exist
        }

        std::size_t count = 0;
        readPrimitive(in, count);

        books.clear();
        books.reserve(count);
        for (std::size_t i = 0; i < count; ++i) {
            Book book;
            book.deserialize(in);
            books.push_back(book);
        }
    }

    void saveMembers() const {
        std::ofstream out(MEMBER_FILE, std::ios::binary | std::ios::trunc);
        if (!out) {
            cout << "Warning: Unable to save members data.\n";
            return;
        }

        std::size_t count = members.size();
        writePrimitive(out, count);
        for (const auto& member : members) {
            member.serialize(out);
        }
    }

    void loadMembers() {
        std::ifstream in(MEMBER_FILE, std::ios::binary);
        if (!in) {
            return;
        }

        std::size_t count = 0;
        readPrimitive(in, count);

        members.clear();
        members.reserve(count);
        for (std::size_t i = 0; i < count; ++i) {
            Member member;
            member.deserialize(in);
            members.push_back(member);
        }
    }

    void saveTransactions() const {
        std::ofstream out(TRANSACTION_FILE, std::ios::binary | std::ios::trunc);
        if (!out) {
            cout << "Warning: Unable to save transactions data.\n";
            return;
        }

        std::size_t count = transactions.size();
        writePrimitive(out, count);
        for (const auto& transaction : transactions) {
            transaction.serialize(out);
        }
    }

    void loadTransactions() {
        std::ifstream in(TRANSACTION_FILE, std::ios::binary);
        if (!in) {
            return;
        }

        std::size_t count = 0;
        readPrimitive(in, count);

        transactions.clear();
        transactions.reserve(count);
        for (std::size_t i = 0; i < count; ++i) {
            Transaction transaction;
            transaction.deserialize(in);
            transactions.push_back(transaction);
        }
    }
};

int main() {
    cout << "Loading Library Management System...\n";
    Library library;
    library.run();
    return 0;
}
