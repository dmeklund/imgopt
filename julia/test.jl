using Convex;
m = 4; n = 5
A = randn(m, n); b = randn(m, 1)
x = Variable(n)

problem = minimize(sumsquares(A * x - b), [x >= 0])
solve!(problem)

problem.status

problem.optval
