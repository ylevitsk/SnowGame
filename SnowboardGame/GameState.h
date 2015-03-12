#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <ctime>

class GameState {
   private:
      std::clock_t start;

   public:
      void startGame(void) {
         start = std::clock();
      }

      double endGame(void) {
         return (std::clock() - start) / (double) CLOCKS_PER_SEC;
      }
}

#endif
