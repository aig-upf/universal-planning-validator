#pragma once

void showErrorMsg( const std::string& s ) {
    std::cerr << "\033[1;31mError: " << s << "\033[0m\n";
}

void showSuccessMsg( const std::string& s ) {
    std::cout << "\033[1;32mSuccess: " << s << "\033[0m\n";
}

void showMsg( const std::string& s ) {
    std::cout << s << "\n";
}
