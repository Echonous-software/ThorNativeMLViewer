#include <iostream>

#include <app/Application.hpp>
#include <core/Error.hpp>

int main() {
    try {
        echonous::Application app("ThorNativeMLViewer", "thor");
        return app.run();
    } catch (const echonous::Error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
} 