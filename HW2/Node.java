//Zhengzhe Yang

import java.util.*;

public class Node {
	HashMap<String, Node> children;
	String label;
	String decision;
	Integer splitCriterion;
	public Node() {
		this.children = new HashMap<String, Node>();
		this.label = "";
		this.splitCriterion = -1;
	}

	public Node(HashMap<String, Node> children, String label, Integer splitCriterion) {
		this.children = children;
		this.label = label;
		this.splitCriterion = splitCriterion;
	}
}