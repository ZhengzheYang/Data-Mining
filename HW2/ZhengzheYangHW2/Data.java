//Zhengzhe Yang

import java.util.*;
import java.io.*;

public class Data {
	ArrayList<ArrayList<String>> data;
 	ArrayList<String> labels;
 	HashSet<String> uniqueAttributes;
 	String decision;

	public Data() {
		data = new ArrayList<>();
		labels = new ArrayList<>();
		uniqueAttributes = new HashSet<>();
		decision = "";
	}

	public Data(String filename) throws IOException {
		//TODO: Load data and fill data and labels
		data = new ArrayList<>();
		labels = new ArrayList<>();
		uniqueAttributes = new HashSet<>();
		File file = new File(filename);
		Scanner scanner = new Scanner(file);
		while(scanner.hasNextLine()) {
			String line = scanner.nextLine();
			String[] temp = line.split("\t");
			// System.out.println(temp[0]);
			ArrayList<String> splitLine = new ArrayList<>();
			for(int i = 1; i < temp.length; i++) {
				splitLine.add(temp[i]);
				uniqueAttributes.add(temp[i]);
			}
			data.add(splitLine);
			labels.add(temp[0]);
		}
	}
}