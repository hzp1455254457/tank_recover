#include "core/Game.h"
#include <iostream>
#include <cstdlib>
#include <fstream>

int main(int argc, char* argv[]) {
    try {
        // Early startup log
        {
            std::ofstream log("D:/tankRecover/build/Release/debug_log.txt", std::ios::app);
            if (log) log << "main: started\n";
        }
        // Create game instance
        BattleCity::Game game;

        // Initialize game
        if (!game.init()) {
            std::cerr << "Failed to initialize game!" << std::endl;
            return EXIT_FAILURE;
        }

        // Run main game loop
        std::cout << "main: about to call game.run()" << std::endl;
        // Also write to log file for reliable capture
        {
            std::ofstream log("D:/tankRecover/build/Release/debug_log.txt", std::ios::app);
            if (log) log << "main: about to call game.run()\n";
        }
        bool shouldExit = game.run();
        std::cout << "main: returned from game.run(), exit=" << shouldExit << std::endl;
        {
            std::ofstream log("D:/tankRecover/build/Release/debug_log.txt", std::ios::app);
            if (log) log << "main: returned from game.run(), exit=" << shouldExit << "\n";
        }

        if (shouldExit) {
            std::cout << "main: Game requested exit" << std::endl;
            return EXIT_SUCCESS; // Exit the program
        }

        // Shutdown game
        game.shutdown();

        return EXIT_SUCCESS;

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred!" << std::endl;
        return EXIT_FAILURE;
    }
}