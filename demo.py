#!/usr/bin/env python

import numpy as np
from matplotlib import pyplot as plt
import matplotlib

def read_txt(filename):
    words = []
    time = []
    with open(filename, 'r') as file_to_read:
        while True:
            lines = file_to_read.readline()
            if not lines:
                break
                pass
            word = [i for i in lines.split()]
            words.append(word)
            pass
    for index in range(len(words)):
        if index % 7 == 6:
            time.append(int(words[index][-2]))
    return time

def read_txt2(filename):
    words = []
    time = []
    with open(filename, 'r') as file_to_read:
        while True:
            lines = file_to_read.readline()
            if not lines:
                break
                pass
            word = [i for i in lines.split()]
            words.append(word)
            pass
    for index in range(len(words)):
        if index % 9 == 7:
            time.append(int(words[index][3][:-2]))
    return time

def display2(times1,times2):
    fig=plt.figure()
    size=len(times1)
    x = np.arange(size)
    ax1=fig.add_subplot(1,1,1)
    ax1.bar(x,times2)
    ax1.yaxis.set_ticks(times2)
    plt.show()


def display(times1,times2):

    size=len(times2)
    x = np.arange(size)
    plt.figure(figsize=(10, 6))
    total_width, n = 0.8, 2
    width = total_width / n
    x = x - (total_width - width) / 2

    plt.bar(x, times1, width=width, facecolor='lightskyblue',label='old')
    plt.bar(x + width,times2, width=width, facecolor='darkgreen',label='new')
    plt.grid(True)
    matplotlib.rc('xtick', labelsize=20)
    matplotlib.rc('ytick', labelsize=20)
    matplotlib.rcParams.update({'font.size': 22})
    plt.ylabel("running time/ms")
    plt.xlabel("sample No.")
    plt.title("algorithm running time/ms")
    plt.legend()
    plt.show()



if __name__ == '__main__':
    filename1 = 'test4.txt'
    filename2 = 'test3.txt'
    times1=read_txt(filename1)
    times2=read_txt(filename2)
    times1.sort(reverse=True)
    times2.sort(reverse=True)
    display(times1,times2)

