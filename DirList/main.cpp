#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <conio.h> // For _getch() to wait for a key press

namespace fs = std::filesystem;

// Function to convert file_time_type to a formatted time string without seconds
std::string format_time(const fs::file_time_type& ft) {
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        ft - fs::file_time_type::clock::now() + std::chrono::system_clock::now());
    auto sctp_time_t = std::chrono::system_clock::to_time_t(sctp); // Convert to time_t

    std::tm tm = *std::localtime(&sctp_time_t); // Convert to local time
    std::ostringstream oss;
    // Format the time without seconds
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M"); // Format: YYYY-MM-DD HH:MM
    return oss.str();
}

// Function to convert bytes to kilobytes
double bytes_to_kilobytes(uintmax_t bytes) {
    return static_cast<double>(bytes) / 1024; // Convert bytes to KB
}

// Function to get file attributes as a string
std::string get_file_attributes(const fs::path& path) {
    std::string attributes;
    auto perms = fs::status(path).permissions();

    if ((perms & fs::perms::owner_read) != fs::perms::none) attributes += "R"; // Read
    if ((perms & fs::perms::owner_write) != fs::perms::none) attributes += "W"; // Write
    if ((perms & fs::perms::owner_exec) != fs::perms::none) attributes += "X"; // Execute

    return attributes.empty() ? "None" : attributes; // Return "None" if no attributes found
}

// Function to escape CSV strings
std::string escape_csv(const std::string& str) {
    std::string escaped = str;
    // Escape double quotes by replacing them with two double quotes
    size_t pos = 0;
    while ((pos = escaped.find("\"", pos)) != std::string::npos) {
        escaped.replace(pos, 1, "\"\"");
        pos += 2; // Move past the inserted quotes
    }
    // Enclose the string in quotes if it contains a comma or a quote
    if (escaped.find(",") != std::string::npos || escaped.find("\"") != std::string::npos) {
        escaped = "\"" + escaped + "\"";
    }
    return escaped;
}

int main() {
    // Program information
    std::cout << "Directory Listing Program v1.2.15.3\n";
    std::cout << "Developed by Abolfazl Najafi, abolfazlbox@gmail.com\n\n";

    // Output .csv file with UTF-8 BOM
    std::ofstream outputFile("file_list.csv", std::ios::out | std::ios::binary);

    // Check if the file opened successfully
    if (!outputFile.is_open()) {
        std::cerr << "Error creating file_list.csv" << std::endl;
        return 1;
    }

    // Write UTF-8 BOM
    const char bom[] = { '\xEF', '\xBB', '\xBF' }; // UTF-8 BOM
    outputFile.write(bom, sizeof(bom));

    // Write CSV header
    outputFile << "Name,Date Modified,Type,Size (KB),Attributes,Location\n"; // Use comma as a delimiter

    // Get the main directory name
    fs::path currentPath = fs::current_path();
    std::string mainDirName = currentPath.filename().string();

    // Iterate over files in the current directory and its subdirectories
    for (const auto& entry : fs::recursive_directory_iterator(currentPath)) {
        if (entry.is_regular_file()) {
            // Remove the file extension from the file name
            std::string fileName = entry.path().filename().replace_extension("").string();
            std::string fileType = entry.path().extension().string();

            // Exclude the generated CSV file and compiled executable
            if (fileName == "file_list" || fileName == "DirList") {
                continue;
            }

            // Remove the dot from the file type
            if (!fileType.empty()) {
                fileType = fileType.substr(1); // Remove the dot from the extension
            }

            // Get file size in KB
            auto fileSize = entry.file_size();
            double fileSizeKB = bytes_to_kilobytes(fileSize);

            // Get modified date
            auto modifiedTime = fs::last_write_time(entry);
            std::string modifiedDate = format_time(modifiedTime);

            // Get file attributes
            std::string attributes = get_file_attributes(entry.path());

            // Determine the folder location for the file
            std::string location;
            fs::path parentPath = entry.path().parent_path();

            // Construct the location string
            std::string relativePath = fs::relative(parentPath, currentPath).string();
            location = mainDirName + "/" + relativePath; // Prepend the main directory name to the relative path
            location.erase(0, location.find_first_not_of("/")); // Remove any leading slashes

            // Write file details to .csv, showing size with two decimal places
            if (fileSizeKB >= 0.01) { // Only include files that are 0.01 KB or larger
                outputFile << escape_csv(fileName) << ","
                           << escape_csv(modifiedDate) << ","
                           << escape_csv(fileType) << ","
                           << std::fixed << std::setprecision(2) << fileSizeKB << ","
                           << escape_csv(attributes) << ","
                           << escape_csv(location) << "\n"; // Comma-separated values
            }
        }
    }

    // Close the file
    outputFile.close();
    std::cout << ">> CSV file 'file_list.csv' generated successfully!\n";
    std::cout << "Press any key to exit..." << std::endl;

    _getch(); // Wait for key
    return 0;
}
