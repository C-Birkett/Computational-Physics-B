# Code by Conan Birkett written to perform data analysis & graphing on results from the cpp DLASystem

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import math

# import data from simulation
data = pd.read_csv('data.csv',
        header = None,
        #names = ('Nc', 'R'),
        )

# annoying pyright error msg when accessing data so do this
numSims = data.shape[1] - 1     # = numcols - 1
Nc = np.array(data.iloc[:, 0])
#R = np.array(data.iloc[:, 1])
R = data.iloc[:, 1:(numSims+1)]

R_mean = np.array(R.mean(axis=1))
R_err = np.array(R.sem(axis=1))

# simple model:
# Nc = (R/a)^df
# => fractal dimension (df) = ln(Nc)/ln(R/a), a is length scale, set a = 1

# better model:
# Nc(R) = (alpha*R)^df + beta
# => df = (1 + beta/(alpha*R)^df) * (ln(Nc)/ln(R))

# Retrieves simple case for beta = 0 and large R, a becomes irrelevant so will set as 1
# later can estimate offset beta

# assuming 'large' r
def fractal_dim(n, r):
    #a = 1
    return math.log(n)/math.log(r)

# error propagation: f = 1/log(x)
# => Df ~ Dx / x log^2(x)
def fractal_dim_err(r, r_err):
    # assuming error in n = 0 => log(n_err) = 1
    return r_err / (r * math.pow(math.log(r),2))

df = np.empty(Nc.size)
df_err = np.empty(Nc.size)
for i in np.arange(0, Nc.size, 1):
    df[i] = fractal_dim(Nc[i], R_mean[i])
    df_err[i] = fractal_dim_err(R_mean[i], R_err[i])


# figures
fig = plt.figure(figsize=(10,10))

ax = fig.add_subplot(111)

ax.errorbar(Nc,
        df,
        yerr = df_err,
        marker = 'x',
        markersize = 7,
        color = 'black',
        ecolor = 'black',
        markerfacecolor = 'black',
        capsize = 6,
        )

ax.set_xlabel('Number of particles')
ax.set_ylabel('Dimension of fractal')

ax.set_title('Fractal dimension')

plt.show()
