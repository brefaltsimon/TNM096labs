package CNF;

import java.util.Set;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.Random;
import java.util.Optional;
import java.util.Collections;

public class InferenceEngine {
    private Clause[] knowledgeBase;

    public InferenceEngine(Clause[] knowledgeBase) {
        this.knowledgeBase = knowledgeBase;
    }

    private void incorporateClause(Set<Clause> KB, Clause a) {
        for (Clause b : KB) {
            if (b.subsumesOrEquals(a)) {
                return;
            }
        }

        for (Clause b : KB) {
            if (a.subsumes(b)) {
                KB.remove(b);
            }
        }

        KB.add(a);
    }

    public Set<Clause> solve() {
        Set<Clause> KB = new HashSet<>();
        ArrayList<Clause> expandedKB;
        Collections.addAll(KB, knowledgeBase);

        do {
            Set<Clause> S = new HashSet<>();
            expandedKB = new ArrayList<>(KB);

            for (int i = 0; i < expandedKB.size() - 1; ++i) {
                for (int j = i + 1; j < expandedKB.size(); ++j) {
                    // construct pair array[i], array[j]
                    Optional<Clause> c = InferenceEngine.resolve(expandedKB.get(i), expandedKB.get(j));
                    if (c.isPresent()) {
                        S.add(c.get());
                    }
                }
            }

            if (S.isEmpty()) {
                return new HashSet<Clause>(KB);
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