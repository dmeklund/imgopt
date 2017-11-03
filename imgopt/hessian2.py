import autograd.numpy as anp
from autograd import elementwise_grad, jacobian

def elementwise_hess(fun, argnum=0):
    """
    From https://github.com/HIPS/autograd/issues/60
    """
    def sum_latter_dims(x):
        return anp.sum(x.reshape(x.shape[0], -1), 1)

    def sum_grad_output(*args, **kwargs):
        return sum_latter_dims(elementwise_grad(fun)(*args, **kwargs))
    return jacobian(sum_grad_output, argnum)
