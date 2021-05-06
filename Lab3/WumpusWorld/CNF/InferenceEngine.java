package CNF;

import java.util.Set;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.Random;
import java.util.Optional;
import java.util.Collections;

public class InferenceEngine {
    private ArrayList<Clause> knowledgeBase;

    public InferenceEngine(ArrayList<Clause> knowledgeBase) {
        this.knowledgeBase = knowledgeBase;
    }

    private void incorporateClause(Set<Clause> KB, Clause a) {
        for (Clause b : KB) {
            if (b.subsumesOrEquals(a)) {
                return;
            }
        }

        Set<Clause> toRemove = new HashSet<>();
        for (Clause b : KB) {
            if (a.subsumes(b)) {
                toRemove.add(b);
            }
        }
        KB.removeAll(toRemove);

        KB.add(a);
    }

    private static ArrayList<Clause[]> getUniquePairs(ArrayList<Clause> clauses) {
        ArrayList<Clause[]> pairs = new ArrayList<>();

        for (int i = 0; i < clauses.size() - 1; ++i) {
            for (int j = i + 1; j < clauses.size(); ++j) {
                pairs.add(new Clause[] { clauses.get(i), clauses.get(j) });
            }
        }

        return pairs;
    }

    public static void removeSubsumptions(Set<Clause> clauses) {
        ArrayList<Clause[]> pairs = getUniquePairs(new ArrayList<Clause>(clauses));

        Set<Clause> toRemove = new HashSet<>();
        for (Clause[] pair : pairs) {
            if (pair[0].subsumes(pair[1]))
                toRemove.add(pair[1]);
            else if (pair[1].subsumes(pair[0]))
                toRemove.add(pair[0]);
        }

        clauses.removeAll(toRemove);
    }

    public Set<Clause> solve() {
        Set<Clause> KB = new HashSet<>();
        ArrayList<Clause> expandedKB;
        KB.addAll(knowledgeBase);

        do {
            Set<Clause> S = new HashSet<>();
            expandedKB = new ArrayList<>(KB);

            for (Clause[] AB : getUniquePairs(expandedKB)){
                Optional<Clause> c = InferenceEngine.resolve(AB[0], AB[1]);
                if (c.isPresent()) {
                    S.add(c.get());
                }
            }

            if (S.isEmpty()) {
                break;
            }

            for (Clause s : S) {
                incorporateClause(KB, s);
            }

        } while (!new HashSet<>(expandedKB).equals(KB));

        return KB;
    }

    public static Optional<Clause> resolve(Clause a, Clause b) {
        Set<String> aNegLits = a.negativeLiterals(b);
        Set<String> bNegLits = b.negativeLiterals(a);

        Set<String> aPos = a.clonePositives();
        Set<String> aNeg = a.cloneNegatives();
        Set<String> bPos = b.clonePositives();
        Set<String> bNeg = b.cloneNegatives();

        if (aNegLits.size() == 0 && bNegLits.size() == 0) {
            return Optional.empty();
        } else if (aNegLits.size() > 0) {
            String literal = getRandomLiteral(aNegLits);
            aNeg.remove(literal);
            bPos.remove(literal);
        } else {
            String literal = getRandomLiteral(bNegLits);
            aPos.remove(literal);
            bNeg.remove(literal);
        }

        aPos.addAll(bPos);
        aNeg.addAll(bNeg);

        Clause c = new Clause(aPos, aNeg);

        if (c.isTautology()) {
            return Optional.empty();
        }

        return Optional.of(c);
    }

    public static String getRandomLiteral(Set<String> a) {
        String[] strings = a.toArray(new String[a.size()]);
        Random rng = new Random();
        int rand = rng.nextInt(strings.length);
        return strings[rand];
    }
}