#include <iostream>
#include <string>
#include <conio.h>
#include <cctype>
#include <ctime>
#include <fstream>
#include <vector>
#include <windows.h>

using namespace std;

class Password;
class User;
class FileHandler;
class Modify;
class UserPanel;
class LoginSystem;

class Password
{
    int EncryptedPassword[100];
    int PasswordLength;

    bool Uppercase_check(char ch)
    {
        return ch >= 'A' && ch <= 'Z';
    }

    bool Lowercase_check(char ch)
    {
        return ch >= 'a' && ch <= 'z';
    }

    bool Digit_check(char ch)
    {
        return ch >= '0' && ch <= '9';
    }

    bool Special_check(char ch)
    {
        return !Uppercase_check(ch) && !Lowercase_check(ch) && !Digit_check(ch);
    }

public:
    Password()
    {
        PasswordLength = 0;

        for (int i = 0; i < 100; i++)
        {
            EncryptedPassword[i] = 0;
        }
    }

    string PasswordInput()
    {
        string password;
        char ch;

        while (true)
        {
            ch = getch();

            if (ch == 13) // Enter key
            {
                break;
            }
            else if (ch == 8) // Backspace key
            {
                if (!password.empty())
                {
                    password.pop_back();
                    cout << "\b \b";
                }
            }
            else
            {
                password += ch;
                cout << "*";
            }
        }

        cout << endl;
        return password;
    }

    bool ValidPasswordVerification(const string &Password)
    {
        if (Password.length() < 8)
            return false;

        bool hasUpper = false;
        bool hasLower = false;
        bool hasDigit = false;
        bool hasSpecial = false;

        for (char ch : Password)
        {
            if (Uppercase_check(ch))
            {
                hasUpper = true;
            }
            else if (Lowercase_check(ch))
            {
                hasLower = true;
            }
            else if (Digit_check(ch))
            {
                hasDigit = true;
            }
            else if (Special_check(ch))
            {
                hasSpecial = true;
            }
        }

        return hasUpper && hasLower && hasDigit && hasSpecial;
    }

    bool PasswordEncrypter(const string &Password)
    {
        if (!ValidPasswordVerification(Password))
        {
            return false;
        }

        PasswordLength = Password.length();
        int i;

        for (i = 0; i < PasswordLength; i++)
        {
            EncryptedPassword[i] = ((Password[i] + 1015) / 2) + 4;
        }

        return true;
    }

    bool VerifyPassword(const string &Password)
    {
        int i;
        int encrypted;

        if (Password.length() != PasswordLength)
        {
            return false;
        }

        for (i = 0; i < PasswordLength; i++)
        {
            encrypted = ((Password[i] + 1015) / 2) + 4;

            if (encrypted != EncryptedPassword[i])
            {
                return false;
            }
        }

        return true;
    }

    string getEncryptedString() const
    {
        string PasswordString = "";

        for (int i = 0; i < PasswordLength; i++)
        {
            PasswordString += to_string(EncryptedPassword[i]) + " ";
        }
        return PasswordString;
    }

    void LoadPasswordString(const string &EncryptedString)
    {
        PasswordLength = 0;
        string temp = "";

        for (char ch : EncryptedString)
        {
            if (ch == ' ')
            {
                if (!temp.empty())
                {
                    EncryptedPassword[PasswordLength] = stoi(temp);
                    PasswordLength++;
                    temp = "";
                }
            }
            else
            {
                temp += ch;
            }
        }

        if (!temp.empty() && PasswordLength < 100)
        {
            EncryptedPassword[PasswordLength] = stoi(temp);
            PasswordLength++;
        }
    }

    int PasswordLengthGet() const
    {
        return PasswordLength;
    }
};

class User
{
    string Username;
    string SecretCode;
    string CreationDate;
    Password UserPassword;
    string EncryptedPasswordData;

public:
    User() // Default Constructor
    {
        Username = "NO DATA";
        SecretCode = "NO DATA";
        CreationDate = "NO DATA";
        EncryptedPasswordData = "NO DATA";
    }

    User(const string &uname, const string &pass, const string &code)
    {
        SetUsername(uname);
        SetPassword(pass);
        SetSecretCode(code);
        SetCreationDate();
    }

    void SetUsername(const string &uname)
    {
        Username = uname;
    }

