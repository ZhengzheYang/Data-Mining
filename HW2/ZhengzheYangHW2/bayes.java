//Zhengzhe Yang

import java.util.*;
import java.io.*;
import java.text.DecimalFormat;

public class bayes {
	static Data D;
	static HashMap<String, ArrayList<HashMap<String, Double>>> trained_model;
	static HashMap<String, Double> priors;
	static int attr_num;
	public static void main(String[] args) throws IOException{
		if(args.length != 3) {
			System.out.println("Not enough arguments or too many. Exit.");
			System.exit(-1);
		}
		String filename = args[0];
		String test = args[1];
		String output = args[2];
		D = new Data(filename);
		trained_model = new HashMap<String, ArrayList<HashMap<String, Double>>>();
		priors = new HashMap<String, Double>();
		attr_num = D.data.get(0).size();
		train();
		test(test, output);
	}

	/* Train the model with naive bayesian clasiffier*/
	public static void train() {
		updatePriors();
		updatePosterior();
	}

	/* Test accuracy and write to an output file */
	public static void test(String filename, String output) throws IOException{
		Data test = new Data(filename);
		ArrayList<String> res = new ArrayList<>();
		HashMap<String, Double> probabilities = new HashMap<>();
		int attr_num = test.data.get(0).size();
		for(ArrayList<String> list : test.data) {
			for(String c : priors.keySet()) {
				double prob = 1.0;
				for(int i = 0; i < attr_num; i++) {
					Double posterior = trained_model.get(c).get(i).get(list.get(i));
					if(posterior == null) {
						prob = 0;
					} else {
						prob *= posterior;
					}
				}
				prob *= priors.get(c);
				probabilities.put(c, prob);
			}

			String result = "";
			double max = -1;
			for(String c : probabilities.keySet()) {
				double prob = probabilities.get(c);
				if(max < prob) {
					max = prob;
					result = c;
				}
			}
			res.add(result);
		}

		double correct = 0;
		for(int i = 0; i < res.size(); i++) {
			if(res.get(i).equals(test.labels.get(i))) {
				correct++;
			}
		}
		double accuracy = correct / res.size() * 100;
		DecimalFormat df2 = new DecimalFormat(".##");

		PrintWriter writer = new PrintWriter(output, "UTF-8"); 
		for(String s : res) {
			writer.println(s);
		}
		writer.println(df2.format(accuracy) + "%");
		writer.close();
	}

	/* update the priors that is simply the probability of each class */
	private static void updatePriors() {
		for(String c : D.labels) {
			priors.put(c, priors.getOrDefault(c, 0.0) + 1);
		}
		for(String s : priors.keySet()) {
			priors.put(s, priors.get(s) / D.labels.size());
		}
	}

	/* update the posterior for each attr with their corresponding class*/
	private static void updatePosterior() {
		for(String s : priors.keySet()) {
			ArrayList<HashMap<String, Double>> list = new ArrayList<>();
			for(int col = 0; col < attr_num; col++) {
				double total = 0.0;
				HashMap<String, Double> posterior = new HashMap<>();
				for(int i = 0; i < D.labels.size(); i++) {
					if(D.labels.get(i).equals(s)) {
						posterior.put(D.data.get(i).get(col), posterior.getOrDefault(D.data.get(i).get(col), 0.0) + 1);
						total++;
					}
				}
				for(String attr : posterior.keySet()) {
					posterior.put(attr, posterior.get(attr) / total);
				}
				list.add(posterior);
			}
			trained_model.put(s, list);
		}
	}
}