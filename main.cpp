#include <iostream>
#include <filesystem>
#include <unordered_set>
#include <string>

namespace fs = std::filesystem;


class FolderManager {
public:
    void createFolder(const fs::path& folderPath) {
        if (!fs::exists(folderPath)) {
            fs::create_directory(folderPath);
            logFolderCreation(folderPath);
        }
    }

private:
    void logFolderCreation(const fs::path& folderPath) {
        std::cout << "Created folder: " << folderPath << std::endl;
    }
};

class FileProcessor {
private:
    FolderManager folderManager;

public:
    void processFilesInFolder(const fs::path& folderPath) {
        if (!fs::exists(folderPath)) {
            logError("Folder does not exist: " + folderPath.string());
            return;
        }

        int totalFiles = countFiles(folderPath);
        int processedFiles = 0;

        for (const auto& entry : fs::recursive_directory_iterator(folderPath)) {
            if (fs::is_regular_file(entry)) {
                processSingleFile(entry.path());
                processedFiles++;
                logProgress(processedFiles, totalFiles);
            }
        }

        logCompletion();
    }

private:
    void processSingleFile(const fs::path& filePath) {
        std::string extension = extractExtension(filePath);
        fs::path parentFolder = filePath.parent_path();
        fs::path extensionFolder = parentFolder / extension;

        folderManager.createFolder(extensionFolder);
        moveFileToFolder(filePath, extensionFolder);
    }

    std::string extractExtension(const fs::path& filePath) {
        std::string extension = filePath.extension().string();
        if (extension.empty()) {
            extension = "no_extension";
        } else if (extension[0] == '.') {
            extension.erase(0, 1);
        }
        return extension;
    }

    void moveFileToFolder(const fs::path& filePath, const fs::path& targetFolder) {
        fs::path targetPath = targetFolder / filePath.filename();
        try {
            fs::rename(filePath, targetPath);
            logFileMove(filePath, targetPath);
        } catch (const fs::filesystem_error& e) {
            logError("Error moving file: " + std::string(e.what()));
        }
    }

    int countFiles(const fs::path& folderPath) {
        return std::distance(fs::recursive_directory_iterator(folderPath), fs::recursive_directory_iterator());
    }

    void logFileMove(const fs::path& oldPath, const fs::path& newPath) {
        std::cout << "Moved file: " << oldPath << " -> " << newPath << std::endl;
    }

    void logProgress(int processed, int total) {
        std::cout << "Progress: " << processed << "/" << total << " files processed.\n";
    }

    void logError(const std::string& message) {
        std::cerr << "Error: " << message << std::endl;
    }

    void logCompletion() {
        std::cout << "\nFile organization completed successfully!" << std::endl;
    }
};


class FileOrganizer {
private:
    FileProcessor fileProcessor;

public:
    void organizeFiles(const std::string& folderPath) {
        displayWelcomeMessage();
        fileProcessor.processFilesInFolder(folderPath);
    }

private:
    void displayWelcomeMessage() {
        std::cout << "===========================================\n";
        std::cout << "        Welcome to File Organizer\n";
        std::cout << "===========================================\n";
    }
};


namespace Utils {
    void printSeparator() {
        std::cout << "-------------------------------------------\n";
    }

    std::string getUserInput(const std::string& prompt) {
        std::string input;
        std::cout << prompt;
        std::cin >> input;
        return input;
    }

    void displayExitMessage() {
        std::cout << "\nThank you for using File Organizer. Goodbye!\n";
    }
}


class MainMenu {
private:
    FileOrganizer fileOrganizer;

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

private:
    void displayMenu() {
        Utils::printSeparator();
        std::cout << "Main Menu:\n";
        std::cout << "1. Organize Files in a Folder\n";
        std::cout << "2. Exit\n";
        Utils::printSeparator();
    }

    int getChoice() {
        int choice;
        std::cout << "Enter your choice (1-2): ";
        std::cin >> choice;
        return choice;
    }

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

    void handleOrganizeFiles() {
        std::string folderPath = Utils::getUserInput("Enter the folder path: ");
        fileOrganizer.organizeFiles(folderPath);
    }
};


int main() {
    MainMenu menu;
    menu.run();
    return 0;
}
