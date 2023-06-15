#include <SDL2/SDL.h>

const int WIDTH = 800;   // Width of the window
const int HEIGHT = 800;  // Height of the window

const int MAX_ITERATIONS = 100;  // Maximum number of iterations for the Mandelbrot algorithm

// Function to calculate the Mandelbrot set
int mandelbrot(double x0, double y0) {
    double x = 0.0;
    double y = 0.0;
    int iterations = 0;

    for (int iterations = 0; iterations < MAX_ITERATIONS; iterations++) {
        double xTemp = x * x - y * y + x0;
        y = 2 * x * y + y0;
        x = xTemp;

        if (x * x + y * y >= 4.0) {
            return iterations;
        }
    }

    return MAX_ITERATIONS;
}

int main(int argc, char* argv[]) {

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    // Create a window
    SDL_Window* window = SDL_CreateWindow("Mandelbrot Set", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create a renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Clear the renderer with white color
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    double zoom = 1.0;  // Zoom factor
    double xOffset = 0.0;  // X-axis offset
    double yOffset = 0.0;  // Y-axis offset

    // Main loop
    bool quit = false;
    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_SPACE:
                            zoom *= 1.0;  // Zoom in
                            break;
                        
                        case SDLK_DOWN:
                            yOffset -= 0.1 / zoom; // Move Up
                            break;
                        
                        case SDLK_UP:
                            yOffset += 0.1 / zoom; // Move Down
                            break;

                        case SDLK_LEFT:
                            xOffset -= 0.1 / zoom; // Move left
                            break;

                        case SDLK_RIGHT:
                            xOffset += 0.1 / zoom; // Move right
                            break;
                    }
                    break;
            }
        }

        // Iterate over each pixel and draw it based on the Mandelbrot set
        for (int x = 0; x < WIDTH; x++) {
            for (int y = 0; y < HEIGHT; y++) {
                // Map pixel coordinates to the complex plane
                double xPos = (x - WIDTH / 2.0) * 4.0 / (WIDTH * zoom) + xOffset;
                double yPos = (y - HEIGHT / 2.0) * 4.0 / (HEIGHT * zoom) + yOffset;

                // Calculate the Mandelbrot value for the current coordinates
                int value = mandelbrot(xPos, yPos);

                // Map the Mandelbrot value to a color
                int red = (value * 5) % 255;
                int green = (value * 10) % 255;
                int blue = (value * 20) % 255;

                // Set the pixel color
                SDL_SetRenderDrawColor(renderer, red, green, blue, 255);
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }

        // Update the screen with the rendered image
        SDL_RenderPresent(renderer);
    }

    // Clean up resources
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

