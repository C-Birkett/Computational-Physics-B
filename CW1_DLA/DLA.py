# Code by candidate# 24669 written to perform data analysis & graphing on results from the cpp DLASystem

# Please note this code is incredibly messy and will fail, it is not designed to run well, it is designed to get me the results and graphs I need
# A good amount of this code is redundant
# to choose a dataset set the 'dataset' variable below
# I would only reccomend setting it to 0 or 4 as these are the working datasets I used for my final report

import numpy as np
import pandas as pd
import matplotlib
matplotlib.use('TkAgg')
import matplotlib.pyplot as plt
import math
from scipy.optimize import curve_fit

datasets = ['data_100x20k.csv','data_100x20k_stick.csv','data_20x50k.csv','data_90k.csv', 'stick_data_100x5k.csv']

#choose dataset here
dataset = 4

# import data from simulation
data = pd.read_csv(datasets[dataset],
        header = None,
        )

# annoying pyright error msg when accessing data so do this
numSims = data.shape[1] - 1     # = numcols - 1
Nc = np.array(data.iloc[:, 0])
#R = np.array(data.iloc[:, 1])
R = data.iloc[:, 1:(numSims+1)]

print(R.shape)

R_mean = np.array(R.mean(axis=1))
R_err = np.array(R.std(axis=1)) #use standard deviation

#vector of vectors that gets mean of every interval of 10 columns
#i.e means of r for different sticking probs
if dataset == 1: probDiff = 10
else: probDiff = 100
#init as zeros with correct size
R_probs_mean = np.array([np.zeros(Nc.size)])
for i in np.arange(0, 10, 1):
    tmp = R.iloc[:, i*probDiff:((i+1)*probDiff)]
    R_probs_mean = np.vstack([R_probs_mean, np.array(tmp.mean(axis=1))])

#delete zeros vector
R_probs_mean = np.delete(R_probs_mean, 0,0)

#95% confidence intervals:
#assumes R are normally distributed!
R_conf95 = 1.96*(1/math.sqrt(numSims))*R_err

R_upper = R_mean + R_conf95
R_lower = R_mean - R_conf95

R_err_upper = R_mean + R_err
R_err_lower = R_mean - R_err

# simple model:
# Nc = (R/a)^df
# => fractal dimension (df) = ln(Nc)/ln(R/a), a is length scale, set a = 1

# better model:
# Nc(R) = (alpha*R)^df + beta
# => df = (1 + beta/(alpha*R)^df) * (ln(Nc)/ln(R))

# Retrieves simple case for beta = 0 and large R, a becomes irrelevant so will set as 1
# later can find beta and alpha by fitting

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


#95% confidence intervals:
#assumes normal distribution
df_conf95 = 1.96*(1/math.sqrt(numSims))*df_err

df_upper = df + df_conf95
df_lower = df - df_conf95

df_err_upper = df + df_err
df_err_lower = df - df_err

#expected relation for curve fitting
def modelFunc(r, d, a, b):
    return np.power(a*r, d) + b

#set dimension with 'known' result for plot
def modelFuncKnownD(r, a, b):
    return np.power(a*r, 1.733) + b

# figures
font = {'size': 18}

fig = plt.figure(figsize=(6,6))

plt.rc('font', **font)

ax = fig.add_subplot(111)

if dataset == 0:
    
    #badly named df_mean stores df for each cluster -> becomes mean
    df_mean = np.array([])
    df_fit_err = np.array([])
    
    #ratio of logs
    alt_mean = np.array([])

    for i in np.arange(0,numSims,1):
        tmpR = np.array(R.iloc[:,i])
        popt, pcov = curve_fit(modelFunc, tmpR, Nc, p0 = [1.7,1,0],)
        df_mean = np.append(df_mean, popt[0])
        df_fit_err = np.append(df_fit_err, np.sqrt(np.diag(pcov)[0])) #std dev in fit
        alt_mean = np.append(alt_mean, fractal_dim(Nc[-1], tmpR[-1]))
        
    df_std = np.std(df_mean)
    df_mean = np.mean(df_mean)
    df_fit_err = np.mean(df_fit_err)
    df_err = np.sqrt(np.power(df_std, 2) + np.power(df_fit_err, 2))
    df_conf95 = 1.96*(1/math.sqrt(numSims))*df_err

    print("Fractal dim = ", df_mean, " pm ", df_err, " confidence interval ", df_conf95)

    alt_conf95 = 1.96*(1/math.sqrt(numSims))*np.std(alt_mean)
    alt_mean = np.mean(alt_mean)

    print("ratio of logs method: d = ", alt_mean, " pm ", alt_conf95)

    # curve fit to mean
    popt, pcov = curve_fit(modelFunc, R_mean, Nc, p0 = [1.7,1,0],) #bounds =(1000,Nc[-1]))

    #results of fit
    print("df fit to mean r = ", popt[0])

    #standard deviation from fit
    #assuming a normal distribution here!
    perr = np.sqrt(np.diag(pcov))
    print("fit error = ", perr)

    #95% confidence interval
    pconf95 = 1.96*(1/math.sqrt(numSims))*perr
    print("confidence interval = ", pconf95[0])

    #plot data
    ax.plot(Nc,
            R_mean,
            marker = 'x',
            markersize = 4,
            color = 'black',
            markerfacecolor = 'black',
            )

    #all of these are pretty much the same

    '''#plot mean fit
    ax.plot(modelFunc(R_mean, *popt),
            R_mean,
            color = 'red'
            )
    '''

    #fit to mean known d
    popt, pcov = curve_fit(modelFuncKnownD, R_mean, Nc, p0 = [1,0],) #bounds =(1000,Nc[-1]))

    '''#plot fit known d
    ax.plot(modelFuncKnownD(R_mean, *popt),
            R_mean,
            color = 'red'
            )
    '''

    #plot fit simple relation
    ax.plot(np.power(R_mean, alt_mean),
            R_mean,
            color = 'blue'
            )



    ''' confidence intervals for R are too small so omit
    ax.plot(Nc,
            #R_err_upper,
            R_upper,
            marker = '.',
            markersize = 1,
            color = 'blue',
            markerfacecolor = 'blue',
            )

    ax.plot(Nc,
            #R_err_lower,
            R_lower,
            marker = '.',
            markersize = 1,
            color = 'blue',
            markerfacecolor = 'blue',
            )
    #'''

    #make figures square
    ax.set_aspect(1./ax.get_data_ratio())

    ax.set_xlabel('Number of particles $N_c$')
    ax.set_ylabel('Size of cluster $r_{max}$')

    plt.show()

