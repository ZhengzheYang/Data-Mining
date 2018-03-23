# Emory University CS378
# Homework 2
# Yicheng (Jason) Wang
# NetId: ywan693

import csv
import sys
import numpy as np

class NaiveBayesian(object):

	def __init__(self):
		self.priors = {}
		self.table = []
		self.classes_ = None

	def _get_attr(self, X):
		return [list(np.unique(i)) for i in zip(*X)]

	def _get_priors(self, y):
		priors = {}
		classes_ = np.unique(y)
		for class_ in classes_:
			priors[class_] = sum(y==class_)*1.0/y.shape[0]
		return priors

	def fit(self, df):
		X, y = list(df[:, 1:]), df[:, 0]
		classes_ = np.unique(y)
		attrs = self._get_attr(X)
		self.priors = self._get_priors(y)

		table = []
		for i in range(1, len(attrs)+1):
			attr = attrs[i-1]
			local_dict = dict()
			for a in attr:
				prob_dict = dict()
				for class_ in classes_:
					df_class = df[y==class_, :]
					class_num = df_class.shape[0]
					attr_num = sum(df_class[:, i] == a)
					prob_dict[class_] = attr_num * 1.0 / class_num
				local_dict[a] = prob_dict

			table.append(local_dict)

		self.table = table
		self.classes_ = classes_

	def predict(self, X_test):
		res = []
		for x in X_test:
			local_res = {}
			for class_ in self.classes_:
				prob = 1
				for i in range(len(list(x))):
					prob *= self.table[i][x[i]][class_]
				local_res[class_] = prob

			max_value = max(local_res.values())
			max_key = [k for k,v in local_res.items() if v == max_value][0]

			res.append(max_key)

		return res

	def score(self, X_test, y_test):
		result = self.predict(X_test)
		y_test = np.array(y_test)
		return sum(result == y_test)*100.0 / y_test.size

def load_data(inputFile):
	reader = csv.reader(open(inputFile), delimiter='\t')
	df = np.array([row for row in reader])
	return df

def main():

	if (len(sys.argv) < 4):
		trainFile, testFile = 'mushroom.training.txt', 'mushroom.test.txt'
		outputFile = 'bayesian.out'
	else:
		trainFile, testFile, outputFile = sys.argv[1:]

	## load data
	df = load_data(trainFile)

	## Fit model
	clf = NaiveBayesian()
	clf.fit(df)

	## Testing 
	df_test = load_data(testFile)
	X_test, y_test = df_test[:,1:], df_test[:, 0]
	score = clf.score(X_test, y_test)

	with open(outputFile, 'w') as f:
		f.write('\n'.join(clf.predict(X_test)))
		f.write("\nThe accuracy rate for the naive bayesian model is {0:.2f} %".format(score))

	print("The accuracy rate for the naive bayesian model is {0:.2f} %".format(score))

if __name__ == '__main__':
	main()