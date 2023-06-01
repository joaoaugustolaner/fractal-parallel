# fractal-parallel
This repository contais a parallel execution of a mandelbrot fractal

---

To run this project make sure you have the required libraries installed: 

* On [Windows](https://learnopengl.com/)
* On [Linux](https://medium.com/geekculture/a-beginners-guide-to-setup-opengl-in-linux-debian-2bfe02ccd1e)


On OS X Apple has a deprecated version of OpenGL since they changed to [Metal API](https://developer.apple.com/metal/), but you can try running on OSX by [migrating your code](https://developer.apple.com/documentation/metal/metal_sample_code_library/migrating_opengl_code_to_metal), [mixing it](https://developer.apple.com/documentation/metal/metal_sample_code_library/mixing_metal_and_opengl_rendering_in_a_view) or following [this archived guide on Apple's Docs](https://developer.apple.com/library/archive/documentation/GraphicsImaging/Conceptual/OpenGL-MacProgGuide/opengl_intro/opengl_intro.html)

---

After succesfully installing the necessary dependencies, you can check if your code compiles with this command:

```bash
gcc main.cpp -lglut -lGLU -lGL -lm -std=c++1z -o out
```

Finally, you can run check the output file by just ```./out``` from your current directory

P.S.: This orientations are to run with `bash/zsh/fish` commands.
