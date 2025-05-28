#include "fluid.h"
#include <algorithm>
#include <omp.h>

Fluid::Fluid()
    : dens(N*N), dens_prev(N*N),
      vel_x(N*N), vel_y(N*N), vel_x_prev(N*N), vel_y_prev(N*N) {
    // Reserve one core for rendering
    int threads = std::max(1, omp_get_max_threads() - 1);
    omp_set_num_threads(threads);
    omp_set_nested(0);
}

Fluid::~Fluid() = default;

void Fluid::addDensity(int x, int y, float amount) {
    dens[IDX(x, y)] += amount;
}

void Fluid::addVelocity(int x, int y, float amount_x, float amount_y) {
    vel_x[IDX(x, y)] += amount_x;
    vel_y[IDX(x, y)] += amount_y;
}

const std::vector<float>& Fluid::getDensity() const { return dens; }
const std::vector<float>& Fluid::getU() const { return vel_x; }
const std::vector<float>& Fluid::getV() const { return vel_y; }

void Fluid::step() {
    // 1) inject sources
    #pragma omp parallel for
    for (int idx = 0; idx < N*N; ++idx) {
        vel_x[idx] += dt * vel_x_prev[idx];
        vel_y[idx] += dt * vel_y_prev[idx];
        dens[idx]  += dt * dens_prev[idx];
    }

    // 2) diffuse velocity
    diffuse(1, vel_x_prev, vel_x, visc);
    diffuse(2, vel_y_prev, vel_y, visc);

    // 3) project velocity
    project(vel_x_prev, vel_y_prev, vel_x, vel_y);

    // 4) advect velocity
    advect(1, vel_x, vel_x_prev, vel_x_prev, vel_y_prev);
    advect(2, vel_y, vel_y_prev, vel_x_prev, vel_y_prev);

    // 5) project again
    project(vel_x, vel_y, vel_x_prev, vel_y_prev);

    // 6) diffuse density
    diffuse(0, dens_prev, dens, diff);

    // 7) advect density
    advect(0, dens, dens_prev, vel_x, vel_y);

    // 8) clear old sources
    #pragma omp parallel for
    for (int idx = 0; idx < N*N; ++idx) {
        vel_x_prev[idx] = 0.0f;
        vel_y_prev[idx] = 0.0f;
        dens_prev[idx]  = 0.0f;
    }
}

void Fluid::render(Color color) const {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            int x = i * SCALE;
            int y = j * SCALE;
            float d = dens[IDX(i, j)];
            d = (d < 0.0f) ? 0.0f : (d > 1.0f ? 1.0f : d);
            switch (color) {
                case BLUE:  glColor3f(0, 0, d); break;
                case GREEN: glColor3f(0, d, 0); break;
                case RED:   glColor3f(d, 0, 0); break;
                case WHITE:
                default:    glColor3f(d, d, d); break;
            }

            glBegin(GL_QUADS);
                glVertex2f(x,y);
                glVertex2f(x + SCALE,  y);
                glVertex2f(x + SCALE,  y + SCALE);
                glVertex2f(x,y + SCALE);
            glEnd();

        }
    }
}

void Fluid::diffuse(int b, std::vector<float>& x, const std::vector<float>& x0, float diff) {
    float a = dt * diff * (N - 2) * (N - 2);
    linearSolve(b, x, x0, a, 1 + 4 * a);
}

void Fluid::linearSolve(int b, std::vector<float>& x, const std::vector<float>& x0, float a, float c) {
    for (int k = 0; k < 20; ++k) {

        #pragma omp parallel for collapse(2)
        for (int j = 1; j < N - 1; ++j) {
            for (int i = 1; i < N - 1; ++i) {
                x[IDX(i, j)] = (x0[IDX(i, j)] + a * (
                    x[IDX(i+1, j)] + x[IDX(i-1, j)] +
                    x[IDX(i, j+1)] + x[IDX(i, j-1)]
                )) / c;
            }
        }
        setBound(b, x);
    }
}

