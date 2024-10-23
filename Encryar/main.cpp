#include <iostream>
#include <fstream>
#include <string>
#include <random>

using namespace std;

// Function to generate a random salt
string generateSalt(int length) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    string salt;
    random_device rd;
    mt19937 generator(rd());
    uniform_int_distribution<> distribution(0, sizeof(charset) - 2); // -2 to avoid null terminator

    for (int i = 0; i < length; ++i) {
        salt += charset[distribution(generator)];
    }

    return salt;
}

// Function to derive a stronger key from the original key and a salt
string deriveKey(const string& key, const string& salt) {
    string derivedKey;
    for (size_t i = 0; i < key.length(); ++i) {
        // XOR the original key character with the salt character
        derivedKey += key[i] ^ salt[i % salt.length()];
    }
    return derivedKey;
}

// Function to encrypt/decrypt the text using XOR and a derived key
string xorCipher(const string& text, const string& key) {
    string output = text;
    int keyLength = key.length();

    for (size_t i = 0; i < text.length(); ++i) {
        output[i] = text[i] ^ key[i % keyLength]; // XOR operation
    }

    return output;
}

// Function to read the entire file content (including special characters)
string readFile(const string& filename) {
    ifstream file(filename, ios::in | ios::binary);  // Open file in binary mode
    if (!file.is_open()) {
        cerr << "Error: Cannot open file " << filename << endl;
        exit(EXIT_FAILURE);
    }
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();
    return content;
}

// Function to write encrypted/decrypted content to a file (in binary mode to ensure no character corruption)
void writeFile(const string& filename, const string& content) {
    ofstream file(filename, ios::out | ios::binary);  // Write file in binary mode
    if (!file.is_open()) {
        cerr << "Error: Cannot write to file " << filename << endl;
        exit(EXIT_FAILURE);
    }
    file << content;
    file.close();
}

// Function to compress the data using run-length encoding (RLE)
string compressRLE(const string& input) {
    string output;
    for (size_t i = 0; i < input.length(); ++i) {
        char currentChar = input[i];
        int count = 1;
        while (i + 1 < input.length() && input[i + 1] == currentChar) {
            count++;
            i++;
        }
        output += currentChar;
        output += count + '0'; // Store count as character (assumes count < 10 for simplicity)
    }
    return output;
}

// Function to decompress the data using run-length encoding (RLE)
string decompressRLE(const string& input) {
    string output;
    for (size_t i = 0; i < input.length(); i += 2) {
        char currentChar = input[i];
        int count = input[i + 1] - '0'; // Convert character count back to integer
        output.append(count, currentChar); // Append currentChar 'count' times
    }
    return output;
}

int main() {
    string operation, filename, key, salt;
    bool firstRun = true;  // Flag to track the first run

    while (true) {
        if (firstRun) {
            // Display the program instructions only on the first run
            cout << "======= Encryar v1.2 - Enhanced Secure XOR Encryption/Decryption Program =======" << endl;
            cout << "\n[1] Encrypting a text file with a private key" << endl;
            cout << "[2] Decrypting a text file with a private key" << endl;
            cout << "\nDeveloper: Abolfazl Najafi, Contact: abolfazlbox@gmail.com" << endl;
            cout << "------------------------------------------------------------" << endl;
            firstRun = false;  // Set the flag to false after the first display
        }

        // Keep asking for a valid option until the user provides '1' or '2'
        while (true) {
            cout << "> Select an option (1-2): ";
            cin >> operation;

            if (operation == "1" || operation == "2") {
                break;  // Valid input, exit the loop
            } else {
                cerr << "Error: Invalid option. Please select a valid option (1 or 2).\n" << endl;
                // Ignore invalid input and try again
                cin.clear();               // Clear the error flag on cin
                cin.ignore(10000, '\n');    // Discard the invalid input
            }
        }

        // Clear input buffer to handle getline after cin
        cin.ignore();

        // Ask for the filename
        cout << "> Enter the filename: ";
        getline(cin, filename);

        // Ask for the key
        cout << "> Enter a private key (at least 8 characters): ";
        getline(cin, key);

        // Ensure the key is at least 8 characters long
        if (key.length() < 8) {
            cerr << "Error: Key must be at least 8 characters long." << endl;
            continue;
        }

        // Generate a random salt
        salt = generateSalt(16);  // Salt length can be adjusted

        // Derive a stronger key using the original key and salt
        string derivedKey = deriveKey(key, salt);

        // Read the input file
        string inputText = readFile(filename);
        string outputText;

        if (operation == "1") { // Encryption
            outputText = xorCipher(inputText, derivedKey);
            string compressedOutput = compressRLE(outputText);
            // Append salt at the beginning of the compressed encrypted file for decryption later
            writeFile("enc_" + filename, salt + compressedOutput);
            cout << "\nEncryption completed. Encrypted file: enc_" << filename << endl;
        } else if (operation == "2") { // Decryption
            // Read the encrypted file and extract the salt
            string encryptedText = readFile(filename);
            salt = encryptedText.substr(0, 16);  // Extracting the salt assuming fixed length
            string compressedContent = encryptedText.substr(16);  // The rest is the actual compressed content

            // Derive the key again using the extracted salt
            string derivedKeyForDecryption = deriveKey(key, salt);
            outputText = xorCipher(decompressRLE(compressedContent), derivedKeyForDecryption);
            writeFile("dec_" + filename, outputText);
            cout << "\nDecryption completed. Decrypted file: dec_" << filename << endl;
        }

        // Ask the user if they want to perform another operation
        string choice;
        cout << "\n> Do you want to perform another operation? (y/n): ";
        cin >> choice;
        if (choice != "y") {
            break; // Exit the loop if the user doesn't want to continue
        }

        // Clear input buffer for the next iteration
        cin.ignore();
    }

    return 0;
}