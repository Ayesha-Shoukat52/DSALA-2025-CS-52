#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <sstream>
#include <iomanip>

using namespace std;

// Bit flags for constraints
const unsigned int PRIMARY_KEY = 1;  // 001
const unsigned int NOT_NULL = 2;     // 010
const unsigned int UNIQUE = 4;        // 100

class Column {
public:
    string name;
    string type;
    unsigned int constraints;

    Column(string n, string t, unsigned int c = 0) : name(n), type(t), constraints(c) {}

    bool hasConstraint(unsigned int flag) const {
        return (constraints & flag) != 0;
    }

    string constraintsToString() const {
        if (constraints == 0) return "0";
        string result;
        if (constraints & PRIMARY_KEY) result += "PRIMARY KEY ";
        if (constraints & NOT_NULL) result += "NOT NULL ";
        if (constraints & UNIQUE) result += "UNIQUE";
        return result;
    }
};

class Row {
public:
    vector<string> values;

    Row() {}
    Row(const vector<string>& vals) : values(vals) {}

    string getValue(int index) const {
        if (index >= 0 && index < values.size()) {
            return values[index];
        }
        return "";
    }
};

class Table {
private:
    string tableName;
    vector<Column> columns;
    vector<Row*> rows;

    bool validateConstraints(const vector<string>& values) const {
        // Check NOT NULL constraints
        for (size_t i = 0; i < columns.size(); i++) {
            if (columns[i].hasConstraint(NOT_NULL) && values[i].empty()) {
                cout << "Error: Column " << columns[i].name << " cannot be NULL" << endl;
                return false;
            }
        }

        // Check UNIQUE constraints
        for (size_t i = 0; i < columns.size(); i++) {
            if (columns[i].hasConstraint(UNIQUE)) {
                for (const auto& row : rows) {
                    if (row->values[i] == values[i]) {
                        cout << "Error: Duplicate value in UNIQUE column " << columns[i].name << endl;
                        return false;
                    }
                }
            }
        }

        // Check PRIMARY KEY (combination of UNIQUE and NOT NULL)
        for (size_t i = 0; i < columns.size(); i++) {
            if (columns[i].hasConstraint(PRIMARY_KEY)) {
                if (values[i].empty()) {
                    cout << "Error: Primary key column " << columns[i].name << " cannot be NULL" << endl;
                    return false;
                }
                for (const auto& row : rows) {
                    if (row->values[i] == values[i]) {
                        cout << "Error: Duplicate primary key value in column " << columns[i].name << endl;
                        return false;
                    }
                }
            }
        }

        return true;
    }

public:
    Table() {}
    Table(const string& name) : tableName(name) {}

    ~Table() {
        for (auto row : rows) {
            delete row;
        }
        rows.clear();
    }

    string getName() const { return tableName; }
    vector<Column>& getColumns() { return columns; }
    const vector<Column>& getColumns() const { return columns; }
    vector<Row*>& getRows() { return rows; }
    const vector<Row*>& getRows() const { return rows; }

    void addColumn(const string& name, const string& type, unsigned int constraints = 0) {
        columns.push_back(Column(name, type, constraints));
        cout << "Column " << name << " added successfully." << endl;
    }

    bool insertRow(const vector<string>& values) {
        if (values.size() != columns.size()) {
            cout << "Error: Column count mismatch. Expected " << columns.size() 
                 << ", got " << values.size() << endl;
            return false;
        }

        // Validate data types
        for (size_t i = 0; i < columns.size(); i++) {
            if (columns[i].type == "int") {
                for (char c : values[i]) {
                    if (!isdigit(c) && c != '-') {
                        cout << "Error: Invalid integer value for column " << columns[i].name << endl;
                        return false;
                    }
                }
            }
        }

        if (!validateConstraints(values)) {
            return false;
        }

        Row* newRow = new Row(values);
        rows.push_back(newRow);
        return true;
    }

