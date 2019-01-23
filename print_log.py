#  import matplotlib
#  matplotlib.use("agg")
#  import tkinter
import matplotlib.pyplot as plt
import numpy as np
import os
#  import csv
import pandas as pd

def plot_single_arr(arr1, yl1='', xl1='', leg1=''):
    x = range(0, len(arr1))
    plt.plot(x, arr1, 'r', label=leg1)
    plt.xlabel(xl1)
    plt.ylabel(yl1)
    plt.legend()
    plt.show()

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

def plot_two_arrs_no_sub(arr1,arr2):
    
    # create 2 plots w/ shared x axis
    #  f, arrs = plt.subplots(1, sharex=True)
    x = range(0, len(arr1))

    labels = [ 'player1', 'player2']
    plt.plot(x, arr1, 'r', label='player 1')
    plt.ylabel('accumulated payoffs')
    plt.xlabel('round(s)')
    plt.legend(labels)
    
    plt.plot(x, arr2, 'b', label='player 2')

    plt.show()
# plot cpu usage of node and wallet in each test step  
def plot_cpu_arr(arr1, arr2, arr_num):
    # create 3 plots w/ shared x axis
    f, arrs = plt.subplots(3, sharex=True)
    x = range(0, len(arr1))

    arrs[0].plot(x, arr1, 'r', label='node')
    arrs[0].set_ylabel('CPU usage of node(%)')
    arrs[0].legend()
    
    arrs[1].plot(x, arr2, 'b', label='wallet')
    arrs[1].set_ylabel('CPU usage of wallet(%) ')
    arrs[1].legend()

    arrs[2].plot(x, arr_num, 'g', label='steps')
    arrs[2].set_ylabel('step number')
    arrs[2].set_xlabel('time step (0.1sec)')
    arrs[2].legend()

    plt.show()

# read accumulated regrets from the csv file
def regret_visualization(filename, players=2):
  data = []  
  #  data_2 = []
  if os.path.exists(filename):
    with open(filename, 'r') as f: 
      for line in f:
        for i in range(players):
            regret = line.rstrip('\n')  
            data_1.append((int)(regret))
  plot_single_arr(data_1, xl1='rounds', yl1='accumulated regrets', leg1='single player')

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
def csv_reader(filename):
    data = []
    df = pd.read_csv(filename)
    col_len = df.shape[1] - 1
    for i in range(col_len):
        data.append(df.iloc[:,i])
    print(data)    
    plot_n_arrs(data, "rounds", "acc_regrets", "player")
def plot_n_arrs(arr,xl1='xlabel', yl1='y1label', leg1='legend'):
    players = len(arr)
    x = range(0, len(arr[0]))

    plt.ylabel(yl1)
    plt.xlabel(xl1)
    for i in range(players):
        plt.plot(x, arr[i], 'r', label=leg1)

    plt.show()
# TESTING 
# configuration
filename = 'regret.csv'
csv_reader(filename)
#  regret_visualization(filename, 3)
#  filename = 'acc_payoffs.csv'
#  two_vectors_visualization(filename, split=False)
#  filename = 'action_history.csv'
#  two_vectors_visualization(filename, split=True)
