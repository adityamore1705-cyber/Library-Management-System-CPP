# 📚 Library Management System in C++

A complete and professional **Library Management System** built using **Object-Oriented Programming (OOP)** in C++. This project is designed for academic and internship submission purposes, with clean architecture, strong modularity, and persistent file storage using binary files.

It allows librarians to manage books, members, and issue/return transactions efficiently through a robust menu-driven command-line interface.

---

### ✨ Key Features

- ➕ Add new books with details (Title, Author, ISBN)
- 👤 Register new library members
- 📖 Issue books to members with automatic due-date tracking
- 🔁 Return books with overdue fine calculation
- 🔎 Search books by title or author (case-insensitive)
- 📚 Display all books with real-time availability status
- 🧾 Display all members and their borrowed books
- 🗑️ Delete books and members with safety checks
- 💾 Persistent storage via binary files:
  - `books.dat`
  - `members.dat`
  - `transactions.dat`
- 🚀 Auto load on startup and save on exit

---

### 🛠️ Tech Stack

| Category | Technology |
|----------|------------|
| Language | C++17 |
| Paradigm | Object-Oriented Programming |
| Storage | Binary File Handling (`fstream`) |
| Interface | Console-based Menu System |

---

### 📁 Project Structure

```bash
Library-Management-System-CPP/
├── src/
│   └── library.cpp
├── .gitignore
├── LICENSE
└── README.md
```

---

### ⚙️ Compilation & Running Instructions

From the root project directory:

```bash
g++ -std=c++17 -O2 -Wall -Wextra -pedantic src/library.cpp -o library
./library
```

---

### 🧪 Sample Output

```text
Loading Library Management System...

============================================
      Library Management System (C++)
============================================
1. Add New Book
2. Register New Member
3. Issue Book
4. Return Book
5. Search Books
6. Display All Books
7. Display All Members
8. Delete Book
9. Delete Member
10. Display Transactions
0. Save & Exit
Enter your choice: 1
Enter book title: Clean Code
Enter author name: Robert C. Martin
Enter ISBN: 9780132350884
Book added successfully! Book ID: 1001
```

---

### 🧠 OOP Concepts Demonstrated

| OOP Concept | How It Is Used |
|-------------|----------------|
| Encapsulation | `Book`, `Member`, `Transaction`, and `Library` classes encapsulate data and behavior |
| Abstraction | High-level library operations exposed through class methods |
| Modularity | Separate methods for issuing, returning, searching, deleting, and persistence |
| Data Integrity | Controlled access via getters/setters and validation checks |
| Reusability | Generic helper functions for serialization and input handling |

---

### 🔮 Future Enhancements

- Admin authentication and role-based access
- ISBN uniqueness validation and duplicate checks
- Fine payment tracking and receipts
- Export reports (CSV/PDF)
- GUI version using Qt / web-based front-end
- Unit testing with Catch2 / GoogleTest

---

### 👨‍💻 Author

**Aditya More**  
GitHub: [@adityamore1705-cyber](https://github.com/adityamore1705-cyber)

---

### 📄 License

This project is licensed under the **MIT License**. See the [LICENSE](LICENSE) file for details.