    void selectAll() const {
        if (rows.empty()) {
            cout << "No records found." << endl;
            return;
        }

        // Print header
        for (const auto& col : columns) {
            cout << left << setw(15) << col.name;
        }
        cout << endl;

        // Print separator
        for (size_t i = 0; i < columns.size(); i++) {
            cout << string(15, '-');
        }
        cout << endl;

        // Print rows
        for (const auto& row : rows) {
            for (const auto& val : row->values) {
                cout << left << setw(15) << val;
            }
            cout << endl;
        }
    }

    void saveToFile(ofstream& file) const {
        file << "TABLE " << tableName << endl;
        
        // Write columns
        for (const auto& col : columns) {
            file << col.name << " " << col.type << " " << col.constraints << endl;
        }
        
        file << "DATA" << endl;
        
        // Write data
        for (const auto& row : rows) {
            for (size_t i = 0; i < row->values.size(); i++) {
                file << row->values[i];
                if (i < row->values.size() - 1) file << " ";
            }
            file << endl;
        }
        file << "END" << endl;
    }

    bool loadFromFile(ifstream& file) {
        string line;
        
        // Read columns
        while (getline(file, line) && line != "DATA") {
            if (line.empty()) continue;
            
            istringstream iss(line);
            string colName, colType;
            unsigned int constraints;
            
            iss >> colName >> colType >> constraints;
            addColumn(colName, colType, constraints);
        }
        
        // Read data
        while (getline(file, line) && line != "END") {
            if (line.empty()) continue;
            
            istringstream iss(line);
            vector<string> values;
            string value;
            
            while (iss >> value) {
                values.push_back(value);
            }
            
            if (!values.empty()) {
                insertRow(values);
            }
        }
        
        return true;
    }
};

class DatabaseEngine {
private:
    vector<Table*> tables;
    Table* currentTable;
    char buffer[256];  // Fixed-size array for query parsing

    vector<string> parseCommand(const string& command) {
        vector<string> tokens;
        istringstream iss(command);
        string token;
        
        while (iss >> token) {
            tokens.push_back(token);
        }
        
        return tokens;
    }

    vector<string> parseValues(const string& valuesStr) {
        vector<string> values;
        string current;
        bool inQuotes = false;
        
        for (char c : valuesStr) {
            if (c == '\'') {
                inQuotes = !inQuotes;
            } else if (c == ',' && !inQuotes) {
                if (!current.empty()) {
                    values.push_back(current);
                    current.clear();
                }
            } else {
                current += c;
            }
        }
        
        if (!current.empty()) {
            values.push_back(current);
        }
        
        return values;
    }

public:
    DatabaseEngine() : currentTable(nullptr) {}

    ~DatabaseEngine() {
        for (auto table : tables) {
            delete table;
        }
        tables.clear();
    }

    void executeCommand(const string& command) {
        vector<string> tokens = parseCommand(command);
        
        if (tokens.empty()) return;
        
        if (tokens[0] == "CREATE" && tokens[1] == "TABLE") {
            createTable(tokens[2]);
        }
        else if (tokens[0] == "INSERT" && tokens[1] == "INTO") {
            insertInto(tokens);
        }
        else if (tokens[0] == "SELECT") {
            if (tokens[1] == "*" && tokens[2] == "FROM") {
                selectFrom(tokens[3]);
            }
        }
        else if (tokens[0] == "SAVE" && tokens[1] == "TO" && tokens[2] == "FILE") {
            saveToFile("database.txt");
        }
        else if (tokens[0] == "LOAD" && tokens[1] == "FROM" && tokens[2] == "FILE") {
            loadFromFile("database.txt");
        }
        else if (tokens[0] == "ADD" && tokens[1] == "COLUMN") {
            addColumn(tokens);
        }
        else {
            cout << "Unknown command!" << endl;
        }
    }

    void createTable(const string& tableName) {
        Table* newTable = new Table(tableName);
        tables.push_back(newTable);
        currentTable = newTable;
        cout << "Table " << tableName << " created successfully." << endl;
    }

