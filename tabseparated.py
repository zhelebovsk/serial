import csv
from matplotlib import pyplot as plt
import re
import numpy as np

class Point:
    def __init__(self, xy, vel):
        self.xy = xy
        self.vel = vel
        self.mean = np.mean(self.vel)
        self.sigma = np.std(self.vel[3:-3])


if __name__ == '__main__':
    p = []
    with open('text.txt', 'r') as f:
        reader = csv.reader(f)
        for row in reader:
            vel = str(row)[2:-3].split(r'\t')[1:]
            xy = vel.pop(0)[1:-1].split(',')
            xy = [re.sub(r'\W', '', st) for st in xy]
            xy = list(map(int, xy))
            vel[-1] = vel[-1][:-1]
            vel = list(map(float, vel))
            vel.sort()
            p.append(Point(xy, vel))

    #splits = np.array_split(p, 5)
    #for count, i in enumerate(splits):
    #    if count % 2 != 0:
    #        splits[count] = np.flip(i)

    A = np.zeros([21, 5])
    for i in p:
        A[i.xy[0], i.xy[1]] = i.mean

    plt.imshow(A)
    plt.show()