# Code by candidate# 24669 written to perform data analysis & graphing on results from the cpp IsingSystem
# to choose a dataset set the 'dataset' variable below


# TODO:
# plot graphs for Ex1_data_2
# start write-up -> write up Ex1

# get data for Ex2 -> energy and magnetisation at equilibrium vs temperature
# maybe make data match Ex3 data
# plot Ex data look at theory and fit relationship or something

# Ex3 -> figure out given data & how to import
# plot against Ex2 stuff (I think?)

# Ex4 -> need to get variance -> may as well just do it in python instead of C++
# derive (and plot?) heat capacity and susceptability
# compare vs given mean field data
# check -> heat capacity should be dE/dT

# Ex5 -> do correlation function if I have time

import numpy as np
import pandas as pd
import matplotlib
matplotlib.use('TkAgg')
import matplotlib.pyplot as plt
import math
from scipy.optimize import curve_fit
from collections import deque as deque

# figure setup
font = {'size': 18}
colors = ['tab:grey', 'tab:brown', 'tab:orange', 'tab:olive', 'tab:green','tab:cyan', 'tab:blue', 'tab:purple', 'tab:pink', 'tab:red']

datasets = ['data.csv', 'Ex1_data_1.csv', 'Ex1_data_2']

# choose dataset here, 0 for just generated
# this is basically to keep analysis seperate between results
dataset = 1

# import data from simulation
data = pd.read_csv(datasets[dataset],
        header = None,
        )

# 2 cols for each sim + 1 col numSweeps
numSims = (data.shape[1] - 1)/2     # = (numcols - 1)/2

sweep = np.array(data.iloc[:, 0])
numSweeps = np.size(sweep)

# System magnetisation
magnetisation = data.iloc[:, np.arange(1, (2*numSims)+1, 2)].copy(deep=True)
# fix indices
magnetisation.reset_index(inplace=True, drop=True)
magnetisation = magnetisation.T # Transpose to index by [sim#][sweep#]
magnetisation.reset_index(inplace=True, drop=True)

# System Energy per spin indexed by [sim#][sweep#]
energy = data.iloc[:, np.arange(2, (2*numSims)+2, 2)].copy(deep=True)

#fix indices
energy.reset_index(inplace=True, drop=True)
energy = energy.T
energy.reset_index(inplace=True, drop=True)

print('Imported data for ', numSims, ' simulations. Magnetisation and Energy datasets are size:')
print(magnetisation.shape)
print(energy.shape)
print('number of MC sweeps taken: ', numSweeps)


# Exercise 1: convergence to steady state
# near t = 2.5 -> 3 appears to not converge as fast -> critical temperature?
if dataset == 1:
    # look at convergence of magnetisation and energy by mean over many sims
    print('Examining convergence of magnetisation and energy:')
    
    # recorded at 9 temperatures 1 -> 5
    temperature = deque([])
    temperatureStart = 1
    temperatureDiff = 0.5


    meanMagnetisation = deque([])
    meanEnergy = deque([])

    # 100 sims recorded at each temperature value
    for i in np.arange(0, 9, 1):
        temperature.append(temperatureStart + i*temperatureDiff)
        meanMagnetisation.append(deque([]))
        meanEnergy.append(deque([]))

        for j in np.arange(0, numSweeps, 1):
            tmpM = magnetisation[100*i:100*(i+1)]
            meanMagnetisation[i].append(tmpM[j].mean())

            tmpE = magnetisation[100*i:100*(i+1)]
            meanEnergy[i].append(tmpE[j].mean())


    meanMagnetisation = np.asarray(meanMagnetisation)
    meanEnergy = np.asarray(meanEnergy)
    
    print('shape of mean values over temperature: ')
    print(meanMagnetisation.shape)
    print(meanEnergy.shape)

    # plot magnetisation and energy convergence to steady state

    # magnetisation
    fig = plt.figure(figsize=(6,6))
    plt.rc('font', **font)
    ax = fig.add_subplot(111)
    for i in np.arange(0,9,1):
        ax.plot(sweep,
                meanMagnetisation[i],
                color = colors[i+1],
                marker = 'x',
                markersize = 5,
                )

    ax.set_aspect(1./ax.get_data_ratio())

    ax.set_xlabel('Number of Monte Carlo sweeps')
    ax.set_ylabel('System magnetisation')

    plt.show()

    # energy
    fig = plt.figure(figsize=(6,6))
    plt.rc('font', **font)
    ax = fig.add_subplot(111)
    for i in np.arange(0,9,1):
        ax.plot(sweep,
                meanEnergy[i],
                color = colors[i+1],
                marker = 'x',
                markersize = 5,
                )

    ax.set_aspect(1./ax.get_data_ratio())

    ax.set_xlabel('Number of Monte Carlo sweeps')
    ax.set_ylabel('System energy')

    plt.show()