    void addColumn(const vector<string>& tokens) {
        if (!currentTable) {
            cout << "No table selected. Create a table first." << endl;
            return;
        }
        
        string colName = tokens[2];
        string colType = tokens[3];
        unsigned int constraints = 0;
        
        if (tokens.size() > 4) {
            for (size_t i = 4; i < tokens.size(); i++) {
                if (tokens[i] == "PRIMARY") constraints |= PRIMARY_KEY;
                else if (tokens[i] == "NOTNULL") constraints |= NOT_NULL;
                else if (tokens[i] == "UNIQUE") constraints |= UNIQUE;
            }
        }
        
        currentTable->addColumn(colName, colType, constraints);
    }

    void insertInto(const vector<string>& tokens) {
        if (!currentTable) {
            cout << "No table selected. Create a table first." << endl;
            return;
        }
        
        // Find VALUES keyword
        size_t valuesPos = 0;
        for (size_t i = 0; i < tokens.size(); i++) {
            if (tokens[i] == "VALUES") {
                valuesPos = i;
                break;
            }
        }
        
        if (valuesPos == 0) {
            cout << "Invalid INSERT syntax" << endl;
            return;
        }
        
        // Parse values
        string valuesStr;
        for (size_t i = valuesPos + 1; i < tokens.size(); i++) {
            valuesStr += tokens[i];
            if (i < tokens.size() - 1) valuesStr += " ";
        }
        
        // Remove parentheses
        if (valuesStr.front() == '(') valuesStr = valuesStr.substr(1);
        if (valuesStr.back() == ')') valuesStr.pop_back();
        
        vector<string> values = parseValues(valuesStr);
        
        if (currentTable->insertRow(values)) {
            cout << "Record inserted." << endl;
        }
    }

    void selectFrom(const string& tableName) {
        Table* table = nullptr;
        for (auto t : tables) {
            if (t->getName() == tableName) {
                table = t;
                break;
            }
        }
        
        if (!table) {
            cout << "Table not found!" << endl;
            return;
        }
        
        table->selectAll();
    }

    void saveToFile(const string& filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            cout << "Error opening file for writing!" << endl;
            return;
        }
        
        for (const auto& table : tables) {
            table->saveToFile(file);
        }
        
        file.close();
        cout << "Database saved to " << filename << endl;
    }

    void loadFromFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Error opening file for reading!" << endl;
            return;
        }
        
        // Clear existing tables
        for (auto table : tables) {
            delete table;
        }
        tables.clear();
        
        string line;
        while (getline(file, line)) {
            if (line.substr(0, 6) == "TABLE ") {
                string tableName = line.substr(6);
                Table* newTable = new Table(tableName);
                if (newTable->loadFromFile(file)) {
                    tables.push_back(newTable);
                    cout << "Table " << tableName << " loaded successfully." << endl;
                } else {
                    delete newTable;
                }
            }
        }
        
        file.close();
        cout << "Database loaded from " << filename << endl;
    }

    void run() {
        cout << "Mini Database Engine v1.0" << endl;
        cout << "Available commands:" << endl;
        cout << "  CREATE TABLE <name>" << endl;
        cout << "  ADD COLUMN <name> <type> [PRIMARY] [NOTNULL] [UNIQUE]" << endl;
        cout << "  INSERT INTO <table> VALUES (val1, val2, ...)" << endl;
        cout << "  SELECT * FROM <table>" << endl;
        cout << "  SAVE TO FILE" << endl;
        cout << "  LOAD FROM FILE" << endl;
        cout << "  EXIT" << endl;
        cout << string(50, '=') << endl;
        
        while (true) {
            cout << "> ";
            cin.getline(buffer, 256);
            
            if (strcmp(buffer, "EXIT") == 0) {
                break;
            }
            
            executeCommand(string(buffer));
        }
    }
};

int main() {
    DatabaseEngine db;
    db.run();
    return 0;
}