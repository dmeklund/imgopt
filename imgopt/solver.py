import autograd
import autograd.numpy as anp
import numpy as np
import pyipopt


class IpoptSolver(object):
    def __init__(
            self,
            objective_func,
            num_var,
            x_L,
            x_U
            # num_constraints,
            # g_L,
            # g_U,
            # nnzj,
            # nnzh):
    ):
        self._objective_func = objective_func
        self._num_var = num_var
        self._x_L = x_L
        self._x_U = x_U
        # self._num_constraints = num_constraints
        # self._g_L = g_L
        # self._g_U = g_U
        # self._nnzj = nnzj
        # self._nnzh = nnzh
        self._eval_grad_f = autograd.grad(self._objective_func)
        self._hess = autograd.hessian(self._objective_func)

    def eval_f(self, x, user_data=None):
        return self._objective_func(x)

    def eval_g(self, x, user_data=None):
        return np.zeros(0)

    def eval_jac_g(self, x, flag, user_data=None):
        if flag:
            return (np.zeros(0), np.zeros(0))
        else:
            return np.zeros(0)

    def eval_h(self, x, lagrange, obj_factor, flag, user_data=None):
        if flag:
            hrow = np.tile(np.arange(self._num_var), self._num_var)
            hcol = np.repeat(np.arange(self._num_var), self._num_var)
            return (hrow, hcol,)
        else:
            return obj_factor * self._hess(x)

    def solve(self, x0, user_data=None):
        nlp = pyipopt.create(
            self._num_var,
            self._x_L,
            self._x_U,
            0,
            np.zeros(0),
            np.zeros(0),
            0,
            self._num_var * self._num_var,
            self.eval_f,
            self._eval_grad_f,
            self.eval_g,
            self.eval_jac_g,
            self.eval_h
        )
        x, zl, zu, constraint_mult, obj, status = nlp.solve(x0, user_data)
        import ipdb; ipdb.set_trace()
        print("Solution: {}".format(x))


def test_ipopt():
    objective_func = lambda x0: anp.sum(x0**2)
    solver = IpoptSolver(objective_func, 2, np.array([1.0, 2.0]), np.array([5.0, 8.0]))
    solver.solve(np.array([2.5, 4.8]))


if __name__ == '__main__':
    test_ipopt()
