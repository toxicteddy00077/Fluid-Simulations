#ifndef FLUID_H
#define FLUID_H

#include <vector>
#include <GL/glut.h>

constexpr int N = 128;
constexpr int SCALE = 6;
const float dt = 0.1f;
const float diff = 0.0001f;
const float visc = 0.0001f;

inline int IDX(int x, int y) {
    return x + N * y;
}

enum Color { WHITE, RED, GREEN, BLUE, MULTI};

class Fluid {
public:
    Fluid();
    ~Fluid();

    void addDensity(int x, int y, float amount);
    void addVelocity(int x, int y, float amount_x, float amount_y);

    void step();

    // just to render the density or velocity field
    void render(Color color) const;

    void fade();

    const std::vector<float>& getDensity() const;
    const std::vector<float>& getU() const;
    const std::vector<float>& getV() const;

private:
    std::vector<float> dens, dens_prev;
    std::vector<float> vel_x, vel_y, vel_x_prev, vel_y_prev;

    void addSource(std::vector<float>& x, const std::vector<float>& s);
    void diffuse(int b, std::vector<float>& x, const std::vector<float>& x0, float diff);
    void linearSolve(int b, std::vector<float>& x, const std::vector<float>& x0, float a, float c);
    void setBound(int b, std::vector<float>& x);
    void project(std::vector<float>& u, std::vector<float>& v,
                 std::vector<float>& p, std::vector<float>& div);
    void advect(int b, std::vector<float>& d, const std::vector<float>& d0,
                const std::vector<float>& u, const std::vector<float>& v);
};

#endif 