void Fluid::setBound(int b, std::vector<float>& x) {

    #pragma omp parallel for
    for (int i = 1; i < N - 1; ++i) {
        x[IDX(0, i)]     = (b == 1) ? -x[IDX(1, i)]     : x[IDX(1, i)];
        x[IDX(N-1, i)]   = (b == 1) ? -x[IDX(N-2, i)]   : x[IDX(N-2, i)];
        x[IDX(i, 0)]     = (b == 2) ? -x[IDX(i, 1)]     : x[IDX(i, 1)];
        x[IDX(i, N-1)]   = (b == 2) ? -x[IDX(i, N-2)]   : x[IDX(i, N-2)];
    }
    // corners
    x[IDX(0,0)]     = 0.5f * (x[IDX(1,0)]     + x[IDX(0,1)]);
    x[IDX(0,N-1)]   = 0.5f * (x[IDX(1,N-1)]   + x[IDX(0,N-2)]);
    x[IDX(N-1,0)]   = 0.5f * (x[IDX(N-2,0)]   + x[IDX(N-1,1)]);
    x[IDX(N-1,N-1)] = 0.5f * (x[IDX(N-2,N-1)] + x[IDX(N-1,N-2)]);
}

void Fluid::project(std::vector<float>& u, std::vector<float>& v,
                    std::vector<float>& p, std::vector<float>& div) {
                        
    #pragma omp parallel for collapse(2)
    for (int j = 1; j < N - 1; ++j) {
        for (int i = 1; i < N - 1; ++i) {
            div[IDX(i,j)] = -0.5f * (
                u[IDX(i+1,j)] - u[IDX(i-1,j)] +
                v[IDX(i,j+1)] - v[IDX(i,j-1)]
            ) / N;
            p[IDX(i,j)] = 0.0f;
        }
    }
    setBound(0, div);
    setBound(0, p);
    linearSolve(0, p, div, 1.0f, 4.0f);

    #pragma omp parallel for collapse(2)
    for (int j = 1; j < N - 1; ++j) {
        for (int i = 1; i < N - 1; ++i) {
            u[IDX(i,j)] -= 0.5f * (p[IDX(i+1,j)] - p[IDX(i-1,j)]) * N;
            v[IDX(i,j)] -= 0.5f * (p[IDX(i,j+1)] - p[IDX(i,j-1)]) * N;
        }
    }
    setBound(1, u);
    setBound(2, v);
}
void Fluid::advect(int b, std::vector<float>& d, const std::vector<float>& d0, const std::vector<float>& u, const std::vector<float>& v) {
    float dt0 = dt * (N - 2);

    #pragma omp parallel for collapse(2)
    for (int j = 1; j < N - 1; ++j) {
        for (int i = 1; i < N - 1; ++i) {
            float x = i - dt0 * u[IDX(i, j)];
            float y = j - dt0 * v[IDX(i, j)];

            if (x < 0.5f) x = 0.5f;
            if (x > N + 0.5f) x = N + 0.5f;
            int i0 = static_cast<int>(x);
            int i1 = i0 + 1;
            
            if (y < 0.5f) y = 0.5f;
            if (y > N + 0.5f) y = N + 0.5f;
            int j0 = static_cast<int>(y);
            int j1 = j0 + 1;

            float s1 = x - i0;
            float s0 = 1 - s1;
            float t1 = y - j0;
            float t0 = 1 - t1;

            d[IDX(i, j)] =
                s0 * (t0 * d0[IDX(i0, j0)] + t1 * d0[IDX(i0, j1)]) +
                s1 * (t0 * d0[IDX(i1, j0)] + t1 * d0[IDX(i1, j1)]);
        }
    }
    setBound(b, d);      
}

void Fluid::fade() {
    #pragma omp parallel for
    for (int i = 0; i < N*N; ++i) {
        dens[i] *= 0.99f;
    }
}

