package CNF;

import java.util.Arrays;
import java.util.HashSet;
import java.util.Set;

public class Clause {
    private Set<String> positives;
    private Set<String> negatives;

    public Clause(String positives, String negatives) {
        this.positives = new HashSet<>(Arrays.asList(positives.split(" ")));
        this.negatives = new HashSet<>(Arrays.asList(negatives.split(" ")));
    }

    public Clause(Set<String> positives, Set<String> negatives) {
        this.positives = positives;
        this.negatives = negatives;
    }

    private static Set<String> intersection(Set<String> a, Set<String> b) {
        boolean aLarger = a.size() > b.size();
        Set<String> temp = new HashSet<>(aLarger ? b : a);
        temp.retainAll(aLarger ? a : b);
        return temp;
    }

    public boolean isTautology() {
        Set<String> temp = Clause.intersection(this.positives, this.negatives);
        return temp.size() != 0;
    }

    public boolean subsumesOrEquals(Clause other) {
        return this.subsumes(other) || this.equals(other);
    }

    public boolean equals(Clause other) {
        return this.positives.equals(other.positives) && this.negatives.equals(other.negatives);
    }

    public boolean subsumes(Clause other) {
        // intersection of A and B is same size as A => A is subset of B
        if (this.positives.size() == other.positives.size() && this.negatives.size() == other.negatives.size()) {
            return false;
        }

        Set<String> tempPos = Clause.intersection(this.positives, other.positives);
        Set<String> tempNeg = Clause.intersection(this.negatives, other.negatives);
        return (this.positives.size() == tempPos.size() && this.negatives.size() == tempNeg.size());
    }

    public Set<String> negativeLiterals(Clause other) {
        return Clause.intersection(this.negatives, other.positives);
    }

    public Set<String> clonePositives() {
        return new HashSet<>(this.positives);
    }

    public Set<String> cloneNegatives() {
        return new HashSet<>(this.negatives);
    }
    
    public String toString() {
        return "Positives: " + positives.toString() + "\nNegatives: " + negatives.toString();
    }
}
