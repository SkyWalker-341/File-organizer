#include <iostream>
#include <filesystem>
#include <unordered_set>
#include <string>

namespace fs = std::filesystem; //fs a shorter alias for std::filesystem


class FolderManager {
public:
    void createFolder(const fs::path& folderPath) {
        if (!fs::exists(folderPath)) {
            fs::create_directory(folderPath);
            logFolderCreation(folderPath);
        }
    }

    /* 
    the class Folder Manager has a method createFolder which 
     -> checks if a folder exists in the specified path and if it doesn't it creates 
        a new one and it is logged
     -> we have used functions like fs::exists and fs::create_directory from the std::filesystem
    */

private:
    void logFolderCreation(const fs::path& folderPath) {
        std::cout << "Created folder: " << folderPath << std::endl;
    }
};
/*
this method logs the creation of the folder
 -> making this private ensures it can't be called outside of the class which is appropriate
 -> logging helps in keeping a track on what our program is doing at runtime 
    which makes it easy for troubleshooting and debugging
*/

class FileProcessor {
private:
    FolderManager folderManager;

    /*
     the folderManager instance is used in this class in order to check if folders exists 
     and create them if they don't
    */

public:
    void processFilesInFolder(const fs::path& folderPath) {
        if (!fs::exists(folderPath)) {
            logError("Folder does not exist: " + folderPath.string());
            return;
        }

        /*
         the method processFilesInFolder checks if a folder exists
         if it doesn't, it logs an error message
        */

        int totalFiles = countFiles(folderPath);
        int processedFiles = 0;

        /*
        -> counts the number of files in the specified folder and stores it
           in totalFiles

        -> processFiles is initialized to zero which is later incremented when
           files are processed, keeping a track on how many files have
           been processed.
        */

        for (const auto& entry : fs::recursive_directory_iterator(folderPath)) {
            if (fs::is_regular_file(entry)) {
                processSingleFile(entry.path());
                processedFiles++;
                logProgress(processedFiles, totalFiles);
            }
        }

        /*
         -> the loop recursively go through the folder specified by the folder path
            and checks if the entry is a regular file

        -> If it is, then the file is processed by the method processSingleFile
        -> processedFiles is then incremented, and the process is logged
        */

        logCompletion();
        // logs a message saying file organization is complete
    }

private:
    void processSingleFile(const fs::path& filePath) {
        std::string extension = extractExtension(filePath);
        fs::path parentFolder = filePath.parent_path();
        fs::path extensionFolder = parentFolder / extension;

        folderManager.createFolder(extensionFolder);
        moveFileToFolder(filePath, extensionFolder);
    }

    /*
     -> the method prcoessSingleFile extracts the extension of the files
     in the folder using extractExtension method

     -> path of the parent folder is obtained using .parent_path() 
     (a method provided by the filesystem module) and stores in parentFolder
     

     -> extension extracted is later added to the parentFolder path
     -> a new Folder is created with the new path created with
     the name of the 'extension'

     -> files are then moved to the new folder using moveFileToFolder method
    */

    std::string extractExtension(const fs::path& filePath) {
        std::string extension = filePath.extension().string();
        if (extension.empty()) {
            extension = "no_extension";
        } else if (extension[0] == '.') {
            extension.erase(0, 1);
        }
        return extension;
    }

    /*
    -> method extractExtension extracts the extension from the specified
    file path using .extension() which is a filesystem method 
    
    ->the extension (eg: .txt) is converted to a string
    ->checks if the extension is empty if it is 'extension' is updated
    with 'no extension'

    ->if not check if the first character is '.' and in order to erase
    the dot we use .erase() which is a member function of <string>
    
    -> since we are removing the first character and only 1 character of the string
    we give(0,1) , 0 for index and 1 for how many characters to be erased
    
    */

    void moveFileToFolder(const fs::path& filePath, const fs::path& targetFolder) {
        fs::path targetPath = targetFolder / filePath.filename();
        try {
            fs::rename(filePath, targetPath);
            logFileMove(filePath, targetPath);
        } catch (const fs::filesystem_error& e) {
            logError("Error moving file: " + std::string(e.what()));
        }
    }

    /*
    ->the menthod moveFiletoFolder moves the file specified in file path
    to the target folder.

    ->a new target path is updated by combining the path of target folder and name of the file
    in the file path

    ->renames/moves the filePath to target path using fs::rename() a function part of
    filesystem module

    ->logs the file move as moved file: file path -> targetPath
     if the move is succesful

    -> if there's and error during the move it catches the exception and logs the error message
    */

