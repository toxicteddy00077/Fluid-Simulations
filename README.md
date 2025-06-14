# Fluid-Simulations

## Description:

A 2D fluid simulation written in vanilla OpenGL alongside parallel rendering with OpenMP. The Physics for the simulation can be simply attributed to Jos Stam's paper[https://www.cs.cmu.edu/afs/cs/academic/class/15462-s13/www/lec_slides/StamFluidforGames.pdf],
which broke down to 3 primary forces:

### Advection:
How fluid properties (like velocity) are transported by the fluid's own motion. Basically moving alongside flow 

### Diffusion:
The process where fluid properties spread out and smooth themselves, like heat dissipating or viscosity smoothing velocities.

### Projection:
A step to ensure the fluid remains incompressible, preventing it from spontaneously expanding or contracting by adjusting velocities with pressure. (Think "enforcing incompressibility.")

## Running the simulation
        g++ -O2 -o fluid_sim main.cpp fluid.cpp helper.cpp -lGL -lGLU -lglut -fopenmp
        ./fluid_sim
        

**Note**: For proper utility of the parallel implementation, set the current display set onto your iGPU(mine is Intel UHD 770 Graphics). Using your default discrete graphics card, will give abysmal performance.
