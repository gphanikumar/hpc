#include <sycl/sycl.hpp>

__SYCL_ALWAYS_INLINE
static void Anisotropy_calc(double *Aniso, double *Aniso_gradient, const double *DERX, const double *DERY, const double *epsilon) {
    const double dx = *DERX;
    const double dy = *DERY;
    const double denom = dx * dx + dy * dy;

    if (denom > 1e-8) {
        const double numer = dx * dx * dx * dx + dy * dy * dy * dy;
        *Aniso = 1.0 - 3.0 * (*epsilon) + 4.0 * (*epsilon) * (numer / denom);
        *Aniso_gradient = -16.0 * (*epsilon) *dx * dy * ((dx * dx - dy * dy)/denom);
    } else {
        *Aniso = 1.0 - 3 * (*epsilon);
        *Aniso_gradient = 0.0;
    }
}