    void SetPassword(const string &pass)
    {
        if (UserPassword.PasswordEncrypter(pass))
        {
            EncryptedPasswordData = UserPassword.getEncryptedString();
        }
    }

    void SetSecretCode(const string &code)
    {
        SecretCode = code;
    }

    void SetCreationDate()
    {
        time_t now = time(0);
        CreationDate = ctime(&now);
        CreationDate.pop_back();
    }

    string GetUsername() const
    {
        return Username;
    }

    string GetEncryptedPasswordData() const
    {
        return EncryptedPasswordData;
    }

    string GetSecretCode() const
    {
        return SecretCode;
    }

    string GetCreationDate() const
    {
        return CreationDate;
    }

    bool verifyPassword(const string &inputPassword)
    {
        if (UserPassword.PasswordLengthGet() == 0)
        {
            UserPassword.LoadPasswordString(EncryptedPasswordData);
        }
        return UserPassword.VerifyPassword(inputPassword);
    }

    bool verifySecretCode(const string &inputCode) const
    {
        return SecretCode == inputCode;
    }

    string toString() const
    {
        return Username + "|" + EncryptedPasswordData + "|" + SecretCode + "|" + CreationDate;
    }

    void fromString(const string &data)
    {
        size_t pos1 = data.find("|");
        size_t pos2 = data.find("|", pos1 + 1);
        size_t pos3 = data.find("|", pos2 + 1);

        if (pos1 != string::npos && pos2 != string::npos && pos3 != string::npos)
        {
            Username = data.substr(0, pos1);
            EncryptedPasswordData = data.substr(pos1 + 1, pos2 - pos1 - 1);
            SecretCode = data.substr(pos2 + 1, pos3 - pos2 - 1);
            CreationDate = data.substr(pos3 + 1);
        }
    }
};

class FileHandler
{
private:
    static const string FILENAME;

    static void writeString(ofstream &file, const string &str)
    {
        int len = str.length();
        file.write(reinterpret_cast<const char *>(&len), sizeof(len));
        file.write(str.c_str(), len);
    }

    static void readString(ifstream &file, string &str)
    {
        int len;
        file.read(reinterpret_cast<char *>(&len), sizeof(len));
        str.resize(len);
        file.read(&str[0], len);
    }

public:
    static bool saveUser(const User &user)
    {
        ofstream file(FILENAME, ios::binary | ios::app);
        if (!file)
        {
            cout << "\n❌ Error: Unable to open file for writing!\n";
            return false;
        }
        getch();
        system("cls");

        writeString(file, user.GetUsername());
        writeString(file, user.GetEncryptedPasswordData());
        writeString(file, user.GetSecretCode());
        writeString(file, user.GetCreationDate());

        file.close();
        return true;
    }

    static bool findUser(const string &username, User &foundUser)
    {
        ifstream file(FILENAME, ios::binary);
        if (!file)
            return false;

        while (file.peek() != EOF)
        {
            string uname, epass, code, date;
            readString(file, uname);
            readString(file, epass);
            readString(file, code);
            readString(file, date);

            if (uname == username)
            {
                string full = uname + "|" + epass + "|" + code + "|" + date;
                foundUser.fromString(full);
                file.close();
                return true;
            }
        }

        file.close();
        return false;
    }

    static bool updateUser(const string &username, const User &updatedUser)
    {
        ifstream file(FILENAME, ios::binary);
        if (!file)
            return false;

        vector<User> users;
        bool found = false;

        while (file.peek() != EOF)
        {
            string uname, epass, code, date;
            readString(file, uname);
            readString(file, epass);
            readString(file, code);
            readString(file, date);

            string full = uname + "|" + epass + "|" + code + "|" + date;
            User temp;
            temp.fromString(full);

            if (uname == username)
            {
                users.push_back(updatedUser);
                found = true;
            }
            else
            {
                users.push_back(temp);
            }
        }

        file.close();

        if (found)
        {
            ofstream outFile(FILENAME, ios::binary | ios::trunc);
            for (const User &user : users)
            {
                writeString(outFile, user.GetUsername());
                writeString(outFile, user.GetEncryptedPasswordData());
                writeString(outFile, user.GetSecretCode());
                writeString(outFile, user.GetCreationDate());
            }
            outFile.close();
        }

        return found;
    }

