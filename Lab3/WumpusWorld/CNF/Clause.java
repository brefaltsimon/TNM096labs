package CNF;

import java.util.Arrays;
import java.util.HashSet;
import java.util.Set;
import java.util.stream.*;

public class Clause {
    private Set<String> positives;
    private Set<String> negatives;

    public Clause(String positives, String negatives) {
        this.positives = new HashSet<>();
        if (!positives.isEmpty()) {
            this.positives.addAll(Arrays.asList(positives.split(" ")));
        }

        this.negatives = new HashSet<>();
        if (!negatives.isEmpty()) {
            this.negatives.addAll(Arrays.asList(negatives.split(" ")));
        }
    }

    public Clause(Set<String> positives, Set<String> negatives) {
        this.positives = positives;
        this.negatives = negatives;
    }

    public Clause getDecorated(int x, int y) {
        Stream<String> ps = positives.stream().map(s -> s + x + y);
        Stream<String> ns = negatives.stream().map(s -> s + x + y);

        HashSet<String> p = new HashSet<>();
        HashSet<String> n = new HashSet<>();
        ps.forEach(p::add);
        ns.forEach(n::add);

        return new Clause(p, n);
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

    @Override
    public boolean equals(Object other) {
        Clause o = (Clause) other;
        return this.positives.equals(o.positives) && this.negatives.equals(o.negatives);
    }

    @Override
    public int hashCode() {
        return this.toString().hashCode();
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
        if (positives.isEmpty() && negatives.isEmpty())
            return "Empty clause";

        return String.join(", ", positives) + (negatives.isEmpty() ? "" : (positives.isEmpty() ? "" : ", ") + "not ")
                + String.join(", not ", negatives);
    }
}
