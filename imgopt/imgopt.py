"""
Image deconstruction.
"""
from __future__ import absolute_import, division, print_function, unicode_literals

import pyipopt
# import numpy as np
import autograd.numpy as anp
import autograd.numpy as np

from solver import IpoptSolver


def construct_image(x, user_data):
    assert len(x) % 5 == 0
    num_lines = len(x) // 5
    reconstructed_image = anp.zeros((user_data.nx, user_data.ny, 3))
    (alpha_vals_0, alpha_vals_1, alpha_vals_2,
     sigma_vals,
     a_vals, b_vals, c_vals) = anp.split(x, 7)
    # alpha_vals_0 = x[:num_lines]
    # alpha_vals_1 = x[num_lines:2*num_lines]
    # alpha_vals_2 = x[]
    # sigma_vals = x[num_lines:2 * num_lines]
    # a_vals = x[2 * num_lines:3 * num_lines]
    # b_vals = x[3 * num_lines:4 * num_lines]
    # c_vals = x[4 * num_lines:5 * num_lines]
    # scale_factor = x[-1]
    xvals = anp.arange(user_data.nx, dtype=np.float64)
    yvals = anp.arange(user_data.ny, dtype=np.float64)
    xvals, yvals = np.meshgrid(xvals, yvals, indexing='xy')
    for ind in range(num_lines):
        line = 1.0 / sigma_vals[ind] * anp.exp(
                -.5 / sigma_vals[ind] ** 2 *
                (a_vals[ind] * xvals + b_vals[ind] * yvals + c_vals[ind]) ** 2 / (a_vals[ind] ** 2 + b_vals[ind] ** 2))
        full_line = np.array([alpha_vals_0[ind]*line, alpha_vals_1[ind]*line, alpha_vals_2[ind]*line])
        reconstructed_image = reconstructed_image + full_line.T
    return reconstructed_image

def eval_f(x, user_data):
    reconstructed_image = construct_image(x, user_data)
    objective_val = np.sum((reconstructed_image - user_data.actual_image)**2)
    return objective_val

from collections import namedtuple
UserData = namedtuple('UserData', ['nx', 'ny', 'actual_image'])

def run_opt():
    orig_input = np.zeros((255,255,3), dtype=np.uint8)
    xvals, yvals = np.meshgrid(np.arange(255, dtype=float), np.arange(255, dtype=float), indexing='xy')
    rvals = np.sqrt((xvals - 125)**2 + (yvals - 125)**2)
    orig_input[:,:,0] = 200 * np.exp(-.5 / 30**2 * rvals**2)
    orig_input[:,:,1] = 150 * np.exp(-.5 / 15**2 * rvals**2)
    orig_input[:,:,2] = 180 * np.exp(-.5 / 88**2 * rvals**2)
    num_lines = 10
    num_vars = num_lines * 7
    x_L = -1e28 * np.ones(num_vars)
    x_U = 1e28 * np.ones(num_vars)
    solver = IpoptSolver(eval_f, num_vars, x_L, x_U)
    x0 = np.zeros(num_vars)
    user_data = UserData(255, 255, orig_input)
    solver.solve(x0, user_data)
    # nlp = pyipopt.create(n, xl, xu, m, gl, gu, nnzj, nnzh, eval_f, eval_grad_f, eval_g, eval_jac_g)


if __name__ == '__main__':
    run_opt()