    static bool usernameExists(const string &username)
    {
        User temp;
        return findUser(username, temp);
    }
};

const string FileHandler::FILENAME = "users.dat";

class Modify
{
public:
    static void modifyMenu(User &user)
    {
        while (true)
        {
            cout << "\n===== MODIFY MENU =====\n";
            cout << "1.  Change Password\n";
            cout << "2.  Change Username\n";
            cout << "3.  Change Secret Code\n";
            cout << "4.  Back to User Panel\n";
            cout << "Enter your choice: ";

            string choice;
            getline(cin, choice);

            system("cls");

            if (choice == "1")
            {
                changePassword(user);
            }
            else if (choice == "2")
            {
                changeUsername(user);
            }
            else if (choice == "3")
            {
                changeSecretCode(user);
            }
            else if (choice == "4")
            {
                break;
            }
            else
            {
                cout << " Invalid choice. Try again.\n";
            }
        }
    }

    static void changePassword(User &user)
    {
        Password NewPass;
        string password;

        cout << "Enter New Password: \n";
        cout << "Instruction For Setting Up The Password\n";
        cout << "Minimum 8 Character \n";
        cout << "1 Special Character \n";
        cout << "1 Digit Character \n";
        cout << "1 UpperCase Character \n";
        cout << "1 LowerCase Character \n";
        password = NewPass.PasswordInput();

        if (!NewPass.ValidPasswordVerification(password))
        {
            cout << "Password does not meet requirements!\n";
            return;
        }

        user.SetPassword(password);

        if (FileHandler::updateUser(user.GetUsername(), user))
        {
            cout << "Password successfully updated.\n";
        }
        else
        {
            cout << "Failed to update password.\n";
        }
    }

    static void changeUsername(User &user)
    {
        string NewUsername;
        string OldUsername;

        cout << "Enter New Username: ";
        getline(cin, NewUsername);

        if (NewUsername.empty())
        {
            cout << "Username Can't Be Empty !\n";
            return;
        }

        if (FileHandler::usernameExists(NewUsername))
        {
            cout << "Username Already Taken, Please Try Another";
            return;
        }

        OldUsername = user.GetUsername();
        user.SetUsername(NewUsername);

        if (FileHandler::updateUser(OldUsername, user))
        {
            cout << " Username successfully updated.\n";
        }
        else
        {
            cout << " Failed to update username in file.\n";
        }
    }

    static void changeSecretCode(User &user)
    {
        string NewCode;

        cout << "Enter New Secret Code : ";
        getline(cin, NewCode);

        if (NewCode.empty())
        {
            cout << "Secret Code Can't Be Empty\n";
            return;
        }

        user.SetSecretCode(NewCode);

        if (FileHandler::updateUser(user.GetUsername(), user))
        {
            cout << " Secret code successfully updated.\n";
        }
        else
        {
            cout << " Failed to update secret code.\n";
        }
    }
};

class UserPanel
{
public:
    static void show(User &user)
    {
        while (true)
        {
            cout << "\n================ USER PANEL ================\n";
            cout << "Welcome, " << user.GetUsername() << "!\n";
            cout << "1. Account Details\n";
            cout << "2. Modification (Password Required)\n";
            cout << "3. Sign Out\n";
            cout << "============================================\n";
            cout << "Enter your choice: ";

            int choice;
            cin >> choice;
            cin.ignore(); // clear newline from buffer

            if (choice == 1)
            {
                cout << "\n------ Account Details ------\n";
                cout << "Username      : " << user.GetUsername() << endl;
                cout << "Password      : " << "****************" << endl;
                cout << "Secret Code   : " << user.GetSecretCode() << endl;
                cout << "Creation Date : " << user.GetCreationDate() << endl;
            }
            else if (choice == 2)
            {
                Password tempPass;
                string input;

                cout << "Enter your password to continue: ";
                input = tempPass.PasswordInput();

                if (user.verifyPassword(input))
                {
                    cout << "\n Password verified.\n";
                    Modify::modifyMenu(user);
                }
                else
                {
                    cout << "\nIncorrect password. Access denied to modification panel.\n";
                }
            }
            else if (choice == 3)
            {
                cout << "\n Signed out successfully.\n";
                break;
            }
            else
            {
                cout << "\n Invalid choice. Try again.\n";
            }
        }
    }
};