    int countFiles(const fs::path& folderPath) {
        return std::distance(fs::recursive_directory_iterator(folderPath), fs::recursive_directory_iterator());
    }

    /*
    -> counts the number of files in the folder specified and its subfolders
    for this:

    we use fs::recursive_directory_iterator to iterate over all the files and folders
    and std::distance calculates the number of entries between the two iterators. This will give the total number of entries
    
    */

    void logFileMove(const fs::path& oldPath, const fs::path& newPath) {
        std::cout << "Moved file: " << oldPath << " -> " << newPath << std::endl;
    }

    /*
    logs the move of the file as moved file: filePath -> targetPath 
    */

    void logProgress(int processed, int total) {
        std::cout << "Progress: " << processed << "/" << total << " files processed.\n";
    }

    /*
    logs the porgress of the process as progress: processedfiles / totalfiles 
    */
    void logError(const std::string& message) {
        std::cerr << "Error: " << message << std::endl;
    }

    /*
    logs the error as Error: error message
    */
    void logCompletion() {
        std::cout << "\nFile organization completed successfully!" << std::endl;
    }

    //logs the completions, here it prints File organization completed successfully!
};


class FileOrganizer {
private:
    FileProcessor fileProcessor;

    /*
    class FileProcessor has the instance of FileProcessor called 'fileprocessor' which is
    used to handle the processing of files
    
    */
public:
    void organizeFiles(const std::string& folderPath) {
        displayWelcomeMessage();
        fileProcessor.processFilesInFolder(folderPath);
    }

    /*
    -> displays a welcome message

    -> the method organizeFiles used to organize the files in specified folder
    by calling the prcoessFilesInFolder method from File Processor class
    uses the fileProcessor instance to do that
    */

private:
    void displayWelcomeMessage() {
        std::cout << "===========================================\n";
        std::cout << "        Welcome to File Organizer\n";
        std::cout << "===========================================\n";
    }
};

// displays the Welcome message


namespace Utils {   //defines the utility functions
    void printSeparator() {
        std::cout << "-------------------------------------------\n";
    }

    // prints a line to seperate

    std::string getUserInput(const std::string& prompt) {
        std::string input;
        std::cout << prompt;
        std::cin >> input;
        return input;
    }

    //prompts for an input and returns the entered strings

    void displayExitMessage() {
        std::cout << "\nThank you for using File Organizer. Goodbye!\n";
    }

    //displays a thank you message when they exit the program
}


class MainMenu {
private:
    FileOrganizer fileOrganizer;
    /*
    uses the fileOrganizer instance of FileOrganizer class to organize files
    */
public:
    void run() {
        while (true) {
            displayMenu();
            int choice = getChoice();
            handleMenuChoice(choice);
            if (choice == 2) break;
        }
        Utils::displayExitMessage();
    }

    /*
    -> displays the Menu and takes input as a choice between 1 and 2
    -> handles the choice using HandleMenuChoice menthod
    -> if the choice is 2, it breaks and displays Exit Message
    
    */

private:
    void displayMenu() {
        Utils::printSeparator();
        std::cout << "Main Menu:\n";
        std::cout << "1. Organize Files in a Folder\n";
        std::cout << "2. Exit\n";
        Utils::printSeparator();
    }

    // displays the Menu and take inputs, all the prompts are between two lines

    int getChoice() {
        int choice;
        std::cout << "Enter your choice (1-2): ";
        std::cin >> choice;
        return choice;
    }

    // choice entered taken as input and returned 

    void handleMenuChoice(int choice) {
        switch (choice) {
        case 1:
            handleOrganizeFiles();
            break;
        case 2:
            break;
        default:
            std::cerr << "Invalid choice. Please try again.\n";
            break;
        }
    }

    /*
    this method Handles the Menu Choices
    -> uses switch two handle them with 2 cases
    -> deafult set as invalid choice and asking the user to try again

    */

    void handleOrganizeFiles() {
        std::string folderPath = Utils::getUserInput("Enter the folder path: ");
        fileOrganizer.organizeFiles(folderPath);
    }

    /*
    -> prompts to enter the folder path which needs to be organized
    -> Organizes the files using organizeFiles method of file Organizer
    */
};


int main() {
    MainMenu menu;
    menu.run();
    return 0;
}

/*
Main function
creates an instance menu of 'MainMenu'
calls run() menthod
and returs 0 indicating succesful execution

*/