# Fluid-Simulations

## Description:

A 2D fluid simulation written in vanilla OpenGL alongside parallel rendering with OpenMP. The Physics for the simulation can be simply attributed to Jos Stam's paper[https://www.cs.cmu.edu/afs/cs/academic/class/15462-s13/www/lec_slides/StamFluidforGames.pdf],
which broke down to 3 primary forces:

# Advection:

# Diffusion:

# Projection:

## Running the simulation

        

**Note**: For proper utility of the parallel implementation, set the current display set onto your iGPU(mine is Intel UHD 770 Graphics). Using your default discrete graphics card, will give abysmal performance.
