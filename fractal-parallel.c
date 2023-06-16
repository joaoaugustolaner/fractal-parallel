/*
fractal.c - Sample Mandelbrot Fractal Display
Starting code for CSE 30341 Project 3.
*/

#include "gfx.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <complex.h>
#include <pthread.h>


/*
Compute the number of iterations at point x, y
in the complex space, up to a maximum of maxiter.
Return the number of iterations at that point.

This example computes the Mandelbrot fractal:
z = z^2 + alpha

Where z is initially zero, and alpha is the location x + iy
in the complex plane.  Note that we are using the "complex"
numeric type in C, which has the special functions cabs()
and cpow() to compute the absolute values and powers of
complex values.
*/

typedef struct {
    int thread_id;
    int num_threads;
    double xmin;
    double xmax;
    double ymin;
    double ymax;
    int maxiter;
} ThreadParams;

// Color map for the fractal
typedef struct {
    int red;
    int green;
    int blue;
} Color;

Color colormap[] = {
    {0, 7, 100},
    {32, 107, 203},
    {237, 255, 255},
    {255, 170, 0},
    {0, 2, 0}
};

#define NUM_COLORS sizeof(colormap) / sizeof(Color)

pthread_mutex_t mutex;

static Color get_color(int iter, int maxiter)
{
    if (iter == maxiter)
        return colormap[NUM_COLORS - 1]; // Set the last color for points inside the set

    double t = (double)iter / maxiter;
    int index = t * (NUM_COLORS - 1);

    double fractal = t * (NUM_COLORS - 1) - index;
    int r = colormap[index].red + fractal * (colormap[index + 1].red - colormap[index].red);
    int g = colormap[index].green + fractal * (colormap[index + 1].green - colormap[index].green);
    int b = colormap[index].blue + fractal * (colormap[index + 1].blue - colormap[index].blue);

    Color color = {r, g, b};
    return color;
}

static int compute_point(double x, double y, int max)
{
    double complex z = 0;
    double complex alpha = x + I * y;

    int iter = 0;

    while (cabs(z) < 4 && iter < max) {
        z = cpow(z, 2) + alpha;
        iter++;
    }

    return iter;
}

void* compute_image_thread(void* params)
{
    ThreadParams* thread_params = (ThreadParams*)params;
    int width = gfx_xsize();
    int height = gfx_ysize();
    int num_pixels = width * height;
    int pixels_per_thread = num_pixels / thread_params->num_threads;
    int start_pixel = thread_params->thread_id * pixels_per_thread;
    int end_pixel = (thread_params->thread_id == thread_params->num_threads - 1) ? num_pixels : (start_pixel + pixels_per_thread);

    for (int pixel = start_pixel; pixel < end_pixel; pixel++) {
        int i = pixel % width;
        int j = pixel / width;

        double x = thread_params->xmin + i * (thread_params->xmax - thread_params->xmin) / width;
        double y = thread_params->ymin + j * (thread_params->ymax - thread_params->ymin) / height;

        int iter = compute_point(x, y, thread_params->maxiter);

        Color color = get_color(iter, thread_params->maxiter);

        pthread_mutex_lock(&mutex);

        gfx_color(color.red, color.green, color.blue);
        gfx_point(i, j);

        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}

/*
Compute an entire image, writing each point to the given bitmap.
Scale the image to the range (xmin-xmax,ymin-ymax).
*/

void compute_image(double xmin, double xmax, double ymin, double ymax, int maxiter)
{
    int num_threads = 12; 
    pthread_t threads[num_threads];
    ThreadParams thread_params[num_threads];

    pthread_mutex_init(&mutex, NULL); 

    for (int i = 0; i < num_threads; i++) {
        thread_params[i].thread_id = i;
        thread_params[i].num_threads = num_threads;
        thread_params[i].xmin = xmin;
        thread_params[i].xmax = xmax;
        thread_params[i].ymin = ymin;
        thread_params[i].ymax = ymax;
        thread_params[i].maxiter = maxiter;

        pthread_create(&threads[i], NULL, compute_image_thread, (void*)&thread_params[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex); 
}

int main(int argc, char* argv[])
{
    // The initial boundaries of the fractal image in x,y space.
    double xmin = -1.5;
    double xmax = 0.5;
    double ymin = -1.0;
    double ymax = 1.0;

    // Maximum number of iterations to compute.
    // Higher values take longer but have more detail.
    int maxiter = 1000;

    // Open a new window.
    gfx_open(1080, 720, "Mandelbrot Fractal");

    // Show the configuration, just in case you want to recreate it.
    printf("coordinates: %lf %lf %lf %lf\n", xmin, xmax, ymin, ymax);

    // Fill it with a dark blue initially.
    gfx_clear_color(255, 255, 255);
    gfx_clear();

    // Display the fractal image
    compute_image(xmin, xmax, ymin, ymax, maxiter);

    while (1) {
        // Wait for a key or mouse click.
        int c = gfx_wait();

        // Quit if q is pressed.
        if (c == 'q') {
            exit(0);
        }
        // Zoom in if left mouse button is clicked.
        else if (c == 1) {
            // Get the mouse coordinates on a click
            int x_click = gfx_xpos();
            int y_click = gfx_ypos();

            // Calculate the new boundaries based on the click position
            double x_center = xmin + x_click * (xmax - xmin) / gfx_xsize();
            double y_center = ymin + y_click * (ymax - ymin) / gfx_ysize();

            double zoom_factor = 0.5;  // Adjust the zoom factor as desired
            double x_width = (xmax - xmin) * zoom_factor;
            double y_height = (ymax - ymin) * zoom_factor;

            xmin = x_center - x_width / 2;
            xmax = x_center + x_width / 2;
            ymin = y_center - y_height / 2;
            ymax = y_center + y_height / 2;

            // Clear the window and display the updated fractal
            gfx_clear();
            compute_image(xmin, xmax, ymin, ymax, maxiter);
        }
        // Zoom out if right mouse button is clicked.
        else if (c == 3) {
            xmin = -1.5;
            xmax = 0.5;
            ymin = -1.0;
            ymax = 1.0;
            // Clear the window and redraw the zoomed-out fractal.
            gfx_clear();
            compute_image(xmin, xmax, ymin, ymax, maxiter);
        }
    }

    return 0;
}
