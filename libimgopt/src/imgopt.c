#include <assert.h>
#include <gc.h>
#include <IpStdCInterface.h>
#include <stddef.h>
#include <string.h>
#include <math.h>

#define xmalloc(sz) GC_MALLOC(sz)

struct ImgOptUserData
{
    size_t nx;
    size_t ny;
    double *target_data;
    size_t num_lines;
};

size_t varsPerLine = 7;
//size_t num_channels = 3;
static int reconstruct_image(const Number *x, const struct ImgOptUserData *params, double *image_out)
{
    const size_t nx = params->nx;
    const size_t ny = params->ny;
    const size_t num_lines = params->num_lines;
    memset(image_out, 0, nx*ny*sizeof *image_out);

    for (size_t line_ind = 0; line_ind < num_lines; ++line_ind)
    {
        const double alpha0 = x[line_ind*varsPerLine];
        const double alpha1 = x[line_ind*varsPerLine+1];
        const double alpha2 = x[line_ind*varsPerLine+2];
        const double sigma = x[line_ind*varsPerLine+3];
        const double aval = x[line_ind*varsPerLine+4];
        const double bval = x[line_ind*varsPerLine+5];
        const double cval = x[line_ind*varsPerLine+6];
        for (size_t ix = 0; ix < nx; ++ix)
        {
            const double xval = ix;
            for (size_t iy = 0; iy < ny; ++iy)
            {
                const double yval = iy;
                const double coeff = (aval*xval + bval*yval + cval);
                double val = 1.0 / sigma * exp(-.5 / (sigma*sigma) * coeff*coeff / (aval*aval + bval*bval));
                image_out[3*iy + 3*ny*ix] += alpha0 * val;
                image_out[1 + 3*iy + 3*ny*ix] += alpha1 * val;
                image_out[2 + 3*iy + 3*ny*ix] += alpha2 * val;
            }
        }
    }
    return 0;
}

static Bool eval_f(Index n, Number *x, Bool new_x, Number *obj_val, UserDataPtr user_data)
{
    (void)n;
    (void)new_x;
    const struct ImgOptUserData *params = user_data;
    const size_t nx = params->nx;
    const size_t ny = params->ny;
    double *reconstructed = xmalloc(3*nx*ny*sizeof *reconstructed);
    reconstruct_image(x, params, reconstructed);
    const size_t len = 3*nx*ny;
    *obj_val = 0;
    const double *orig = params->target_data;
    for (size_t img_ind = 0; img_ind < len; ++img_ind)
    {
        const double delta = orig[img_ind] - reconstructed[img_ind];
        *obj_val += delta * delta;
    }
    return TRUE;
}


extern Bool eval_grad_f(Index n, Number *x, Bool new_x, Number *grad_f, UserDataPtr user_data)
{
    (void)n;
    (void)new_x;
    const struct ImgOptUserData *params = user_data;
    const size_t nx = params->nx;
    const size_t ny = params->ny;
    const size_t num_lines = params->num_lines;
    double *reconstructed = xmalloc(3*nx*ny*sizeof *reconstructed);
    reconstruct_image(x, params, reconstructed);
    const double *orig = params->target_data;
    size_t var_ind = 0;
    memset(grad_f, 0, num_lines * varsPerLine * sizeof *grad_f);
    for (size_t line_ind = 0; line_ind < num_lines; ++line_ind)
    {
        const double alpha[] = {x[line_ind*varsPerLine], x[line_ind*varsPerLine+1], x[line_ind*varsPerLine+2]};
        const double sigma = x[line_ind*varsPerLine+3];
        const double aval = x[line_ind*varsPerLine+4];
        const double bval = x[line_ind*varsPerLine+5];
        const double cval = x[line_ind*varsPerLine+6];
        for (size_t ix = 0; ix < nx; ++ix)
        {
            const double xval = ix;
            for (size_t iy = 0; iy < ny; ++iy)
            {
                const double yval = iy;
                for (size_t channel_ind = 0; channel_ind < 3; ++channel_ind)
                {
                    const double denom = aval*aval + bval*bval;
                    const double sigma2 = sigma*sigma;
                    const double sigma3 = sigma2*sigma;
                    // d Objective / d alpha_i
                    const double delta = orig[channel_ind+3*iy+3*ny*ix] - reconstructed[channel_ind+3*iy+3*ny*ix];
                    const double coeff = (aval*xval + bval*yval + cval);
                    const double exp_val = exp(-.5 / sigma2 * (coeff*coeff) / denom);
                    grad_f[channel_ind + line_ind*num_lines] += 2 * delta * (1/sigma * exp_val);
                    // dO/d sigma
                    grad_f[3 + line_ind*varsPerLine] +=
                            2 * delta * alpha[channel_ind] / sigma2
                            * ((aval*xval + bval*yval + cval)/(sigma2 * denom) - 1/sigma2)
                            * exp_val;
                    // dO/da_i
                    grad_f[4 + line_ind*varsPerLine] +=
                            2 * delta * alpha[channel_ind] / sigma
                            * ((aval*aval*xval + aval*bval*yval + aval*cval) / (sigma2 *denom*denom)
                               - xval / (2*sigma2*denom))
                            * exp_val;
                    // dO/db_i
                    grad_f[5 + line_ind*varsPerLine] +=
                            2 * delta * alpha[channel_ind] / sigma
                            * ((bval*aval*xval + bval*bval*yval + bval*cval) / (sigma2 * denom*denom)
                               - yval / (2*sigma2*denom))
                            * exp_val;
                    // dO/dc_i
                    grad_f[6 + line_ind*varsPerLine] += delta * alpha[channel_ind] / (sigma3 * denom) * exp_val;
                }
            }
        }
    }
    return TRUE;
}


static Bool eval_g(Index n, Number *x, Bool new_x, Index m, Number *g, UserDataPtr user_data)
{
    assert(m == 0);
    return TRUE;
}


static Bool eval_jac_g(
        Index n,
        Number *x,
        Bool new_x,
        Index m,
        Index nele_jac,
        Index *iRow,
        Index *jCol,
        Number *values,
        UserDataPtr user_data)
{
    assert(m == 0);
    return TRUE;
}

extern Bool eval_h(
        Index n,
        Number *x,
        Bool new_x,
        Number obj_factor,
        Index m,
        Number *lambda,
        Bool new_lambda,
        Index nele_hess,
        Index *iRow,
        Index *jCol,
        Number *values,
        UserDataPtr user_data)
{
    const struct ImgOptUserData *params = user_data;
    size_t num_lines = params->num_lines;
    long idx = 0;
    if (values == NULL)
    {
        for (size_t row = 0; row < varsPerLine*num_lines; ++row)
        {
            for (size_t col = 0; col <= row; ++col)
            {
                iRow[idx] = (int)row;
                jCol[idx] = (int)col;
                ++idx;
            }
        }
    }
    else
    {

    }
    return TRUE;
}