class LoginSystem
{
    bool LoggedIn;

public:
    void MainMenu()
    {
        while (true)
        {
            cout << "\n=========== LOGIN SYSTEM MENU ===========\n";
            cout << "1. Sign Up\n";
            cout << "2. Login\n";
            cout << "3. Exit\n";
            cout << "=========================================\n";
            cout << "Enter your choice: ";

            int choice;
            cin >> choice;
            cin.ignore(); // clear newline from buffer
            // getch();
            system("cls");

            switch (choice)
            {
            case 1:
                signUp();
                break;
            case 2:
                login();
                break;
            case 3:
                cout << "\n Exiting the program. Goodbye!\n";
                system("cls");
                return;
            default:
                cout << "\n Invalid choice. Try again.\n";
                system("cls");
                break;
            }
        }
    }

private:
    void signUp()
    {
        string Username;
        string SecretCode;
        string UserPassword;

        cout << "SIGN UP \n";
        cout << "Enter a Username: ";
        getline(cin, Username);

        if (FileHandler::usernameExists(Username))
        {
            cout << "\n Username already exists. Please try another.\n";
            return;
        }

        Password passObj;

        cout << "Create Password \n ";
        cout << "Instruction For Setting Up The Password\n";
        cout << "Minimum 8 Character \n";
        cout << "1 Special Character \n";
        cout << "1 Digit Character \n";
        cout << "1 UpperCase Character \n";
        cout << "1 LowerCase Character \n";

        UserPassword = passObj.PasswordInput();

        if (!passObj.ValidPasswordVerification(UserPassword))
        {
            cout << "\n Password doesn't meet requirements.\n";
            system("cls");
        }
        else
        {
            cout << "Enter a Secret Code (used for recovery): ";
            getline(cin, SecretCode);
            system("cls");

            User newUser(Username, UserPassword, SecretCode);
            if (FileHandler::saveUser(newUser))
            {
                cout << "\n Sign Up successfully!\n";
                system("cls");
                LoggedIn = true;
            }
            else
            {
                cout << "\n Failed to Sign Up. Please Try Again\n";
                system("cls");
            }
        }
    }

    void login()
    {
        string username;
        string password;
        bool authenticated = false;
        int attempt;
        string code;

        system("cls");
        cout << "\nLOGIN\n";
        cout << "Enter Username: ";
        getline(cin, username);

        User user;

        if (!FileHandler::findUser(username, user))
        {
            cout << "\n User Not Found. Please Try Again.\n";
            system("cls");
            return;
        }

        Password passObj;

        for (attempt = 1; attempt <= 3; attempt++)
        {
            cout << "Enter Password (Attempt " << attempt << "): ";
            password = passObj.PasswordInput();

            if (user.verifyPassword(password))
            {
                system("cls");
                cout << "\n Login Successful! Welcome, " << user.GetUsername() << "\n";
                authenticated = true;
                LoggedIn = true;
                UserPanel::show(user);
                break;
            }
            else
            {
                cout << " Incorrect Password. Please Try Again.\n";
            }
        }

        if (!authenticated)
        {
            cout << "\n All Password Attempts Exhausted.\n";
            cout << "Enter your Secret Code for Recovery: ";
            getline(cin, code);

            if (user.verifySecretCode(code))
            {
                system("cls");
                cout << " Correct Secret Code.\n";

                while (true)
                {
                    system("cls");
                    cout << "Choose An Option\n";
                    cout << "1.  Login to Account" << endl;
                    cout << "2.  Change Password" << endl;
                    cout << "Enter choice (1-2): ";

                    int choice;
                    cin >> choice;
                    cin.ignore(); // clear newline from buffer
                    system("cls");

                    if (choice == 1)
                    {
                        cout << "\n Login successful!" << endl;
                        LoggedIn = true;
                        UserPanel::show(user);
                        break;
                    }
                    else if (choice == 2)
                    {
                        Modify::changePassword(user);
                        return;
                    }
                    else
                    {
                        cout << "❌ Invalid choice! Please enter 1 or 2." << endl;
                    }
                }
            }
            else
            {
                cout << " Incorrect Secret Code. Returning to Main Menu...\n";
            }
        }
    }
};

int main()
{
    LoginSystem System;
    System.LoginSystem::MainMenu();
    return 0;
}