#sticking stuff
prob_dims = np.array([])
prob_dims_err = np.array([])

alt_dims = np.array([])
alt_dims_err = np.array([])

colors = ['tab:grey', 'tab:brown', 'tab:orange', 'tab:olive', 'tab:green','tab:cyan', 'tab:blue', 'tab:purple', 'tab:pink', 'tab:red']

for i in np.arange(0,10,1):

    #skip first 2k for fit
    #Nc_trunc = Nc[50:]
    #R_trunc = R_probs_mean[i][50:]
    
    #fit to data
    mean_popt, mean_pcov = curve_fit(modelFunc, R_probs_mean[i], Nc, p0 = [1.7,1,0])
    #popt, pcov = curve_fit(modelFunc, R_trunc, Nc_trunc, p0 = [1.7,1,0])

    df_mean = np.array([])
    df_fit_err = np.array([])
    alt_mean = np.array([])
    for n in np.arange(0,probDiff,1):
        #print("i = ", i, "n = ", n)
        tmpR = np.array(R.iloc[:,(probDiff*i + n)])
        popt, pcov = curve_fit(modelFunc, tmpR, Nc, p0 = [1.7,1,0], maxfev = 1600)
        df_mean = np.append(df_mean, popt[0])
        df_fit_err = np.append(df_fit_err, np.sqrt(np.diag(pcov)[0])) #std dev in fit
        alt_mean = np.append(alt_mean, fractal_dim(Nc[-1], tmpR[-1]))
        
    df_std = np.std(df_mean)
    df_mean = np.mean(df_mean)
    df_fit_err = np.mean(df_fit_err)
    df_err = np.sqrt(np.power(df_std, 2) + np.power(df_fit_err, 2))
    df_conf95 = 1.96*(1/math.sqrt(probDiff))*df_err

    prob_dims = np.append(prob_dims, df_mean)
    prob_dims_err = np.append(prob_dims_err, df_conf95)

    alt_dims = np.append(alt_dims, np.mean(alt_mean))
    alt_conf95 = 1.96*(1/math.sqrt(probDiff))*np.std(alt_mean)
    alt_dims_err = np.append(alt_dims_err, alt_conf95)

    #plot data (mean)
    ax.plot(Nc,
            R_probs_mean[i],
            marker = 'x',
            markersize = 4,
            color = 'black',
            markerfacecolor = 'black',
            )

    #change popt dim to mean dim -> mean dim does not fit mean R !
    #mean_popt = [prob_dims[i], *mean_popt[1:]]
    #print(mean_popt)

    #plot fit (mean)
    ax.plot(modelFunc(R_probs_mean[i], *mean_popt),
            R_probs_mean[i],
            color = colors[i]
            )
    

    '''#plot fit
    ax.plot(modelFunc(R_trunc, *popt),
            R_trunc,
            color = 'red'
            )
    '''

print("sticking probability dimensions w/ err")
print(prob_dims)
print(prob_dims_err)

print("alternate method:")
print(alt_dims)
print(alt_dims_err)

#make figures square
ax.set_aspect(1./ax.get_data_ratio())

ax.set_xlabel('Number of particles $N_c$')
ax.set_ylabel('Size of cluster $r_{max}$')

plt.show()

# dimension vs sticking prob
fig = plt.figure(figsize=(6,6))

plt.rc('font', **font)

ax = fig.add_subplot(111)


ax.errorbar(np.arange(1,0,-0.1),
        prob_dims,
        yerr=prob_dims_err,
        marker = 'x',
        markersize = 7,
        color = 'black',
        markerfacecolor = 'black',
        ecolor = 'black',
        capsize = 4,
        linestyle = 'none',
        )

ax.errorbar(np.arange(1,0,-0.1),
        #prob_dims,
        #yerr=prob_dims_err,
        alt_dims,
        yerr = alt_dims_err,
        marker = 'o',
        markersize = 7,
        color = 'blue',
        markerfacecolor = 'blue',
        ecolor = 'blue',
        capsize = 4,
        linestyle = 'none',
        )

ax.set_aspect(1./ax.get_data_ratio())

ax.set_xlabel('Probability of particle adhesion')
ax.set_ylabel('DLA cluster dimension $d_f$')

plt.show()
