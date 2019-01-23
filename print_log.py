#  import matplotlib
#  matplotlib.use("agg")
#  import tkinter
import matplotlib.pyplot as plt
import numpy as np
import os
#  import csv
import pandas as pd

def plot_two_arrs(arr1,arr2,xl1='xlabel', yl1='y1label',yl2='y2label', leg1='legend1', leg2='legend2'):
    # create 2 plots w/ shared x axis
    f, arrs = plt.subplots(2, sharex=True)
    x = range(0, len(arr1))

    arrs[0].plot(x, arr1, 'r', label=leg1)
    arrs[0].set_ylabel(yl1)
    arrs[0].legend()
    
    arrs[1].plot(x, arr2, 'b', label=leg2)
    arrs[1].set_ylabel(yl2)
    arrs[1].set_xlabel(xl1)
    arrs[1].legend()

    plt.show()

# read accumulated payoffs of 2 players
def two_vectors_visualization(filename, split=True):
  data_1 = []
  data_2 = []
  if os.path.exists(filename):
    with open(filename, 'r') as f: 
      for line in f:
        acc1 = line.split(',')[0]
        acc2 = line.split(',')[1].rstrip('\n')
        data_1.append((int)(acc1))
        data_2.append((int)(acc2))
  if(split):
      plot_two_arrs(data_1, data_2, yl1='action no.', yl2='action no.', xl1='rounds', leg1='p1', leg2='p2')
  else:
      plot_two_arrs_no_sub(data_1, data_2)

def csv_reader(filename, x, y, l):
    data = []
    df = pd.read_csv(filename)
    col_len = df.shape[1] - 1
    for i in range(col_len):
        data.append(df.iloc[:,i])
    #  print(data)    
    plot_n_arrs(data, x, y, l)
def plot_n_arrs(arr,xl1='xlabel', yl1='y1label', leg1='legend'):
    players = len(arr)
    x = range(0, len(arr[0]))

    plt.ylabel(yl1)
    plt.xlabel(xl1)
    color = ['r','b','y','c','m','g','k']
    for i in range(players):
        str_leg = leg1 + str(i)
        plt.plot(x, arr[i], color[i], label=str_leg)

    plt.legend(loc='upper left')
    plt.show()
# TESTING 
# configuration
numberGames = 3
for i in range(numberGames):
    filename = 'regret' + str(i) + '.csv'
    csv_reader(filename, "rounds", "acc_regrets", "player")
#  filename = 'acc_payoffs.csv'
#  two_vectors_visualization(filename, split=False)
#  filename = 'action_history.csv'
#  two_vectors_visualization(filename, split=True)
