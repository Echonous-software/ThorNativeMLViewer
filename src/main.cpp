#include <app/Application.hpp>
#include <core/Error.hpp>
#include <iostream>
#include <exception>

int main() {
    try {
        thor::app::Application app;
        
        if (!app.initialize()) {
            std::cerr << "Failed to initialize application" << std::endl;
            return -1;
        }
        
        return app.run();
        
    } catch (const thor::core::ThorException& e) {
        std::cerr << "Thor Exception: " << e.what() << std::endl;
        return -1;
    } catch (const std::exception& e) {
        std::cerr << "Standard Exception: " << e.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "Unknown exception occurred" << std::endl;
        return -1;
    }
} 