//Zhengzhe Yang

import java.util.*;
import java.io.*;
import java.text.DecimalFormat;

public class c45 {
	static Data D;
	static ArrayList<Integer> attrList;
	public static void main(String[] args) throws IOException {
		// String filename = "small.training.txt";
		// String test = "small.test.txt";
		if(args.length != 3) {
			System.out.println("Not enough arguments or too many. Exit.");
			System.exit(-1);
		}
		String filename = args[0];
		String test = args[1];
		String output = args[2];
		D = new Data(filename);
		attrList = new ArrayList<>();
		for(int i = 0; i < D.data.get(0).size(); i++) {
			attrList.add(i);
		}
		Node root = Generate_Decision_Tree(D, attrList);
		testAccuracy(root, test, output);
		// System.out.println("Accuracy is " + testAccuracy(root, test) + "%");
	}

	/* test the accuracy and write classified results to an output file */
	private static void testAccuracy(Node root, String test, String output) throws IOException {
		Data testData = new Data(test);
		ArrayList<String> res = new ArrayList<>();
		for(ArrayList<String> list : testData.data) {
			Node head = root;
			while(head.label.equals("")) {
				head = head.children.get(list.get(head.splitCriterion));
			}
			res.add(head.label);
		}

		double error = 0.0;
		for(int i = 0; i < res.size(); i++) {
			if(!res.get(i).equals(testData.labels.get(i))) {
				error++;
			}
		}

		double accuracy = (res.size() - error) / res.size() * 100;
		DecimalFormat df2 = new DecimalFormat(".##");

		PrintWriter writer = new PrintWriter(output, "UTF-8"); 
		for(String s : res) {
			writer.println(s);
		}
		writer.println(df2.format(accuracy) + "%");
		writer.close();
	}

	/* Generate a decision tree and return the root */
	public static Node Generate_Decision_Tree(Data D, ArrayList<Integer> attrList) {
		Node root = new Node();

		if(allSame(D)) {
			root.label = D.labels.get(0);
			return root;
		}

		if(attrList.isEmpty()) {
			root.label = majorityClass(D);
			return root;
		}

		int splitCriterion = AttributeSelectionMethod(D, attrList);
		root.splitCriterion = splitCriterion;
		attrList.remove(splitCriterion);

		for(Data Dj : split(D, splitCriterion)) {
			if(Dj.data.isEmpty()) {
				Node leaf = new Node();
				leaf.label = majorityClass(D);
				root.children.put(Dj.decision, leaf);
			} else {
				root.children.put(Dj.decision, Generate_Decision_Tree(Dj, attrList));
			}
		}
		return root;
	}

	/* Split the data with split criterion. Return an arraylist of Data */
	private static ArrayList<Data> split(Data D, int splitCriterion) {
		ArrayList<Data> outcome = new ArrayList<>();

		for(String s : D.uniqueAttributes) {
			Data Dj = new Data();
			for(int i = 0; i < D.data.size(); i++) {
				ArrayList<String> list = D.data.get(i);
				if(list.get(splitCriterion).equals(s)) {
					Dj.data.add(list);
					Dj.labels.add(D.labels.get(i));
				}
			}
			Dj.decision = s;
			Dj.uniqueAttributes = D.uniqueAttributes;
			outcome.add(Dj);
		}
		return outcome;
	}


	/* Select the attribute with the most gain ratio */
	private static int AttributeSelectionMethod(Data D, ArrayList<Integer> attrList) {
		double maxGainRatio = Double.NEGATIVE_INFINITY;
		int maxIndex = -1;
		for(int i = 0; i < attrList.size(); i++) {
			double temp = getGainRatio(D, attrList.get(i));
			if(maxGainRatio < temp) {
				maxGainRatio = temp;
				maxIndex = i;
			}
		}
		return maxIndex;
	}

	/* Get the gain ratio */
	private static double getGainRatio(Data D, int i) {
		double splitInfo = getEntropy(countHelper(D, i, "allAttribute"), D.data.size());
		if(splitInfo == 0) {
			return 0;
		}
		return getGain(D, i) / splitInfo;
	}

	/* Get the gain */
	private static double getGain(Data D, int i) {
		return getEntropy(countHelper(D, i, "allLabel"), D.labels.size()) - getAttributeInfo(D, countHelper(D, i, "allAttribute"), D.data.size(), i);
	}

	/* Calculate the attribute's info */
	private static double getAttributeInfo(Data D, HashMap<String, Integer> map, int total, int i) {
		double info = 0.0;
		for(String s : map.keySet()) {
			info += 1.0 * map.get(s) / total * getEntropy(countHelper(D, i, s), map.get(s));
		}
		return info;
	}

	/* Count the requested items and return a hash map */
	private static HashMap<String, Integer> countHelper(Data D, int i, String command) {
		HashMap<String, Integer> counter = new HashMap<>();
		if(command.equals("allLabel")) {
			for(String s : D.labels) {
				counter.put(s, counter.getOrDefault(s, 0) + 1);
			}
		} else if(command.equals("allAttribute")) {
			for(int index = 0; index < D.data.size(); index++) {
				String s = D.data.get(index).get(i);
				counter.put(s, counter.getOrDefault(s, 0) + 1);
			}
		} else {
			for(int index = 0; index < D.data.size(); index++) {
				if(command.equals(D.data.get(index).get(i))) {
					counter.put(D.labels.get(index), counter.getOrDefault(D.labels.get(index), 0) + 1);
				}
			}
		}
		return counter;
	}

	/* Take in a hash map, count and calculate the entropy */
	private static double getEntropy(HashMap<String, Integer> counter, int total) {
		double info = 0.0;
		for(String s : counter.keySet()) {
				double temp = 1.0 * counter.get(s) / total;
				info += (-1.0) * temp * Math.log(temp) / Math.log(2);
		}
		return info; 
	}

	/* Check if the attributes in D all have the same class. */
	private static boolean allSame(Data D) {
		HashSet<String> unique = new HashSet<>(D.labels);
		if(unique.size() == 1) {
			return true;
		}
		return false;
	}

	/* Get the mojority vote */
	private static String majorityClass(Data D) {
		HashMap<String, Integer> counter = new HashMap<>();
		String majorityClass = "";
		int max = -1;

		for(String s : D.labels) {
			counter.put(s, counter.getOrDefault(s, 0) + 1);
		}

		for(String s : counter.keySet()) {
			if(counter.get(s) > max) {
				max = counter.get(s);
				majorityClass = s;
			}
		}
		return majorityClass;
	}
}
