# Emory University CS378
# Homework 2
# Yicheng (Jason) Wang
# NetId: ywan693

import csv
import random
import copy
import sys
import numpy as np

def load_data(inputFile):
	reader = csv.reader(open(inputFile))
	df = np.array([row for row in reader if row])
	X, y = df[:,:-1],df[:,-1]
	X = X.astype(np.float)
	return X, y

def _minmax_normalize(X):
	res = []
	for column in X.T:
		column = (column - column.min()) / (column.max()-column.min())
		res.append(column)

	# print(np.array(res).T)
	return np.array(res).T


def _get_objects(k, X):
	points = []
	index_set = set()

	for i in range(k):
		index = random.randint(0, len(X)-1)
		while index in index_set:
			index = random.randint(0, len(X)-1)
		points.append(X[index])
		index_set.add(index)

	return np.array(points)

def _get_diff(C, C_old):
	res = 0
	for i in range(len(C)):
		res += np.linalg.norm(C[i]-C_old[i], 2)

	return res

def _get_distance(p, C):
	res = []
	for i in range(len(C)):
		res.append(np.linalg.norm(p-C[i], 2))
	return res

def k_means(k, X):
	C = _get_objects(k, X)

	C_old = np.zeros(C.shape)
	clusters = np.zeros(len(X))
	diff = _get_diff(C, C_old)

	while diff != 0:
		for i in range(len(X)):
			index = np.argmin(_get_distance(X[i], C))
			clusters[i] = index

		C_old = copy.deepcopy(C)

		for i in range(k):
			if list(X[clusters==i]):
				C[i] = np.mean(X[clusters==i], axis=0)
			else:
				print("Bad luck, random numbers are too close, Please run again")
				sys.exit(0)

		diff = _get_diff(C, C_old)
	
	return clusters, C

def get_sse(k, X, clusters, C):
	sse = 0
	for i in range(k):
		for xi in X[clusters == i]:
			sse += np.linalg.norm(xi-C[i], 2) ** 2

	return sse

def main():
	## naive handling of commandline input
	if len(sys.argv) != 4:
		inputFile, k, outputFile = 'iris.data.txt', 3, 'iris.out'
	else:
		inputFile, k, outputFile = sys.argv[1:]
		k = int(k)

	## load data
	X, y = load_data(inputFile)
	# X_normal = _minmax_normalize(X)

	## perform k-means algorithm
	clusters, C = k_means(k, X)

	## calculate SSE
	sse = get_sse(k, X, clusters, C)
	print("The SSE of the clusters is: %.2f" %sse)	

	## print results
	clusters = list(map(int, clusters))

	with open(outputFile, 'w') as f:
		for i in clusters:
			f.write(str(i)+'\n')
		f.write("The SSE of the cluster is: %.2f" %sse)

if __name__ == '__main__':
	main()

