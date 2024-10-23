
# Directory Listing and CSV Export

This repository contains a C++ program that lists the contents of a specified directory and exports the details of each file into a `.csv` file. The program processes files in the directory and provides the following information:

- File Name
- Last Modified Date (formatted as `YYYY-MM-DD HH:MM`)
- File Type (Extension)
- File Size (in KB)
- File Attributes (Read/Write/Execute permissions)
- File Location (relative to the main directory)

## Features

1. **Directory Listing**: Iterates through all files in a specified directory, collecting information.
2. **File Details**: Retrieves file attributes, including size, modification time, and permissions.
3. **CSV Export**: The collected information is exported in CSV format with proper escaping for any special characters.
4. **File Size Filtering**: Files smaller than 0.01 KB are excluded from the output.

## Usage

To compile and run the program:

1. **Compile the program**:
    ```bash
    g++ main.cpp -o DirList versioninfo.o -std=c++17 -static-libgcc -static-libstdc++
    ```

2. **Run the program**:
    ```bash
    ./DirList
    ```

After running the program, it will generate a `file_list.csv` file containing the details of the files in the directory.

## Requirements

- C++17 or later
- A C++ compiler such as MingW
- Filesystem support for handling directory and file operations.

## License

This project is licensed under the MIT License.
