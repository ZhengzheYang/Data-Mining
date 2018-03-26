import numpy as np
import math
import random, copy
import sys

class kmeans:
    def __init__(self, k, data):
        self.data = data
        self.k = k
        self.means = []

    # Euclidean distance
    def getDistance(self, x, mean):
        return math.sqrt(np.sum((np.array(x) - np.array(mean)) ** 2))

    ## Normalize to be in [0, 1]
    def minMaxNormalize(self):
        for i in range(self.data.num_col):
            old_max = max(self.data.values[:, i])
            old_min = min(self.data.values[:, i])
            self.data.values[:, i] = (self.data.values[:, i] - old_min) / (old_max - old_min)

    # Get random initial means based on the number of k's
    def getRandomInitials(self):
        partitions = []
        indices = [i for i in range(self.data.num_row)]
        for i in range(self.k):
            index = random.choice(indices)
            partitions.append(self.data.values[index])

        return partitions

    # Check if it's terminal condition, that is the mean doesn't change
    def terminal(self, means, means_old):
        if not means_old:
            return False

        for part1, part2 in zip(means, means_old):
            if any(i != j for i, j in zip(part1, part2)):
                return False

        return True

    # The algorithm to the clustering
    def kmeans(self):
        self.minMaxNormalize()
        self.means = self.getRandomInitials()
        means_old = []
        clusters = np.zeros(len(self.data.values)).astype(int)
        while not self.terminal(self.means, means_old):
            means_old = copy.deepcopy(self.means)
            for i, x in enumerate(self.data.values):
                distances = [self.getDistance(x, mean) for mean in self.means]
                clusters[i] = np.argmin(distances)
            self.means = self.updateMeans(self.means, clusters)
        return clusters

    # update the means using a dict
    def updateMeans(self, means, clusters):
        new_means = []
        counter = dict()
        for i in range(len(means)):
            counter[i] = []
        for i, cluster in enumerate(clusters):
            counter[cluster].append(self.data.values[i])

        for key, value in counter.iteritems():
            if not value:
                new_means.append(np.zeros(len(means[0])))
                continue
            new_means.append(np.mean(value, axis=0))
        return new_means

    # Cluster evaluation using Squared Sum Error
    def evaluateSSE(self, clusters):
        sse = 0.0
        for i, cluster in enumerate(clusters):
            sse += np.sum((self.data.values[i] - self.means[cluster]) ** 2)
        return sse

    # test only
    def printClusters(self, clusters):
        cluster = {}
        for i in range(len(self.means)):
            cluster[i] = []
        for i, x in enumerate(clusters):
            cluster[x].append(self.data.values[i])
        for key, value in cluster.iteritems():
            print(key, value)

class loadData:
    def __init__(self, values, labels, num_row, num_col, mean):
        self.values = values
        self.labels = labels
        self.num_row = num_row
        self.num_col = num_col
        self.mean = mean

    @classmethod
    def loadDataFromFileName(cls, fileName):
        dataset = open(fileName).readlines()
        values = []
        labels = []
        for line in dataset:
            line = line.split(",")
            label = line.pop(len(line) - 1)
            line = [float(x) for x in line]
            values.append(line[:len(line)])
            labels.append(label)
        values.pop(len(values) - 1)
        num_row = len(values)
        num_col = len(values[0])
        # print(values)
        mean = np.mean(values, axis=0)
        return cls(np.array(values), np.array(labels), num_row, num_col, mean)

def main():
    if len(sys.argv) != 4:
        print("Wrong number of arguments. Exit")
        exit(1)

    k = int(sys.argv[2])
    fileName = sys.argv[1]
    outputName = sys.argv[3]
    data = loadData.loadDataFromFileName(fileName)
    model = kmeans(k, data)
    clustered = model.kmeans()
    fileToWrite = open(outputName, 'w')
    for c in clustered:
        fileToWrite.write('%s\n' % c)
    sse = model.evaluateSSE(clustered)
    fileToWrite.write('The SSE is %s\n' % sse)
    fileToWrite.close()
    print("The squared sum error is %.3f" % sse)

if __name__ == '__main__':
    main()